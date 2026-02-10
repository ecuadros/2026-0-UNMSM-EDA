#ifndef __DOUBLE_LINKED_LIST_H__
#define __DOUBLE_LINKED_LIST_H__
#include <mutex>
#include <iostream>
#include "../general/types.h"
#include "../util.h"
using namespace std;

//Nodo para listas doblemente enlazadas
template <typename Traits>
class NodeDoubleLinkedList{
    public:
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeDoubleLinkedList<Traits>;
    
    value_type m_data;
    Node *m_pNext=nullptr, *m_pPrev=nullptr;

    NodeDoubleLinkedList(){};
    NodeDoubleLinkedList(const value_type& data): m_data(data){};

};

//Traits para listas doblemente enlazadas
template <typename T, typename _Func>
struct DoubleListTrait{
    using value_type = T;
    using Func       = _Func;
};

//Clase Lista Doble
template <typename Traits>
class DoubleLinkedList{
    using Node = NodeDoubleLinkedList<Traits>;

    private:
        Node* m_pFirst = nullptr;
        Node* m_pLast = nullptr;
        size_t m_size = 0;

        mutex m_mutex; // Mutex para sincronización

    public:
        DoubleLinkedList(){};
        //Destructor para liberar memoria
        ~DoubleLinkedList(){
            /*Para circular, si la lista esta vacía, salir
            if (!m_pFirst) return;*/
            Node* current = m_pFirst;

            /*Para circular: Recorrer hasta volver al primero
            do {
                Node* temp = current;
                current = current->m_pNext;
                delete temp;
            } while (current != m_pFirst);*/

            while (current) {
                Node* temp = current;
                current = current->m_pNext;
                delete temp;
            }
            m_pFirst = m_pLast = nullptr;
            m_size = 0;
        }
        //Insertar al final de la lista
        void push_back(const typename Traits::value_type& value){
            lock_guard<mutex> lock(m_mutex); // Bloqueo para sincronización
            Node* newNode = new Node(value);

            if (!m_pFirst) {
                m_pFirst = m_pLast = newNode;
                /*Para que sea circular
                m_pFirst->m_pNext = m_pFirst
                m_pFirst->m_pPrev = m_pFirst; */

            } else {
                m_pLast->m_pNext = newNode;
                newNode->m_pPrev = m_pLast;
                /*Enlazar con el primero
                newNode->m_pNext = m_pFirst;
                m_pFirst->m_pPrev = newNode;*/
            
                m_pLast = newNode;
            }
            ++m_size;
        }
        // TODO: ForwardIterator para listas doblemente enlazadas
        class ForwardIterator{
            Node* m_ptr;
            //Node* m_start; // Para listas circulares, guarda el inicio

            public:
                ForwardIterator(Node* ptr) : m_ptr(ptr) {}//, m_start(ptr) {} //Para listas circulares

                ForwardIterator& operator++() {
                    m_ptr = m_ptr->m_pNext;
                    //Si volvemos al inicio en lista circular, terminamos
                    //if (m_ptr == m_start) m_ptr = nullptr;
                    return *this;
                }

                bool operator!=(const ForwardIterator& other) { 
                    return m_ptr != other.m_ptr; 
                }

                typename Traits::value_type& operator*() { return m_ptr->m_data; }

        };
        // TODO: BackwardIterator para listas doblemente enlazadas
        class BackwardIterator{
            Node* m_ptr;
            //Node* m_start; // Para listas circulares, guarda el inicio

            public:
                BackwardIterator(Node* ptr) : m_ptr(ptr) {} //, m_start(ptr) {} //Para listas circulares

                BackwardIterator& operator++() {
                    m_ptr = m_ptr->m_pPrev;
                    //Si volvemos al inicio en lista circular, terminamos
                    //if (m_ptr == m_start) m_ptr = nullptr;
                    return *this;
                }

                bool operator!=(const BackwardIterator& other) { 
                    return m_ptr != other.m_ptr; 
                }

                typename Traits::value_type& operator*() { 
                    return m_ptr->m_data; 
                }

        };

        Node* First() { return m_pFirst; };
        typename Traits::value_type& FirstValue() { 
            return m_pFirst->m_data; 
        }

        Node* Last() { return m_pLast; };
        typename Traits::value_type& LastValue() { 
            return m_pLast->m_data; 
        }

        ForwardIterator begin() { return ForwardIterator(m_pFirst); }
        ForwardIterator end() { return ForwardIterator(nullptr); }
        
        BackwardIterator rbegin() { return BackwardIterator(m_pLast); }
        BackwardIterator rend() { return BackwardIterator(nullptr); }

        // Foreach
        void Foreach (typename Traits::Func func){
            for (auto it = begin(); it != end(); ++it) {
                func(*it);
            }
        }

        // Constructor copia
        DoubleLinkedList(const DoubleLinkedList& other) {
            Node* current = other.m_pFirst;
            while (current) {
                push_back(current->m_data);
                current = current->m_pNext;
            }
            //Si la lista es circular, se detiene al volver al inicio
            // Node* current = other.m_pFirst;
            // do {
            //     push_back(current->m_data);
            //     current = current->m_pNext;
            // } while (current != other.m_pFirst);
        }
        
        //move constructor
        DoubleLinkedList(DoubleLinkedList&& other){
            m_pFirst = other.m_pFirst;
            m_pLast = other.m_pLast;
            m_size = other.m_size;

            other.m_pFirst = nullptr;
            other.m_pLast = nullptr;
            other.m_size = 0;
        }



};

// Imprimir la lista doblemente enlazada
template <typename Traits>
ostream &operator<<(ostream &os, DoubleLinkedList<Traits> &list) {
    os << "[";
    for (auto it = list.begin(); it != list.end(); ++it) {
        os << *it << ",";
    }
    os << "]" << endl;
    return os;
};

template <typename Traits>
istream &operator>>(istream &is, DoubleLinkedList<Traits> &list) {
    typename Traits::value_type value;
    while (is >> value) {
        list.push_back(value);
    }
    return is;
};


#endif // __DOUBLE_LINKED_LIST_H__