cmake_minimum_required(VERSION 3.18.0)

project(tcp-lab C CXX)

set(common_srcs "timer.cc" "utils.cc")
set(srcs "session.cc" "connection.cc")

add_library(common STATIC ${common_srcs})
add_library(ftp STATIC ${srcs})


