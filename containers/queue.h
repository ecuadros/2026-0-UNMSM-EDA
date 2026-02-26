#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <iostream>
#include "../general/types.h"
#include "../util.h"
#include "StaQue.h"
#include "DobleLinkedList.h"

using namespace std;
template<typename T>
class CQueue : protected CDoubleLinkedList<AscendingTrait<T> >{
public:
    using Base = CDoubleLinkedList<AscendingTrait<T> >;
    using value_type = T;

    CQueue() = default;

    //constructor copia
    CQueue(const CQueue& other) : Base(other) {}
    //move constructor
    CQueue(CQueue&& other) noexcept : Base(std::move(other)) {}

    //destructor seguro
    ~CQueue() = default;

    //operadores
    void Push(const T& val){
        this -> push_back(val);
    }
    //decolador
    void Pop(){
        std::lock_guard<std::mutex> lock(this -> m_mutex);
        if(this -> m_pRoot){
            auto* temp = this -> m_pRoot;

            //movemos la raiz al siguiente
            this -> m_pRoot = this -> m_pRoot -> GetNext();

            if(this -> m_pRoot){
                this -> m_pRoot -> GetPrevRef() = nullptr; //desconectamos el nodo anterior

        } else {
            this -> m_pLast = nullptr; //la cola quedo vacia
        }
        delete temp;
        this -> m_nElements--;
        }
    }
    T Front() const {
        std::lock_guard <std::mutex> lock(this->m_mutex);
        if (this->m_pRoot) {
            return this->m_pRoot->GetValue();
        }
        throw std::out_of_range("Cola vacia");
    }
    bool IsEmpty() const { return this->getSize() == 0; }

    // ">>" "<<"" leer y imprimir
    //imprimir (FIFO)
    friend ostream& operator<<(ostream& os, CQueue<T>& queue) {
        //usamos begin por que es el primero en entrar y primero en salir
        for(auto it = queue.begin(); it != queue.end(); ++it) {
            os << *it << "\n";
        }
        return os;
    }
    //leer
    friend istream &operator>>(istream &is, CQueue<T>& queue) {
        T val;
        cout<<"ingrese un valor a la cola"<<endl;
        is >> val;
        queue.Push(val);
        return is;
    }
};


#endif // __QUEUE_H__