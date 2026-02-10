#ifndef __DOUBLE_LINKED_LIST_H__
#define __DOUBLE_LINKED_LIST_H__

#include <iostream>
#include <mutex>
#include <assert.h>
#include "../algorithms/sorting.h"
#include "../general/types.h"
#include "../util.h"
#include "GeneralIterator.h"
#include "../list_traits.h" 
using namespace std;


template <typename Container>
class DoubleLinkedListIterator : public GeneralIterator<Container> {
    using Parent = GeneralIterator<Container>;
    using Node = typename Container::Node;

public:
    DoubleLinkedListIterator(Container *pContainer,  Node *m_data, Size pos) : Parent(pContainer, pos, m_data) {}

    typename Container::value_type &operator*() {
        return Parent::m_data->GetValueRef();
    }
};

template <typename Container> 
class DoubleLinkedListForwardIterator : public DoubleLinkedListIterator<Container> {
    using Parent = DoubleLinkedListIterator<Container>;
    using Node = typename Container::Node;  

public:
    DoubleLinkedListForwardIterator(Container *pContainer, Node *m_data, Size pos = 0) : Parent(pContainer, m_data, pos) {}
    DoubleLinkedListForwardIterator(DoubleLinkedListForwardIterator<Container> &another):  Parent(another){}

    DoubleLinkedListForwardIterator<Container> &operator++() {
        if( Parent::m_pos < Parent::m_pContainer->getSize()) {
            ++Parent::m_pos;
            Parent::m_data = Parent::m_data->GetNext();
        }
        return *this;
    }
};

template <typename Container> 
class DoubleLinkedListBackwardIterator : public DoubleLinkedListIterator<Container> {
    using Parent = DoubleLinkedListIterator<Container>;
    using Node = typename Container::Node;

public:
    DoubleLinkedListBackwardIterator(Container *pContainer,Node *m_data, Size pos) : Parent(pContainer, m_data, pos) {}
    DoubleLinkedListBackwardIterator(DoubleLinkedListBackwardIterator<Container> &another):  Parent(another){}

    DoubleLinkedListBackwardIterator<Container> &operator++() {
        if( Parent::m_pos > - 1) {
            --Parent::m_pos;
            Parent::m_data = Parent::m_data->GetPrev();
        }
        return *this;
    }
};

// Iterators para listas enlazadas

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
    NodeDoubleLinkedList( value_type _value, ref_type _ref = -1, Node *pPrev = nullptr, Node *pNext = nullptr)
        : m_data(_value), m_ref(_ref), m_pPrev(pPrev), m_pNext(pNext){   }
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

template <typename Traits>
class CDoubleLinkedList {

public:
    using  value_type  = typename Traits::value_type;
    using  Node = NodeDoubleLinkedList<Traits>;
    using CompareFunc = typename Traits::Func;    

private:
    using  forward_iterator  = DoubleLinkedListForwardIterator < CDoubleLinkedList<Traits> >;
    friend forward_iterator;
    using  backward_iterator = DoubleLinkedListBackwardIterator < CDoubleLinkedList<Traits> >;
    friend backward_iterator;
    friend GeneralIterator< CDoubleLinkedList<Traits> >;

    // mutex que nos permitirá 'encerrar' los datos del CDoubleLinkedList en un solo hilo,
    // evitando que otros lo modifiquen en momentos críticos

protected:
    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;

    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref);
    mutable mutex mtx;

public:
    CDoubleLinkedList(){}

    CDoubleLinkedList(const CDoubleLinkedList& another) {

        lock_guard<mutex> lock(another.mtx);
        Node* i = another.m_pRoot;
        while (i != nullptr) {
            this->push_back(i->GetValueRef(), i->GetRef());
            i = i->GetNext();
        }
    }

    CDoubleLinkedList(CDoubleLinkedList&& another) noexcept
    {
        lock_guard<mutex> lock(another.mtx);

        m_pRoot = another.m_pRoot;
        m_pLast = another.m_pLast;
        m_nElements = another.m_nElements;

        another.m_pRoot = nullptr;
        another.m_pLast = nullptr;
        another.m_nElements = 0;
    }

    virtual ~CDoubleLinkedList() {
        Node* temp1 = m_pRoot;
        Node* temp2;
        while (temp1 != nullptr) {
            temp2 = temp1->GetNextRef();
            delete temp1;
            temp1 = temp2;
        }
    }

    forward_iterator begin()
    { return forward_iterator(this, m_pRoot); }
    forward_iterator end()
    { return forward_iterator(this, m_pLast, getSize());}

    backward_iterator rbegin()
    { return backward_iterator(this, m_pLast, getSize() - 1);  }
    backward_iterator rend()
    { return backward_iterator(this,m_pRoot, -1);  }

    value_type &operator[](Size index) {
        assert(index < m_nElements);

        Node *temp = m_pRoot;
        for(int i = 0; i < index; ++i) {
            temp = temp->GetNext();
        }
        return temp->GetValueRef();
    }

    template <typename ObjFunc, typename ...Args>
    auto FirstThat(ObjFunc of, Args... args){
        return ::FirstThat(*this, of, args...);
    }
    
    template <typename ObjFunc, typename ...Args>
    void Foreach(ObjFunc of, Args... args){
        ::Foreach(*this, of, args...);
    }

    void push_back(value_type val, ref_type ref);
    void Insert(const value_type &val, ref_type ref);
    size_t getSize(){ return m_nElements;  }
    void sort();
private:

    friend ostream &operator<<(ostream &os, CDoubleLinkedList<Traits> &container){
        os << "CDoubleLinkedList: size = " << container.getSize() << endl;
        os << "[";
        NodeDoubleLinkedList<Traits>* i = container.m_pRoot;
        for (size_t j = 0; j < container.getSize(); ++j){
            os << "(" << i->GetValue() << ":" << i->GetRef() << ")->";
            i = i->GetNext();
        }
        os << "NULL]" << endl;
        return os;
    }

    friend istream &operator>>(istream &is, CDoubleLinkedList<Traits> &container) {
        typename Traits::value_type val;
        ref_type ref;
        char letter;

        if (!(is >> letter) || letter != '(')
            return is;

        if (!(is >> val >> letter) || letter != ':')
            return is;

        if (!(is >> ref >> letter) || letter != ')')
            return is;

        container.Insert(val, ref);
        return is;
    }

};

template <typename Traits>
void CDoubleLinkedList<Traits>::push_back(value_type val, ref_type ref){
    Node *pNewNode = new Node(val, ref);
    if( !m_pRoot ) {
        m_pRoot = pNewNode;
    }
    else {
        m_pLast->GetNextRef() = pNewNode;
        pNewNode->GetPrevRef() = m_pLast;
    }
    m_pLast = pNewNode;
    ++m_nElements;
}

template <typename Traits>
void CDoubleLinkedList<Traits>::sort() {
    BurbujaRecursivo(*this, m_nElements, CompareFunc{});
}

template <typename Traits>
void CDoubleLinkedList<Traits>::InternalInsert(Node *&rParent, const value_type &val, ref_type ref){
    if( !rParent || CompareFunc()(rParent->GetValue(), val) ){
        Node *pNew = new Node(val, ref, rParent ? rParent->GetPrev() : nullptr, rParent);
        if(rParent)
            rParent->GetPrevRef() = pNew;
        rParent = pNew;
        ++m_nElements;
        if(pNew->GetNext() == nullptr){
            m_pLast = pNew;
        }
        return;
    }
    else if ( rParent->GetNext() == m_pRoot ) {
        Node *pNew = new Node(val, ref, rParent, nullptr);
        rParent->GetNextRef() = pNew;
        m_pLast = pNew;
        ++m_nElements;
        return;
    }
    InternalInsert(rParent->GetNextRef(), val, ref);
}

template <typename Traits>
void CDoubleLinkedList<Traits>::Insert(const value_type &val, ref_type ref){
    InternalInsert(m_pRoot, val, ref);
}

#endif // __DOUBLE_LINKED_LIST_H__