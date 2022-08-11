#pragma once

//include C
#include <sys/stat.h>

//include C++
#include <iostream>
#include <climits>
#include <cerrno>

namespace ftp{
    enum op_code{
        NONE,
        ATTR,
        READDIR,
        OPEN,
        CLOSE,
        READ,
        WRITE
    };
    struct attr_query{
        char path[PATH_MAX];
        void ntoh();
        void hton();
    };
    struct attr_reply{
        int error;
        struct stat attr;        
        void ntoh();
        void hton();
    };
    struct readdir_query{
        char path[PATH_MAX];
        void ntoh();
        void hton();
    };
    struct readdir_reply_node{
        int error;
        char path[PATH_MAX];
        struct stat attr;        
        void ntoh();
        void hton();
    };
    struct open_query{
        char path[PATH_MAX];
        int mode;
        void ntoh();
        void hton();
    };
    struct open_reply{
        int error;
        int fd;
        void ntoh();
        void hton();
    };
    struct close_query{
        int fd;
        void ntoh();
        void hton();
    };
    struct close_reply{
        int error;
        void ntoh();
        void hton();
    };
    struct read_query{
        int fd;
        int offset;
        int size;
        void ntoh();
        void hton();
    };
    struct read_reply{
        int error;
        int size;
        void ntoh();
        void hton();
    };
    struct write_query{
        int fd;
        int offset;
        int size;
        void ntoh();
        void hton();
    };
    struct write_reply{
        int error;
        void ntoh();
        void hton();
    };
}
