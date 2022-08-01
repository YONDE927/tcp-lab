#pragma once
#include <memory>

using std::shared_ptr;

void print_error();

class Session{
    private:
        int socket;
    private:
        enum Flag {
            CLIENT,
            SERVER,
        };

        struct Datagram {
            int size;
            shared_ptr<char> buffer;
        };

        //For Server and Client 
        Session(int _socket):socket(_socket){};
        int send_datagram(shared_ptr<Datagram> buffer, int flag);
        shared_ptr<Datagram> recv_datagram(int flag);
};
