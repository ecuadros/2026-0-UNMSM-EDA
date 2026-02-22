#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <iostream>
#include <utility>
#include <mutex>
#include "../general/types.h"
#include "../util.h"

using namespace std;

// Traits
template <typename T, typename _Func>
struct QueueTrait{
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct QueueAscendingTrait : 
    public QueueTrait<T, std::greater<T> >{
};

template <typename T>
struct QueueDescendingTrait : 
    public QueueTrait<T, std::less<T> >{
};

// Node
template <typename Traits>
class NodeQueue{
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeQueue<Traits>;
private:
    value_type m_data;
    ref_type   m_ref;
    Node *m_pNext = nullptr;

public:
    NodeQueue(){}
    NodeQueue( value_type _value, ref_type _ref = -1, Node *_pNext = nullptr)
        : m_data(_value), m_ref(_ref), m_pNext(_pNext){   }
        
    value_type  GetValue   () const { return m_data; }
    value_type &GetValueRef() { return m_data; }

    ref_type    GetRef     () const { return m_ref;   }
    ref_type   &GetRefRef  () { return m_ref;   }

    Node      * GetNext     () const { return m_pNext;   }
    Node      *&GetNextRef  () { return m_pNext;   }

    Node &operator=(const Node &another){
        m_data = another.GetValue();
        m_ref   = another.GetRef();
        return *this;
    }
    bool operator==(const Node &another) const
    { return m_data == another.GetValue();   }
    bool operator<(const Node &another) const
    { return m_data < another.GetValue();   }
};

// Forward declarations
template <typename Traits> class CQueue;
template <typename Traits> ostream &operator<<(ostream &os, CQueue<Traits> &container);
template <typename Traits> istream &operator>>(istream &is, CQueue<Traits> &container);

// Queue (FIFO)
template <typename Traits>
class CQueue {
public:
    using  value_type  = typename Traits::value_type;
    using  Node = NodeQueue<Traits>;

private:
    Node *m_pFront = nullptr;
    Node *m_pRear  = nullptr;
    size_t m_nElements = 0;
    mutable mutex m_mtx;

public:
    CQueue(){}

    // constructor copia
    CQueue(const CQueue &otro);

    // move constructor
    CQueue(CQueue &&otro) noexcept;

    // destructor
    virtual ~CQueue();

    // push
    void push(value_type val, ref_type ref);

    // pop
    value_type pop();

    size_t getSize() const { return m_nElements; }
    bool   empty()   const { return m_nElements == 0; }

    // operator<<
    friend ostream &operator<< <>(ostream &os, CQueue<Traits> &container);

    // operator>>
    friend istream &operator>> <>(istream &is, CQueue<Traits> &container);
};

// constructor copia
template <typename Traits>
CQueue<Traits>::CQueue(const CQueue &otro){
    lock_guard<mutex> lock(otro.m_mtx);
    Node *pCurr = otro.m_pFront;
    while( pCurr ){
        Node *pNew = new Node(pCurr->GetValue(), pCurr->GetRef());
        if( !m_pFront )
            m_pFront = pNew;
        else
            m_pRear->GetNextRef() = pNew;
        m_pRear = pNew;
        ++m_nElements;
        pCurr = pCurr->GetNext();
    }
}

// move constructor
template <typename Traits>
CQueue<Traits>::CQueue(CQueue &&otro) noexcept {
    lock_guard<mutex> lock(otro.m_mtx);
    m_pFront    = otro.m_pFront;
    m_pRear     = otro.m_pRear;
    m_nElements = otro.m_nElements;
    otro.m_pFront    = nullptr;
    otro.m_pRear     = nullptr;
    otro.m_nElements = 0;
}

// destructor
template <typename Traits>
CQueue<Traits>::~CQueue(){
    lock_guard<mutex> lock(m_mtx);
    Node *pCurr = m_pFront;
    while( pCurr ){
        Node *pNxt = pCurr->GetNext();
        delete pCurr;
        pCurr = pNxt;
    }
}

// push
template <typename Traits>
void CQueue<Traits>::push(value_type val, ref_type ref){
    lock_guard<mutex> lock(m_mtx);
    Node *pNew = new Node(val, ref);
    if( !m_pRear )
        m_pFront = pNew;
    else
        m_pRear->GetNextRef() = pNew;
    m_pRear = pNew;
    ++m_nElements;
}

// pop
template <typename Traits>
typename CQueue<Traits>::value_type CQueue<Traits>::pop(){
    lock_guard<mutex> lock(m_mtx);
    if( !m_pFront ) throw runtime_error("Queue vacia");
    Node *pOld = m_pFront;
    value_type val = pOld->GetValue();
    m_pFront = m_pFront->GetNext();
    if( !m_pFront ) m_pRear = nullptr;
    delete pOld;
    --m_nElements;
    return val;
}

// operator<<
template <typename Traits>
ostream &operator<<(ostream &os, CQueue<Traits> &container){
    using Node = NodeQueue<Traits>;
    os << "CQueue: size = " << container.getSize() << endl;
    os << "[";
    Node *pCurr = container.m_pFront;
    while( pCurr ){
        os << "(" << pCurr->GetValue() << ":" << pCurr->GetRef() << ")";
        pCurr = pCurr->GetNext();
        if( pCurr ) os << ",";
    }
    os << "]" << endl;
    return os;
}

// operator>>
template <typename Traits>
istream &operator>>(istream &is, CQueue<Traits> &container){
    using value_type = typename Traits::value_type;
    size_t size;
    if( is >> size ){
        value_type value;
        ref_type ref;
        for( size_t i = 0; i < size; ++i ){
            is >> value >> ref;
            container.push(value, ref);
        }
    }
    return is;
}

#endif // __QUEUE_H__