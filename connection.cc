//C include
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

//C++ include
#include <iostream>
#include <list>
#include <string>
#include <thread>
#include <memory>

#include "utils.h"

namespace netobj{

    class Server;
    class ServerWorker;
    class Client;

    struct Host {
        int socket;
        struct sockaddr_in addr;
    };

    class Server{
        private:
            Host server;
            std::list<std::shared_ptr<ServerWorker>> workers;
        public:
            Server();
            ~Server();
            int set_host(std::string ip, short port);
            int start_listen();
            int stop_listen();
            Host accept_client();
            template <class F, class ...Args>
            int run(F&& work, Args&&... args);
    };

    class ServerWorker: public std::enable_shared_from_this<Server> {
        private:
            Host client;
            Host server;
            std::thread thread;
        public:
            template <class F, class ...Args>
            ServerWorker(const Host& _server, const Host& _client, F&& function, Args&&... args);
            ~ServerWorker();
    };

    class Client{
        private:
            Host client;
            Host server;
        public:
            int set_host();
            int connect_node();
    };

    //Server
    Server::Server(){}

    Server::~Server(){
        int rc{0};

        if(server.socket > 0){
            if(shutdown(server.socket, SHUT_RDWR) < 0){
                print_error();
            }
            if(close(server.socket) < 0){
                print_error();
            }
        }
    }

    int Server::set_host(std::string ip, short port){
        int rc{0};
        server.addr.sin_family = AF_INET;
        server.addr.sin_addr.s_addr = inet_addr(ip.c_str());
        if(server.addr.sin_addr.s_addr == 0){
            std::cout << "invalid ip" << std::endl;
            return -1;
        }
        server.addr.sin_port = htons(port);
        return 0;
    }

    int Server::start_listen(){
        int rc{0};
        struct sockaddr_in servaddr{0};
        int yes = 1;

        //ソケット生成
        server.socket = socket(AF_INET, SOCK_STREAM, 0);
        if(server.socket < 0){
            print_error();
            return -1;
        }

        if(setsockopt(server.socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes)) < 0){
            print_error();
            return -1;
        }

        //ポート接続
        rc = bind(server.socket, (struct sockaddr*)&server.addr, sizeof(sockaddr_in));
        if(rc < 0){
            print_error();
            return -1;
        }

        //接続待ち
        rc = listen(server.socket, 5);
        if(rc < 0){
            print_error();
            return -1;
        }
        return server.socket;
    }

    int Server::stop_listen(){
        if(server.socket > 0){
            if(shutdown(server.socket, SHUT_RDWR) < 0){
                print_error();
            }
            if(close(server.socket) < 0){
                print_error();
            }
        }
        server.socket = 0;
        return 0;
    }

    Host Server::accept_client(){
        Host client{0};
        char ip[INET_ADDRSTRLEN] = {0};
        int tmp{0};
        //クライアントソケット生成
        tmp = sizeof(sockaddr_in);
        client.socket = accept(server.socket, (struct sockaddr*)&client.addr, (socklen_t*)&tmp);
        if(client.socket < 0){
            print_error();
            return client;
        }

        char str[INET_ADDRSTRLEN] = {0};
        inet_ntop(AF_INET, &client.addr.sin_addr, str, INET_ADDRSTRLEN);
        std::cout << "accept ip " << str << "\naccept port " << client.addr.sin_port << std::endl;
        return client;
    }

    template <class F, class ...Args>
    int Server::run(F&& work, Args&&... args){
        Host client{0};

        while(1){
            client = this->accept_client();
            if(client.socket <= 0){
                std::cout << "accept_client() error" << std::endl;
                continue;
            }

            std::shared_ptr<ServerWorker> worker(new ServerWorker(this->server, client, work, args...));
            workers.push_front(worker);
        }
        return 0;
    }

    //ServerWorker
    template <class F, class ...Args>
    ServerWorker::ServerWorker(const Host& _server, const Host& _client, F&& work, Args&&... args){
        client = _client;
        server = _server;
        thread(work, shared_from_this(), args...);
    }

    ServerWorker::~ServerWorker(){
        thread.join();
        if(client.socket > 0){
            if(shutdown(client.socket, SHUT_RDWR) < 0){
                print_error();
            }
            if(close(client.socket) < 0){
                print_error();
            }
        }
    }
};
