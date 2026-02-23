#ifndef __DOUBLE_LINKED_LIST_H__
#define __DOUBLE_LINKED_LIST_H__

#include <iostream>
#include <fstream>
#include "../general/types.h"
#include "../util.h"

using namespace std;

// Traits
template <typename T, typename _Func>
struct DListTrait{
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct DAscendingTrait : 
    public DListTrait<T, std::greater<T> >{
};

template <typename T>
struct DDescendingTrait : 
    public DListTrait<T, std::less<T> >{
};

// Node
template <typename Traits>
class NodeDoubleLinkedList{
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeDoubleLinkedList<Traits>;
private:
    value_type m_data;
    ref_type   m_ref;
    Node *m_pNext = nullptr;
    Node *m_pPrev = nullptr;

public:
    NodeDoubleLinkedList(){}
    NodeDoubleLinkedList( value_type _value, ref_type _ref = -1)
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

// ForwardIterator
template <typename Traits>
class CDoubleLinkedList;

template <typename Traits>
struct DListForwardIterator {
    using value_type  = typename Traits::value_type;
    using Node        = NodeDoubleLinkedList<Traits>;
    Node *m_pNode = nullptr;

    DListForwardIterator(Node *pNode = nullptr) : m_pNode(pNode) {}
    DListForwardIterator(DListForwardIterator<Traits> &another) : m_pNode(another.m_pNode) {}
    virtual ~DListForwardIterator(){}

    DListForwardIterator<Traits> &operator++(){
        if( m_pNode ) m_pNode = m_pNode->GetNext();
        return *this;
    }
    bool operator!=(const DListForwardIterator<Traits> &another){
        return m_pNode != another.m_pNode;
    }
    value_type &operator*(){ return m_pNode->GetValueRef(); }
};

// BackwardIterator
template <typename Traits>
struct DListBackwardIterator {
    using value_type  = typename Traits::value_type;
    using Node        = NodeDoubleLinkedList<Traits>;
    Node *m_pNode = nullptr;

    DListBackwardIterator(Node *pNode = nullptr) : m_pNode(pNode) {}
    DListBackwardIterator(DListBackwardIterator<Traits> &another) : m_pNode(another.m_pNode) {}
    virtual ~DListBackwardIterator(){}

    DListBackwardIterator<Traits> &operator++(){
        if( m_pNode ) m_pNode = m_pNode->GetPrev();
        return *this;
    }
    bool operator!=(const DListBackwardIterator<Traits> &another){
        return m_pNode != another.m_pNode;
    }
    value_type &operator*(){ return m_pNode->GetValueRef(); }
};

// DoubleLinkedList
template <typename Traits>
class CDoubleLinkedList {
public:
    using  value_type  = typename Traits::value_type;
    using  Node = NodeDoubleLinkedList<Traits>;
    using  forward_iterator  = DListForwardIterator<Traits>;
    using  backward_iterator = DListBackwardIterator<Traits>;
    using  iterator = forward_iterator;

private:
    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;

public:
    // Constructor default
    CDoubleLinkedList(){}
    
    // Constructor copia
    CDoubleLinkedList(const CDoubleLinkedList &otro){
        Node *pCurr = otro.m_pRoot;
        while( pCurr ){
            push_back(pCurr->GetValue(), pCurr->GetRef());
            pCurr = pCurr->GetNext();
        }
    }

    // Move constructor
    CDoubleLinkedList(CDoubleLinkedList &&otro) noexcept {
        m_pRoot     = otro.m_pRoot;
        m_pLast     = otro.m_pLast;
        m_nElements = otro.m_nElements;
        otro.m_pRoot     = nullptr;
        otro.m_pLast     = nullptr;
        otro.m_nElements = 0;
    }

    // Destructor
    ~CDoubleLinkedList(){
        Node *pCurr = m_pRoot;
        while( pCurr ){
            Node *pNxt = pCurr->GetNext();
            delete pCurr;
            pCurr = pNxt;
        }
    }

    void push_back(value_type val, ref_type ref);
    void push_front(value_type val, ref_type ref);
    void Insert(const value_type &val, ref_type ref);
    
    size_t getSize() const { return m_nElements; }

    // Forward Iterator
    forward_iterator begin() { return forward_iterator(m_pRoot); }
    forward_iterator end()   { return forward_iterator(nullptr); }

    // Backward Iterator
    backward_iterator rbegin(){ return backward_iterator(m_pLast); }
    backward_iterator rend()  { return backward_iterator(nullptr); }

    // Operator<<
    friend ostream &operator<<(ostream &os, CDoubleLinkedList<Traits> &container){
        os << "CDoubleLinkedList: size = " << container.getSize() << endl;
        os << "[";
        Node *pCurr = container.m_pRoot;
        while( pCurr ){
            os << "(" << pCurr->GetValue() << ":" << pCurr->GetRef() << ")";
            pCurr = pCurr->GetNext();
            if( pCurr ) os << ",";
        }
        os << "]" << endl;
        return os;
    }

    // Operator>>
    friend istream &operator>>(istream &is, CDoubleLinkedList<Traits> &container){
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
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref, Node *pPrev);
};

// push_back
template <typename Traits>
void CDoubleLinkedList<Traits>::push_back(value_type val, ref_type ref){
    Node *pNew = new Node(val, ref);
    if( !m_pRoot ){
        m_pRoot = pNew;
    } else {
        m_pLast->GetNextRef() = pNew;
        pNew->GetPrevRef() = m_pLast;
    }
    m_pLast = pNew;
    ++m_nElements;
}

// push_front
template <typename Traits>
void CDoubleLinkedList<Traits>::push_front(value_type val, ref_type ref){
    Node *pNew = new Node(val, ref);
    if( !m_pRoot ){
        m_pLast = pNew;
    } else {
        pNew->GetNextRef() = m_pRoot;
        m_pRoot->GetPrevRef() = pNew;
    }
    m_pRoot = pNew;
    ++m_nElements;
}

// InternalInsert (ordenado, mantiene prev)
template <typename Traits>
void CDoubleLinkedList<Traits>::InternalInsert(Node *&rParent, const value_type &val, ref_type ref, Node *pPrev){
    if( !rParent || rParent->GetValue() > val ){
        Node *pNew = new Node(val, ref);
        pNew->GetNextRef() = rParent;
        pNew->GetPrevRef() = pPrev;
        if( rParent )
            rParent->GetPrevRef() = pNew;
        rParent = pNew;
        ++m_nElements;
        if( !pNew->GetNext() )
            m_pLast = pNew;
        return;
    }
    InternalInsert(rParent->GetNextRef(), val, ref, rParent);
}

// Insert
template <typename Traits>
void CDoubleLinkedList<Traits>::Insert(const value_type &val, ref_type ref){
    InternalInsert(m_pRoot, val, ref, nullptr);
}

#endif // __DOUBLE_LINKED_LIST_H__
