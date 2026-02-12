#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <iostream>
#include "../general/types.h"
#include "../util.h"
#include  <mutex>
#include  <utility>
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
Node*  m_pFirst;
size_t m_nElements = 0;
mutable std::mutex m_Block;
public:
CQueue (){};
//COPY CONSTRUCTOR
CQueue (const CQueue &another) {
        std::lock_guard<std::mutex> lock(another.m_Block); 
        
    }
//MOVE CONSTRUCTOR
 CQueue(CQueue &&another) noexcept {
        std::lock_guard<std::mutex> lock(another.m_Block); 
        
    }
void Push(const Value_type &Val );
void Pop();
size_t getSize(){ return m_nElements;  }
virtual ~CQueue();



 friend ostream &operator<<(ostream &os, CQueue<Traits> &container){
        std::lock_guard<std::mutex> lock(container.m_Block);
        
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
    
    ++m_nElements;
}
template <typename Traits>
void  CQueue<Traits>::Pop(){
 std::lock_guard<std::mutex> lock(m_Block);
    
    --m_nElements;
}

template <typename Traits>
CQueue<Traits>::~CQueue() {
    
}

#endif // __QUEUE_H__