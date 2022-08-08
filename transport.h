#pragma once
#include <memory>
#include <vector>

using std::vector;

//ここではスタックメモリのデータを送る可能性も考えられるので、生ポインタを使用するのでリークに気をつけること
//構造体を送るときは値とアライメントが全て完結しているものに限る。メンバ変数にポインタが含むような構造体は送らないこと
class Transporter{
    private:
        int socket;
    public:
        Transporter(int _socket):socket(_socket){};
        template<class T>
        int send_data(const T& buffer, int flag);
        template<class T>
        int send_data(const vector<T>& buffer, int flag);
        template<class T>
        int recv_data(T& buffer, int flag);
        template<class T>
        int recv_data(vector<T>& buffer, int flag);
};
