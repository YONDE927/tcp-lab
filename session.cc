//include C library
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>

//include C++ library
#include <iostream>
#include <memory>
#include <system_error>
#include <cerrno>

#include "session.h"
#include "utils.h"

int Session::send_datagram(shared_ptr<Session::Datagram> buffer, int flag){
    int sendsize{0}, network_size{0};

    network_size = htonl(buffer->size); 
    sendsize = send(socket, &network_size, sizeof(int), 0);
    if(sendsize < 0){
        const std::error_category& cat = std::generic_category();
        std::cout << cat.message(errno) << std::endl;
        return -1;
    }

    sendsize = send(socket, buffer.get(), sizeof(int), 0);
    if(sendsize < 0){
        const std::error_category& cat = std::generic_category();
        std::cout << cat.message(errno) << std::endl;
        return -1;
    }
    return sendsize;
}

shared_ptr<Session::Datagram> Session::recv_datagram(int flag){
    int recvsize{0}, size{0};
    shared_ptr<Session::Datagram> datagram(new Session::Datagram);

    recvsize = recv(socket, &size, sizeof(int), 0);
    if(recvsize < 0){
        const std::error_category& cat = std::generic_category();
        std::cout << cat.message(errno) << std::endl;
        return datagram;
    }
    size = ntohl(size);

    shared_ptr<char> buffer(new char[size]);
    recvsize = recv(socket, buffer.get(), size, 0);
    if(recvsize < 0){
        const std::error_category& cat = std::generic_category();
        std::cout << cat.message(errno) << std::endl;
        return datagram;
    }
    return datagram;
}

int test_main(){
    int rc{0}, tmp{0},listenfd{0};
    int yes = 1;
    short port = 50080;
    struct sockaddr_in servaddr{0};

    //ソケット生成
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd < 0){
        print_error();
        std::exit(0);
    }

    if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes)) < 0){
        print_error();
        std::exit(0);
    }

    //アドレス生成
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);


    //ポート接続
    rc = bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if(rc < 0){
        print_error();
        std::exit(0);
        return -1;
    }

    //接続待ち
    rc = listen(listenfd, 5);
    if(rc < 0){
        print_error();
        std::exit(0);
        return -1;
    }

    //クライアント接続
    struct sockaddr_in cliaddr;
    int clientfd{0}, recvsize{0}, sendsize{0};

    tmp = sizeof(cliaddr);
    clientfd = accept(listenfd, (struct sockaddr*)&cliaddr, (socklen_t*)&tmp);
    if(clientfd < 0){
        print_error();
        std::exit(0);
    }

    //ソケット設定
    if(setsockopt(clientfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes)) < 0){
        print_error();
        std::exit(0);
    }

    //受信とエコー送信
    char recvbuffer[1024] = {0};
    while((recvsize = recv(clientfd, recvbuffer, 1024, 0)) > 0){
        std::cout << "received seg:" << std::endl;
        std::cout << recvbuffer << std::endl;

        //エコー送信
        sendsize = send(clientfd, recvbuffer, 1024, 0);
        if(sendsize < 0){
            print_error();
            std::exit(0);
        }

        bzero(recvbuffer, 1024);
    }

    if(recvsize == 0){
        std::cout << "received EOF" << std::endl;
    }

    if(recvsize < 0){
        print_error();
        std::exit(0);
    }

    shutdown(clientfd, SHUT_RDWR);
    close(clientfd);
    close(listenfd);
    return 0;
}
