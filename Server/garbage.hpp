#ifndef GARBAGE_H
#define GARBAGE_H

#include <iostream>
#include <exception>
#include "Linked_List.hpp"
#include "vsptrNT.hpp"
#include <stdint.h>

/**
 * @brief package, that saves an id and a counter
 */
class package{
public:
    int id;
    int ref_counter;
    /**
     * @brief constructor of package
     * @param _id id of the package
     * @return a package
     */
    package(int _id){
        id = _id;
    };
    /**
     * @brief cunstructor for package
     * @return a package
     */
    package(){
        ref_counter=1;
    };
    /**
     * @brief Destroy the package object
     * 
     */
    virtual ~package(){};
    /**
     * @brief returns the type of T* in string form
     * 
     * @return std::string of T* type
     */
    virtual std::string ret_Type(){
        return "";
    };
    /**
     * @brief returns the value of the T* in string form
     * @return std::string of the value
     */
    virtual std::string ret_Val(){
        return "";
    };
    /**
     * @brief returns the memory address of the T* in string form
     * 
     * @return std::string of the memory address
     */
    virtual std::string ret_Mem_Addr(){
        return "";
    };
};
/**
 * @brief derived class of package, stores the pointer of the data to store.
 * @tparam J Type specifier of the class
 */
template<typename J>
class specific_package: public package{
public:
    J data;
    /**
     * @brief Construct a new specific package object
     * 
     * @param _id id of the package
     * @param _data data to store
     */
    specific_package(int _id, J _data):package(_id){
        data = _data;
        ref_counter=1;
    };
    /**
     * @brief Construct a new specific package object
     * 
     */
    specific_package(){
        ref_counter=1;
    };
    /**
     * @brief Destroy the specific package object
     */
    ~specific_package(){};
    /**
     * @brief specifies the function from package
     * @return std::string returns type of data in a string 
     */
    std::string ret_Type(){
        return typeid(data).name();
    };
    /**
     * @brief specifies the function from package
     * 
     * @return std::string returns value of data in a string
     */
    std::string ret_Val(){
        return std::to_string(data);
    };
    /**
     * @brief specifies the function from package
     * 
     * @return std::string memory address of data in a string
     */
    std::string ret_Mem_Addr(){
        long addr = (long)&data;
        return std::to_string(addr);
    };
};

class GarbageCollector{
    private:
        /**
         * @brief Construct a new Garbage Collector object
         * 
         */
        GarbageCollector();
        static GarbageCollector* recolector;
        int contador;
        lista<package*> package_List;
        lista<vsptrNT*> vsptr_List[31];
        /**
         * @brief binary search of the packages by id
         * 
         * @param id id of the package to search
         * @return package* pakage to find
         */
        package* binary_search_id(int id);
    public:
        /**
         * @brief Destroy the Garbage Collector object
         * 
         */
        ~GarbageCollector(){};
        /**
         * @brief Get the Garbage Collector object
         * 
         * @return GarbageCollector* 
         */
        static GarbageCollector* getGarbageCollector();
        /**
         * @brief Get the Contador object
         * 
         * @return int 
         */
        int getContador();
        /**
         * @brief Set the Contador object
         * 
         * @param cont 
         */
        void setContador(int cont);
        /**
         * @brief Get the Pkg List object
         * 
         * @return lista<package*> 
         */
        lista<package*> get_Pkg_List();
        /**
         * @brief adds a package to the list of packages
         * 
         * @param to_add package pointer to add
         */
        void add_Pkg_To_List(package* to_add);
        /**
         * @brief Get the Vsptr List object
         * 
         * @return lista<vsptrNT*>* 
         */
        lista<vsptrNT*>* get_Vsptr_List();
        /**
         * @brief add a VSPtr to the pointer list
         * @param to_add pointer to add
         * @param client client to associate with
         */
        void add_Vsptr_To_List(vsptrNT* to_add, int client);
        /**
         * @brief add a reference to the package with associated id
         * 
         * @param id id to look by
         */
        void add_ref(int id);
        /**
         * @brief lower the reference to the package wit the associated id
         * 
         * @param id id to look by
         */
        void lower_ref(int id);
        /**
         * @brief function that sweeps the lists and clears memory
         * 
         */
        void delete_pkgs();

};
#endif
