#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__
#include <iostream>
#include "../general/types.h"
#include "../util.h"
using namespace std;

// TODO: Traits para listas enlazadas
template <typename T, typename _Func>
struct ListTrait{
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct AscendingTrait : 
    public ListTrait<T, std::greater<T> >{
};

template <typename T>
struct DescendingTrait : 
    public ListTrait<T, std::less<T> >{
};

// Iterators para listas enlazadas

template <typename Traits>
class NodeLinkedList{

    using  value_type  = typename Traits::value_type;
    using  Node        = NodeLinkedList<Traits>;
    
private:
    value_type m_data;
    ref_type   m_ref;
    Node *m_pNext = nullptr;

public:
    NodeLinkedList(){}
    NodeLinkedList( value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref){   }
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

template <typename Container>
class LinkedListForwardIterator{
       using value_type=typename Container::value_type;
       using Node      =typename Container::Node;
       private:
       Node *m_pCurrent =nullptr;
       public:
      LinkedListForwardIterator(Node *pNode) : m_pCurrent(pNode) {}
      bool operator!=(const LinkedListForwardIterator &other) const {
        return m_pCurrent != other.m_pCurrent;
    }
      LinkedListForwardIterator &operator++() {
        if (m_pCurrent) {
            m_pCurrent = m_pCurrent->GetNext(); 
        }
        return *this;
    }
    value_type &operator*() {
        return m_pCurrent->GetValueRef();
    }  
};
template <typename Traits>
class CLinkedList {
    using  value_type  = typename Traits::value_type;
     using  forward_iterator  = LinkedListForwardIterator < CLinkedList<Traits> >;
     friend forward_iterator;
    
    using  Node = NodeLinkedList<Traits>;

    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;
public:
    CLinkedList(){}
    virtual ~CLinkedList();
    // TODO: Constructor copia
    // TODO: Move Constructor
    // TODO: Destructor seguro y virtual
    // TODO: Concurrencia (mutex)
    // TODO: Iterators begin() end()
    // TODO: Operadores de acceso []

    void push_back(const value_type &val, ref_type ref);
    void Insert(const value_type &val, ref_type ref);
    size_t getSize(){ return m_nElements;  }
    forward_iterator begin() {
        return forward_iterator(m_pRoot); 
    }

    forward_iterator end() {
        return forward_iterator(nullptr); 
    }
private:
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref);

    // TODO: Persistencia (write)
    friend ostream &operator<<(ostream &os, CLinkedList<Traits> &container){
        os << "CLinkedList: size = " << container.getSize() << endl;
        os << "[";
        Node *pCurrent = container.m_pRoot;
        while (pCurrent != nullptr) {
            os << "(" << pCurrent->GetValue() << ":" << pCurrent->GetRef() << ")";
            if (pCurrent->GetNext() != nullptr) {
                os << ", ";
            }
            pCurrent = pCurrent->GetNext();
        }
        
        os << "]" << endl;
        return os;
    }
    // TODO: Persistencia (read)
};

template <typename Traits>
void CLinkedList<Traits>::push_back(const value_type &val, ref_type ref){
    Node *pNewNode = new Node(val, ref);
    if( !m_pRoot )
        m_pRoot = pNewNode;
    m_pLast = pNewNode;
    ++m_nElements;
};

template <typename Traits>
void CLinkedList<Traits>::InternalInsert(Node *&rParent, const value_type &val, ref_type ref){
    // TODO: Agregar algo para el caso de circular
    if( !rParent || rParent->GetValue() > val ){
        Node *pNew = new Node(val, ref);
        pNew->GetNextRef() = rParent;
        rParent = pNew;
        ++m_nElements;
        return;
    }
    InternalInsert(rParent->GetNextRef(), val, ref);
}

template <typename Traits>
void CLinkedList<Traits>::Insert(const value_type &val, ref_type ref){
    InternalInsert(m_pRoot, val, ref);
}

template <typename Traits>
CLinkedList<Traits>::~CLinkedList() {
    Node *pCurrent = m_pRoot;
    while (pCurrent) {
        Node *pNext = pCurrent->GetNext(); 
        delete pCurrent;                   
        pCurrent = pNext;                  
    }
    m_pRoot = nullptr; 
}

#endif // __LINKEDLIST_H__
