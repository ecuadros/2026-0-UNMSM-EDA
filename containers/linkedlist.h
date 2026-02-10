#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include <iostream>
#include <mutex>
#include <stdexcept>    
#include "../general/types.h"
#include "../util.h"

using namespace std;


// TRAITS

template <typename T, typename _Func>
struct ListTrait{
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct AscendingTrait : public ListTrait<T, greater<T> > {};

template <typename T>
struct DescendingTrait : public ListTrait<T, less<T> > {};


// Forward declarations
template <typename Traits> class NodeLinkedList;
template <typename Traits> class CLinkedList;
template <typename Traits> class LinkedListForwardIterator;


// NODO

template <typename Traits>
class NodeLinkedList{
    template <typename T> friend class CLinkedList;
    template <typename T> friend class LinkedListForwardIterator;
    
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeLinkedList<Traits>;
private:
    value_type m_data;
    ref_type   m_ref;
    Node *m_pNext = nullptr;

public:
    NodeLinkedList() {}
    
    NodeLinkedList(value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref) { }
        
    NodeLinkedList(value_type _value, ref_type _ref, Node *pNext)
        : m_data(_value), m_ref(_ref), m_pNext(pNext) { }
        
    value_type  GetValue   () const { return m_data; }
    value_type &GetValueRef()       { return m_data; }

    ref_type    GetRef     () const { return m_ref;   }
    ref_type   &GetRefRef  ()       { return m_ref;   }

    Node      * GetNext     () const { return m_pNext;   }
    Node      *&GetNextRef  ()       { return m_pNext;   }
};


// ITERADOR (Forward)

template <typename Traits>
class LinkedListForwardIterator {
    using Node = NodeLinkedList<Traits>;
    using value_type = typename Traits::value_type;
    Node *m_pCurrent;

public:
    LinkedListForwardIterator(Node *pNode) : m_pCurrent(pNode) {}
    
    value_type &operator*() { return m_pCurrent->GetValueRef(); }
    
    LinkedListForwardIterator &operator++() {
        if(m_pCurrent) m_pCurrent = m_pCurrent->GetNext();
        return *this;
    }
    
    bool operator!=(const LinkedListForwardIterator &other) const {
        return m_pCurrent != other.m_pCurrent;
    }
    
    bool operator==(const LinkedListForwardIterator &other) const {
        return m_pCurrent == other.m_pCurrent;
    }
};

// CLASE LISTA ENLAZADA

template <typename Traits>
class CLinkedList {
    using  value_type  = typename Traits::value_type;
    using  forward_iterator  = LinkedListForwardIterator<Traits>;
    using  Node = NodeLinkedList<Traits>;

    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;
    
    // Concurrencia
    mutable mutex m_mutex;

public:
    CLinkedList() {}
    
    // Constructor Copia
    CLinkedList(const CLinkedList &other){
        lock_guard<mutex> lock(other.m_mutex);
        Node *pCurrent = other.m_pRoot;
        while(pCurrent){
            push_back(pCurrent->GetValue(), pCurrent->GetRef());
            pCurrent = pCurrent->GetNext();
        }
    }
    
    // Move Constructor
    CLinkedList(CLinkedList &&other) noexcept {
        lock_guard<mutex> lock(other.m_mutex);
        m_pRoot = other.m_pRoot;
        m_pLast = other.m_pLast;
        m_nElements = other.m_nElements;
        
        other.m_pRoot = nullptr;
        other.m_pLast = nullptr;
        other.m_nElements = 0;
    }
    
    // Destructor
    virtual ~CLinkedList(){ Clear(); }
    
    // Iterators begin/end
    forward_iterator begin() { return forward_iterator(m_pRoot); }
    forward_iterator end()   { return forward_iterator(nullptr); }
    
    // Operator []
    value_type &operator[](size_t index){
        lock_guard<mutex> lock(m_mutex);
        if(index >= m_nElements) throw out_of_range("Index out of range");
        Node *pCurrent = m_pRoot;
        for(size_t i = 0; i < index; ++i) pCurrent = pCurrent->GetNext();
        return pCurrent->GetValueRef();
    }
    
    const value_type &operator[](size_t index) const {
        lock_guard<mutex> lock(m_mutex);
        if(index >= m_nElements) throw out_of_range("Index out of range");
        Node *pCurrent = m_pRoot;
        for(size_t i = 0; i < index; ++i) pCurrent = pCurrent->GetNext();
        return pCurrent->GetValue();
    }

    // Foreach
    template<typename Function, typename... Args>
    void Foreach(Function fn, Args... args){
        lock_guard<mutex> lock(m_mutex);
        Node *pCurrent = m_pRoot;
        while(pCurrent){
            fn(pCurrent->GetValueRef(), args...);
            pCurrent = pCurrent->GetNext();
        }
    }
    
    // FirstThat
    template<typename Predicate>
    value_type *FirstThat(Predicate pred){
        lock_guard<mutex> lock(m_mutex);
        Node *pCurrent = m_pRoot;
        while(pCurrent){
            if(pred(pCurrent->GetValue())) return &(pCurrent->GetValueRef());
            pCurrent = pCurrent->GetNext();
        }
        return nullptr;
    }
    
    void Clear(){
        lock_guard<mutex> lock(m_mutex);
        Node *pCurrent = m_pRoot;
        while(pCurrent){
            Node *pNext = pCurrent->GetNext();
            delete pCurrent;
            pCurrent = pNext;
        }
        m_pRoot = nullptr;
        m_pLast = nullptr;
        m_nElements = 0;
    }

    void push_back(const value_type &val, ref_type ref = -1);
    void Insert(const value_type &val, ref_type ref = -1);
    size_t getSize() const { return m_nElements;  }
    
private:
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref);

    // Operator << (Output)
    friend ostream &operator<<(ostream &os, CLinkedList<Traits> &container){
        lock_guard<mutex> lock(container.m_mutex);
        os << "CLinkedList: size = " << container.getSize() << endl;
        os << "[ ";
        Node *pCurrent = container.m_pRoot;
        while(pCurrent){
            os << "(" << pCurrent->GetValue() << ":" << pCurrent->GetRef() << ")";
            pCurrent = pCurrent->GetNext();
            if(pCurrent) os << ", ";
        }
        os << " ]" << endl;
        return os;
    }

   // Operator >> (Input)
    friend istream &operator>>(istream &is, CLinkedList<Traits> &container){
        // ERROR ANTERIOR: value_type val; (El compilador no lo encontraba)
        typename Traits::value_type val; 
        ref_type ref;

        if(is >> val >> ref){
             container.push_back(val, ref);
        }
        return is;
    }
};

// IMPLEMENTACIONES

template <typename Traits>
void CLinkedList<Traits>::push_back(const value_type &val, ref_type ref){
    lock_guard<mutex> lock(m_mutex);
    Node *pNewNode = new Node(val, ref);
    if( !m_pRoot ){
        m_pRoot = pNewNode;
        m_pLast = pNewNode;
    } else {
        m_pLast->GetNextRef() = pNewNode;
        m_pLast = pNewNode;
    }
    ++m_nElements;
}

template <typename Traits>
void CLinkedList<Traits>::InternalInsert(Node *&rParent, const value_type &val, ref_type ref){
    
    if( !rParent || typename Traits::Func()(rParent->GetValue(), val) ){
        Node *pNew = new Node(val, ref, rParent);
        rParent = pNew;

        if(pNew->GetNext() == nullptr) m_pLast = pNew;
        ++m_nElements;
        return;
    }
    InternalInsert(rParent->GetNextRef(), val, ref);
}

template <typename Traits>
void CLinkedList<Traits>::Insert(const value_type &val, ref_type ref){
    lock_guard<mutex> lock(m_mutex);
    InternalInsert(m_pRoot, val, ref);
}

#endif // __LINKEDLIST_H__