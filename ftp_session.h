//C include
#include <sys/stat.h>

//C++ include
#include <string>
#include <map>
#include <vector>
#include "transport.h"
#include "connection.h"

namespace ftp{
    class ftp_client: protected netobj::client{
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

    class ftp_server_worker: protected netobj::server_worker{
        private:
            ftp_server_worker(int socket);
        public:
            ~ftp_server_worker();
            void run();
            int ftp_server_attr(struct attr_query& query);
            int ftp_server_readdir(struct readdir_query& query);
            int ftp_server_open(struct open_query& query);
            int ftp_server_close(struct close_query& query);
            int ftp_server_read(struct read_query& query);
            int ftp_server_write(struct write_query& query);
    };
}
