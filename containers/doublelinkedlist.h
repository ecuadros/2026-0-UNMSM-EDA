#ifndef __DOUBLE_LINKED_LIST_H__
#define __DOUBLE_LINKED_LIST_H__

#include <iostream>
#include <cassert>
#include <utility>
#include <mutex>
#include <fstream>
#include "../general/types.h"
#include "../util.h"
#include "GeneralIterator.h"
using namespace std;

template <typename T, typename _Func>
struct ListTrait{
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct AscendingTrait : public ListTrait<T, greater<T> >{};

template <typename T>
struct DescendingTrait : public ListTrait<T, less<T> >{};

// TODO: ForwardIterator para listas doblemente enlazadas
template <typename Container>
class DoubleLinkedListForwardIterator : public GeneralIterator <Container>
{
    using Parent = GeneralIterator <Container>;
    using Node   = typename Container::Node;

public:
    DoubleLinkedListForwardIterator(Container *pContainer, Node* pNode = nullptr)
        : Parent(pContainer, 0), m_pCurrent(pNode){}
    
    DoubleLinkedListForwardIterator<Container> &operator++(){
        if(m_pCurrent != nullptr){
            m_pCurrent = m_pCurrent -> GetNext();
        }
        return *this;
    }

    typename Container::value_type& operator*(){
        return m_pCurrent -> GetValueRef();
    }

    bool operator!=(const DoubleLinkedListForwardIterator<Container>& another)const {
        return m_pCurrent != another.m_pCurrent;
    }

private:
    Node* m_pCurrent;
};

// TODO: BackwardIterator para listas doblemente enlazadas
template <typename Container>
class DoubleLinkedListBackwardIterator : public GeneralIterator <Container>
{
    using Parent = GeneralIterator <Container>;
    using Node   = typename Container::Node;

public:
    DoubleLinkedListBackwardIterator(Container *pContainer, Node* pNode = nullptr)
        : Parent(pContainer, 0), m_pCurrent(pNode){}
    
    DoubleLinkedListBackwardIterator<Container> &operator++(){
        if(m_pCurrent != nullptr){
            m_pCurrent = m_pCurrent -> GetPrev();
        }
        return *this;
    }

    typename Container::value_type& operator*(){
        return m_pCurrent -> GetValueRef();
    }

    bool operator!=(const DoubleLinkedListBackwardIterator<Container>& another)const {
        return m_pCurrent != another.m_pCurrent;
    }

private:
    Node* m_pCurrent;
};

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
    NodeDoubleLinkedList() : m_pNext(nullptr), m_pPrev(nullptr) {}
    
    NodeDoubleLinkedList(value_type _value, ref_type _ref = -1, Node* _pNext = nullptr, Node* _pPrev = nullptr)
        : m_data(_value), m_ref(_ref), m_pNext(_pNext), m_pPrev(_pPrev) {}

    value_type  GetValue    () const { return m_data; }
    value_type &GetValueRef () { return m_data; }

    ref_type    GetRef      () const { return m_ref; }
    ref_type   &GetRefRef   () { return m_ref; }

    Node      * GetNext     () const { return m_pNext; }
    Node      *&GetNextRef  () { return m_pNext; }

    Node      * GetPrev     () const { return m_pPrev; }
    Node      *&GetPrevRef  () { return m_pPrev; }
};

template <typename Traits>
class CDoubleLinkedList {
public:
    using  value_type  = typename Traits::value_type;
    using  forward_iterator  = DoubleLinkedListForwardIterator < CDoubleLinkedList<Traits> >;
    using  backward_iterator = DoubleLinkedListBackwardIterator < CDoubleLinkedList<Traits> >; // <--- NUEVO
    using  Node = NodeDoubleLinkedList<Traits>;
    
    friend forward_iterator;
    friend backward_iterator;
    friend GeneralIterator< CDoubleLinkedList<Traits> >;

protected: 
    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;

    // Use recursive_mutex para evitar Deadlocks con la clase hija (CircularDoubleLinkedList)
    // Mutable para poder bloquear en funciones const
    mutable recursive_mutex m_mutex;

public:
    CDoubleLinkedList(): m_pRoot(nullptr), m_pLast(nullptr), m_nElements(0){}

    // Constructor copia
    CDoubleLinkedList(const CDoubleLinkedList<Traits> &another);

    // Move Constructor
    CDoubleLinkedList(CDoubleLinkedList<Traits> &&another) noexcept
        : m_pRoot(nullptr), m_pLast(nullptr), m_nElements(0)
    {
        lock_guard<recursive_mutex> lock(another.m_mutex);
        m_pRoot = exchange(another.m_pRoot, nullptr);
        m_pLast = exchange(another.m_pLast, nullptr);
        m_nElements = exchange(another.m_nElements, 0);
    }

    // Destructor seguro y virtual
    virtual ~CDoubleLinkedList();

    // Iterators begin() end()
    forward_iterator begin(){ return forward_iterator(this, m_pRoot); }
    forward_iterator end(){ return forward_iterator(this, nullptr); }

    backward_iterator rbegin(){ return backward_iterator(this, m_pLast); }
    backward_iterator rend(){ return backward_iterator(this, nullptr); }
    
    // Operadores de acceso []
    value_type& operator[](size_t index);
    const value_type& operator[](size_t index) const;

    size_t getSize() const { 
        lock_guard<recursive_mutex> lock(m_mutex);
        return m_nElements;
    }

    void push_back(const value_type &val, ref_type ref);
    void Insert(const value_type &val, ref_type ref) {
        lock_guard<recursive_mutex> lock(m_mutex);
        InternalInsert(m_pRoot, nullptr, val, ref); 
    }

    template <typename ObjFunc, typename ...Args>
    void Foreach(ObjFunc of, Args... args) { 
        ::Foreach(*this, of, args...);
    }

    template <typename ObjFunc, typename ...Args>
    auto FirstThat(ObjFunc of, Args... args) {
        return ::FirstThat(*this, of, args...);
    }

private:
    void InternalInsert(Node *&rParent, Node *pPrev, const value_type &val, ref_type ref);

    // Persistencia (write)
    friend ostream &operator<<(ostream &os, CDoubleLinkedList<Traits> &container){
        lock_guard<recursive_mutex> lock(container.m_mutex);
        os << "CDoubleLinkedList: size = " << container.getSize() << endl;
        os << "[";
        Node* pTemp = container.m_pRoot;
        if (pTemp) {
            do {
                os << "(" << pTemp->GetValue() << ":" << pTemp->GetRef() << "),";
                pTemp = pTemp->GetNext();
            } while (pTemp && pTemp != container.m_pRoot); 
        }
        os << "]" << endl;
        return os;
    }

    // Persistencia (read)
    friend istream &operator>>(istream &is, CDoubleLinkedList<Traits> &container) {
        lock_guard<recursive_mutex> lock(container.m_mutex);

        char c;
        value_type val;
        ref_type ref; 

        if (container.m_pLast && container.m_pLast->GetNext() == container.m_pRoot) {
            container.m_pLast->GetNextRef() = nullptr;
            if(container.m_pRoot) container.m_pRoot->GetPrevRef() = nullptr;
        }

        while (container.m_pRoot) {
            typename CDoubleLinkedList<Traits>::Node* pNext = container.m_pRoot->GetNext();
            delete container.m_pRoot;
            container.m_pRoot = pNext;
        }
        container.m_pLast = nullptr; 
        container.m_nElements = 0;
        
        while (is >> c && c != '[') {} // Ignora todo hasta encontrar el inicio de la lista '['

        while (is >> c && c != ']') {
            if (c == '(') {
                is >> val;
                is>> c;
                if(c == ':') { 
                    is >> ref; 
                    container.Insert(val, ref);
                }
                while(is >> c && c != ',' && c != ']' && c != ')');
            }
        }
        return is;
    }
};

template <typename Traits>
CDoubleLinkedList<Traits>::CDoubleLinkedList(const CDoubleLinkedList<Traits> &another)
    : m_pRoot(nullptr), m_pLast(nullptr), m_nElements(0)
{
    lock_guard<recursive_mutex> lock(another.m_mutex);
    if (!another.m_pRoot) return;
    Node* pCurrent = another.m_pRoot;
    do {
        this->push_back(pCurrent->GetValue(), pCurrent->GetRef());
        pCurrent = pCurrent->GetNext();
    } while (pCurrent != nullptr && pCurrent != another.m_pRoot);
}

template <typename Traits>
CDoubleLinkedList<Traits>::~CDoubleLinkedList() {
    lock_guard<recursive_mutex> lock(m_mutex);
    Node* pCurrent = m_pRoot;
    while (pCurrent != nullptr) {
        Node* pNext = pCurrent->GetNext();
        delete pCurrent;
        pCurrent = pNext;
    }
    m_pRoot = nullptr; m_pLast = nullptr; m_nElements = 0;
}

// Implementacion para leer y modificar el valor del nodo
template <typename Traits>
typename CDoubleLinkedList<Traits>::value_type& CDoubleLinkedList<Traits>::operator[](size_t index) {
    lock_guard<recursive_mutex> lock(m_mutex);
    assert(index < m_nElements);
    Node* pCurrent = m_pRoot;
    for (size_t i = 0; i < index; ++i) pCurrent = pCurrent->GetNext();
    return pCurrent->GetValueRef();
}

// Implementacion para mostrar el valor del nodo sin posibilidad de cambio
template <typename Traits>
const typename CDoubleLinkedList<Traits>::value_type& CDoubleLinkedList<Traits>::operator[](size_t index) const {
    lock_guard<recursive_mutex> lock(m_mutex);
    assert(index < m_nElements);
    Node* pCurrent = m_pRoot;
    for (size_t i = 0; i < index; ++i) pCurrent = pCurrent->GetNext();
    return pCurrent->GetValue();
}

template <typename Traits>
void CDoubleLinkedList<Traits>::push_back(const value_type &val, ref_type ref){
    lock_guard<recursive_mutex> lock(m_mutex);
    Node *pNewNode = new Node(val, ref);
    
    if( !m_pRoot ) {
        m_pRoot = pNewNode;
        m_pLast = pNewNode;
    }
    else {
        m_pLast -> GetNextRef() = pNewNode;
        pNewNode->GetPrevRef() = m_pLast;
        m_pLast = pNewNode;
    }
    ++m_nElements;
}

template <typename Traits>
void CDoubleLinkedList<Traits>::InternalInsert(Node *&rParent, Node *pPrev, const value_type &val, ref_type ref) {

    bool bShouldInsert = !rParent || typename Traits::Func()(rParent->GetValue(), val);

    if (bShouldInsert) {
        Node *pNew = new Node(val, ref, rParent, pPrev);
        rParent = pNew;

        if (pNew->GetNext())
            pNew->GetNext()->GetPrevRef() = pNew;
        else
            m_pLast = pNew;
        ++m_nElements;
        return;
        
    }
    InternalInsert(rParent->GetNextRef(), rParent, val, ref);
}

template <typename Traits>
class CCircularDoubleLinkedList : public CDoubleLinkedList<Traits> {
    using Node = typename CDoubleLinkedList<Traits>::Node;
    using value_type = typename Traits::value_type;
public:
    CCircularDoubleLinkedList() : CDoubleLinkedList<Traits>() {}

    CCircularDoubleLinkedList(const CCircularDoubleLinkedList<Traits> &another) 
        : CDoubleLinkedList<Traits>(another)
    {   
        lock_guard<recursive_mutex> lock(this->m_mutex);
        if (this->m_pLast) {
            this->m_pLast->GetNextRef() = this->m_pRoot;
            this->m_pRoot->GetPrevRef() = this->m_pLast;
        }
    }

    CCircularDoubleLinkedList(CCircularDoubleLinkedList<Traits> &&another) noexcept
        : CDoubleLinkedList<Traits>(move(another)) {}

    ~CCircularDoubleLinkedList() {
        lock_guard<recursive_mutex> lock(this->m_mutex);
        if (this->m_pLast) {
            this->m_pLast->GetNextRef() = nullptr;
            if(this->m_pRoot) this->m_pRoot->GetPrevRef() = nullptr;
        }
    }

    void push_back(const value_type &val, long ref = -1) {
        lock_guard<recursive_mutex> lock(this->m_mutex);
        CDoubleLinkedList<Traits>::push_back(val, ref);
        if (this->m_pLast) {
            this->m_pLast->GetNextRef() = this->m_pRoot;
            this->m_pRoot->GetPrevRef() = this->m_pLast;
        }
    }

    template <typename ObjFunc, typename ...Args>
    void Foreach(ObjFunc of, Args... args){
        lock_guard<recursive_mutex> lock(this->m_mutex);
        if (!this->m_pRoot) return;
        Node* pCurrent = this->m_pRoot;
        do {
            of(pCurrent->GetValueRef(), args...);
            pCurrent = pCurrent->GetNext();
        } while (pCurrent != this->m_pRoot);
    }

    template <typename ObjFunc, typename ...Args>
    auto FirstThat(ObjFunc of, Args... args){
        lock_guard<recursive_mutex> lock(this->m_mutex);
        if (!this->m_pRoot) return this->end();
        Node* pCurrent = this->m_pRoot;
        do {
            if( of(pCurrent->GetValueRef(), args...) ) 
                return typename CDoubleLinkedList<Traits>::forward_iterator(this, pCurrent);
            pCurrent = pCurrent->GetNext();
        } while (pCurrent != this->m_pRoot);
        return this->end();
    }
};

#endif // __DOUBLE_LINKED_LIST_H__