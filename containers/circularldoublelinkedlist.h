#ifndef __CIRCULAR_DOUBLE_LINKED_LIST_H__
#define __CIRCULAR_DOUBLE_LINKED_LIST_H__

#include <iostream>
#include <fstream>
#include "../general/types.h"
#include "../util.h"

using namespace std;

// Traits
template <typename T, typename _Func>
struct CDListTrait{
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct CDAscendingTrait : 
    public CDListTrait<T, std::greater<T> >{
};

template <typename T>
struct CDDescendingTrait : 
    public CDListTrait<T, std::less<T> >{
};

// Node
template <typename Traits>
class NodeCircularDoubleLinkedList{
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeCircularDoubleLinkedList<Traits>;
private:
    value_type m_data;
    ref_type   m_ref;
    Node *m_pNext = nullptr;
    Node *m_pPrev = nullptr;

public:
    NodeCircularDoubleLinkedList(){}
    NodeCircularDoubleLinkedList( value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref){   }
        
    value_type  GetValue   () const { return m_data; }
    value_type &GetValueRef() { return m_data; }
    ref_type    GetRef     () const { return m_ref;   }
    ref_type   &GetRefRef  () { return m_ref;   }
    Node      * GetNext     () const { return m_pNext;   }
    Node      *&GetNextRef  () { return m_pNext;   }
    Node      * GetPrev     () const { return m_pPrev;   }
    Node      *&GetPrevRef  () { return m_pPrev;   }

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

// ForwardIterator (circular: para cuando vuelve al root)
template <typename Traits>
class CCircularDoubleLinkedList;

template <typename Traits>
struct CDListForwardIterator {
    using value_type  = typename Traits::value_type;
    using Node        = NodeCircularDoubleLinkedList<Traits>;
    Node *m_pNode = nullptr;
    Node *m_pRoot = nullptr;

    CDListForwardIterator(Node *pNode = nullptr, Node *pRoot = nullptr) 
        : m_pNode(pNode), m_pRoot(pRoot) {}
    CDListForwardIterator(CDListForwardIterator<Traits> &another) 
        : m_pNode(another.m_pNode), m_pRoot(another.m_pRoot) {}
    virtual ~CDListForwardIterator(){}

    CDListForwardIterator<Traits> &operator++(){
        if( m_pNode ){
            m_pNode = m_pNode->GetNext();
            if( m_pNode == m_pRoot ) m_pNode = nullptr; // Vuelta completa
        }
        return *this;
    }
    bool operator!=(const CDListForwardIterator<Traits> &another){
        return m_pNode != another.m_pNode;
    }
    value_type &operator*(){ return m_pNode->GetValueRef(); }
};

// BackwardIterator (circular: para cuando vuelve al last)
template <typename Traits>
struct CDListBackwardIterator {
    using value_type  = typename Traits::value_type;
    using Node        = NodeCircularDoubleLinkedList<Traits>;
    Node *m_pNode = nullptr;
    Node *m_pLast = nullptr;

    CDListBackwardIterator(Node *pNode = nullptr, Node *pLast = nullptr) 
        : m_pNode(pNode), m_pLast(pLast) {}
    CDListBackwardIterator(CDListBackwardIterator<Traits> &another) 
        : m_pNode(another.m_pNode), m_pLast(another.m_pLast) {}
    virtual ~CDListBackwardIterator(){}

    CDListBackwardIterator<Traits> &operator++(){
        if( m_pNode ){
            m_pNode = m_pNode->GetPrev();
            if( m_pNode == m_pLast ) m_pNode = nullptr; // Vuelta completa
        }
        return *this;
    }
    bool operator!=(const CDListBackwardIterator<Traits> &another){
        return m_pNode != another.m_pNode;
    }
    value_type &operator*(){ return m_pNode->GetValueRef(); }
};

// CircularDoubleLinkedList
template <typename Traits>
class CCircularDoubleLinkedList {
public:
    using  value_type  = typename Traits::value_type;
    using  Node = NodeCircularDoubleLinkedList<Traits>;
    using  forward_iterator  = CDListForwardIterator<Traits>;
    using  backward_iterator = CDListBackwardIterator<Traits>;
    using  iterator = forward_iterator;

private:
    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;

public:
    // Constructor default
    CCircularDoubleLinkedList(){}
    
    // Constructor copia
    CCircularDoubleLinkedList(const CCircularDoubleLinkedList &otro){
        if( otro.m_pRoot ){
            Node *pCurr = otro.m_pRoot;
            do {
                push_back(pCurr->GetValue(), pCurr->GetRef());
                pCurr = pCurr->GetNext();
            } while( pCurr != otro.m_pRoot );
        }
    }

    // Move constructor
    CCircularDoubleLinkedList(CCircularDoubleLinkedList &&otro) noexcept {
        m_pRoot     = otro.m_pRoot;
        m_pLast     = otro.m_pLast;
        m_nElements = otro.m_nElements;
        otro.m_pRoot     = nullptr;
        otro.m_pLast     = nullptr;
        otro.m_nElements = 0;
    }

    // Destructor
    ~CCircularDoubleLinkedList(){
        if( m_pRoot ){
            m_pLast->GetNextRef() = nullptr; // Romper circulo forward
            m_pRoot->GetPrevRef() = nullptr; // Romper circulo backward
            Node *pCurr = m_pRoot;
            while( pCurr ){
                Node *pNxt = pCurr->GetNext();
                delete pCurr;
                pCurr = pNxt;
            }
        }
    }

    void push_back(value_type val, ref_type ref);
    void push_front(value_type val, ref_type ref);
    void Insert(const value_type &val, ref_type ref);
    
    size_t getSize() const { return m_nElements; }

    // Forward Iterator
    forward_iterator begin() { return forward_iterator(m_pRoot, m_pRoot); }
    forward_iterator end()   { return forward_iterator(nullptr, m_pRoot); }

    // Backward Iterator
    backward_iterator rbegin(){ return backward_iterator(m_pLast, m_pLast); }
    backward_iterator rend()  { return backward_iterator(nullptr, m_pLast); }

    // Operator<<
    friend ostream &operator<<(ostream &os, CCircularDoubleLinkedList<Traits> &container){
        os << "CCircularDoubleLinkedList: size = " << container.getSize() << endl;
        os << "[";
        if( container.m_pRoot ){
            Node *pCurr = container.m_pRoot;
            do {
                os << "(" << pCurr->GetValue() << ":" << pCurr->GetRef() << ")";
                pCurr = pCurr->GetNext();
                if( pCurr != container.m_pRoot ) os << ",";
            } while( pCurr != container.m_pRoot );
        }
        os << "]" << endl;
        return os;
    }

    // Operator>>
    friend istream &operator>>(istream &is, CCircularDoubleLinkedList<Traits> &container){
        size_t size;
        if( is >> size ){
            value_type val;
            ref_type ref;
            for( size_t i = 0; i < size; ++i ){
                is >> val >> ref;
                container.push_back(val, ref);
            }
        }
        return is;
    }

private:
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref);
};

// push_back
template <typename Traits>
void CCircularDoubleLinkedList<Traits>::push_back(value_type val, ref_type ref){
    Node *pNew = new Node(val, ref);
    if( !m_pRoot ){
        m_pRoot = pNew;
        m_pLast = pNew;
        pNew->GetNextRef() = pNew; // Apunta a si mismo
        pNew->GetPrevRef() = pNew;
    } else {
        pNew->GetPrevRef() = m_pLast;
        pNew->GetNextRef() = m_pRoot;
        m_pLast->GetNextRef() = pNew;
        m_pRoot->GetPrevRef() = pNew;
        m_pLast = pNew;
    }
    ++m_nElements;
}

// push_front
template <typename Traits>
void CCircularDoubleLinkedList<Traits>::push_front(value_type val, ref_type ref){
    Node *pNew = new Node(val, ref);
    if( !m_pRoot ){
        m_pRoot = pNew;
        m_pLast = pNew;
        pNew->GetNextRef() = pNew;
        pNew->GetPrevRef() = pNew;
    } else {
        pNew->GetNextRef() = m_pRoot;
        pNew->GetPrevRef() = m_pLast;
        m_pRoot->GetPrevRef() = pNew;
        m_pLast->GetNextRef() = pNew;
        m_pRoot = pNew;
    }
    ++m_nElements;
}

// InternalInsert (ordenado, circular)
template <typename Traits>
void CCircularDoubleLinkedList<Traits>::InternalInsert(Node *&rParent, const value_type &val, ref_type ref){
    // Lista vacia
    if( !m_pRoot ){
        Node *pNew = new Node(val, ref);
        m_pRoot = pNew;
        m_pLast = pNew;
        pNew->GetNextRef() = pNew;
        pNew->GetPrevRef() = pNew;
        ++m_nElements;
        return;
    }

    // Insertar al inicio (menor que root)
    if( rParent == m_pRoot && rParent->GetValue() > val ){
        Node *pNew = new Node(val, ref);
        pNew->GetNextRef() = m_pRoot;
        pNew->GetPrevRef() = m_pLast;
        m_pRoot->GetPrevRef() = pNew;
        m_pLast->GetNextRef() = pNew;
        m_pRoot = pNew;
        ++m_nElements;
        return;
    }

    // Buscar posicion: si el siguiente es root o el siguiente es mayor
    if( rParent->GetNext() == m_pRoot || rParent->GetNext()->GetValue() > val ){
        Node *pNew = new Node(val, ref);
        Node *pNext = rParent->GetNext();
        pNew->GetNextRef() = pNext;
        pNew->GetPrevRef() = rParent;
        rParent->GetNextRef() = pNew;
        pNext->GetPrevRef() = pNew;
        if( rParent == m_pLast ) m_pLast = pNew;
        ++m_nElements;
        return;
    }

    InternalInsert(rParent->GetNextRef(), val, ref);
}

// Insert
template <typename Traits>
void CCircularDoubleLinkedList<Traits>::Insert(const value_type &val, ref_type ref){
    InternalInsert(m_pRoot, val, ref);
}

#endif // __CIRCULAR_DOUBLE_LINKED_LIST_H__
