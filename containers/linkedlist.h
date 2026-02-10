#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include <iostream>
#include <mutex>
#include "../general/types.h"
#include "../util.h"
#include "GeneralIterator.h"
#include "../foreach.h"

using namespace std;

// Rasgos para saber si ordenar mayor o menor
template <typename T, typename _Func>
struct ListTrait{
    using value_type = T;
    using Func       = _Func;
};
template <typename T> struct AscendingTrait : public ListTrait<T, std::greater<T> > {};
template <typename T> struct DescendingTrait : public ListTrait<T, std::less<T> > {};

// Iterador para recorrer la lista
template <typename Container>
class LinkedListForwardIterator : public GeneralIterator<Container>
{
    using Parent = GeneralIterator<Container>;
    using Node   = typename Container::Node;
public:
    LinkedListForwardIterator(Container *pContainer, Node* pNode) 
        : Parent(pContainer, 0) { 
        this->m_data = pNode; // Guardo el nodo actual
    }
    // Ver si llegamos al final
    bool operator!=(const LinkedListForwardIterator<Container> &another){
        return this->m_data != another.m_data;
    }
    // Avanzar al siguiente (++it)
    LinkedListForwardIterator<Container> &operator++(){
        if(this->m_data) this->m_data = this->m_data->GetNext();
        return *this;
    }
    // Sacar el valor (*)
    typename Container::value_type &operator*(){
        return this->m_data->GetValueRef();
    }
};

// Clase Nodo (cajita con dato y puntero)
template <typename Traits>
class NodeLinkedList{
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeLinkedList<Traits>;
private:
    value_type m_data;
    ref_type   m_ref;
    Node      *m_pNext = nullptr;
public:
    NodeLinkedList(){}
    NodeLinkedList( value_type _value, ref_type _ref = -1, Node* next = nullptr)
        : m_data(_value), m_ref(_ref), m_pNext(next){   }
    
    // Getters y Setters normales
    value_type  GetValue   () const { return m_data; }
    value_type &GetValueRef() { return m_data; }
    ref_type    GetRef     () const { return m_ref;   }
    Node      * GetNext      () const { return m_pNext;   }
    Node      *&GetNextRef  () { return m_pNext;   }
    void SetNext(Node* next) { m_pNext = next; }
    
    bool operator==(const Node &another) const { return m_data == another.GetValue(); }
    bool operator<(const Node &another) const { return m_data < another.GetValue(); }
};

// CLASE LE (Lista Simple)

template <typename Traits>
class CLinkedList {
public:
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeLinkedList<Traits>;
    using  forward_iterator  = LinkedListForwardIterator < CLinkedList<Traits> >;
    friend forward_iterator;
    friend GeneralIterator< CLinkedList<Traits> >;

    Node* m_data = nullptr; // Variable extra para que no falle GeneralIterator

// Protected para que la circular lo use
protected: 
    Node   *m_pRoot     = nullptr;
    Node   *m_pLast     = nullptr;
    size_t  m_nElements = 0;
    std::mutex m_mutex; // Candado para hilos

public:
    CLinkedList(){}

    // Copiar lista de otro lado
    CLinkedList(const CLinkedList &other) {
        Node* tmp = other.m_pRoot;
        while(tmp != nullptr){
            value_type val = tmp->GetValue(); 
            this->push_back(val, tmp->GetRef());
            tmp = tmp->GetNext();
        }
    }

    virtual ~CLinkedList(){ clear(); }

    // Borrar todo de memoria
    void clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        Node* current = m_pRoot;
        while (current != nullptr) {
            Node* next = current->GetNext();
            delete current;
            current = next;
        }
        m_pRoot = nullptr; m_pLast = nullptr; m_nElements = 0;
    }

    virtual void push_back(value_type val, ref_type ref);
    void Insert(const value_type &val, ref_type ref);
    size_t getSize() const { return m_nElements;  }

    // Buscar por indice []
    value_type& operator[](size_t index) {
        std::lock_guard<std::mutex> lock(m_mutex);
        Node* tmp = m_pRoot;
        for(size_t i = 0; i < index && tmp != nullptr; ++i) tmp = tmp->GetNext();
        return tmp->GetValueRef();
    }

    forward_iterator begin() { return forward_iterator(this, m_pRoot); }
    forward_iterator end()   { return forward_iterator(this, nullptr); }

    // Usar Foreach global
    template <typename ObjFunc, typename ...Args>
    void Foreach(ObjFunc of, Args... args){
        ::Foreach(this->begin(), this->end(), of, args...);
    }

    // Buscar el primero que cumpla
    template <typename ObjFunc, typename ...Args>
    auto FirstThat(ObjFunc of, Args... args){
        return ::FirstThat(this->begin(), this->end(), of, args...);
    }

protected:
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref);

    // Imprimir lista con cout
    friend ostream &operator<<(ostream &os, CLinkedList<Traits> &container){
        auto it = container.begin();
        for (size_t i = 0; i < container.getSize(); ++i) { 
             os << *it << " -> ";
             ++it;
        }
        return os;
    }
};

template <typename Traits>
void CLinkedList<Traits>::push_back(value_type val, ref_type ref){
    std::lock_guard<std::mutex> lock(m_mutex);
    Node *pNewNode = new Node(val, ref);
    if( !m_pRoot ) m_pRoot = pNewNode;
    else m_pLast->SetNext(pNewNode);
    m_pLast = pNewNode;
    ++m_nElements;
}

// Insertar ordenado (recursivo)
template <typename Traits>
void CLinkedList<Traits>::InternalInsert(Node *&rParent, const value_type &val, ref_type ref){
    if( !rParent || typename Traits::Func()(rParent->GetValue(), val) ){ 
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
    std::lock_guard<std::mutex> lock(m_mutex);
    InternalInsert(m_pRoot, val, ref);
}

// CLASE LEC (Lista Circular)

template <typename Traits>
class CLinkedListCircular : public CLinkedList<Traits> {
public:
    using Base = CLinkedList<Traits>;
    using typename Base::value_type;

    CLinkedListCircular() : Base() {}

    // Romper el circulo antes de destruir
    ~CLinkedListCircular() {
        if (this->m_pLast) {
            this->m_pLast->SetNext(nullptr); 
        }
    }

    // Insertar al final y cerrar circulo
    void push_back(value_type val, ref_type ref) override {
        Base::push_back(val, ref);
        if (this->m_pLast && this->m_pRoot) {
            this->m_pLast->SetNext(this->m_pRoot); // Vuelta al inicio
        }
    }
    
    // Chequear si es circular
    bool EsCircular() {
        if (!this->m_pLast) return false;
        return (this->m_pLast->GetNext() == this->m_pRoot);
    }
};

#endif // __LINKEDLIST_H__