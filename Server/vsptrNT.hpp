#ifndef VSPTRNT_H
#define VSPTRNT_H

#include <stdio.h>
#include <iostream>
class vsptrNT{
public:
    int id;
    int localID;
    /**
     * @brief Construct a new vsptr N T object
     * 
     * @param _id id of the pointer
     */
    vsptrNT(int _id){
        id = _id;
    };
    /**
     * @brief Construct a new vsptr N T object
     * 
     */
    vsptrNT(){};
    /**
     * @brief Destroy the vsptr N T object
     * 
     */
    virtual ~vsptrNT(){};
    /**
     * @brief returns the type of the data
     * 
     * @return std::string the type in a string
     */
    virtual std::string ret_Type(){
        return "";
    };
    /**
     * @brief returns the value of the data
     * 
     * @return std::string the value in a string
     */
    virtual std::string ret_Val(){
        return "";
    };
    /**
     * @brief returns the memory address of the data
     * 
     * @return std::string the memory address in a string
     */
    virtual std::string ret_Mem_Addr(){
        return "";
    };
    /**
     * @brief returns the id of the pointer
     * 
     * @return std::string the id in a string
     */
    virtual std::string ret_Id(){
        return "";
    };
    /**
     * @brief returns the local_id of the vsptr
     * 
     * @return std::string the local_id in a string
     */
    virtual std::string ret_Local_Id(){
        return "";
    };
};


#endif // VSPTRNT_H
