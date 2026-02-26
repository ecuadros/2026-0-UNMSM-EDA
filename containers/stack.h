#ifndef __STACK_H__
#define __STACK_H__

#include <iostream>
#include "../general/types.h"
#include "DobleLinkedList.h"
using namespace std;

template<typename T>
class CStack : protected CDoubleLinkedList<AscendingTrait<T> >{
public:
    using Base = CDoubleLinkedList<AscendingTrait<T> >;
    using value_type = T;

    CStack() = default;


    //constructor copia reutilizamos del padre
    CStack(const CStack& other) : Base(other) {}

    //move constructor
    CStack(CStack&& other) noexcept : Base(std::move(other)) {}

    ~CStack() =default; //destrutor del padre limpa la memoria

    //operadores
    void Push(const T& val){
        //reutilizamos el push_back de la lista
        this -> push_back(val);
    }
    void Pop(){
        std::lock_guard<std::mutex> lock(this -> m_mutex);
        if(this -> m_pLast){
            auto* temp = this -> m_pLast;

            //movemos el puntedo de Last hacia atras
            this -> m_pLast = this -> m_pLast -> GetPrev();

            if(this -> m_pLast){
                this -> m_pLast -> GetNextRef() = nullptr; //desconectamos el nodo anterior
            }
            else{
                this -> m_pRoot = nullptr;//la pila quedo vacia
            }
            delete temp;
            this -> m_nElements--;
        }
    }
//ver el tope sin borrar
T Tope() const {
    std::lock_guard<std::mutex> lock(this -> m_mutex);
    if(this -> m_pLast){
        return this -> m_pLast -> GetValue();
    } throw std::out_of_range("Pila vacia");
    }
    bool IsEmpty() const {  return this -> getSize()=0; }

    //persistencia (whire)
    friend ostream& operator<<(ostream& os, CStack<T>& stack) {
        for(auto it = stack.rbegin(); it != stack.rend(); ++it) {
            os << *it << "\n";
        }
        return os;
    }

    //persistencia (read)
    friend istream &operator>>(istream &is, CStack<T>& stack) {
        T val;
        cout << "Ingrese un valor: ";
        is >> val;
        stack.Push(val);
        return is;
    }
};
#endif // __STACK_H__