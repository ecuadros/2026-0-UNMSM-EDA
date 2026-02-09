#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__
#include <iostream>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include "../general/types.h"
#include "GeneralIterator.h"
using namespace std;

//traits
template <typename T, typename _Func>
struct ListTrait{
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct AscendingTrait : public ListTrait<T, std::greater<T> >{};

template <typename T>
struct DescendingTrait : public ListTrait<T, std::less<T> >{};

//iterator
template <typename Container>
class LinkedListIterator : public GeneralIterator<Container>{
    using Parent = GeneralIterator<Container>;
    using Node   = typename Container::Node;
public:
    LinkedListIterator(Container *pContainer, Node *pNode)
                      : Parent(pContainer, pNode){}
    LinkedListIterator(const LinkedListIterator<Container> &another)
                      : Parent(another){}

    LinkedListIterator<Container> &operator++(){
        if(Parent::m_pNode){
            Parent::m_pNode = Parent::m_pNode->GetNext();
            if (Parent::m_pNode == Parent::m_pContainer->m_pRoot)
                Parent::m_pNode = nullptr;
        }    
        return *this;
    }
    LinkedListIterator operator+(size_t n) const {
        LinkedListIterator temp = *this;
        for (size_t i=0; i<n && temp.m_pNode; ++i) {
            temp.m_pNode = temp.m_pNode->GetNext();
            if (temp.m_pNode == Parent::m_pContainer->m_pRoot)
                temp.m_pNode = nullptr;
        }
        return temp;
    }
};

// nodo
template <typename Traits>
class NodeLinkedList{
    using  value_type       = typename Traits::value_type;
    using  Node             = NodeLinkedList<Traits>;

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

//lista enlazada
template <typename Traits>
class CLinkedList {
    using  value_type       = typename Traits::value_type;
    using Compare           = typename Traits::Func;
    using  Node             = NodeLinkedList<Traits>;
    using forward_iterator  = LinkedListIterator <CLinkedList <Traits> >;
    
    friend forward_iterator;
    friend GeneralIterator< CLinkedList<Traits> >;

    Node *m_pRoot      = nullptr;
    Node *m_pLast      = nullptr;
    size_t m_nElements = 0;

    mutable mutex m_mtx;
    using LOCK = std::lock_guard<std::mutex>;

public:
    CLinkedList(){}
    //contructor de copia
    CLinkedList(const CLinkedList<Traits> &another){
        LOCK lock(another.m_mtx);
        Node *node_act = another.m_pRoot;
        for (size_t i = 0; i < another.m_nElements; ++i) {
            this->Insert(node_act->GetValue(), node_act->GetRef());
            node_act = node_act->GetNext();
        }
    }
    //move contructor
    CLinkedList(CLinkedList &&another) noexcept{
        LOCK lock(another.m_mtx);
        m_pRoot     = another.m_pRoot;
        m_pLast     = another.m_pLast;
        m_nElements = another.m_nElements;
        another.m_pRoot     = nullptr;
        another.m_pLast     = nullptr;
        another.m_nElements = 0;
    }
    //destructor
    virtual ~CLinkedList(){
        if (m_pLast) m_pLast->GetNextRef() = nullptr;
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
    //operador de acceso
    value_type &operator[](size_t index){
        LOCK lock(m_mtx);
        Node *node_act = m_pRoot;
        for (size_t i=0; i<index && node_act; ++i)
            node_act = node_act->GetNext();
        if (node_act) return node_act->GetValueRef();
        throw out_of_range("no existe index");
    }

    void    push_back (const value_type &val, ref_type ref);
    void    Insert    (const value_type &val, ref_type ref);
    size_t  getSize() { LOCK lock(m_mtx); return m_nElements;  }

private:
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref);
    //persistencia write
    friend ostream &operator<<(ostream &os, CLinkedList<Traits> &container){
        LOCK lock(container.m_mtx);
        os << "CLinkedList: size = " << container.m_nElements << endl;
        os << "[";
        Node *node_act = container.m_pRoot;
        for (size_t i=0; i<container.m_nElements; ++i){ // Usar contador
            os << "(" << node_act->GetValue() << ":" << node_act->GetRef() << ")";
            node_act = node_act->GetNext();
            if(i < container.m_nElements - 1) os << ",";
        }
        os << "]" << endl;
        return os;
    }
    //persistencia read
    friend istream &operator>>(istream &is, CLinkedList<Traits> &container) {
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
void CLinkedList<Traits>::push_back(const value_type &val, ref_type ref){
    std::lock_guard<std::mutex> lock(m_mtx);
    Node *pNewNode = new Node(val, ref);
    if( !m_pRoot ) {
        m_pRoot = pNewNode;
        pNewNode->GetNextRef() = m_pRoot;
    } else {
        m_pLast->GetNextRef() = pNewNode;
        pNewNode->GetNextRef() = m_pRoot;
        }
    m_pLast = pNewNode;
    ++m_nElements;
}

template <typename Traits>
void CLinkedList<Traits>::InternalInsert(Node *&rParent, const value_type &val, ref_type ref){
    if( !rParent || Compare()(rParent->GetValue(), val) ){
        Node *pNew = new Node(val, ref, rParent);
        rParent = pNew;
        if (pNew->GetNext() == nullptr) m_pLast = pNew;
        ++m_nElements;
        return;
    }
    InternalInsert(rParent->GetNextRef(), val, ref);
}

template <typename Traits>
void CLinkedList<Traits>::Insert(const value_type &val, ref_type ref){
    std::lock_guard<std::mutex> lock(m_mtx);
    if (m_pLast) m_pLast->GetNextRef() = nullptr;
    InternalInsert(m_pRoot, val, ref);
    if (m_pLast) m_pLast->GetNextRef() = m_pRoot;
}

#endif // __LINKEDLIST_H__