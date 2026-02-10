#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__
#include <iostream>
#include "../general/types.h"
#include "../util.h"
#include "GeneralIterator.h"
#include <mutex>
#include <assert.h>
using namespace std;

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
    NodeLinkedList( value_type _value, ref_type _ref = -1, Node *_pNext = nullptr)
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

template <typename Container>
class LinkedListForwardIterator : public GeneralIterator< Container >{
    using Parent = GeneralIterator< Container >;
    using Node   = typename Container::Node;
public:
    LinkedListForwardIterator( Container *pContainer, Node* pNode = nullptr)
        : Parent(pContainer, 0), m_pCurrent(pNode){} 

    LinkedListForwardIterator<Container> &operator++(){
        if ( m_pCurrent != nullptr ){
            m_pCurrent = m_pCurrent->GetNext();
            
            if(m_pCurrent == this->m_pContainer->getRoot())
                m_pCurrent = nullptr;

            this->m_pos++;
        }
        return *this;
    }
    typename Container::value_type &operator*(){
        return m_pCurrent->GetValueRef();
    }

    bool operator!=(const LinkedListForwardIterator<Container> &another)const{
        return m_pCurrent != another.m_pCurrent;       
    }
private:
    Node *m_pCurrent;
};

template <typename Traits>
class CLinkedList {
    using  value_type  = typename Traits::value_type;
    using  Node = NodeLinkedList<Traits>;
    using  forward_iterator  = LinkedListForwardIterator < CLinkedList<Traits> >;

    friend forward_iterator;
    friend class GeneralIterator< CLinkedList<Traits> >;

private:
    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;
    mutable std:: mutex m_mtx;

public:
    CLinkedList(){}
    CLinkedList(const CLinkedList<Traits>& other);
    CLinkedList(CLinkedList<Traits>&& other) noexcept{
        lock_guard<mutex> lock(other.m_mtx);
        m_pRoot           = other.m_pRoot;
        m_pLast           = other.m_pLast;
        m_nElements       = other.m_nElements;
        other.m_pRoot     = nullptr;
        other.m_pLast     = nullptr;
        other.m_nElements = 0;
    }
    virtual ~CLinkedList();

    value_type& operator[](size_t index);
    size_t getSize() const { return m_nElements; }
    Node * getRoot() const { return m_pRoot; }

    void push_back(const value_type &val, ref_type ref);
    void Insert(const value_type &val, ref_type ref);
    
    forward_iterator begin() { return forward_iterator(this, m_pRoot); }
    forward_iterator end  () { return forward_iterator(this, nullptr); }
    
    template <typename ObjFunc, typename ...Args>
    void Foreach ( ObjFunc of, Args... args ){
        ::Foreach(*this, of, args...);
    }

    template <typename ObjFunc, typename ...Args>
    auto FirstThat ( ObjFunc of, Args... args ){
        return ::FirstThat(*this, of, args...);
    }
private:
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref);

    friend ostream &operator<<(ostream &os, CLinkedList<Traits> &container){
        os << "CLinkedList: size = " << container.getSize() << endl;
        os << "[";
        Node *pCurrent = container.m_pRoot;

        for(size_t i = 0; i < container.getSize(); ++i) {
            if (!pCurrent)
                break;
            os << "(" << pCurrent->GetValue() << ": " << pCurrent->GetRef() << ")";
            pCurrent = pCurrent->GetNext();
            if( i < container.getSize() - 1 ) 
                os << ", ";
        }
        os << "]" << endl;
        return os;
    }

    friend istream &operator>>(istream &is, CLinkedList<Traits> &Container){
        value_type val;
        ref_type   ref;
        if ( is >> val >> ref )
            Container.push_back(val,ref);
        return is;
    }

    Node* get_node_at(size_t index) {
        assert(index < m_nElements);
        Node *pTemp = m_pRoot;
        for (size_t i = 0; i < index; ++i)
            pTemp = pTemp->GetNext();
        return pTemp;
    }
};

template <typename Traits>
CLinkedList<Traits>::CLinkedList(const CLinkedList<Traits>& other){
    lock_guard<mutex> lock(other.m_mtx);
    m_pRoot = nullptr;
    m_pLast = nullptr;
    m_nElements = 0;
    Node* pTemp = other.m_pRoot;
    while ( pTemp ) {
        this->push_back(pTemp->GetValue(), pTemp->GetRef());
        pTemp = pTemp->GetNext();
    }
}

template <typename Traits>
CLinkedList<Traits>::~CLinkedList(){
    lock_guard<mutex> lock(m_mtx);

    if (!m_pRoot)
        return;

    if (m_pLast && m_pLast->GetNext() == m_pRoot) 
        m_pLast->GetNextRef() = nullptr;
    
    Node *pCurrent = m_pRoot;

    while( pCurrent != nullptr ){
        Node *pNext = pCurrent->GetNext();
        delete pCurrent;
        pCurrent = pNext;
    }
    m_pRoot = nullptr;
    m_pLast = nullptr;
}

template <typename Traits>
void CLinkedList<Traits>::push_back(const value_type &val, ref_type ref){
    lock_guard<mutex> lock (m_mtx);
    Node *pNewNode = new Node(val, ref);
    pNewNode->GetNextRef() = nullptr;
    if( !m_pRoot )
        m_pRoot = pNewNode;
    else {
        if (!m_pLast){
            Node *pTemp = m_pRoot;
            while (pTemp->GetNext() != nullptr)
                pTemp = pTemp->GetNext();
            m_pLast = pTemp;
        }
        m_pLast->GetNextRef() = pNewNode;
    }
    m_pLast = pNewNode;
    ++m_nElements;
}

template <typename Traits>
void CLinkedList<Traits>::InternalInsert(Node *&rParent, const value_type &val, ref_type ref){
    typename Traits::Func compare;
    if (!rParent || compare(rParent->GetValue(), val)) {
        Node *pNew = new Node(val, ref, rParent);
        rParent = pNew; 
        if (pNew->GetNext() == nullptr || pNew->GetNext() == m_pRoot) 
            m_pLast = pNew;
        ++m_nElements;
        return;
    }
    if (rParent->GetNext() == m_pRoot) {
        Node *pNew = new Node(val, ref, m_pRoot);
        rParent->GetNextRef() = pNew;
        m_pLast = pNew;
        ++m_nElements;
        return;
    }
    InternalInsert(rParent->GetNextRef(), val, ref);
}

template <typename Traits>
void CLinkedList<Traits>::Insert(const value_type &val, ref_type ref){
    lock_guard<mutex> lock(m_mtx);
    InternalInsert(m_pRoot, val, ref);
}

template <typename Traits>
typename CLinkedList<Traits>::value_type& CLinkedList<Traits>::operator[](size_t index) {
    lock_guard<mutex> lock(m_mtx); 
    return get_node_at(index)->GetValueRef();
}

#endif // __LINKEDLIST_H__
