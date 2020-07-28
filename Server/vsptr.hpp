#ifndef VSPTR_HPP
#define VSPTR_HPP

#include <iostream>
#include "vsptrNT.hpp"
#include "garbage.hpp"
/**
 * @brief derived class from vsptrNT, stores a pointer of the data
 * @tparam T 
 */
template<typename T>
class VSPtr: public vsptrNT{
private:
    T* dato;
    /**
     * @brief Construct a new VSPtr object
     * 
     * @param client client to associate
     */
    VSPtr(int client):vsptrNT(){
        id = -1;
        dato = nullptr;
        if(GarbageCollector::getGarbageCollector()==NULL){
            GarbageCollector::getGarbageCollector();
        }
         GarbageCollector* gc = GarbageCollector::getGarbageCollector();
         gc->add_Vsptr_To_List(this, client);
    };
public:
/**
 * @brief a way to instanciate VSPtr
 * 
 * @param client client to instanciate
 * @return VSPtr<T>* pointer of the new VSPtr
 */
    static VSPtr<T>* New(int client){
        return new VSPtr<T>(client);
    };
    /**
     * @brief Destroy the VSPtr object
     * 
     */
    ~VSPtr(){
        GarbageCollector* gc = GarbageCollector::getGarbageCollector();
        gc->lower_ref(this->id);
    };
    /**
     * @brief returns the data
     * 
     * @return T  data
     */
    T operator &(){
        return (*dato);
    };
    /**
     * @brief returns this same object
     * 
     * @return VSPtr<T> this
     */
    VSPtr<T> operator *(){
        return *this;
    };
    /**
     * @brief asign a new value to the pointer
     * 
     * @param dataNueva new data
     */
    void operator=(T dataNueva){
        if(id == -1){
            id = GarbageCollector::getGarbageCollector()->getContador();
            specific_package<T>* pkg = new specific_package<T>(id,dataNueva);
            GarbageCollector::getGarbageCollector()->add_Pkg_To_List(pkg);
            dato = &(pkg->data);
            GarbageCollector::getGarbageCollector()->setContador(id+1);
        }else{
            GarbageCollector::getGarbageCollector()->lower_ref(id);
            id = GarbageCollector::getGarbageCollector()->getContador();
            specific_package<T>* pkg = new specific_package<T>(id,dataNueva);
            GarbageCollector::getGarbageCollector()->add_Pkg_To_List(pkg);
            dato = &(pkg->data);
            GarbageCollector::getGarbageCollector()->setContador(id+1);
        }
    };
    /**
     * @brief asigns a new value to the pointer 
     * @param dataNueva new pointer containing the data
     */
    void operator=(VSPtr<T> dataNueva){
        if(id == -1){
            id = dataNueva.id;
            dato = dataNueva.dato;
            GarbageCollector::getGarbageCollector()->add_ref(id);
            GarbageCollector::getGarbageCollector()->add_ref(id);
        }else{
            GarbageCollector::getGarbageCollector()->lower_ref(id);
            id = dataNueva.id;
            dato = dataNueva.dato;
            GarbageCollector::getGarbageCollector()->add_ref(id);
            GarbageCollector::getGarbageCollector()->add_ref(id);
        }
    };
    /**
     * @brief return the type of the pointer
     * 
     * @return std::string of type
     */
    std::string ret_Type(){
        return typeid(*dato).name();
    };
    /**
     * @brief returns the value stored in data
     * 
     * @return std::string data stored in data
     */
    std::string ret_Val(){
        if (dato==nullptr){
            return "null";
        }else{
            return std::to_string(*dato);
        }
    };
    /**
     * @brief returns the id of the pointer
     * 
     * @return std::string id in a string
     */
    std::string ret_Id(){
        return std::to_string(id);
    };
    /**
     * @brief returns the memory address of the data
     * 
     * @return std::string the memory address in a string
     */
    std::string ret_Mem_Addr(){
        return std::to_string((long)dato);
    };

};


#endif // VSPTR_HPP
