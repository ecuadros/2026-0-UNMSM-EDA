#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <iostream>
#include "../general/types.h"
#include "../util.h"
#include  <mutex>
#include  <utility>
#include <stdexcept>
using namespace std;
template <typename T>
struct QueueTrait
{
    using Value_type = T;
};

template <typename Traits>
class NodeQueue{
using Value_type = typename Traits::Value_type;
using Node       = NodeQueue<Traits>;
private:
Value_type m_data;
Node *m_pNext = nullptr;
public:
NodeQueue(){};
NodeQueue(Value_type _value)
        : m_data(_value), m_pNext(nullptr) { };
Value_type  GetValue   () const { return m_data; }
Value_type &GetValueRef() { return m_data; }


Node      * GetNext     () const { return m_pNext;   }
Node      *&GetNextRef  () { return m_pNext;   }

};
template <typename Traits>
class CQueue{
using Value_type = typename Traits::Value_type;
using Node       = NodeQueue<Traits>;
private:
Node*  m_pFirst=nullptr;
Node*  m_pLast=nullptr;
size_t m_nElements = 0;
mutable std::mutex m_Block;
public:
CQueue (){};
//COPY CONSTRUCTOR
CQueue (const CQueue &another) {
        std::lock_guard<std::mutex> lock(another.m_Block); 
        m_pFirst = nullptr;
        m_pLast  = nullptr;
        m_nElements = 0;
        Node* pTemp=another.m_pFirst;
        while(pTemp){
            Push(pTemp->GetValue());
            pTemp=pTemp->GetNextRef();
        }
    }
//MOVE CONSTRUCTOR
 CQueue(CQueue &&another) noexcept {
        std::lock_guard<std::mutex> lock(another.m_Block); 
        m_pLast     = std::exchange(another.m_pLast, nullptr);
        m_pFirst    = std::exchange(another.m_pFirst, nullptr);
        m_nElements = std::exchange(another.m_nElements, 0);
    }
void Push(const Value_type &Val );
Value_type Pop();
size_t getSize(){ return m_nElements;  }
virtual ~CQueue();
friend ostream &operator<<(ostream &os, CQueue<Traits> &container){
        std::lock_guard<std::mutex> lock(container.m_Block);
        Node* pTemp=container.m_pFirst;
        while (pTemp){
         os << " [ " << pTemp->GetValue()  << "  ]  " << " -> " ;
         pTemp=pTemp->GetNext();
         }
        os<< " NULL "<<endl;
        return os;
    }

    friend istream &operator>>(istream &is, CQueue<Traits> &container) {
        Value_type val;
        is >> val ; 
        container.Push(val);
        return is;
    }   
};

template <typename Traits>
void  CQueue<Traits>::Push(const Value_type &val){
    std::lock_guard<std::mutex> lock(m_Block);
    Node* pNew =new Node(val);
    if(!m_pFirst){    m_pFirst=pNew;    }
    else         {  m_pLast->GetNextRef()=pNew; }
    m_pLast=pNew;
    ++m_nElements;
}
template <typename Traits>
typename Traits::Value_type CQueue<Traits>::Pop(){
 std::lock_guard<std::mutex> lock(m_Block);
    if(!m_pLast){ throw std::runtime_error("Cola vacia"); }
    Value_type  Valor=m_pFirst->GetValue();
    Node* pTemp=m_pFirst;
    m_pFirst=m_pFirst->GetNext();
    if (!m_pFirst) { m_pLast=nullptr;}
    delete pTemp;
    --m_nElements;
    return Valor;
}

template <typename Traits>
CQueue<Traits>::~CQueue() {
   Node* pTemp=m_pFirst;
   while(pTemp){
    Node *pNext=pTemp->GetNext();
    delete pTemp;
    pTemp=pNext;
   }
   m_pFirst=nullptr;
   m_pLast=nullptr;
}

#endif // __QUEUE_H__