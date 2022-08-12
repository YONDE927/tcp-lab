//C include
#include <endian.h>

#include "ftp_query.h"

namespace ftp{
    void print_stat(struct stat& attr){
        std::cout << attr.st_size << " "
        << attr.st_size << " " 
        << attr.st_uid << " " 
        << attr.st_mtime << std::endl;
    }
    void htonstat(struct stat& attr){
        attr.st_size = htobe32(attr.st_size);
        attr.st_mode = htobe32(attr.st_mode);
        attr.st_gid = htobe32(attr.st_gid);
        attr.st_uid = htobe32(attr.st_uid);
        attr.st_blksize = htobe64(attr.st_blksize);
        attr.st_blocks = htobe64(attr.st_blocks);
        attr.st_ino = htobe64(attr.st_ino);
        attr.st_dev = htobe64(attr.st_dev);
        attr.st_rdev = htobe64(attr.st_rdev);
        attr.st_nlink = htobe64(attr.st_nlink);
        attr.st_mtime = htobe64(attr.st_mtime);
        attr.st_atime = htobe64(attr.st_atime);
        attr.st_ctime = htobe64(attr.st_ctime);
    }
    void ntohstat(struct stat& attr){
        attr.st_size = be32toh(attr.st_size);
        attr.st_mode = be32toh(attr.st_mode);
        attr.st_gid = be32toh(attr.st_gid);
        attr.st_uid = be32toh(attr.st_uid);
        attr.st_blksize = be64toh(attr.st_blksize);
        attr.st_blocks = be64toh(attr.st_blocks);
        attr.st_ino = be64toh(attr.st_ino);
        attr.st_dev = be64toh(attr.st_dev);
        attr.st_rdev = be64toh(attr.st_rdev);
        attr.st_nlink = be64toh(attr.st_nlink);
        attr.st_mtime = be64toh(attr.st_mtime);
        attr.st_atime = be64toh(attr.st_atime);
        attr.st_ctime = be64toh(attr.st_ctime);
    }
    void attr_query::ntoh(){
        return;
    }
    void attr_query::hton(){
        return;
    }
    void attr_reply::ntoh(){
        error = be32toh(error);
        ntohstat(attr);
        return;
    }
    void attr_reply::hton(){
        error = htobe32(error);
        htonstat(attr);
        return;
    }
    void readdir_query::ntoh(){
        return;
    }
    void readdir_query::hton(){
        return;
    }
    void readdir_reply_node::ntoh(){
        error = be32toh(error);
        ntohstat(attr);
        return;
    }
    void readdir_reply_node::hton(){
        error = htobe32(error);
        htonstat(attr);
        return;
    }
    void open_query::ntoh(){
        mode = be32toh(mode);
        return;
    }
    void open_query::hton(){
        mode = htobe32(mode);
        return;
    }
    void open_reply::ntoh(){
        error = be32toh(error);
        fd = be32toh(fd);
        return;
    }
    void open_reply::hton(){
        error = htobe32(error);
        fd = htobe32(fd);
        return;
    }
    void close_query::ntoh(){
        fd = be32toh(fd);
        return;
    }
    void close_query::hton(){
        fd = htobe32(fd);
        return;
    }
    void close_reply::ntoh(){
        error = be32toh(error);
        return;
    }
    void close_reply::hton(){
        error = htobe32(error);
        return;
    }
    void read_query::ntoh(){
        fd = be32toh(fd);
        offset = be32toh(offset);
        size = be32toh(size);
        return;
    }
    void read_query::hton(){
        fd = htobe32(fd);
        offset = htobe32(offset);
        size = htobe32(size);
        return;
    }
    void read_reply::ntoh(){
        error = be32toh(error);
        return;
    }
    void read_reply::hton(){
        error = htobe32(error);
        return;
    }
    void write_query::ntoh(){
        fd = be32toh(fd);
        offset = be32toh(offset);
        size = be32toh(size);
        return;
    }
    void write_query::hton(){
        fd = htobe32(fd);
        offset = htobe32(offset);
        size = htobe32(size);
        return;
    }
    void write_reply::ntoh(){
        error = be32toh(error);
        return;
    }
    void write_reply::hton(){
        error = htobe32(error);
        return;
    }
}

#ifdef FTP_QUERY_TEST
#include <unistd.h>
#include <string.h>
#include <iostream>
int test_ftp_query(){
    //attr
    ftp::attr_query attr_q1{"file"};
    ftp::attr_query attr_q2 = attr_q1;
    ftp::attr_reply attr_r1{0};
    ftp::attr_reply attr_r2{0};
    if(stat("sample", &attr_r1.attr) < 0){
        attr_r1.error = errno;
    }
    attr_r2 = attr_r1;

    attr_q1.hton();
    attr_q1.ntoh();
    if(memcmp(&attr_q1, &attr_q2, sizeof(ftp::attr_query)) != 0){
        std::cout << "attr_query alignment error" << std::endl;
        return -1;
    }else{
        std::cout << "attr_query alignment clear" << std::endl;
    }
    attr_r1.hton();
    attr_r1.ntoh();
    if(memcmp(&attr_r1, &attr_r2, sizeof(ftp::attr_reply)) != 0){
        std::cout << "attr_reply alignment error" << std::endl;
        return -1;
    }else{
        std::cout << "attr_reply alignment clear" << std::endl;
    }
    return 0;
}

int main(){
    if(test_ftp_query() < 0){
        std::cout << "query test fail" << std::endl;
    }else{
        std::cout << "query test clear" << std::endl;
    }
    return 0;
}
#endif
