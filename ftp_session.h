#pragma once

//include C++
#include <iostream>
#include <climits>

namespace ftp_session{
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
    }
    struct readdir_query{
        char path[PATH_MAX];
    }
    struct open_query{
        char path[PATH_MAX];
        int mode;
    }
    struct close_query{
        int fd;
    }
    struct read_query{
        int fd;
        int offset;
        int size;
    }
    struct write_query{
        int fd;
        int offset;
        int size;
    }
}
