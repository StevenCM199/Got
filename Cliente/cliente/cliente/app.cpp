#include "Linked_list.h"
#include <stdio.h>
#include <string>
#include <winsock.h>
#include <fstream>
#include <iostream>
#include <curl/curl.h>
#include <filesystem>
#include <sstream>

namespace fs = std::filesystem;

#pragma comment(lib, "ws2_32.lib")

CURL* curl;
CURLcode res;

int main() {

    lista<std::string>* listaArchivos = new lista<std::string>();
    // std::cout << listaArchivos->size();
    std::string command = "";
    std::string carpetaActual = fs::current_path().string();
    std::string nombreRepo = "";
    //iniciar conexion http

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
    SockAddr.sin_port = htons(3002);
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);
    std::cout << "Connecting...\n";
    if (connect(Socket, (SOCKADDR*)(&SockAddr), sizeof(SockAddr)) != 0) {
        std::cout << "Could not connect";
        system("pause");
        return 1;
    }
    std::cout << "Connected.\n";

    while (true) {
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
        std::string* commandArray = new std::string[contador + 1];

        try {
            for (int arr_pos = 0; arr_pos < contador + 1; arr_pos++) {
                for (; command[pos_in_msg] != ' '; pos_in_msg++) {
                    commandArray[arr_pos] += command[pos_in_msg];
                }pos_in_msg++;
            }
        }
        catch (...) {
            std::cerr << "Sintaxis incorrecta\n";
            return -1;
        }

        if (commandArray[0] == "init") {
            curl_global_init(CURL_GLOBAL_ALL);
            curl = curl_easy_init();

            nombreRepo = commandArray[1];

            if (curl) {
                curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3002/init");             
                std::string buf("nombre=" + nombreRepo);
                //buf += nombreRepo;
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buf.c_str());
                res = curl_easy_perform(curl);

                if (res != CURLE_OK)
                    fprintf(stderr, "curl_easy_perform() failed: %s\n",

                        curl_easy_strerror(res));
                curl_easy_cleanup(curl);
            }

            curl_global_cleanup();

            //Crear gotignore
            std::ofstream MyFile("gotignore.txt");
            MyFile.close();
        }
        else if (commandArray[0] == "help") {
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
        else if (commandArray[0] == "add") {
            
            // agregar todos los archivos
            if (commandArray[1] == "-A") {
                
                for (auto& p : fs::recursive_directory_iterator(carpetaActual)) {
                    if (!p.is_directory()) {
                        listaArchivos->insert(p.path().filename().string());
                    }
                }
            }
            // agrega archivos especificos
            else  if (commandArray[1] != "") {
                for (int i = 1; i < contador+1; i++) {
                    std::string archivo;
                    archivo = commandArray[i];
                    listaArchivos->insert(archivo);
                }
            }
        }
        else if (commandArray[0] == "commit") {
            std::string commitName = commandArray[1];
            std::string completeText = "";
            std::string nameRepo = carpetaActual.substr(carpetaActual.find_last_of("\\") + 1, carpetaActual.size() - 1);

            curl_global_init(CURL_GLOBAL_ALL);
            curl = curl_easy_init();

            if (curl) {
                curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3002/commit");
                std::string buf("commitName=" + commitName + "&" + "nameRepo=" + nameRepo);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buf.c_str());
                res = curl_easy_perform(curl);

                if (res != CURLE_OK)
                    fprintf(stderr, "curl_easy_perform() failed: %s\n",

                        curl_easy_strerror(res));
                curl_easy_cleanup(curl);
            }

            curl_global_cleanup();

            for (int i=0; i < listaArchivos->size(); i++) {
                std::string file = listaArchivos->get_data_by_pos(i);
                std::string absolutePath = fs::absolute(file).string();
                absolutePath = absolutePath.substr(absolutePath.find(nameRepo), absolutePath.size());
                //std::cout << absolutePath;
                std::ifstream readFile(file);
                std::string myText;
                
                while(std::getline(readFile, myText)) {
                    completeText += myText + "\n";
                }

                curl_global_init(CURL_GLOBAL_ALL);
                curl = curl_easy_init();

                if (curl) {
                    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3002/file");
                    std::string buf("commitName=" + commitName + "&" + "file=" + file + "&" + "absolutePath=" +
                        absolutePath + "&" + "nameRepo=" + nameRepo + "&" + "data=" + completeText);
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buf.c_str());
                    res = curl_easy_perform(curl);

                    if (res != CURLE_OK)
                        fprintf(stderr, "curl_easy_perform() failed: %s\n",

                            curl_easy_strerror(res));
                    curl_easy_cleanup(curl);
                }

                curl_global_cleanup();
            }





        }
        else if (commandArray[0] == "status") {
            curl_global_init(CURL_GLOBAL_ALL);
            curl = curl_easy_init();
            if (contador + 1 == 1) {
                if (curl) {
                    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3002/status");
                    /*std::string buf("commitName=" + commitName + "&" + "file=" + file + "&" + "absolutePath=" +
                        absolutePath + "&" + "nameRepo=" + nameRepo + "&" + "data=" + completeText);
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buf.c_str());*/
                    res = curl_easy_perform(curl);

                    if (res != CURLE_OK)
                        fprintf(stderr, "curl_easy_perform() failed: %s\n",

                            curl_easy_strerror(res));
                    curl_easy_cleanup(curl);
                }
            }
            else if (contador + 1 == 2) {
                //historial cambios
                //commandArray[2]
                if (curl) {
                    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3002/status");
                    /*std::string buf("commitName=" + commitName + "&" + "file=" + file + "&" + "absolutePath=" +
                        absolutePath + "&" + "nameRepo=" + nameRepo + "&" + "data=" + completeText);
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buf.c_str());*/
                    res = curl_easy_perform(curl);

                    if (res != CURLE_OK)
                        fprintf(stderr, "curl_easy_perform() failed: %s\n",

                            curl_easy_strerror(res));
                    curl_easy_cleanup(curl);
                }
            }

            curl_global_cleanup();

        }
        else if (commandArray[0] == "rollback") {
            curl_global_init(CURL_GLOBAL_ALL);
            curl = curl_easy_init();
            if (contador + 1 == 3) {
                //enviar nombre archivo commandArray[1] y del commit
                // recibir el archivo de ese commit
                // write archivo local
                if (curl) {
                    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3002/rollback");
                    /*std::string buf("commitName=" + commitName + "&" + "file=" + file + "&" + "absolutePath=" +
                        absolutePath + "&" + "nameRepo=" + nameRepo + "&" + "data=" + completeText);
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buf.c_str());*/
                    res = curl_easy_perform(curl);

                    if (res != CURLE_OK)
                        fprintf(stderr, "curl_easy_perform() failed: %s\n",

                            curl_easy_strerror(res));
                    curl_easy_cleanup(curl);
                }
            }

            curl_global_cleanup();
        }
        else if (commandArray[0] == "reset") {
            curl_global_init(CURL_GLOBAL_ALL);
            curl = curl_easy_init();
            if (contador + 1 == 2) {
            // recibe nombre del archivo commandArray[1]
            // escribe el ultimo commit
                if (curl) {
                    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3002/reset");
                    /*std::string buf("commitName=" + commitName + "&" + "file=" + file + "&" + "absolutePath=" +
                        absolutePath + "&" + "nameRepo=" + nameRepo + "&" + "data=" + completeText);
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buf.c_str());*/
                    res = curl_easy_perform(curl);

                    if (res != CURLE_OK)
                        fprintf(stderr, "curl_easy_perform() failed: %s\n",

                            curl_easy_strerror(res));
                    curl_easy_cleanup(curl);
                }
            }

            curl_global_cleanup();
        }
        else if (commandArray[0] == "sync") {
            curl_global_init(CURL_GLOBAL_ALL);
            curl = curl_easy_init();
            // Recibe el ultimo commit del file especificado
            // Print ambas versiones
            // Escriba 1 para conservar el del servidor, 2 para conservar version local

            if (curl) {
                curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3002/sync");
                /*std::string buf("commitName=" + commitName + "&" + "file=" + file + "&" + "absolutePath=" +
                    absolutePath + "&" + "nameRepo=" + nameRepo + "&" + "data=" + completeText);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buf.c_str());*/
                res = curl_easy_perform(curl);

                if (res != CURLE_OK)
                    fprintf(stderr, "curl_easy_perform() failed: %s\n",

                        curl_easy_strerror(res));
                curl_easy_cleanup(curl);
            }

            curl_global_cleanup();
        }
    }


    return 0;
}