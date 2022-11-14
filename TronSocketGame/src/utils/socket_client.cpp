#include <iostream>
#include <string>
#include <sstream>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define TRUE 1
#define PORT 8888
using namespace std;

class socket_client
{
    private:
        int sock = 0, valread, client_fd;
        bool closed = true;
    public:
        socket_client()
        {
            init_connection();
        }

        ~socket_client()
        {
            if(!closed) close_conn();
        }

        void close_conn() {
            try
            {
                close(client_fd);
                closed = true;
            }
            catch(const std::exception& e)
            {
                cerr << e.what() << '\n';
            }
        }

        int init_connection() 
        {
            if(!closed) return 0;
            struct sockaddr_in serv_addr;
            char buffer[1025] = {0};

            if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            {
                cout << "\n Socket creation error \n" << endl;
                return -1;
            }

            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(PORT);

            if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
            {
                cout << "\nInvalid address/ Address not supported \n" << endl;
                return -1;
            }

            if ((client_fd = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0)
            {
                cout << "\nConnection Failed \n" << endl;
                return -1;
            }

            closed = false;
            return 0;
        }

        void send_msg(string msg) {
            send(sock, msg.c_str(), msg.length(), 0);
        }

        string read_msg() {
            char buffer[1025]; 
            int valread = read(sock, buffer, 1024);
            buffer[valread] = '\0';
            string s(buffer);
            return s;
        }
};

