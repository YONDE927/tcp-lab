#pragma once
#include <memory>
#include <vector>

using std::shared_ptr, std::vector;

class Transporter{
    private:
        int socket;
    public:
        struct Datagram {
            int size;
            shared_ptr<char> buffer;
        };
        Transporter(int _socket):socket(_socket){};
        template<class T>
        int send_data(shared_ptr<T> buffer, int flag);
        template<class T>
        int send_data(shared_ptr<vector<T>> buffer, int flag);
        template<class T>
        int recv_data(shared_ptr<T> buffer, int flag);
        template<class T>
        int recv_data(shared_ptr<vector<T>> buffer, int flag);
};
