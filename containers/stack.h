#ifndef __STACK_H__
#define __STACK_H__

#include <iostream>
#include <mutex>
#include <assert.h>
#include "../general/types.h"
#include "../util.h"
#include "linkedlist.h"

using namespace std;

template<typename T>
struct StackTrait {
    using value_type = T;
};

template <typename T>
class CNode : public NodeLinkedList< StackTrait<T> > {
public:
    CNode(const T &value, ref_type ref) : NodeLinkedList< StackTrait<T> >(value, ref) {};
    // override de la función GetNext para que obtengamos CNode* y no NodeLinkedList*
          CNode<T>* GetNext()       { return static_cast<      CNode<T>*>(NodeLinkedList< StackTrait<T> >::GetNext()); }
    const CNode<T>* GetNext() const { return static_cast<const CNode<T>*>(NodeLinkedList< StackTrait<T> >::GetNext()); }
    // esta versión es para objetos que sean constantes, como lo hacemos para copiar
};

template <typename T>
struct CStack {

    // En este caso, rehusamos los Nodos de la lista enlazada,
    // tienen atributos y métodos necesarios para la CStack
    using Node = CNode<T>;

    mutable mutex mtx;
    Node *SLast = nullptr;
    size_t n_Elements = 0;

public:
    // Constructor normal
    CStack() {}

    //Copy Constructor
    CStack( const CStack<T> &another ) {
        lock_guard<mutex> lock(another.mtx); 

        Node *iter = another.SLast, *iterThis;
        for (size_t n = 0; n < another.n_Elements; ++n) {
            this->push(0, 0);
        }
        iter = another.SLast; 
        iterThis = SLast;
        for (size_t n = 0; n < another.n_Elements; ++n) {
            iterThis->GetValueRef() = iter->GetValue();
            iterThis->GetRefRef() = iter->GetRef();
            iterThis = iterThis->GetNext();
            iter = iter->GetNext();
        }
            
        n_Elements = another.n_Elements;
    }

    //Move constructor
    CStack( CStack<T> &&another) noexcept {
        lock_guard<mutex> lock(another.mtx);

        SLast = another.SLast;
        another.SLast = nullptr;
        n_Elements = another.n_Elements;
        another.n_Elements = 0;
    }

    virtual ~CStack() {
        Node *temp;
        while(SLast) {
            temp = SLast->GetNext();
            delete SLast;
            SLast = temp;
        }
    }

    void push(const T &value, ref_type ref);
    T pop();
    T top();
    bool empty();
    size_t getSize() { lock_guard<mutex> lock(mtx); return n_Elements; };
    // Uso de mutex en caso hagan un push antes de devolver n_Elements


    // Opertor <<, se imprime en forma de una pila de objetos
    friend ostream &operator<<(ostream &os, CStack<T> &container){
        Node *iter = container.SLast;

        os << "CStack: size = " << container.getSize() << endl<<endl;
        os << "Cima de pila" << endl;
        for (size_t i = 0; i < container.getSize(); ++i){
             os << "\t(" << iter->GetValue() << ", " << iter->GetRef() << ")" << endl;
             iter = iter->GetNext();
        }
        os << "Fondo de pila" << endl;;
        os << endl;
        return os;
    }

    friend istream &operator>>(istream &is, CStack<T> &container) {
        T val;
        ref_type ref;
        char symbol;

        if (!(is >> symbol) || symbol != '(')
            return is;

        if (!(is >> val >> symbol) || symbol != ':')
            return is;

        if (!(is >> ref >> symbol) || symbol != ')')
            return is;

        container.push(val, ref);
        return is;
    }

};

template <typename T>
void CStack<T>::push(const T &value, ref_type ref) {
    lock_guard<mutex> lock(mtx);

    Node *lastNode = new Node(value, ref);
    lastNode->GetNextRef() = SLast;
    SLast = lastNode;
    ++n_Elements;
}

template <typename T>
T CStack<T>::pop() {
    lock_guard<mutex> lock(mtx);
    assert(SLast != nullptr);

    T value = SLast->GetValue();
    Node *temp = SLast;
    SLast = SLast->GetNext();
    delete temp;
    --n_Elements;

    return value;
}

template <typename T>
T CStack<T>::top() {
    lock_guard<mutex> lock(mtx);
    assert(SLast != nullptr);

    return SLast->GetValue();
}

template <typename T>
bool CStack<T>::empty() {
    return SLast == nullptr;
}

void DemoStack();

#endif // __STACK_H__