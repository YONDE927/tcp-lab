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

using std::vector;

//構造体を送るときは値とアライメントが全て完結しているものに限る。メンバ変数にポインタが含むような構造体は送らないこと
template<class T>
int Transporter::send_data(const T& buffer, int flag){
    int send_size{0}, network_size{0};

    send_size = send(socket, &buffer, sizeof(T), 0);
    if(send_size < 0){
        print_error();
        return -1;
    }
    return send_size;
}

//配列などを送信する場合
template<class T>
int Transporter::send_data(const vector<T>& buffer, int flag){
    int send_size{0}, buffer_size{0}, network_size{0};

    buffer_size = buffer.size() * sizeof(T);
    network_size = htonl(buffer_size); 
    send_size = send(socket, &network_size, sizeof(int), 0);
    if(send_size < 0){
        print_error();
        return -1;
    }

    send_size = send(socket, buffer.data(), buffer_size, 0);
    if(send_size < 0){
        print_error();
        return -1;
    }
    return send_size;
}

template<class T>
int Transporter::recv_data(T& buffer, int flag){
    int recv_size{0}, buffer_size{0};

    recv_size = recv(socket, &buffer, sizeof(T), 0);
    if(recv_size < 0){
        print_error();
        return -1;
    }
    return recv_size;
}

template<class T>
int Transporter::recv_data(vector<T>& buffer, int flag){
    int recv_size{0}, buffer_size{0};

    recv_size = recv(socket, &buffer_size, sizeof(int), 0);
    if(recv_size < 0){
        print_error();
        return -1; 
    }
    buffer_size = ntohl(buffer_size);

    buffer.resize(buffer_size / sizeof(T));
    recv_size = recv(socket, buffer.data(), buffer_size, 0);
    if(recv_size < 0){
        print_error();
        return -1; 
    }
    return recv_size;
}

#ifdef TRANSPORT_TEST
struct Sample_Struct{
    int x;
    double y;
    char z[256];
};

int test_simple_transport(int socket, int server_or_clinet){
    const std::string message = "Hello World";
    const Sample_Struct send_struct = {5, 10, "Hello World"};
    Sample_Struct recv_struct;
    std::vector<char> send_message(message.begin(), message.end());
    std::vector<char> recv_message(0);
    int send_size{0}, recv_size{0};

    if(socket <= 0){
        std::cout << "socket is invalid" << std::endl;
        return -1;
    }

    Transporter transporter(socket);

    if(server_or_clinet){
        //server
        recv_size = transporter.recv_data(recv_message, 0);
        if(recv_size < 0){
            print_error();
            shutdown(socket, SHUT_RDWR);
            close(socket);
            return -1;
        }else if(recv_size == 0){
            std::cout << "[Server] received EOF" << std::endl;
            shutdown(socket, SHUT_RDWR);
            close(socket);
            return -1;
        }else{
            std::cout << "[Server] received: " << recv_message.data() << std::endl;
        }

        //echo back
        send_size = transporter.send_data(recv_message, 0);
        if(send_size < 0){
            print_error();
            shutdown(socket, SHUT_RDWR);
            close(socket);
            return -1;
        }else if(send_size != recv_size){
            std::cout << "[Server] send not all data" << std::endl;
            return -1;
        }else{
            std::cout << "[Server] send all data" << std::endl;
        }

        //recv struct
        recv_size = transporter.recv_data(recv_struct, 0);
        if(recv_size < 0){
            print_error();
            shutdown(socket, SHUT_RDWR);
            close(socket);
            return -1;
        }else if(recv_size == 0){
            std::cout << "[Server] received EOF" << std::endl;
            shutdown(socket, SHUT_RDWR);
            close(socket);
            return -1;
        }else{
            std::cout << "[Server] received: " << recv_struct.x << recv_struct.y << recv_struct.z << std::endl;
        }

        //echo back
        send_size = transporter.send_data(recv_struct, 0);
        if(send_size < 0){
            print_error();
            shutdown(socket, SHUT_RDWR);
            close(socket);
            return -1;
        }else if(send_size != sizeof(Sample_Struct)){
            std::cout << "[Server] send not all data" << std::endl;
            return -1;
        }else{
            std::cout << "[Server] send all data" << std::endl;
        }
    }else{
        //client
        send_size = transporter.send_data(send_message, 0);
        if(send_size < 0){
            std::cout << "[Client] send data error" << std::endl;
            print_error();
            shutdown(socket, SHUT_RDWR);
            close(socket);
            return -1;
        }else if(send_size != send_message.size()){
            std::cout << "[Client] send not all data char" << std::endl;
            return -1;
        }else{
            std::cout << "[Client] send all data" << std::endl;
        }
        //recv echo
        recv_size = transporter.recv_data(recv_message, 0); 
        if(recv_size < 0){
            print_error();
            shutdown(socket, SHUT_RDWR);
            close(socket);
            return -1;
        }else if(recv_size == 0){
            std::cout << "[Client] received EOF" << std::endl;
            shutdown(socket, SHUT_RDWR);
            close(socket);
            return -1;
        }else{
            std::cout << "[Client] received: " << recv_message.data() << std::endl;
        }

        //send struct
        send_size = transporter.send_data(send_struct, 0);
        if(send_size < 0){
            std::cout << "[Client] send data error" << std::endl;
            print_error();
            shutdown(socket, SHUT_RDWR);
            close(socket);
            return -1;
        }else if(send_size != sizeof(Sample_Struct)){
            std::cout << "[Client] send not all data" << std::endl;
            return -1;
        }else{
            std::cout << "[Client] send all data" << std::endl;
        }
        //recv echo
        recv_size = transporter.recv_data(recv_struct, 0); 
        if(recv_size < 0){
            print_error();
            shutdown(socket, SHUT_RDWR);
            close(socket);
            return -1;
        }else if(recv_size == 0){
            std::cout << "[Client] received EOF" << std::endl;
            shutdown(socket, SHUT_RDWR);
            close(socket);
            return -1;
        }else{
            std::cout << "[Client] received: " << recv_struct.x << recv_struct.y << recv_struct.z << std::endl;
        }
    }
    return 0;
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
