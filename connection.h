#pragma once

//C++ include
#include <string>
#include "transport.h"

//C include
#include <sys/socket.h>
#include <arpa/inet.h>

namespace netobj{
    class Server;
    class Client;

    class server{
        private:
            int server_socket{0};
            sockaddr_in server_addr{0};
        public:
            server();
            ~server();
            int get_socket();
            int set_host(std::string ip, short port);
            int start_listen();
            int stop_listen();
            int accept_client();
            template <class WORKER_FUNC, class ...Args>
            int run(WORKER_FUNC runner, Args&&... args);
    };

    class server_worker{
        private:
            const int server_socket{0};
            transport::Transporter transporter;
            server_worker(int socket);
        public:
            ~server_worker();
            int get_socket();
            virtual void run();
            virtual void session();
    };

    class client{
        private:
            int client_socket{0};
            sockaddr_in client_addr{0};
            transport::Transporter transporter;
        public:
            client();
            ~client();
            int get_socket();
            int set_host(std::string ip, short port);
            int connect_server();
            int reconnect_server();
    };
}
