#ifndef SOCKET_H
#define SOCKET_H
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include "garbage.hpp"
#include <climits>
#include "vsptr.hpp"


class Socket_S{
private:
    int listening;
    sockaddr_in hint;
    /**
     * @brief Set the port of the socket
     * 
     * @param port port to bind too
     * @param _ip string of the ip
     */
    void set_port(int port, std::string _ip);
    sockaddr_in client;
    socklen_t client_Size = sizeof(client);
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];
    bool end = false;
    GarbageCollector* gar_col;
    /**
     * @brief give VSPtr a new value
     * @param type type of the data
     * @param val_ptr new value of ptr
     * @param ptr ptr to change
     */
    void give_VSPtr_New_Value(char type, vsptrNT* val_ptr, vsptrNT* ptr);
    /**
     * @brief creates a VSPtr of the specified type
     * @param type type of data
     * @param client client to associate to 
     * @param local_id local id of the pointer
     * @return vsptrNT* 
     */
    vsptrNT* createVSPtr(char type, int client, int local_id);
    /**
     * @brief Get the json in the buffer
     * @param buffer buffer to look from
     * @return std::string json in string form
     */
    std::string get_json(char* buffer);
    /**
     * @brief give VSPtr a new value
     * @param type type of data
     * @param new_val value un a string
     * @param ptr pointer to change the value of
     */
    static void give_VSPtr_New_Value(char type, const std::string& new_val, vsptrNT* ptr);
public:
    /**
     * @brief Socket_S constructor
     */
    Socket_S();
    /**
     * @brief start the socket
     * @param _port port to bind to
     * @param _ip ip to start in
     * @return int identifier if evreything worked correctly
     */
    int start(int _port = 54000, std::string _ip = "0.0.0.0");
    /**
     * @brief mark socket to start listening
     * @return int identifier if evreything worked correctly
     */
    int mark_listening();
    /**
     * @brief get data of the garbage collector by client
     * @param client id of client associated too
     * @return std::string data from the Garbage Collector
     */
    std::string data_GC(int client);
};


#endif // SOCKET_H
