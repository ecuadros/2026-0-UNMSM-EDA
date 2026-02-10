#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__
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
class LinkedListForwardIterator : public GeneralIterator<Container> {
    using Parent = GeneralIterator<Container>;
    using Node = typename Container::Node;

public:
    LinkedListForwardIterator(Container *pContainer, Node *m_data, Size pos = 0) : Parent(pContainer, pos, m_data) {}
    LinkedListForwardIterator(LinkedListForwardIterator<Container> &another):  Parent(another){}

    typename Container::value_type &operator*() {
        return Parent::m_data->GetValueRef();
    }

    LinkedListForwardIterator<Container> &operator++() {
        if( Parent::m_pos < Parent::m_pContainer->getSize()) {
            ++Parent::m_pos;
            Parent::m_data = Parent::m_data->GetNext();
        }
        return *this;
    }
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
    NodeLinkedList( value_type _value, ref_type _ref = -1, Node *pNext = nullptr)
        : m_data(_value), m_ref(_ref), m_pNext(pNext){   }
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


template <typename Traits>
class CLinkedList {
    using  value_type  = typename Traits::value_type;

protected:
    using  Node = NodeLinkedList<Traits>;  
    using  CompareFunc = typename Traits::Func;
    using  forward_iterator  = LinkedListForwardIterator < CLinkedList<Traits> >;
    friend forward_iterator;
    friend GeneralIterator < CLinkedList<Traits> >;

    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;
    mutable mutex mtx;

    // Algoritmo que usaremos para destruir nodos, lo hacemos de esta forma 
    // porque también sirve para la lista enlazada circular
    void destroy_nodes();
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref);

public:
    CLinkedList(){}

    CLinkedList(const CLinkedList<Traits>& another) {

        lock_guard<mutex> lock(another.mtx);

        Node* i = another.m_pRoot;
        while (i) {
            this->push_back(i->GetValueRef(), i->GetRef());
            i = i->GetNext();
        }
    }

    CLinkedList(CLinkedList<Traits>&& another) noexcept
    {
        lock_guard<mutex> lock(another.mtx);

        m_pRoot = another.m_pRoot;
        m_pLast = another.m_pLast;
        m_nElements = another.m_nElements;

        another.m_pRoot = nullptr;
        another.m_pLast = nullptr;
        another.m_nElements = 0;
    }

    virtual ~CLinkedList() {
        destroy_nodes();
    }

    forward_iterator begin()
    { return forward_iterator(this, m_pRoot); }
    forward_iterator end()
    { return forward_iterator(this, m_pLast, getSize());}


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
    void sort();
    size_t getSize(){ return m_nElements;  }
private:
    // TODO: Persistencia (write)
    friend ostream &operator<<(ostream &os, CLinkedList<Traits> &container){
        os << "CLinkedList: size = " << container.getSize() << endl;
        os << "[";
        NodeLinkedList<Traits>* i = container.m_pRoot;
        for (size_t j = 0; j < container.getSize(); ++j){
            os << "(" << i->GetValue() << ":" << i->GetRef() << ")->";
            i = i->GetNext();
        }
        os << "NULL]" << endl;
        return os;
    }
    // TODO: Persistencia (read)
 
    friend istream &operator>>(istream &is, CLinkedList<Traits> &container) {
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
void CLinkedList<Traits>::destroy_nodes() {
    if (!m_pRoot) return;
    Node *temp1 = this->m_pRoot, *temp2;
    auto n = m_nElements;

    for(auto i = 0; i < n; ++i) {
        temp2 = temp1->GetNext();
        delete temp1;
        temp1 = temp2;
    }
    m_nElements = 0;
    m_pRoot = nullptr;
    m_pLast = nullptr;
}

template <typename Traits>
void CLinkedList<Traits>::push_back(value_type val, ref_type ref){
    Node *pNewNode = new Node(val, ref);
    if( !m_pRoot ) {
        m_pRoot = pNewNode;
        m_pLast = pNewNode;
    }
    else {
        m_pLast->GetNextRef() = pNewNode;
        m_pLast = pNewNode;
    }
    ++m_nElements;
}

template <typename Traits>
void CLinkedList<Traits>::sort() {
    BurbujaRecursivo(*this, m_nElements, CompareFunc{});
}

template <typename Traits>
void CLinkedList<Traits>::InternalInsert(Node *&rParent, const value_type &val, ref_type ref){
    // TODO: Agregar algo para el caso de circular
    if( !rParent || CompareFunc()(rParent->GetValue(), val) ){
        Node *pNew = new Node(val, ref, rParent);
        rParent = pNew;
        ++m_nElements;

        if(pNew->GetNext() == nullptr){
            m_pLast = pNew;
        }
        return;
    }
    else if ( rParent->GetNext() == m_pRoot ) {
        Node *pNew = new Node(val, ref);
        m_pLast = pNew;
        rParent->GetNextRef() = pNew;
        return;
    }
    InternalInsert(rParent->GetNextRef(), val, ref);
}

template <typename Traits>
void CLinkedList<Traits>::Insert(const value_type &val, ref_type ref){
    InternalInsert(m_pRoot, val, ref);
}

#endif // __LINKEDLIST_H__
