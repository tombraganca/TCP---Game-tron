#include <stdio.h>
#include <string.h> //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>    //close
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros

#include <iostream>
#include <string>
#include <sstream>

#include <thread>
#include <mutex>
#include <list>

#define TRUE 1
#define FALSE 0
#define PORT 8888
#define PLAYERS 2

using namespace std;

class safe_queue
{
    mutex m;
    list<string> str_queue;

public:
    safe_queue(){};
    void add(const string &s)
    {
        const lock_guard<mutex> lock(m);
        str_queue.push_back(s);
    }

    bool pop(string &s)
    {
        const lock_guard<mutex> lock(m);
        if (!str_queue.empty())
        {
            s = str_queue.front();
            str_queue.pop_front();
            return true;
        }

        return false;
    }
};

int main(int argc, char *argv[])
{
    int opt = TRUE;
    int players_ready = 0;
    int master_socket, addrlen, new_socket, client_socket[PLAYERS],
        max_clients = PLAYERS, activity, i, valread, socket_descriptor;
    int max_sd;
    struct sockaddr_in address;

    char buffer[1025]; // data buffer of 1K

    // set of socket descriptors
    fd_set readfds;

    // a message
    char *message = "ECHO Daemon v1.0 \r\n";

    // initialise all client_socket[] to 0 so not checked
    for (i = 0; i < max_clients; i++)
    {
        client_socket[i] = 0;
    }

    // create a master socket
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // set master socket to allow multiple connections ,
    // this is just a good habit, it will work without this
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
                   sizeof(opt)) < 0)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // bind the socket to localhost port 8888
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", PORT);

    // try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // accept the incoming connection
    addrlen = sizeof(address);
    puts("Waiting for connections ...");

    safe_queue sq;

    bool running = true;

    auto io_thread = thread([&]
                            {
        string s;
        while (1){
            printf("game");
        } }); // thread.

    while (TRUE)
    {
        printf("Inicio");
        // clear the socket set
        FD_ZERO(&readfds);

        // add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        // add child sockets to set
        for (i = 0; i < max_clients; i++)
        {
            // socket descriptor
            socket_descriptor = client_socket[i];

            // if valid socket descriptor then add to read list
            if (socket_descriptor > 0)
                FD_SET(socket_descriptor, &readfds);

            // highest file descriptor number, need it for the select function
            if (socket_descriptor > max_sd)
                max_sd = socket_descriptor;
        }

        // wait for an activity on one of the sockets , timeout is NULL ,
        // so wait indefinitely
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR))
        {
            printf("select error");
        }

        // If something happened on the master socket ,
        // then its an incoming connection
        if (FD_ISSET(master_socket, &readfds))
        {
            if ((new_socket = accept(master_socket,
                                     (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            // inform user of socket number - used in send and receive commands
            // printf("New connection , socket fd is %d , ip is : %s , port : %d\n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            // send new connection greeting message
            if (send(new_socket, message, strlen(message), 0) != strlen(message))
            {
                perror("send");
            }

            puts("Welcome message sent successfully");

            // add new socket to array of sockets
            for (i = 0; i < max_clients; i++)
            {
                // if position is empty
                if (client_socket[i] == 0)
                {
                    client_socket[i] = new_socket;
                    // printf("Adding to list of sockets as %d\n", i);

                    break;
                }
            }
            printf("meio");
        }

        // else its some IO operation on some other socket
        for (i = 0; i < max_clients; i++)
        {
            socket_descriptor = client_socket[i];

            if (FD_ISSET(socket_descriptor, &readfds))
            {
                // Check if it was for closing , and also read the
                // incoming message
                if ((valread = read(socket_descriptor, buffer, 1024)) == 0)
                {
                    // Somebody disconnected , get his details and print
                    getpeername(socket_descriptor, (struct sockaddr *)&address,
                                (socklen_t *)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n",
                           inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                    // Close the socket and mark as 0 in list for reuse
                    close(socket_descriptor);
                    client_socket[i] = 0;
                }

                // Echo back the message that came in
                else
                {
                    // set the string terminating NULL byte on the end
                    // of the data read
                    buffer[valread] = '\0';

                    send(socket_descriptor, buffer, strlen(buffer), 0);

                    for (int j = 0; j < max_clients; j++)
                    {
                        if (client_socket[j] != 0)
                        {
                            send(client_socket[j], buffer, strlen(buffer), 0);
                        }
                    }

                    if (buffer[0] == 'r')
                    {
                        players_ready++;
                        printf("players ready: %d\n", players_ready);
                    }
                    if (players_ready == 2)
                    {
                        for (int j = 0; j < max_clients; j++)
                        {
                            send(client_socket[j], "start\n", strlen("start\n"), 0);
                        }
                        printf("sending start signal to clients\n");

                        // logica do game
                    }
                }
            }
        }

        printf("fim");
    }

    return 0;
}