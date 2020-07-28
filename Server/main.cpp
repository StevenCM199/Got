#include <iostream>
#include "vsptr.hpp"
#include "garbage.hpp"
#include <thread>
#include "pthread.h"
#include <unistd.h>
#include <fstream>
#include "socket_S.h"
#include "md5.h"

bool is_not_finished = true;
std::thread delete_t(){
    GarbageCollector* gc = GarbageCollector::getGarbageCollector();
    while(is_not_finished){
        gc->delete_pkgs();
        sleep(10);
    }
};
int main(){
    GarbageCollector::getGarbageCollector();
    std::thread p(delete_t);
    Socket_S socket = Socket_S();
    socket.start();
    is_not_finished = false;
    p.join();
    return 0;
}