#include "app.h"
#include <stdio.h>
#include <string>
#include <winsock.h>
#include <iostream>
#include <fstream>
#include "Linked_list.h"
#pragma comment(lib, "ws2_32.lib")


int main(){

    Lista<std::fstream> listaArchivos = new Lista<std::fstream>();

    std::string command = ""; 
    // iniciar conexion http

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartup failed.\n";
        system("pause");
        return 1;
    }
    SOCKET Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct hostent* host;
    host = gethostbyname("127.0.0.1");
    SOCKADDR_IN SockAddr;
    SockAddr.sin_port = htons(3003);
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);
    std::cout << "Connecting...\n";
    if (connect(Socket, (SOCKADDR*)(&SockAddr), sizeof(SockAddr)) != 0) {
        std::cout << "Could not connect";
        system("pause");
        return 1;
    }
    std::cout << "Connected.\n";

    while(true){
        std::cout << "got "; 
        std::getline(std::cin, command);
        int contador = 0;
        int pos_in_msg = 0;

        for (int i = 0; i < command.size(); i++) {
            
            if (i == command.size() - 1 && command[i] != ' ') {
                command += ' ';
                contador--;
            }
            else if (command[i] == ' ') {
                contador++;
            }
        }
        std::string* commandArray = new std::string[contador+1];

        try {
            for (int arr_pos = 0; arr_pos < contador+1; arr_pos++) {
                for (; command[pos_in_msg] != ' '; pos_in_msg++) {
                    commandArray[arr_pos] += command[pos_in_msg];
                }pos_in_msg++;
            }
        }
        catch (...) {
            std::cerr << "Sintaxis incorrecta\n";
            return -1;
        }



        if(commandArray[0] == "init"){
            if (commandArray->size() == 2) {
                //Enviar post

                char* header = "POST \init\commandArray[1] HTTP/1.1\r\n";
                send(Socket, header, strlen(header), 0);


                //Crear gotignore
                std::ofstream MyFile("gotignore.txt");
                MyFile.close();
            }
            else {
                std::cout << "Error en el comando" << std::endl;
            }
        }
        else if(commandArray[0] == "help"){
            std::cout << "got init <name>: Instancia un nuevo repositorio en el servidor y lo identifica con el nombre indicado por <name>. \n";
            std::cout << "got add [-A] [name]: Permite agregar todos los archivos que no esten registrados o que tengan nuevos cambios al repositorio. \n";
            std::cout << "Ignora los archivos que esten configurados en .gotignore. El usuario puede indicar cada archivo por agregar, o puede usar el flag -A  \n";
            std::cout << "para agregar todos los archivos relevantes. \n";
            std::cout << "got commit <mensaje>: Envia los archivos agregados y pendientes de commit al servidor. \n";
            std::cout << "Se debe especificar un mensaje a la hora de hacer el commit\n";
            std::cout << "got status <file>: muestra cuales archivos han sido cambiados, agregados o eliminados de acuerdo con el commit anterior.\n"; 
            std::cout << "Si el usuario especifica <file>, muestra el historial de cambios\n";
            std::cout << "got rollback <file> <commit>: Permite regresar un archivo en el tiempo a un commit especifico.\n";
            std::cout << "got reset <file>: Deshace cambios locales para un archivo y lo regresa al ultimo commit.\n";
            std::cout << "got sync <file>: Recupera los cambios para un archivo en el servidor y lo sincroniza con el archivo en el cliente. \n";
        }
        else if(commandArray[0] == "add"){
            // lista de los archivos que se van a agregar
            std::fstream file;
            file.open("file.dat", std::ios::out | std::ios::in);

        }
        else if(commandArray[0] == "commit"){
            // for del tamaño de la lista:
            char* header = "POST \commit HTTP/1.1\r\n"
                "Host: 127.0.0.1\r\n"
                "Content-Type: multipart/form-data; boundary=myboundary\r\n"
                "--myboundary\r\n"
                "Content-Type: application/octet-stream\r\n"
                "Content-Disposition: form-data; name=\"myfile\"; filename=\"myfile.ext\"; modification-date= \"date\";\r\n"
                "Content-Transfer-Encoding: 8bit\r\n"
                "\r\n";
            send(Socket, header, strlen(header), 0);

            // while para recorrer el archivo 
            // send the raw file bytes here...

            char* footer = "\r\n"
                "--myboundary--\r\n";
            send(Socket, footer, strlen(footer), 0);
        }
        else if(commandArray[0] == "status"){
            
        }
        else if(commandArray[0] == "rollback"){
            
        }
        else if(commandArray[0] == "reset"){
            
        }
        else if(commandArray[0] == "sync"){
            
        }
    }
    

    return 0; 
}
