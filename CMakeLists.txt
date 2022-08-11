cmake_minimum_required(VERSION 3.18.0)

project(tcp-lab C CXX)

set(common_srcs "timer.cc" "utils.cc")
set(srcs "transport.cc" "connection.cc" "ftp_query.cc")

find_package(Threads REQUIRED)

add_library(common STATIC ${common_srcs})
#add_library(ftp STATIC ${srcs})

add_executable(ftp ${srcs})
#target_compile_definitions(ftp PRIVATE CONNECTION_TEST)
#target_compile_definitions(ftp PRIVATE TRANSPORT_TEST)
target_compile_definitions(ftp PRIVATE FTP_QUERY_TEST)
target_link_libraries(ftp common)
target_link_libraries(ftp Threads::Threads)

