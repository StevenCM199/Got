#include "Linked_list.h"
#include <stdio.h>
#include <string>
#include <winsock.h>
#include <fstream>
#include <iostream>
#include <curl/curl.h>
#include <filesystem>
#include <sstream>
#include <Windows.h>
#pragma warning(disable : 4996)

namespace fs = std::filesystem;

#pragma comment(lib, "ws2_32.lib")

/***
 * @brief Funcion utilizada para escribir HTTP response en el archivo
 */
static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream)
{
    size_t written = fwrite(ptr, size, nmemb, (FILE*)stream);
    return written;
}

/***
 * @brief Utilizada para escribir en el archivo al hacer sync
 */
size_t CurlWrite_CallbackFunc_StdString(void* contents, size_t size, size_t nmemb, std::string* s)
{
    size_t newLength = size * nmemb;
    try
    {
        s->append((char*)contents, newLength);
    }
    catch (std::bad_alloc& e)
    {
        //handle memory problem
        return 0;
    }
    return newLength;
}

int main() {

    CURL* curl;
    CURLcode res;
    FILE* pagefile;

    lista<std::string>* listaArchivos = new lista<std::string>();
    // std::cout << listaArchivos->size();
    std::string command = "";
    std::string carpetaActual = fs::current_path().string();
    std::string nombreRepo = "";
    std::string serverSync = "";

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
    std::cout << "Ingrese su nombre de usuario: ";
    std::string username;
    std::getline(std::cin, username);


    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3002/user");
        std::string buf("nombre=" + username);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buf.c_str());
        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",

                curl_easy_strerror(res));
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

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
            std::cout << "Archivos pendientes de commit:" << std::endl;
            // agregar todos los archivos
            if (commandArray[1] == "-A") {
                
                for (auto& p : fs::recursive_directory_iterator(carpetaActual)) {
                    if (!p.is_directory()) {
                        std::cout << p.path().generic_string() << std::endl;
                        listaArchivos->insert(p.path().generic_string());
                    }
                }
            }
            // agrega archivos especificos
            else  if (commandArray[1] != "") {
                for (int i = 1; i < contador+1; i++) {
                    std::string archivo;
                    archivo = commandArray[i]; // subcarpeta/sub/sub/file.txt
                    std::string absPath = fs::absolute(archivo).string();
                    std::cout << absPath << std::endl;
                    listaArchivos->insert(absPath);
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

            Sleep(5);

            for (int i=0; i < listaArchivos->size(); i++) {
                std::string absolutePath = listaArchivos->get_data_by_pos(i); //file.txt
                fs::path tmpPath = absolutePath;
                std::string file = tmpPath.filename().string(); //Cambiar a nombre de archivo
                absolutePath = absolutePath.substr(absolutePath.find(nameRepo), absolutePath.size());
                std::string relativePath = fs::path(absolutePath).generic_string();
                std::ifstream readFile;
                std::string myText;
                
                readFile.open(listaArchivos->get_data_by_pos(i));

                if (!readFile) {
                    std::cout << "Unable to open file";
                    exit(1); // terminate with error
                }

                while(std::getline(readFile, myText)) {
                    completeText += myText + "\n";
                }

                readFile.close();

                curl_global_init(CURL_GLOBAL_ALL);
                curl = curl_easy_init();

                if (curl) {
                    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3002/file");
                    std::string buf("commitName=" + commitName + "&" + "file=" + file + "&" + "absolutePath=" +
                        relativePath + "&" + "nameRepo=" + nameRepo + "&" + "data=" + completeText);
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buf.c_str());
                    res = curl_easy_perform(curl);

                    if (res != CURLE_OK)
                        fprintf(stderr, "curl_easy_perform() failed: %s\n",

                            curl_easy_strerror(res));
                    curl_easy_cleanup(curl);
                }

                curl_global_cleanup();
            }

            listaArchivos->delete_list();
        }
        else if (commandArray[0] == "status") {
            std::string nameRepo = carpetaActual.substr(carpetaActual.find_last_of("\\") + 1, carpetaActual.size() - 1);
            curl_global_init(CURL_GLOBAL_ALL);
            curl = curl_easy_init();
            if (contador + 1 == 1) {
                lista<std::string>* localPaths = new lista<std::string>();
                lista<std::string>* remotePaths = new lista<std::string>();
                for (auto& p : fs::recursive_directory_iterator(carpetaActual)) {
                    if (!p.is_directory()) {
                        localPaths->insert(p.path().generic_string());
                    }
                }
     
                if (curl) {
                    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3002/status");
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");

                    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrite_CallbackFunc_StdString);
                    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &serverSync);

                    res = curl_easy_perform(curl);

                    if (res != CURLE_OK)
                        fprintf(stderr, "curl_easy_perform() failed: %s\n",

                            curl_easy_strerror(res));
                    curl_easy_cleanup(curl);
                }

                std::string elemento = "";
                for (int i = 0; i < serverSync.size(); i++) {
                    if (serverSync[i] != '&') {
                        elemento += serverSync[i];
                    }
                    else if (serverSync[i] == '&') {
                        remotePaths->insert(elemento);
                        std::string elemento = "";
                    }
                }

                for (int j = 0; j < remotePaths->size(); j++) {
                    std::string actualRemote = remotePaths->get_data_by_pos(j);
                    std::string remotePath = actualRemote.substr(0, actualRemote.find_first_of(','));
                    std::string remoteData = actualRemote.substr(actualRemote.find_first_of(','),actualRemote.size()-1);

                    // comparaciones
                    for (int k = 0; k < localPaths->size(); k++) {
                        if (localPaths->get_data_by_pos(k).find(remotePath) != std::string::npos) {
                            std::ifstream readFile(localPaths->get_data_by_pos(k));
                            std::string myText;
                            std::string completeText = "";

                            while (std::getline(readFile, myText)) {
                                completeText += myText + "\n";
                            }
                            if (remoteData != completeText) {
                                std::cout << remotePath + " ha sido modificado" << std::endl;
                                break;
                            }
                        }

                        else if (k == localPaths->size() - 1) {
                            if (localPaths->get_data_by_pos(k).find(remotePath) == std::string::npos) {
                                std::cout << remotePath + " ha sido eliminado" << std::endl;
                            }
                        }
                    }
                }

                for (int k = 0; k < localPaths->size(); k++) {

                    for (int a = 0; a < remotePaths->size(); a++){
                        std::string actualRemote = remotePaths->get_data_by_pos(a);
                        std::string remotePath = actualRemote.substr(0, actualRemote.find_first_of(','));
                        if (localPaths->get_data_by_pos(k).find(actualRemote) != std::string::npos) {                          
                            break;
                        }
                        if (a == remotePaths->size() - 1) {
                            if (localPaths->get_data_by_pos(k).find(remotePath) == std::string::npos) {
                                std::cout << localPaths->get_data_by_pos(k) + " ha sido agregado localmente" << std::endl;
                            }

                        }

                    }
                }
            }
            else if (contador + 1 == 2) {
                //historial cambios
                std::string file = commandArray[1];
                std::string absolutePath = fs::absolute(file).string();
                absolutePath = absolutePath.substr(absolutePath.find(nameRepo), absolutePath.size());
                std::string relativePath = fs::path(absolutePath).generic_string();
                if (curl) {
                    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3002/statusFile");
                    std::string buf("file=" + file + "&" + "absolutePath=" + relativePath);
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buf.c_str());
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
            std::string nameRepo = carpetaActual.substr(carpetaActual.find_last_of("\\") + 1, carpetaActual.size() - 1);
            std::string commitName = commandArray[2];
            curl_global_init(CURL_GLOBAL_ALL);
            curl = curl_easy_init();
            if (contador + 1 == 3) {
                // recibe nombre del archivo commandArray[1]
                std::string file = commandArray[1];
                std::string absolutePath = fs::absolute(file).string();
                absolutePath = absolutePath.substr(absolutePath.find(nameRepo), absolutePath.size());
                std::string relativePath = fs::path(absolutePath).generic_string();
                FILE* wfd = std::fopen(file.c_str(), "w");
                if (curl) {
                    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3002/rollback");
                    std::string buf("file=" + file + "&" + "absolutePath=" + relativePath + "&" + "commitName=" + commitName);
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buf.c_str());

                    /* send all data to this function  */
                    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

                    /* open the file */
                    pagefile = fopen(file.c_str(), "wb");
                    if (pagefile) {
                        /* write the page body to this file handle */
                        curl_easy_setopt(curl, CURLOPT_WRITEDATA, pagefile);

                        /* get it! */
                        res = curl_easy_perform(curl);

                        /* close the header file */
                        fclose(pagefile);
                    }

                    if (res != CURLE_OK)
                        fprintf(stderr, "curl_easy_perform() failed: %s\n",

                            curl_easy_strerror(res));
                    curl_easy_cleanup(curl);
                }
            }

            curl_global_cleanup();
        }
        else if (commandArray[0] == "reset") {
            std::string nameRepo = carpetaActual.substr(carpetaActual.find_last_of("\\") + 1, carpetaActual.size() - 1);
            curl_global_init(CURL_GLOBAL_ALL);
            curl = curl_easy_init();
            if (contador + 1 == 2) {
                // recibe nombre del archivo commandArray[1]
                std::string file = commandArray[1];
                std::string absolutePath = fs::absolute(file).string();
                absolutePath = absolutePath.substr(absolutePath.find(nameRepo), absolutePath.size());
                std::string relativePath = fs::path(absolutePath).generic_string();
                
                if (curl) {
                    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3002/reset");
                    std::string buf("absolutePath=" + relativePath);
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buf.c_str());
                    
                    /* send all data to this function  */
                    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

                    /* open the file */
                    pagefile = fopen(file.c_str(), "wb");
                    if (pagefile) {
                        /* write the page body to this file handle */
                        curl_easy_setopt(curl, CURLOPT_WRITEDATA, pagefile);

                        /* get it! */
                        res = curl_easy_perform(curl);

                        /* close the header file */
                        fclose(pagefile);
                    }

                    if (res != CURLE_OK)
                        fprintf(stderr, "curl_easy_perform() failed: %s\n",

                            curl_easy_strerror(res));
                    curl_easy_cleanup(curl);
                }
            }
            curl_global_cleanup();
        }
        else if (commandArray[0] == "sync") {
        std::string nameRepo = carpetaActual.substr(carpetaActual.find_last_of("\\") + 1, carpetaActual.size() - 1);
        curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init();
        if (contador + 1 == 2) {
            // recibe nombre del archivo commandArray[1]
            std::string file = commandArray[1];
            std::string absolutePath = fs::absolute(file).string();
            absolutePath = absolutePath.substr(absolutePath.find(nameRepo), absolutePath.size());
            std::string relativePath = fs::path(absolutePath).generic_string();
            std::ifstream readFile(file);
            std::string myText;
            std::string completeText = "";

            if (curl) {
                curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3002/sync");
                std::string buf("absolutePath=" + relativePath);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buf.c_str());

                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrite_CallbackFunc_StdString);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &serverSync);
                
                res = curl_easy_perform(curl);

                if (res != CURLE_OK)
                    fprintf(stderr, "curl_easy_perform() failed: %s\n",

                    curl_easy_strerror(res));
                    curl_easy_cleanup(curl);
            }


            while (std::getline(readFile, myText)) {
                completeText += myText + "\n";
            }
            std::cout << "Archivo local" << std::endl;
            std::cout << completeText << std::endl;

            std::cout << "Archivo servidor" << std::endl;
            std::cout << serverSync << std::endl;
            int saveFile;
            std::cout << "Digite 1 para conservar version local o 2 para conservar version remota: " << std::endl;
            std::cin >> saveFile;

            if (saveFile == 1) {
                std::ofstream ofs(file, std::ofstream::trunc);
                ofs << completeText;
                ofs.close();
            }
            else if (saveFile == 2) {
                std::ofstream ofs(file, std::ofstream::trunc);
                ofs << serverSync;
                ofs.close();
            }

        }

        curl_global_cleanup();

        }
    }


    return 0;
}