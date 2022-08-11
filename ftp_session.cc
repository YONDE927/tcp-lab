//C++ include
#include <iostream>
#include "transport.h"
#include "connection.h"
#include "ftp_session.h"

//C include
#include <unistd.h>

namespace ftp{
    //ftp_client
    ftp_client::ftp_client(){}
    ftp_client::~ftp_client(){}

    int ftp_client::ftp_client_attr(std::string& path, struct stat& stbuf){
        return 0;
    }
    int ftp_client::ftp_client_readdir(std::string& path, std::map<std::string, struct stat>& dir){
        return 0;
    }
    int ftp_client::ftp_client_open(std::string& path, int mode){
        return 0;
    }
    int ftp_client::ftp_client_close(int fd){
        return 0;
    }
    int ftp_client::ftp_client_read(int fd, int offset, int size, std::vector<char>& buffer){
        return 0;
    }
    int ftp_client::ftp_client_write(int fd, int offset, int size, const std::vector<char>& buffer){
        return 0;
    }

    //ftp_server_worker
    ftp_server_worker::ftp_server_worker(int socket): server_worker(socket){
    }
    ftp_server_worker::~ftp_server_worker(){
    }
}

