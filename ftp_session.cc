//C++ include
#include <iostream>
#include "ftp_session.h"
#include "ftp_query.h"
#include "utils.h"

//C include
#include <unistd.h>
#include <string.h>

namespace ftp{
    //ftp_client
    ftp_client::ftp_client(){}
    ftp_client::~ftp_client(){}

    int ftp_client::ftp_client_attr(std::string& path, struct stat& stbuf){
        op_code type = ATTR;
        attr_query query{0};
        attr_reply reply{0};
        
        if(transporter.send_data(type, 0) < 0){
            std::cout << "[client] send opcode fail" << std::endl;
            return -1;
        }

        if(path.size() > 255){
            std::cout << "[client] path size is too large" << std::endl;
            return -1;
        }

        strcpy(query.path, path.c_str());

        if(transporter.send_data(query,0) < 0){
            std::cout << "[client] send attr_query fail" << std::endl;
            return -1;
        }

        if(transporter.recv_data(reply, 0) < 0){
            std::cout << "[client] recv attr_reply fail" << std::endl;
            return -1;
        }

        if(reply.error != 0){
            return -reply.error;
        }

        stbuf = reply.attr;
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
    ftp_server::ftp_server(int socket): netobj::server(socket){}

    void ftp_server::run(int socket){
        int iserror{1};
        op_code type{NONE};

        ftp_server server(socket);

        while(iserror){
            type = NONE;
            if(server.transporter.recv_data(type, 0) < 0){
                std::cout << "[server] recv op_code fail" << std::endl;
                return;
            }

            switch(type){
                case ATTR:
                    if(server.ftp_server_attr() < 0){
                        std::cout << "[server] recv ftp_server_attr fail" << std::endl;
                        iserror = 0;
                    }
                    break;
                default:
                    break;
            }
        }
    }

    int ftp_server::ftp_server_attr(){
        attr_query query{0};
        attr_reply reply{0};

        if(transporter.recv_data(query, 0) < 0){
            std::cout << "[server] recv attr_query fail" << std::endl;
            return -1;
        }

        if(stat(query.path, &reply.attr) < 0){
            print_error(__func__);
            reply.error = errno;
        }

        if(transporter.send_data(reply, 0) < 0){
            std::cout << "[server] send attr_reply fail" << std::endl;
            return -1;
        }
        return 0;
    }
}

//#define FTP_SESSION_TEST
#ifdef FTP_SESSION_TEST
#include <thread>
#include <chrono>
#include <csignal>
#include <sys/wait.h>
int main(){
    int status{0};
    pid_t server_process{0}, client_process{0}, wpid{0};    
    std::string sample_path = "sample";

    server_process = fork();
    if(server_process < 0){
        std::cout << "fork fail" << std::endl;
        return -1;
    }else if(server_process == 0){
        netobj::server_master server_master; 
        if(server_master.set_host("127.0.0.1", 8080) < 0){
            std::cout << "[server] set_host error" << std::endl;
        }
        if(server_master.run(ftp::ftp_server::run) < 0){
            std::cout << "[server] run error" << std::endl;
        }
        return 0;
    }
        
    client_process = fork();
    if(client_process < 0){
        std::cout << "fork fail" << std::endl;
        return -1;
    }else if(client_process == 0){
        std::this_thread::sleep_for(std::chrono::seconds(1));
        ftp::ftp_client client; 

        if(client.set_host("127.0.0.1", 8080) < 0){
            std::cout << "[client] set_host error" << std::endl;
            return -1;
        }

        if(client.connect_server() < 0){
            std::cout << "[client] connect_node error" << std::endl;
            return -1;
        }
        
        struct stat attr{0};
        if(client.ftp_client_attr(sample_path, attr) < 0){
            std::cout << "[client] ftp_client_attr error" << std::endl;
            return -1;
        }
        ftp::print_stat(attr);
        return 0;
    }

    if(waitpid(client_process, &status, 0) < 0){
        std::cout << "waitpid client_process fail" << std::endl;
        kill(client_process, SIGKILL);
    }

    kill(server_process, SIGKILL);
    return 0;
}
#endif
