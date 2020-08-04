// Server Unity.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Tcplistener.h"
#include "Linked_list.h"
#include <stdlib.h>

//function pointer is declared
void Listener_MesssageRec(Tcplistener* listener, int client, std::string msg);

int main(){
    Tcplistener server(54100, "127.0.0.1", Listener_MesssageRec);
    if (server.Init()) {
        server.Run();
    }
}

void Listener_MesssageRec(Tcplistener* listener, int client, std::string msg) {
    
};
