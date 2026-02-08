#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__
#include <iostream>
#include <cassert>
#include <utility>
#include <mutex>
#include "../general/types.h"
#include "../util.h"
#include "GeneralIterator.h"


using namespace std;

// TODO: Traits para listas enlazadas
template <typename T, typename _Func>
struct ListTrait{
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct AscendingTrait : 
    public ListTrait<T, greater<T> >{
};

template <typename T>
struct DescendingTrait : 
    public ListTrait<T, less<T> >{
};

// Iterators para listas enlazadas
template <typename Container>
class LinkedListForwardIterator : public GeneralIterator <Container>
{
    using Parent = GeneralIterator <Container>;
    using Node   = typename Container::Node;

public:
    LinkedListForwardIterator(Container *pContainer, Node* pNode = nullptr)
        : Parent(pContainer, 0), m_pCurrent(pNode){}
    LinkedListForwardIterator<Container> &operator++(){
        if(m_pCurrent != nullptr){
            m_pCurrent = m_pCurrent -> GetNext();
        }
        return *this;
        }

    typename Container::value_type& operator*(){
        return m_pCurrent -> GetValueRef();
    }

    bool operator!=(const LinkedListForwardIterator<Container>& another)const {
        return m_pCurrent != another.m_pCurrent;
    }

private:
    Node* m_pCurrent;
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
    NodeLinkedList() : m_pNext(nullptr) {}
    NodeLinkedList(value_type _value, ref_type _ref = -1, Node* _pNext = nullptr)
        : m_data(_value), m_ref(_ref), m_pNext(_pNext) {}
    
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
    bool operator==(const Node &another) const { return m_data == another.GetValue(); }
    bool operator<(const Node &another) const { return m_data < another.GetValue(); }
};
template <typename Traits>
class CLinkedList {
public:
    using  value_type  = typename Traits::value_type;
    using  forward_iterator  = LinkedListForwardIterator < CLinkedList<Traits> >;
    using  Node = NodeLinkedList<Traits>;
    friend forward_iterator;
    friend GeneralIterator< CLinkedList<Traits> >;

protected: // Permite a la clase hija (CircularLinkedList) manipular los punteros internos y seguir privado para el resto.
    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;

    // Use recursive_mutex para evitar Deadlocks con la clase hija (CircularLinkedList)
    // Mutable para poder bloquear en funciones const
    mutable recursive_mutex m_mutex;

public:
    CLinkedList(): m_pRoot(nullptr), m_pLast(nullptr), m_nElements(0){}

    // TODO: Constructor copia
    CLinkedList(const CLinkedList<Traits> &another);

    // TODO: Move Constructor
    CLinkedList(CLinkedList<Traits> &&another) noexcept
        : m_pRoot(nullptr), m_pLast(nullptr), m_nElements(0)
    {
        lock_guard<recursive_mutex> lock(another.m_mutex);
        
        m_pRoot = exchange(another.m_pRoot, nullptr);
        m_pLast = exchange(another.m_pLast, nullptr);
        m_nElements = exchange(another.m_nElements, 0);
    }

    // TODO: Destructor seguro y virtual
    virtual ~CLinkedList();

    // TODO: Concurrencia (mutex)

    // TODO: Iterators begin() end()
    forward_iterator begin(){ return forward_iterator(this, m_pRoot); }
    forward_iterator end(){ return forward_iterator(this, nullptr); }

    // TODO: Operadores de acceso []
    value_type& operator[](size_t index);
    const value_type& operator[](size_t index) const;

    void push_back(const value_type &val, ref_type ref);

    void Insert(const value_type &val, ref_type ref){
        lock_guard<recursive_mutex> lock(m_mutex);
        InternalInsert(m_pRoot, val, ref);
    }

    size_t getSize() const { 
        lock_guard<recursive_mutex> lock(m_mutex);
        return m_nElements;
    }

    template <typename ObjFunc, typename ...Args>
    void Foreach(ObjFunc of, Args... args){
        ::Foreach(*this, of, args...);
    }

    template <typename ObjFunc, typename ...Args>
    auto FirstThat(ObjFunc of, Args... args){
        return ::FirstThat(*this, of, args...);
    }
    
private:
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref);

    // TODO: Persistencia (write)
    friend ostream &operator<<(ostream &os, CLinkedList<Traits> &container){
        lock_guard<recursive_mutex> lock(container.m_mutex);

        os << "CLinkedList: size = " << container.getSize() << endl;
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
    // TODO: Persistencia (read)
    friend istream &operator>>(istream &is, CLinkedList<Traits> &container) {
        lock_guard<recursive_mutex> lock(container.m_mutex);

        char c;
        value_type val;
        ref_type ref;

        if (container.m_pLast && container.m_pLast->GetNext() == container.m_pRoot) {
            container.m_pLast->GetNextRef() = nullptr;
        }

        while (container.m_pRoot) {
            typename CLinkedList<Traits>::Node* pNext = container.m_pRoot->GetNext();
            delete container.m_pRoot;
            container.m_pRoot = pNext;
        }
        container.m_pLast = nullptr;
        container.m_nElements = 0;

        while (is >> c && c != '[') {}  // Ignora todo hasta encontrar el inicio de la lista '['

        while (is >> c && c != ']') {
            if (c == '(') {
                is >> val;
                is >> c;
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
CLinkedList<Traits>::CLinkedList(const CLinkedList<Traits> &another)
    : m_pRoot(nullptr), m_pLast(nullptr), m_nElements(0)
{   
    lock_guard<recursive_mutex> lock(another.m_mutex);
    if (another.m_pRoot == nullptr) return;
    Node* pCurrent = another.m_pRoot;
    do {
        auto val = pCurrent->GetValue();
        auto ref = pCurrent->GetRef();
        this->push_back(val, ref);
        
        pCurrent = pCurrent->GetNext();
    
    } while (pCurrent != nullptr && pCurrent != another.m_pRoot);
}

template <typename Traits>
CLinkedList<Traits>::~CLinkedList() {
    lock_guard<recursive_mutex> lock(m_mutex);

    Node* pCurrent = m_pRoot;
    while (pCurrent != nullptr) {
        Node* pNext = pCurrent->GetNext();
        delete pCurrent;
        pCurrent = pNext;
    }
    m_pRoot = nullptr;
    m_pLast = nullptr;
    m_nElements = 0;
}

// Implementacion para leer y modificar el valor del nodo
template <typename Traits>
typename CLinkedList<Traits>::value_type& CLinkedList<Traits>::operator[](size_t index) {
    lock_guard<recursive_mutex> lock(m_mutex);

    assert(index < m_nElements);
    Node* pCurrent = m_pRoot;
    for (size_t i = 0; i < index; ++i) {
        pCurrent = pCurrent->GetNext();
    }
    return pCurrent->GetValueRef();
}

// Implementacion para mostrar el valor del nodo sin posibilidad de cambio
template <typename Traits>
const typename CLinkedList<Traits>::value_type& CLinkedList<Traits>::operator[](size_t index) const {
    lock_guard<recursive_mutex> lock(m_mutex);

    assert(index < m_nElements);
    Node* pCurrent = m_pRoot;
    for (size_t i = 0; i < index; ++i) {
        pCurrent = pCurrent->GetNext();
    }
    return pCurrent->GetValue();
}

template <typename Traits>
void CLinkedList<Traits>::push_back( const value_type &val, ref_type ref){
    lock_guard<recursive_mutex> lock(m_mutex);

    Node *pNewNode = new Node(val, ref);
    if( !m_pRoot )
        m_pRoot = pNewNode;
    else
        m_pLast -> GetNextRef() = pNewNode;
    m_pLast = pNewNode;
    ++m_nElements;
}

template <typename Traits>
void CLinkedList<Traits>::InternalInsert(Node *&rParent, const value_type &val, ref_type ref){
    // TODO: Agregar algo para el caso de circular
    bool bIsCircularLink = (m_pLast && &rParent == &m_pLast->GetNextRef());

    if( !rParent || bIsCircularLink || typename Traits::Func()(rParent->GetValue(), val) ){
        Node *pNew = new Node(val, ref, rParent);
        rParent = pNew;

        if (m_pLast && pNew->GetNext() == m_pRoot && m_pRoot != pNew) {
             m_pLast->GetNextRef() = pNew;
        }

        Node *pTemp = m_pRoot;
        while (pTemp && pTemp->GetNext() && pTemp->GetNext() != m_pRoot) {
            pTemp = pTemp->GetNext();
        }
        m_pLast = pTemp;

        ++m_nElements;
        return;
    }
    InternalInsert(rParent->GetNextRef(), val, ref);
}

template <typename Traits>
class CCircularLinkedList : public CLinkedList<Traits> {
    using Node = typename CLinkedList<Traits>::Node;
    using value_type = typename Traits::value_type;
public:
    CCircularLinkedList() : CLinkedList<Traits>() {}

    CCircularLinkedList(const CCircularLinkedList<Traits> &another) 
        : CLinkedList<Traits>(another)
    {   lock_guard<recursive_mutex> lock(this->m_mutex);
        if (this->m_pLast) 
            this->m_pLast->GetNextRef() = this->m_pRoot;
    }

    CCircularLinkedList(CCircularLinkedList<Traits> &&another) noexcept
        : CLinkedList<Traits>(move(another)) {}

    virtual ~CCircularLinkedList() {
        lock_guard<recursive_mutex> lock(this->m_mutex);
        if (this->m_pLast) {
            this->m_pLast->GetNextRef() = nullptr;
        }
    }

    void push_back(const value_type &val, long ref = -1) {
        lock_guard<recursive_mutex> lock(this->m_mutex);
        CLinkedList<Traits>::push_back(val, ref);
        if (this->m_pLast) {
            this->m_pLast->GetNextRef() = this->m_pRoot;
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
            if( of(pCurrent->GetValueRef(), args...) ){
                return typename CLinkedList<Traits>::forward_iterator(this, pCurrent);
            }
            pCurrent = pCurrent->GetNext();
        } while (pCurrent != this->m_pRoot);
        return this->end();
    }
};

#endif // __LINKEDLIST_H__
