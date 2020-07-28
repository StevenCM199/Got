cmake_minimum_required(VERSION 3.17)
project(Server)

set(CMAKE_CXX_STANDARD 14)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include_directories(.)

add_executable(Server
        garbage.cpp
        garbage.hpp
        Linked_List.hpp
        main.cpp
        md5.cpp
        md5.h
        socket_S.cpp
        socket_S.h
        vsptr.hpp
        vsptrNT.hpp)
target_link_libraries(Server Threads::Threads)
find_package(Doxygen REQUIRED)
doxygen_add_docs(docs . ALL)