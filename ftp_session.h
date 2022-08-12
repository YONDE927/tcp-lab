#pragma once
//C include
#include <sys/stat.h>

//C++ include
#include <string>
#include <map>
#include <vector>
#include "transport.h"
#include "connection.h"

namespace ftp{
    class ftp_client: public netobj::client{
        public:
            ftp_client();
            ~ftp_client();
            int ftp_client_attr(std::string& path, struct stat& stbuf);
            int ftp_client_readdir(std::string& path, std::map<std::string, struct stat>& dir);
            int ftp_client_open(std::string& path, int mode);
            int ftp_client_close(int fd);
            int ftp_client_read(int fd, int offset, int size, std::vector<char>& buffer);
            int ftp_client_write(int fd, int offset, int size, const std::vector<char>& buffer);
    };

    class ftp_server: public netobj::server{
        private:
            ftp_server(int socket);
        public:
            static void run(int socket);
            int ftp_server_attr();
            int ftp_server_readdir();
            int ftp_server_open();
            int ftp_server_close();
            int ftp_server_read();
            int ftp_server_write();
    };
}
