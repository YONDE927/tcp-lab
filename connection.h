#pragma once

#include <string>

namespace netobj{
    class Server;
    class Client;

    class Server{
        public:
            int server_socket{0};
            sockaddr_in server_addr{0};
            Server();
            ~Server();
            int set_host(std::string ip, short port);
            int start_listen();
            int stop_listen();
            int accept_client();
            template <class F, class ...Args>
            int run(F session, Args&&... args);
    };

    class Client{
        public:
            int client_socket{0};
            sockaddr_in client_addr{0};
            Client();
            ~Client();
            int set_host(std::string ip, short port);
            int connect_node();
            int reconnect_node();
    };
}
