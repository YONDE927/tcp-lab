#pragma once

//C++ include
#include <string>
#include <thread>
#include "transport.h"

//C include
#include <sys/socket.h>
#include <arpa/inet.h>

namespace netobj{
    class server_master{
        protected:
            int server_socket{0};
            sockaddr_in server_addr{0};
        public:
            server_master();
            ~server_master();
            int get_socket();
            int set_host(std::string ip, short port);
            int start_listen();
            int stop_listen();
            int accept_client();
            template <class WORKER_FUNC, class ...Args>
            int run(WORKER_FUNC runner, Args&&... args){
                if(server_socket <= 0){
                    std::cout << "[server] server begin listening" << std::endl;
                    if(server_master::start_listen() < 0){
                        return -1;
                    }
                }

                while(1){
                    int client_socket = this->accept_client();
                    if(client_socket <= 0){
                        std::cout << "[server] accept_client() error" << std::endl;
                        continue;
                    }
                    try{
                        std::thread session_thread(runner, client_socket, args...);
                        session_thread.detach();
                    }catch(std::system_error &e){
                        std::cout << e.what() << std::endl;
                        close(client_socket);
                        continue;
                    }
                }
                return 0;
            }
    };

    class server{
        protected:
            const int server_socket{0};
            transport::Transporter transporter;
            server(int socket);
        public:
            ~server();
            int get_socket();
            virtual void run(){};
    };

    class client{
        protected:
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
