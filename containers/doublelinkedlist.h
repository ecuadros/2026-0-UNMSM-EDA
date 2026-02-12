#ifndef __DOUBLE_LINKED_LIST_H__
#define __DOUBLE_LINKED_LIST_H__

#include <iostream>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include "../general/types.h"
#include "GeneralIterator.h"

using namespace std;

//iterator
template <typename Container>
class DoubleLinkedListForwardIterator : public GeneralIterator<Container>{
    using Parent = GeneralIterator<Container>;
    using Node   = typename Container::Node;
public:
    DoubleLinkedListForwardIterator(Container *pContainer, Node *pNode)
                      : Parent(pContainer, pNode){}
    DoubleLinkedListForwardIterator(const DoubleLinkedListForwardIterator<Container> &another)
                      : Parent(another){}

    DoubleLinkedListForwardIterator<Container> &operator++(){
        if(Parent::m_pNode){
            Parent::m_pNode = Parent::m_pNode->GetNext();
            if (Parent::m_pNode == Parent::m_pContainer->m_pRoot)
                Parent::m_pNode = nullptr;
        }    
        return *this;
    }
    DoubleLinkedListForwardIterator operator+(size_t n) const {
        DoubleLinkedListForwardIterator temp = *this;
        for (size_t i=0; i<n && temp.m_pNode; ++i) {
            temp.m_pNode = temp.m_pNode->GetNext();
            if (temp.m_pNode == Parent::m_pContainer->m_pRoot)
                temp.m_pNode = nullptr;
        }
        return temp;
    }
};

template <typename Container>
class DoubleLinkedListBackwardIterator : public GeneralIterator<Container>{
    using Parent = GeneralIterator<Container>;
    using Node   = typename Container::Node;
public:
    DoubleLinkedListBackwardIterator(Container *pContainer, Node *pNode)
                      : Parent(pContainer, pNode){}
    DoubleLinkedListBackwardIterator(const DoubleLinkedListBackwardIterator<Container> &another)
                      : Parent(another){}

    DoubleLinkedListBackwardIterator<Container> &operator++(){
        if(Parent::m_pNode){
            Parent::m_pNode = Parent::m_pNode->GetPrev();
            if (Parent::m_pNode == Parent::m_pContainer->m_pLast)
                Parent::m_pNode = nullptr;
        }    
        return *this;
    }
    DoubleLinkedListBackwardIterator operator+(size_t n) const {
        DoubleLinkedListBackwardIterator temp = *this;
        for (size_t i=0; i<n && temp.m_pNode; ++i) {
            temp.m_pNode = temp.m_pNode->GetPrev();
            if (temp.m_pNode == Parent::m_pContainer->m_pLast)
                temp.m_pNode = nullptr;
        }
        return temp;
    }
};

// nodo
template <typename Traits>
class NodeDoubleLinkedList{
    using  value_type       = typename Traits::value_type;
    using  Node             = NodeDoubleLinkedList<Traits>;

private:
    value_type m_data;
    ref_type   m_ref;
    Node *m_pNext = nullptr;
    Node *m_pPrev = nullptr;

public:
    NodeDoubleLinkedList(){}
    NodeDoubleLinkedList( value_type _value, ref_type _ref = -1, Node *_pNext = nullptr, Node *_pPrev = nullptr)
                  : m_data(_value), m_ref(_ref), m_pNext(_pNext), m_pPrev(_pPrev){   }

    value_type  GetValue   () const { return m_data; }
    value_type &GetValueRef() { return m_data; }

    ref_type    GetRef     () const { return m_ref;   }
    ref_type   &GetRefRef  () { return m_ref;   }

    Node      * GetNext     () const { return m_pNext;   }
    Node      *&GetNextRef  () { return m_pNext;   }

    Node      * GetPrev     () const { return m_pPrev; }
    Node      *&GetPrevRef  () { return m_pPrev; }

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

//lista enlazada
template <typename Traits>
class CDoubleLinkedList {
public:
    using  value_type       = typename Traits::value_type;
    using Compare           = typename Traits::Func;
    using  Node             = NodeDoubleLinkedList<Traits>;
    using forward_iterator  = DoubleLinkedListForwardIterator <CDoubleLinkedList <Traits> >;
    using backward_iterator = DoubleLinkedListBackwardIterator <CDoubleLinkedList <Traits> >;
    
    friend forward_iterator;
    friend backward_iterator;
    friend struct GeneralIterator< CDoubleLinkedList<Traits> >;

private:
    Node *m_pRoot      = nullptr;
    Node *m_pLast      = nullptr;
    size_t m_nElements = 0;

    mutable mutex m_mtx;
    using LOCK = std::lock_guard<std::mutex>;

public:
    CDoubleLinkedList(){}
    //contructor de copia
    CDoubleLinkedList(const CDoubleLinkedList<Traits> &another){
        LOCK lock(another.m_mtx);
        Node *node_act = another.m_pRoot;
        for (size_t i = 0; i < another.m_nElements; ++i) {
            this->Insert(node_act->GetValue(), node_act->GetRef());
            node_act = node_act->GetNext();
        }
    }
    //move contructor
    CDoubleLinkedList(CDoubleLinkedList &&another) noexcept{
        LOCK lock(another.m_mtx);
        m_pRoot     = another.m_pRoot;
        m_pLast     = another.m_pLast;
        m_nElements = another.m_nElements;
        another.m_pRoot     = nullptr;
        another.m_pLast     = nullptr;
        another.m_nElements = 0;
    }
    //destructor
    virtual ~CDoubleLinkedList(){
        if (m_pLast) m_pLast->GetNextRef() = nullptr;
        if (m_pRoot) m_pRoot->GetPrevRef() = nullptr;
        Node *node_act = m_pRoot;
        for(;node_act;){
            Node *node_next = node_act->GetNext();
            delete node_act;
            node_act = node_next;
        }
    }
    //iteradores
    forward_iterator begin() { return forward_iterator(this, m_pRoot); }
    forward_iterator end()   { return forward_iterator(this, nullptr); }
    backward_iterator rbegin() { return backward_iterator(this, m_pLast); }
    backward_iterator rend()   { return backward_iterator(this, nullptr); }

    //operador de acceso
    value_type &operator[](size_t index){
        LOCK lock(m_mtx);
        Node *node_act = m_pRoot;
        for (size_t i=0; i<index && node_act; ++i)
            node_act = node_act->GetNext();
        if (node_act) return node_act->GetValueRef();
        throw out_of_range("no existe index");
    }

    //operator =
    CDoubleLinkedList &operator=(CDoubleLinkedList &&another) noexcept{
    if (this != &another) {
        this->~CDoubleLinkedList();
        m_pRoot     = another.m_pRoot;
        m_pLast     = another.m_pLast;
        m_nElements = another.m_nElements;
        another.m_pRoot     = nullptr;
        another.m_pLast     = nullptr;
        another.m_nElements = 0;
        }
        return *this;
    }

    void    push_back (const value_type &val, ref_type ref);
    void    Insert    (const value_type &val, ref_type ref);
    size_t  getSize() const { LOCK lock(m_mtx); return m_nElements;  }
    void    pop_back();

private:
    void InternalInsert(Node *&rParent, Node *pPrev, const value_type &val, ref_type ref);
    //persistencia write
    friend ostream &operator<<(ostream &os, CDoubleLinkedList<Traits> &container){
        LOCK lock(container.m_mtx);
        os << "CDoubleLinkedList: size = " << container.m_nElements << endl;
        os << "[";
        Node *node_act = container.m_pRoot;
        for (size_t i=0; i<container.m_nElements; ++i){
            os << "(" << node_act->GetValue() << ":" << node_act->GetRef() << ")";
            node_act = node_act->GetNext();
            if(i < container.m_nElements - 1) os << ",";
        }
        os << "]" << endl;
        return os;
    }
    //persistencia read
    friend istream &operator>>(istream &is, CDoubleLinkedList<Traits> &container) {
    T2 line;
    for (; getline(is, line); ) {
        if (!line.empty() && line[0] == '[') {
            for (size_t pos = 1; pos < line.length() && line[pos] != ']'; ++pos) {
                if (line[pos] == '(') {
                    size_t _value = line.find(':', pos);
                    size_t _refer = line.find(')', pos);
            
                    typename Traits::value_type val;
                    ref_type ref;
                    
                    stringstream(line.substr(pos + 1, _value - pos - 1))       >> val;
                    stringstream(line.substr(_value + 1, _refer - _value - 1)) >> ref;
                    
                    container.push_back(val, ref);
                    pos = _refer;
                    }
                } break;
            }
        }
        return is;
    }
};

template <typename Traits>
void CDoubleLinkedList<Traits>::push_back(const value_type &val, ref_type ref){
    std::lock_guard<std::mutex> lock(m_mtx);
    Node *pNewNode = new Node(val, ref);
    if( !m_pRoot ) {
        m_pRoot = pNewNode;
        pNewNode->GetNextRef() = pNewNode->GetPrevRef() = m_pRoot;
    } else {
        pNewNode->GetNextRef() = m_pRoot;
        pNewNode->GetPrevRef() = m_pLast;
        m_pLast->GetNextRef() = pNewNode;
        m_pRoot->GetPrevRef() = pNewNode;
        }
    m_pLast = pNewNode;
    ++m_nElements;
}

template <typename Traits>
void CDoubleLinkedList<Traits>::InternalInsert(Node *&rParent, Node *pPrev, const value_type &val, ref_type ref) {
    if( !rParent || Compare()(rParent->GetValue(), val) ){
        Node *pNew = new Node(val, ref, rParent, pPrev);
        if (rParent) rParent->GetPrevRef() = pNew;
        rParent = pNew;
        if (pNew->GetNext() == nullptr) m_pLast = pNew;
        ++m_nElements;
        return;
    }
    InternalInsert(rParent->GetNextRef(), rParent, val, ref);
}

template <typename Traits>
void CDoubleLinkedList<Traits>::Insert(const value_type &val, ref_type ref){
    std::lock_guard<std::mutex> lock(m_mtx);
    if (m_pLast) m_pLast->GetNextRef() = nullptr;
    if (m_pRoot) m_pRoot->GetPrevRef() = nullptr;
    InternalInsert(m_pRoot, nullptr, val, ref);
    if (m_pLast && m_pRoot) {
        m_pLast->GetNextRef() = m_pRoot;
        m_pRoot->GetPrevRef() = m_pLast;
    }
}

template <typename Traits>
void CDoubleLinkedList<Traits>::pop_back() {
    std::lock_guard<std::mutex> lock(m_mtx);
    if (!m_pRoot) return;
    Node* pDelete = m_pLast;
    if (m_nElements == 1) { m_pRoot = m_pLast = nullptr; } 
    else {
        m_pLast = m_pLast->GetPrev();
        m_pLast->GetNextRef() = m_pRoot;
        m_pRoot->GetPrevRef() = m_pLast;
    }
    delete pDelete;
    --m_nElements;
}

#endif // __DOUBLE_LINKED_LIST_H__