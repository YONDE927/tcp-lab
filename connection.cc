//C include
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>

//C++ include
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <memory>
#include <csignal>

#include "utils.h"

namespace netobj{

    class Server;
    class ServerWorker;
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

    //Server
    Server::Server(){}

    Server::~Server(){
        int rc{0};

        if(server_socket > 0){
            if(shutdown(server_socket, SHUT_RDWR) < 0){
                print_error();
            }
            if(close(server_socket) < 0){
                print_error();
            }
        }
    }

    int Server::set_host(std::string ip, short port){
        int rc{0};
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
        if(server_addr.sin_addr.s_addr < 0){
            std::cout << "[Server] invalid ip" << std::endl;
            return -1;
        }
        server_addr.sin_port = htons(port);
        return 0;
    }

    int Server::start_listen(){
        int rc{0};
        int yes{1};

        //ソケット生成
        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if(server_socket < 0){
            print_error();
            return -1;
        }

        if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes)) < 0){
            print_error();
            return -1;
        }

        //ポート接続
        if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(sockaddr_in)) < 0){
            print_error();
            return -1;
        }

        //接続待ち
        if(listen(server_socket, 5) < 0){
            print_error();
            return -1;
        }
        return server_socket;
    }

    int Server::stop_listen(){
        if(server_socket > 0){
            if(shutdown(server_socket, SHUT_RDWR) < 0){
                print_error();
            }
            if(close(server_socket) < 0){
                print_error();
            }
        }
        server_socket = 0;
        return 0;
    }

    int Server::accept_client(){
        int client_socket{0};
        sockaddr_in client_addr{0};
        char ip[INET_ADDRSTRLEN] = {0};
        int tmp{0};

        //クライアントソケット生成
        tmp = sizeof(sockaddr_in);
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, (socklen_t*)&tmp);
        if(client_socket < 0){
            print_error();
            return client_socket;
        }

        char str[INET_ADDRSTRLEN] = {0};
        inet_ntop(AF_INET, &client_addr.sin_addr, str, INET_ADDRSTRLEN);
        std::cout << "[Server] accept ip " << str << " accept port " << client_addr.sin_port << std::endl;
        return client_socket;
    }

    template <class F, class ...Args>
    int Server::run(F session, Args&&... args){
        if(server_socket <= 0){
            std::cout << "[Server] server begin listening" << std::endl;
            if(Server::start_listen() < 0){
                return -1;
            }
        }

        while(1){
            int client_socket = this->accept_client();
            if(client_socket <= 0){
                std::cout << "[Server] accept_client() error" << std::endl;
                continue;
            }
            try{
                std::thread session_thread(session, client_socket, args...);
                session_thread.detach();
            }catch(std::system_error &e){
                std::cout << e.what() << std::endl;
                close(client_socket);
                continue;
            }
        }
        return 0;
    }

    //Client
    Client::Client(){}

    Client::~Client(){
        if(client_socket > 0){
            if(shutdown(client_socket, SHUT_RDWR) < 0){
                print_error();
            }
            if(close(client_socket) < 0){
                print_error();
            }
        }
    }

    int Client::set_host(std::string ip, short port){
        int rc{0};
        client_addr.sin_family = AF_INET;
        client_addr.sin_addr.s_addr = inet_addr(ip.c_str());
        if(client_addr.sin_addr.s_addr < 0){
            std::cout << "[Client] invalid ip" << std::endl;
            return -1;
        }
        client_addr.sin_port = htons(port);
        return 0;
    }

    int Client::connect_node(){
        int yes{1};
        socklen_t socklen{sizeof(sockaddr_in)};

        client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if(client_socket < 0){
            std::cout << "[Client] socket error" << std::endl;
            print_error();
            return -1;
        }

        if(setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes)) < 0){
            std::cout << "[Client] socket option error" << std::endl;
            print_error();
            return -1;
        }
        
        if(connect(client_socket, (struct sockaddr*)&client_addr, sizeof(sockaddr_in)) < 0){
            std::cout << "[Client] connet error" << std::endl;
            print_error();
            return -1;
        }

        return client_socket;
    }

    int Client::reconnect_node(){
        if(client_socket > 0){
            if(shutdown(client_socket, SHUT_RDWR) < 0){
                print_error();
                return -1;
            }
            if(close(client_socket) < 0){
                print_error();
                return -1;
            }
        }
        client_socket = 0;
        return this->connect_node();
    }
};

int test_simple_session(int socket, int server_or_clinet){
    const std::string message = "Hello World";
    std::vector<char> send_message(message.begin(),message.end());
    std::vector<char> recv_message(send_message.size() + 1, 0);
    int sendsize{0}, recvsize{0};

    if(socket <= 0){
        std::cout << "socket is invalid" << std::endl;
        return -1;
    }
    if(server_or_clinet){
        //server
        recvsize = recv(socket, recv_message.data(), recv_message.size() + 1, 0);
        if(recvsize < 0){
            print_error();
            shutdown(socket, SHUT_RDWR);
            close(socket);
            return -1;
        }else if(recvsize == 0){
            std::cout << "[Server] received EOF" << std::endl;
            shutdown(socket, SHUT_RDWR);
            close(socket);
            return -1;
        }else{
            std::cout << "[Server] received: " << recv_message.data() << std::endl;
        }

        //echo back
        sendsize = send(socket, recv_message.data(), recv_message.size() + 1, 0);
        if(sendsize < 0){
            print_error();
            shutdown(socket, SHUT_RDWR);
            close(socket);
            return -1;
        }else if(sendsize != recvsize){
            std::cout << "[Server] send not all data" << std::endl;
            return -1;
        }else{
            std::cout << "[Server] send all data" << std::endl;
            return 0;
        }
    }else{
        //client
        sendsize = send(socket, send_message.data(), recv_message.size() + 1, 0);
        if(sendsize < 0){
            std::cout << "[Client] send data error" << std::endl;
            print_error();
            shutdown(socket, SHUT_RDWR);
            close(socket);
            return -1;
        }else if(sendsize != recv_message.size() + 1){
            std::cout << "[Client] send not all data" << std::endl;
            return -1;
        }else{
            std::cout << "[Client] send all data" << std::endl;
        }
        //recv echo
        recvsize = recv(socket, recv_message.data(), recv_message.size() + 1, 0);
        if(recvsize < 0){
            print_error();
            shutdown(socket, SHUT_RDWR);
            close(socket);
            return -1;
        }else if(recvsize == 0){
            std::cout << "[Client] received EOF" << std::endl;
            shutdown(socket, SHUT_RDWR);
            close(socket);
            return -1;
        }else{
            std::cout << "[Client] received: " << recv_message.data() << std::endl;
            return 0;
        }
    }
}

int test_server_connection(){
    netobj::Server server; 
    if(server.set_host("127.0.0.1", 8080) < 0){
        std::cout << "[Server] set_host error" << std::endl;
    }
    if(server.run(test_simple_session, 1) < 0){
        std::cout << "[Server] run error" << std::endl;
    }
    return 0;
}

int test_client_connection(){
    int rc{0};
    netobj::Client client; 

    if(client.set_host("127.0.0.1", 8080) < 0){
        std::cout << "[Client] set_host error" << std::endl;
        return -1;
    }

    if(client.connect_node() < 0){
        std::cout << "[Client] connect_node error" << std::endl;
        return -1;
    }

    if(test_simple_session(client.client_socket, 0) < 0){
        std::cout << "[Client] test_simple_session error" << std::endl;
        return -1;
    }
    return 0; 
}

#ifdef CONNECTION_TEST
int main(){
    int status{0};
    pid_t server_process{0}, client_process{0}, wpid{0};    

    server_process = fork();
    if(server_process < 0){
        std::cout << "fork fail" << std::endl;
        return -1;
    }else if(server_process == 0){
        if(test_server_connection() < 0){
            std::cout << "[Server] test_server_connection fail" << std::endl;
        }
        return 0;
    }
        
    client_process = fork();
    if(client_process < 0){
        std::cout << "fork fail" << std::endl;
        return -1;
    }else if(client_process == 0){
        if(test_client_connection() < 0){
            std::cout << "[Client] test_client_connection fail" << std::endl;
        }
        return 0;
    }

    if(waitpid(client_process, &status, 0) < 0){
        std::cout << "waitpid client_process fail" << std::endl;
        kill(client_process, SIGKILL);
    }

    kill(server_process, SIGKILL);
}
#endif
