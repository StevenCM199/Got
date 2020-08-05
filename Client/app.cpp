#include "app.h"
#include <stdio.h>
#include <string>

int main(){
    std::string command; 
    // iniciar conexion http
    while(true){
        std::cout << "got "; 
        std::getline(std::cin, command);

        // parsear comando

        if(command == "init"){

        }
        else if(command == "help"){
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
        else if(command == "add"){
            
        }
        else if(command == "commit"){
            
        }
        else if(command == "status"){
            
        }
        else if(command == "rollback"){
            
        }
        else if(command == "reset"){
            
        }
        else if(command == "sync"){
            
        }
    }
    

    return 0; 
}