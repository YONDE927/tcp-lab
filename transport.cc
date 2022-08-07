//include C library
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>

//include C++ library
#include <iostream>
#include <vector>
#include <memory>
#include <system_error>
#include <thread>
#include <chrono>
#include <cerrno>
#include <csignal>

#include "connection.h"
#include "transport.h"
#include "utils.h"

template<class T>
int Transporter::send_data(shared_ptr<T> buffer, int flag){
    int sendsize{0}, network_size{0};

    network_size = htonl(sizeof(T)); 
    sendsize = send(socket, &network_size, sizeof(int), 0);
    if(sendsize < 0){
        print_error();
        return -1;
    }

    sendsize = send(socket, buffer->data(), sizeof(T), 0);
    if(sendsize < 0){
        print_error();
        return -1;
    }
    return sendsize;
}

//配列などを送信する場合
template<class T>
int Transporter::send_data(shared_ptr<std::vector<T>> buffer, int flag){
    int sendsize{0}, network_size{0};

    network_size = htonl(buffer->size()); 
    sendsize = send(socket, &network_size, sizeof(int), 0);
    if(sendsize < 0){
        print_error();
        return -1;
    }

    sendsize = send(socket, buffer->data(), buffer->size(), 0);
    if(sendsize < 0){
        print_error();
        return -1;
    }
    return sendsize;
}

template<class T>
int Transporter::recv_data(shared_ptr<T> buffer, int flag){
    int recvsize{0}, size{0};
    shared_ptr<Transporter::Datagram> datagram(new Transporter::Datagram);

    recvsize = recv(socket, &size, sizeof(int), 0);
    if(recvsize < 0){
        print_error();
        return -1;
    }
    size = ntohl(size);

    recvsize = recv(socket, buffer->data(), size, 0);
    if(recvsize < 0){
        print_error();
        return -1;
    }
    return recvsize;
}

template<class T>
int Transporter::recv_data(shared_ptr<vector<T>> buffer, int flag){
    int recvsize{0}, size{0};
    shared_ptr<Transporter::Datagram> datagram(new Transporter::Datagram);

    recvsize = recv(socket, &size, sizeof(int), 0);
    if(recvsize < 0){
        print_error();
        return -1; 
    }
    size = ntohl(size);

    buffer->resize(size);
    recvsize = recv(socket, buffer->data(), size, 0);
    if(recvsize < 0){
        print_error();
        return -1; 
    }
    return recvsize;
}

#ifdef TRANSPORT_TEST
int test_simple_transport(int socket, int server_or_clinet){
    const std::string message = "Hello World";
    std::shared_ptr<std::vector<char>> send_message(new std::vector<char>(message.begin(), message.end()));
    std::shared_ptr<std::vector<char>> recv_message(new std::vector<char>(0));
    int sendsize{0}, recvsize{0};

    if(socket <= 0){
        std::cout << "socket is invalid" << std::endl;
        return -1;
    }

    Transporter transporter(socket);

    if(server_or_clinet){
        //server
        recvsize = transporter.recv_data(recv_message, 0);
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
            std::cout << "[Server] received: " << recv_message->data() << std::endl;
        }

        //echo back
        sendsize = transporter.send_data(recv_message, 0);
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
        sendsize = transporter.send_data(send_message, 0);
        if(sendsize < 0){
            std::cout << "[Client] send data error" << std::endl;
            print_error();
            shutdown(socket, SHUT_RDWR);
            close(socket);
            return -1;
        }else if(sendsize != send_message->size()){
            std::cout << "[Client] send not all data" << std::endl;
            return -1;
        }else{
            std::cout << "[Client] send all data" << std::endl;
        }
        //recv echo
        recvsize = transporter.recv_data(recv_message, 0); 
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
            std::cout << "[Client] received: " << recv_message->data() << std::endl;
            return 0;
        }
    }
}

int main(){
    int status{0};
    pid_t server_process{0}, client_process{0}, wpid{0};    

    server_process = fork();
    if(server_process < 0){
        std::cout << "fork fail" << std::endl;
        return -1;
    }else if(server_process == 0){
        netobj::Server server; 
        if(server.set_host("127.0.0.1", 8080) < 0){
            std::cout << "[Server] set_host error" << std::endl;
        }
        if(server.run(test_simple_transport, 1) < 0){
            std::cout << "[Server] run error" << std::endl;
        }
        return 0;
    }
        
    client_process = fork();
    if(client_process < 0){
        std::cout << "fork fail" << std::endl;
        return -1;
    }else if(client_process == 0){
        std::this_thread::sleep_for(std::chrono::seconds(1));
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
            
        if(test_simple_transport(client.client_socket, 0) < 0){
            std::cout << "[Client] test_simple_trasport error" << std::endl;
            return -1;
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
