#include "socket_S.h"
#include <climits>
#include <poll.h>
#include "include/rapidjson/document.h"
#include "include/rapidjson/istreamwrapper.h"
#include "include/rapidjson/writer.h"
#include "include/rapidjson/stringbuffer.h"
#include "include/rapidjson/ostreamwrapper.h"
#include <fstream>
#include "md5.h"
Socket_S::Socket_S(){};
void Socket_S::set_port(int port, std::string ip){
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port); //htons = host to network short (Little Indian o Big Indian)
    inet_pton(AF_INET, ip.c_str(), &hint.sin_addr); //ip del server
};

int Socket_S::start(int _port, std::string _ip){
    gar_col = GarbageCollector::getGarbageCollector();
    listening = socket(AF_INET, SOCK_STREAM, 0);
    if(listening == -1){
        std::cerr<<"could not creat socket \n";
        return -1;
    }
    // se crea el puerto del server
    set_port(_port, _ip);
    if(bind(listening, (sockaddr*)&hint, sizeof(hint))==-1){
        std::cerr<<"can't bind to port \n";
        return -2;
    }
    return mark_listening();
};

int Socket_S::mark_listening(){
    listen(listening, SOMAXCONN);
    fd_set master;
    FD_ZERO(&master);
    FD_SET(listening, &master);

    struct pollfd poll_set[32];
    int numfds = 0;
    memset(poll_set, '\0', sizeof(poll_set));
    poll_set[0].fd = listening;
    poll_set[0].events = POLLIN;
    numfds++;


    
    while(true){
        poll(poll_set, numfds, -1);
        bool is_client_arr[32];
        for(int i = 0; i < numfds; i++){
            if( poll_set[i].revents & POLLIN){
                if(poll_set[i].fd == listening){
                    int clientSock = accept(listening, (sockaddr*)&client, &client_Size);
                    poll_set[numfds].fd = clientSock;
                    poll_set[numfds].events = POLLIN;
                    numfds++;
                    std::cout<<"CONNECTED"<<std::endl;
                    is_client_arr[i-1]=false;
                }else{
                    char buffer[4096];
                    memset(buffer, 0, 4096);
                    int bytesIn = recv(poll_set[i].fd, buffer, 4096, 0);
                    if( bytesIn <= 0 ){
                        close(poll_set[i].fd);           
                        poll_set[i].events = 0;
                        gar_col->get_Vsptr_List()[i-1].clear_list();
                        for(int j = i; j<numfds; j++){
                            poll_set[j] = poll_set[j + 1];
                        }
                        numfds--;
                        std::cout<<"se elimina un cliente"<<std::endl;
                    }
                    //crear vsp
                    if(buffer[0] == '$'){
                        if(is_client_arr[i-1]) {
                            std::cout<<"se crea un puntero"<<std::endl;
                            std::string LocalIdStr;
                            std::string tipo;
                            std::string json_str = get_json(buffer);
                            rapidjson::Document document;
                            document.Parse<0>(json_str.c_str()).HasParseError();
                            LocalIdStr = document["localId"].GetString();
                            tipo = document["tipo"].GetString();
                            std::cout<<tipo+"\n";
                            std::cout<<LocalIdStr+"\n";
                            try {
                                createVSPtr(tipo.c_str()[0], i - 1, std::stoi(LocalIdStr));
                                send(poll_set[i].fd, "VSPtr created", sizeof("VSPtr created"), 0);
                            } catch (...) {
                                std::cout << "no se pudo crear un puntero" << std::endl;
                                send(poll_set[i].fd, "Error creating VSPtr", sizeof("Error creating VSPtr"), 0);
                            }
                        }
                    }//asignar valor VSP
                    else if(buffer[0]== '#'){
                        if(is_client_arr[i-1]) {
                            std::string json_str = get_json(buffer);
                            rapidjson::Document document;
                            document.Parse<0>(json_str.c_str()).HasParseError();
                            int local_Id_VSPtr = std::stoi(document["localId"].GetString());
                            if (buffer[1] == 'd') { ;
                                std::cout<<"se crea un paquete"<<std::endl;
                                lista<vsptrNT *> list_ptr = gar_col->get_Vsptr_List()[i - 1];
                                for (int a = 0; a < list_ptr.get_object_counter(); a++) {
                                    vsptrNT *ptr = list_ptr.get_data_by_pos(a);
                                    if (ptr->localID == local_Id_VSPtr) {
                                        char type = ptr->ret_Type().c_str()[0];
                                        give_VSPtr_New_Value(type, document["dato"].GetString(), ptr);
                                        send(poll_set[i].fd, "pkg created", sizeof("pkg created"), 0);
                                        break;
                                    }
                                }
                            } else if (buffer[1] == 'p') {
                                std::cout<<"se asigna valor"<<std::endl;
                                std::string vsptr_id_str = document["dato"].GetString();
                                int VSP_data_Id = std::stoi(vsptr_id_str);
                                lista<vsptrNT *> list_ptr = gar_col->get_Vsptr_List()[i - 1];
                                for (int a = 0; a < list_ptr.get_object_counter(); a++) {
                                    vsptrNT *ptr = list_ptr.get_data_by_pos(a);
                                    if (ptr->localID == VSP_data_Id) {
                                        for (int b = 0; b < list_ptr.get_object_counter(); b++) {
                                            vsptrNT *data_ptr = list_ptr.get_data_by_pos(b);
                                            if (data_ptr->localID == local_Id_VSPtr) {
                                                char type = data_ptr->ret_Type().c_str()[0];
                                                give_VSPtr_New_Value(type, ptr, data_ptr);
                                                send(poll_set[i].fd, "valor asignado", sizeof("valor asignado"), 0);
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }//devolver valor dentro del VSPtr(&)
                    else if(buffer[0]== '&'){
                        if(is_client_arr[i-1]) {
                            std::cout<< "se devuelve el valor"<<std::endl;
                            int pkg_id = -81;
                            int local_Id_VSPtr = -97;

                            std::string msg = "{ \"tipo\" : \"";
                            std::string local_Id_Str = "";
                            for (int a = 1; buffer[a] != '*'; a++) {
                                local_Id_Str += buffer[a];
                            }
                            local_Id_VSPtr = std::stoi(local_Id_Str);
                            lista<vsptrNT *> list_ptr = gar_col->get_Vsptr_List()[i - 1];
                            lista<package *> list_pkg = gar_col->get_Pkg_List();

                            for (int a = 0; a < list_ptr.get_object_counter(); a++) {
                                vsptrNT *ptr = list_ptr.get_data_by_pos(a);
                                if (ptr->localID == local_Id_VSPtr) {
                                    msg += ptr->ret_Type() + "\",\"dato\" : \"";
                                    pkg_id = ptr->id;
                                    break;
                                }
                            }

                            for (int a = 0; a < list_pkg.get_object_counter(); a++) {
                                package *pkg = list_pkg.get_data_by_pos(a);
                                if (pkg->id == pkg_id) {
                                    std::string nuLL = "null";
                                    if(pkg->ret_Val() == nuLL){
                                        msg += 0 + "\"}";
                                    }else {
                                        msg += pkg->ret_Val() + "\"}";
                                    }
                                    break;
                                }
                            }
                            std::cout<<msg<<std::endl;
                            send(poll_set[i].fd, msg.c_str(), msg.size(), 0);
                        }
                    }//borrar valor
                    else if(buffer[0]== '~'){
                        if(is_client_arr[i-1]) {
                            std::cout<< "se borra el ptr"<<std::endl;
                            std::string local_Id_Str;
                            for (int a = 1; buffer[a] != '*'; a++) {
                                local_Id_Str += buffer[a];
                            }
                            lista<vsptrNT *> list_ptr = gar_col->get_Vsptr_List()[i - 1];
                            int local_Id_VSPtr = std::stoi(local_Id_Str);
                            for (int a = 0; a < list_ptr.get_object_counter(); a++) {
                                vsptrNT *ptr = list_ptr.get_data_by_pos(a);
                                if (ptr->localID == local_Id_VSPtr) {
                                    delete ptr;
                                    send(poll_set[i].fd, "dato borrado", sizeof("dato borrado"), 0);
                                    break;
                                }
                            }
                        }
                    }else if(buffer[0] == '^'){
                        std::string usuario;
                        std::string password;
                        int h;
                        for(h = 1; buffer[h] != ',' ;h++) {
                            usuario+=buffer[h];
                        }h++;
                        for(h; buffer[h] != '*' ;h++) {
                            password+=buffer[h];
                        }
                        password = md5(password.c_str());
                        std::ifstream ifs("JSONFiles/package.json");
                        rapidjson::IStreamWrapper isw (ifs);
                        rapidjson::Document doc;
                        doc.ParseStream(isw).HasParseError();

                        const auto& user_data_arr = doc["clients"];
                        assert(user_data_arr.IsArray());
                        for (rapidjson::SizeType o = 0; o < user_data_arr.Size(); o++){ // Uses SizeType instead of size_t
                            const rapidjson::Value& c = user_data_arr[o];
                            std::string user = c["usuario"].GetString();
                            std::string pass = c["password"].GetString();
                            if(user==usuario && pass==password){
                                is_client_arr[i-1] = true;
                                break;
                            }
                        }
                        if(is_client_arr[i-1]){
                            send(poll_set[i].fd, "success", sizeof("success"), 0);
                            std::cout<< "entro el usuario"<<std::endl;
                        }else{
                            send(poll_set[i].fd, "error", sizeof("error"), 0);
                        }
                    }else if(buffer[0] == '='){
                        if(is_client_arr[i-1]) {
                            std::string msg = data_GC(i-1);
                            send(poll_set[i].fd, msg.c_str(), msg.size(), 0);
                        }
                    }
                }
            }
        }   
    }
};


std::string Socket_S::data_GC(int client){
    std::string msg;
    for(int i = 0; i < GarbageCollector::getGarbageCollector()->get_Pkg_List().get_object_counter();i++){
            package* pack = GarbageCollector::getGarbageCollector()->get_Pkg_List().get_data_by_pos(i);
            std::string val = pack->ret_Val().c_str();
            std::string tipo = pack->ret_Type().c_str();
            std::string addr = pack->ret_Mem_Addr().c_str();
            std::string id = std::to_string(pack->id);
            std::string ref = std::to_string(pack->ref_counter);
            msg += id+","+tipo+","+val+","+addr+","+ref;
            if((i+1) == GarbageCollector::getGarbageCollector()->get_Pkg_List().get_object_counter()){
                msg+= "&";
            }else{
                msg+= "*";
            };
    };
    for(int i = 0; i < GarbageCollector::getGarbageCollector()->get_Vsptr_List()[client].get_object_counter();i++){
        vsptrNT* ptr = GarbageCollector::getGarbageCollector()->get_Vsptr_List()[client].get_data_by_pos(i);
        std::string id = ptr->ret_Id().c_str();
        std::string type = ptr->ret_Type().c_str();
        std::string value = ptr->ret_Val().c_str();
        msg += id +","+ type + ","+ value;
        if((i+1) == GarbageCollector::getGarbageCollector()->get_Vsptr_List()[client].get_object_counter()){
            msg+= ";";
        }else{
            msg+= "*";
        };
    };
    return msg;
}

vsptrNT* Socket_S::createVSPtr(char type, int client, int local_id){
    vsptrNT* ptr = nullptr;
    if(type == 'i'){//int
        ptr = VSPtr<int>::New(client);
    }else if(type == 'd'){//double
        ptr = VSPtr<double>::New(client);
    }else if(type == 'b'){//bool
        ptr = VSPtr<bool>::New(client);
    }else if(type == 'f'){//float
        ptr = VSPtr<float>::New(client);
    }else if(type == 'c'){//char
        ptr = VSPtr<char>::New(client);
    }else if(type == 'l'){//long
        ptr = VSPtr<long>::New(client);
    }else if(type == 'x'){//long long
        ptr = VSPtr<long long>::New(client);
    }else if(type == 'e'){//long double
        ptr = VSPtr<long double>::New(client);
    }else{
        int var = 1231/0;
    }ptr->localID = local_id;
    return ptr;
}

void Socket_S::give_VSPtr_New_Value(char type, const std::string& new_val, vsptrNT* ptr){
    if(type == 'i'){
        auto ptrA = (VSPtr<int>*)ptr;
        *ptrA = std::stoi(new_val);
    }else if(type == 'd'){
        auto ptrA = (VSPtr<double>*)ptr;
        *ptrA = std::stod(new_val);
    }else if(type == 'f'){
        auto ptrA = (VSPtr<float>*)ptr;
        *ptrA = std::stof(new_val);
    }else if(type == 'c'){
        auto ptrA = (VSPtr<char>*)ptr;
        *ptrA = (new_val).c_str()[0];
    }else if(type == 'b'){
        auto ptrA = (VSPtr<bool>*)ptr;
        *ptrA = std::stoi(new_val);
    }else if(type == 'l'){
        auto ptrA = (VSPtr<long>*)ptr;
        *ptrA = std::stol(new_val);
    }else if(type == 'x'){
        auto ptrA = (VSPtr<long long>*)ptr;
        *ptrA = std::stoll(new_val);
    }else if(type == 'e'){
        auto ptrA = (VSPtr<long double>*)ptr;
        *ptrA = std::stold(new_val);
    }
}

void Socket_S::give_VSPtr_New_Value(char type, vsptrNT* val_ptr, vsptrNT* ptr){
    if(type == 'i'){
        auto ptrA = (VSPtr<int>*)ptr;
        auto ptrB = (VSPtr<int>*)val_ptr;
        *ptrA = *ptrB;
    }else if(type == 'd'){
        auto ptrA = (VSPtr<double>*)ptr;
        auto ptrB = (VSPtr<double>*)val_ptr;
        *ptrA = *ptrB;
    }else if(type == 'f'){
        auto ptrA = (VSPtr<float>*)ptr;
        auto ptrB = (VSPtr<float>*)val_ptr;
        *ptrA = *ptrB;
    }else if(type == 'c'){
        auto ptrA = (VSPtr<char>*)ptr;
        auto ptrB = (VSPtr<char>*)val_ptr;
        *ptrA = *ptrB;;
    }else if(type == 'b'){
        auto ptrA = (VSPtr<bool>*)ptr;
        auto ptrB = (VSPtr<bool>*)val_ptr;
        *ptrA = *ptrB;
    }else if(type == 'l'){
        auto ptrA = (VSPtr<long>*)ptr;
        auto ptrB = (VSPtr<long>*)val_ptr;
        *ptrA = *ptrB;
    }else if(type == 'x'){
        auto ptrA = (VSPtr<long long>*)ptr;
        auto ptrB = (VSPtr<long long>*)val_ptr;
        *ptrA = *ptrB;
    }else if(type == 'e'){
        auto ptrA = (VSPtr<long double>*)ptr;
        auto ptrB = (VSPtr<long double>*)val_ptr;
        *ptrA = *ptrB;
    }
}

std::string Socket_S::get_json(char* buffer){
    std::string json_str;
    int a = 0;
    while(buffer[a] != '{'){
        a++;
    }
    for (a; buffer[a] != '}'; a++) {
        json_str += buffer[a];
    }json_str += '}';
    return json_str;

}
