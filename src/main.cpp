#include <iostream>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>

#define PORT 54000

using namespace std;

int main()
{
    // create a socket
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener <= 0)
    {
        cerr << "Error creating socket" << endl;
        return -1;
    }

    // bind the socket to an IP address and port
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(PORT);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

    if (bind(listener, (sockaddr *)&hint, sizeof(hint)) == -1)
    {
        cerr << "Error binding to IP/port" << endl;
        return -2;
    }
    // verify with listening
    if (listen(listener, SOMAXCONN) == -1)
    {
        cerr << "Can't bind to IP/root!" << endl;
        return -3;
    }

    sockaddr_in client;
    socklen_t clientSize;
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];

    int clientSocket = accept(listener, (sockaddr *)&client, &clientSize);

    if (clientSocket == -1)
    {
        cerr << "Problem with client connecting!" << endl;
        return -4;
    }

    memset(host, 0, NI_MAXHOST);
    memset(svc, 0, NI_MAXSERV);

    int result = getnameinfo((sockaddr *)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);

    if (result)
    {
        cout << host << " connected on " << svc << endl;
    }
    else
    {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        cout << host << " connected on " << ntohs(client.sin_port) << endl;
    }

    char buf[4096];
    while (true)
    {

        // clear the buffer
        memset(buf, 0, 4096);

        // wait for a message
        int bytesRecv = recv(clientSocket, buf, 4096, 0);
        if (bytesRecv == -1)
        {
            cerr << "There was a connection issue" << endl;
        }
        if (bytesRecv == 0)
        {
            cout << "The client disconnected" << endl;
            break;
        }

        // display message
        cout << "Received: " << string(buf, 0, bytesRecv) << endl;
        
        send(clientSocket, buf, bytesRecv + 1, 0);
    }

    // close the socket
    close(clientSocket);

    return 0;
}