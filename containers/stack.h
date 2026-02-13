#ifndef __STACK_H__
#define __STACK_H__

#include <iostream>
#include "../general/types.h"
#include "../util.h"
#include <mutex>
#include <utility>
#include <stdexcept>
using namespace std;
template <typename T>
struct StackTrait
{
    using Value_type = T;
};

template <typename Traits>
class NodeStack{
using Value_type = typename Traits::Value_type;
using Node       = NodeStack<Traits>;
private:
Value_type m_data;
Node *m_pNext = nullptr;
public:
NodeStack(){};
NodeStack( Value_type _value)
        : m_data(_value), m_pNext(nullptr) { };
Value_type  GetValue   () const { return m_data; }
Value_type &GetValueRef() { return m_data; }


Node      * GetNext     () const { return m_pNext;   }
Node      *&GetNextRef  () { return m_pNext;   }

};

template <typename Traits>
class CStack{
using Value_type = typename Traits::Value_type;
using Node       = NodeStack<Traits>;    
private:
Node* n_Top=nullptr;
size_t m_nElements = 0;
mutable std::mutex m_Block;
public:
CStack (){};
//COPY CONSTRUCTOR
CStack (const CStack &another) {
        std::lock_guard<std::mutex> lock(another.m_Block); 
        if (!another.n_Top ) {
        n_Top = nullptr;
        m_nElements = 0;
        return;
        }
    n_Top = new Node(another.n_Top->GetValue());
    Node* p_Destiny = n_Top;                
    Node* p_Lector  = another.n_Top->GetNext(); 
    while (p_Lector) {
        Node* pNew = new Node(p_Lector->GetValue());
        p_Destiny->GetNextRef() = pNew; 
        p_Destiny = pNew;
        p_Lector = p_Lector->GetNext();
    }
    m_nElements = another.m_nElements;
    }
//MOVE CONSTRUCTOR
 CStack(CStack &&another) noexcept {
        std::lock_guard<std::mutex> lock(another.m_Block); 
        n_Top    = std::exchange(another.n_Top, nullptr);
        m_nElements = std::exchange(another.m_nElements, 0);
    }
void Push(const Value_type &Val );
Value_type Pop();
size_t getSize(){ return m_nElements;  }
virtual ~CStack();



 friend ostream &operator<<(ostream &os, CStack<Traits> &container){
        std::lock_guard<std::mutex> lock(container.m_Block);
        Node *p_Temp = container.n_Top;
        while (p_Temp ) {
            os  << "   " << p_Temp->GetValue() << "   " << endl;
            p_Temp = p_Temp->GetNext();
        }
            os  << "   NULL    "<<endl;
        return os;
    }

    friend istream &operator>>(istream &is, CStack<Traits> &container) {
        Value_type val;
        is >> val ; 
        container.Push(val);
        return is;
    }    
};

template <typename Traits>
void  CStack<Traits>::Push(const Value_type &val){
    std::lock_guard<std::mutex> lock(m_Block);
    Node *pNew = new Node(val);
    pNew->GetNextRef() = n_Top; 

    n_Top = pNew;
    ++m_nElements;
}
template <typename Traits>
typename Traits::Value_type CStack<Traits>::Pop(){
 std::lock_guard<std::mutex> lock(m_Block);
    if(!n_Top){ throw std::runtime_error("Pila vacia"); }
    Value_type Valor=n_Top->GetValue();
    Node* pTemp = n_Top;
    n_Top = n_Top->GetNext(); 
    delete pTemp;
    --m_nElements;
    return Valor;
}

template <typename Traits>
CStack<Traits>::~CStack() {
    Node *p_Temp = n_Top;
    while (p_Temp) {
        Node *pNext = p_Temp->GetNext(); 
        delete p_Temp;                   
        p_Temp = pNext;                  
    }
    n_Top = nullptr; 
}


#endif // __STACK_H__