//
// Created by domtaxx on 2/19/20.
//

#ifndef LISTA_H
#define LISTA_H
#include<iostream>
#include<cstddef>
#include<ostream>
/**
 * @brief the structure to define a node
 * @tparam T type of the node
 */
template<typename T>
struct node {
    T data;
    node* next;
    node* before;
};

/**
 * @brief its a list that has an associated type.
 *
 * @tparam generic, it represents a generic type.
 */
template<typename generic>
class lista {
private:
    // Atributes
    int object_counter = 0;
    node<generic>* head;
    node<generic>* end;
    //Private methods
        /**
         * @brief function that delete a node in the list,
         * this design was taken from:
         * https://www.geeksforgeeks.org/delete-a-node-in-a-doubly-linked-list/
         * @param del node to delete.
         */
    void deleteNode(node<generic>* del) {
        if (head == nullptr || del == nullptr) {
            return;
        }if (end == del) {
            end = del->before;
        }if (head == del) {
            head = del->next;
        }if (del->next != nullptr) {
            del->next->before = del->before;
        }if (del->before != nullptr) {
            del->before->next = del->next;
        }
        delete del;
        object_counter -= 1;
        return;
    };

    /**
     * @brief Get the node by pos object
     * @param pos position of the node
     * @return node<generic>* the pointer to that node
     */

     /**
      * @brief Construct a new lista object
      */
    node<generic>* get_node_by_pos(int pos) {
        if (object_counter <= 0 || pos >= object_counter) {
            return nullptr;
        }
        else {
            node<generic>* temp = head;
            for (int i = 0; i < pos; i++) {
                temp = temp->next;
            }return temp;
        }
    };
public:
/**
 * @brief Construct a new lista object
 * 
 */
    lista() {};

    /**
     * @brief a function to validate if the list is empty
     * @return true if the list is empty
     * @return false if the list has atleast one element
     */
    bool isEmpty() {
        if (object_counter == 0) {
            return true;
        }
        else {
            return false;
        }
    };

    /**
     * @brief Construct a new lista object with its first element
     * @param data_to_add data to be stored
     */
    lista(generic data_to_add) {
        node<generic>* Node = new node<generic>({ data_to_add,nullptr,nullptr });
        head = Node;
        end = Node;
        object_counter = 1;
    };

    /**
     * @brief a method to insert at the end of the list
     * @param data_to_add data to add to list
     * @tparam generic, it represents a generic type.
     */
    void insert(generic data_to_add) {
        node<generic>* Node = new node<generic>({ data_to_add,nullptr,nullptr });
        if (object_counter == 0) {
            head = Node;
        }
        else {
            end->next = Node;
            Node->before = end;
        }end = Node;
        object_counter += 1;
    };

    /**
     * @brief deletes a node in the position specified
     * @param pos position to eliminate
     * @tparam generic, it represents a generic type.
     */
    void delete_by_pos(int pos) {
        if (!(pos > object_counter - 1 || pos < 0)) {
            node<generic>* temp = get_node_by_pos(pos);
            deleteNode(temp);
        }
    };

    /**
     * @brief Get the data by pos object
     * @tparam generic, it represents a generic type.
     * @param pos position of the data to look for.
     * @return generic, it returns the value stored in that position
     */
    generic get_data_by_pos(int pos) {
        if (object_counter < 0 || pos >= object_counter) {
            return nullptr;
        }
        else {
            node<generic>* temp = head;
            generic retVal = temp->data;
            for (int i = 0; i < pos; i++) {
                temp = temp->next;
            }
            retVal = temp->data;
            return retVal;
        }
    };

    /**
     * @brief prints out the elements of the list
     */
    void print() {
        node<generic>* temp = head;
        for (int i = 0; i < object_counter; i++) {
            std::cout << temp->data << std::endl;
            if (!(i == object_counter - 1)) {
                temp = temp->next;
            }
        };
    };
    /**
     * @brief insert a new node in the desired position, it can be placed at the head,
     * end or even in the body of the list.
     * @param pos postition to insert
     * @param data_to_add data to add
     */
    void insert(generic data_to_add, int pos) {
        if (pos < object_counter && pos >= 0) {
            if (pos == object_counter - 1) {
                insert(data_to_add);
            }
            else {
                node<generic>* Node = new node<generic>({ data_to_add,nullptr,nullptr });
                if (pos == 0) {
                    head->before = Node;
                    Node->next = head;
                    head = Node;
                }
                else {
                    node<generic>* temp = get_node_by_pos(pos);
                    Node->before = temp->before;
                    temp->before->next = Node;
                    Node->next = temp;
                    temp->before = Node;
                }object_counter += 1;
            }
        }
    };
    /**
     * @brief swap objects in corresponding position
     * @param pos1 first position
     * @param pos2 second position
     */
    void swap(int pos1, int pos2) {
        generic temp_data = get_data_by_pos(pos1);
        rewrite(get_data_by_pos(pos2), pos1);
        rewrite(temp_data, pos2);
    };

    /**
     * @brief Get the object counter object
     * @return int return the object_counter
     */
    int get_object_counter() {
        return object_counter;
    }
    /**
     * @brief rewrite data in specific pos
     * @param new_data new data to chansge
     * @param pos to change data from
     */
    void rewrite(generic new_data, int pos) {
        get_node_by_pos(pos)->data = new_data;
    };

    /**
     * @brief is_End
     * @param pos
     * @return true if it's the final pos
     */
    bool is_End(int pos) {
        if (pos >= object_counter - 1) {
            return true;
        }
        else {
            return false;
        }
    };
    /**
     * @brief borra todos los elementos de la lista
     * 
     */
    void delete_list() {
        for (int i = 0; i < object_counter - 1; i++) {
            delete_by_pos(0);
        }
    }

    int size() {
        int contador = 0;
        for (int i = 0; i < object_counter; i++) {
            contador += 1;
        }
        return contador; 
    }
};
#endif //

