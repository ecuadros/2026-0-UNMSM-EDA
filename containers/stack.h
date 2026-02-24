#ifndef __STACK_H__
#define __STACK_H__

#include <iostream>
#include <utility>
#include <mutex>
#include "../general/types.h"
#include "../util.h"

using namespace std;

// Traits
template <typename T, typename _Func>
struct StackTrait{
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct StackAscendingTrait : 
    public StackTrait<T, std::greater<T> >{
};

template <typename T>
struct StackDescendingTrait : 
    public StackTrait<T, std::less<T> >{
};

// Node
template <typename Traits>
class NodeStack{
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeStack<Traits>;
private:
    value_type m_data;
    ref_type   m_ref;
    Node *m_pNext = nullptr;

public:
    NodeStack(){}
    NodeStack( value_type _value, ref_type _ref = -1, Node *_pNext = nullptr)
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
template <typename Traits> class CStack;
template <typename Traits> ostream &operator<<(ostream &os, CStack<Traits> &container);
template <typename Traits> istream &operator>>(istream &is, CStack<Traits> &container);

// Stack (LIFO)
template <typename Traits>
class CStack {
public:
    using  value_type  = typename Traits::value_type;
    using  Node = NodeStack<Traits>;

private:
    Node *m_pTop = nullptr;
    size_t m_nElements = 0;
    mutable mutex m_mtx;

public:
    CStack(){}

    // constructor copia
    CStack(const CStack &otro);

    // move constructor
    CStack(CStack &&otro) noexcept;

    // destructor
    virtual ~CStack();

    // push
    void push(value_type val, ref_type ref);

    // pop
    value_type pop();

    size_t getSize() const { return m_nElements; }
    bool   empty()   const { return m_nElements == 0; }

    // operator<<
    friend ostream &operator<< <>(ostream &os, CStack<Traits> &container);

    // operator>>
    friend istream &operator>> <>(istream &is, CStack<Traits> &container);
};

// constructor copia
template <typename Traits>
CStack<Traits>::CStack(const CStack &otro){
    lock_guard<mutex> lock(otro.m_mtx);
    Node *pCurr = otro.m_pTop;
    Node *pTail = nullptr;
    while( pCurr ){
        Node *pNew = new Node(pCurr->GetValue(), pCurr->GetRef());
        if( !m_pTop )
            m_pTop = pNew;
        else
            pTail->GetNextRef() = pNew;
        pTail = pNew;
        ++m_nElements;
        pCurr = pCurr->GetNext();
    }
}

// move constructor
template <typename Traits>
CStack<Traits>::CStack(CStack &&otro) noexcept {
    lock_guard<mutex> lock(otro.m_mtx);
    m_pTop      = otro.m_pTop;
    m_nElements = otro.m_nElements;
    otro.m_pTop      = nullptr;
    otro.m_nElements = 0;
}

// destructor
template <typename Traits>
CStack<Traits>::~CStack(){
    lock_guard<mutex> lock(m_mtx);
    Node *pCurr = m_pTop;
    while( pCurr ){
        Node *pNxt = pCurr->GetNext();
        delete pCurr;
        pCurr = pNxt;
    }
}

// push
template <typename Traits>
void CStack<Traits>::push(value_type val, ref_type ref){
    lock_guard<mutex> lock(m_mtx);
    Node *pNew = new Node(val, ref, m_pTop);
    m_pTop = pNew;
    ++m_nElements;
}

// pop
template <typename Traits>
typename CStack<Traits>::value_type CStack<Traits>::pop(){
    lock_guard<mutex> lock(m_mtx);
    if( !m_pTop ) throw runtime_error("Stack vacio");
    Node *pOld = m_pTop;
    value_type val = pOld->GetValue();
    m_pTop = m_pTop->GetNext();
    delete pOld;
    --m_nElements;
    return val;
}

// operator<<
template <typename Traits>
ostream &operator<<(ostream &os, CStack<Traits> &container){
    using Node = NodeStack<Traits>;
    os << "CStack: size = " << container.getSize() << endl;
    os << "[";
    Node *pCurr = container.m_pTop;
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
istream &operator>>(istream &is, CStack<Traits> &container){
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

#endif // __STACK_H__