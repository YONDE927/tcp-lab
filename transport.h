//Transporterのマニュアル
//
//伝送できる構造体
//基本型のみをメンバ変数に持つ・外部参照の構造体は伝送しない
//可変長データ（配列）を伝送するときは、別途std::vector型に変換して送信する。vectorの要素型も上のルールと同じ条件に従う
//
//ホスト間の同意
//送受信間で伝送する構造体は一致していなければいけない
//配列の場合も要素型が一致すること
//
//ソケットエラー処理の責任
//これはソケット上のデータ通信を簡略するのみであり、ソケットエラーは呼び出し側で処理することを想定している。

#pragma once
#include <memory>
#include <vector>

using std::vector;

//ここではスタックメモリのデータを送る可能性も考えられるので、生ポインタを使用するのでリークに気をつけること
//構造体を送るときは値とアライメントが全て完結しているものに限る。メンバ変数にポインタが含むような構造体は送らないこと
namespace transport{
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
}
