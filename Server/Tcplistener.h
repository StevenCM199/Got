#pragma once
#include <WS2tcpip.h>
#include <string>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")

/**
 * @brief clase para conectar los sockets
 * 
 */
class Tcplistener;
//Callback to data received
/**
 * @brief funcion que se encarga de manejar los mensajes
 * 
 */
typedef void(*MessageRecievedHandler)(Tcplistener* listener, int sockid, std::string msg);
class Tcplistener{
public:
	/**
	 * @brief Construct a new Tcplistener object
	 * 
	 * @param _port puerto para hacer bind
	 * @param _ip ip del servidor
	 * @param handler funcion a encargarse del mensaje
	 */
	Tcplistener(int _port, std::string _ip, MessageRecievedHandler handler);
	/**
	 * @brief Destroy the Tcplistener object
	 * 
	 */
	~Tcplistener();
	/**
	 * @brief envia un mensaje al socket cliente
	 * 
	 * @param clientSocket cliente a enviar
	 * @param msg mensaje a enviar
	 */
	void Send(int clientSocket, std::string msg);
	/**
	 * @brief iniciar el servidor
	 * 
	 * @return true se logra crear el socket
	 * @return false no se creo el socket
	 */
	bool Init();
	/**
	 * @brief ejecutar el servidor
	 * 
	 */
	void Run();
	/**
	 * @brief limpiar el socket 
	 */
	void cleanup();
private:
	std::string my_Ip_Adrr;
	int my_Port;
	MessageRecievedHandler msg_Rec;
	/**
	 * @brief Create a socket object
	 * 
	 * @return SOCKET creado
	 */
	SOCKET create_socket();
	/**
	 * @brief esperar al socket cliente
	 * 
	 * @param listening socket que escucha
	 * @return SOCKET socket cliente
	 */
	SOCKET wait_For_Socket(SOCKET listening);
};

