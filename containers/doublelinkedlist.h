#ifndef __DOUBLYLINKEDLIST_H__
#define __DOUBLYLINKEDLIST_H__

#include <iostream>
#include <mutex>
#include "../general/types.h"
#include "../util.h"
#include "GeneralIterator.h"
#include "../foreach.h"

using namespace std;

// Iterador para LDE (avanza y retrocede)
template <typename Container>
class DoublyLinkedListIterator : public GeneralIterator<Container>
{
    using Parent = GeneralIterator<Container>;
    using Node   = typename Container::Node;
public:
    DoublyLinkedListIterator(Container *pContainer, Node* pNode) 
        : Parent(pContainer, 0) { 
        this->m_data = pNode; 
    }
    bool operator!=(const DoublyLinkedListIterator<Container> &another){
        return this->m_data != another.m_data;
    }
    // Avanzar (next)
    DoublyLinkedListIterator<Container> &operator++(){
        if(this->m_data) this->m_data = this->m_data->GetNext();
        return *this;
    }
    // Retroceder (prev)
    DoublyLinkedListIterator<Container> &operator--(){
        if(this->m_data) this->m_data = this->m_data->GetPrev();
        return *this;
    }
    typename Container::value_type &operator*(){
        return this->m_data->GetValueRef();
    }
};

// Nodo Doble
template <typename Traits>
class NodeDoublyLinkedList{
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeDoublyLinkedList<Traits>;
private:
    value_type m_data;
    ref_type   m_ref;
    Node      *m_pNext = nullptr;
    Node      *m_pPrev = nullptr;

public:
    NodeDoublyLinkedList(){}
    NodeDoublyLinkedList(value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref){}
    
    value_type &GetValueRef() { return m_data; }
    value_type  GetValue() const { return m_data; }
    ref_type    GetRef() const { return m_ref; }

    Node *GetNext() const { return m_pNext; }
    Node *GetPrev() const { return m_pPrev; }
    
    void SetNext(Node* next) { m_pNext = next; }
    void SetPrev(Node* prev) { m_pPrev = prev; }
};

// CLASE LDE (Lista Doblemente Enlazada)

template <typename Traits>
class CDoublyLinkedList {
public:
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeDoublyLinkedList<Traits>;
    using  iterator    = DoublyLinkedListIterator < CDoublyLinkedList<Traits> >;
    
    friend iterator;
    friend GeneralIterator< CDoublyLinkedList<Traits> >;

    Node* m_data = nullptr;

protected:
    Node   *m_pRoot     = nullptr;
    Node   *m_pLast     = nullptr;
    size_t  m_nElements = 0;
    std::mutex m_mutex;

public:
    CDoublyLinkedList(){}

    virtual ~CDoublyLinkedList(){ clear(); }

    // Copy Constructor
    CDoublyLinkedList(const CDoublyLinkedList &other) {
         Node* tmp = other.m_pRoot;
         while(tmp){
             this->push_back(tmp->GetValue(), tmp->GetRef());
             tmp = tmp->GetNext();
         }
    }

    // Move Constructor 
    CDoublyLinkedList(CDoublyLinkedList &&other) noexcept {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        m_pRoot = other.m_pRoot;
        m_pLast = other.m_pLast;
        m_nElements = other.m_nElements;
        
        other.m_pRoot = nullptr;
        other.m_pLast = nullptr;
        other.m_nElements = 0;
    }

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
    virtual void Insert(const value_type &val, ref_type ref);
    size_t getSize() const { return m_nElements;  }

    iterator begin() { return iterator(this, m_pRoot); }
    iterator end()   { return iterator(this, nullptr); }
    iterator rbegin() { return iterator(this, m_pLast); } 
    iterator rend()   { return iterator(this, nullptr); }

    template <typename ObjFunc, typename ...Args>
    void Foreach(ObjFunc of, Args... args){
        ::Foreach(this->begin(), this->end(), of, args...);
    }

    // Imprimir
    friend ostream &operator<<(ostream &os, CDoublyLinkedList<Traits> &container){
        auto it = container.begin();
        for (size_t i = 0; i < container.getSize(); ++i) { 
             os << *it << " <-> "; 
             ++it;
        }
        return os;
    }
};

template <typename Traits>
void CDoublyLinkedList<Traits>::push_back(value_type val, ref_type ref){
    std::lock_guard<std::mutex> lock(m_mutex);
    Node *pNew = new Node(val, ref);
    if(!m_pRoot) { 
        m_pRoot = pNew; m_pLast = pNew;
    } else {
        pNew->SetPrev(m_pLast);
        m_pLast->SetNext(pNew);
        m_pLast = pNew;
    }
    ++m_nElements;
}

template <typename Traits>
void CDoublyLinkedList<Traits>::Insert(const value_type &val, ref_type ref){
    std::lock_guard<std::mutex> lock(m_mutex);
    Node* current = m_pRoot;
    Node* previous = nullptr;
    while(current && !typename Traits::Func()(current->GetValue(), val)) {
        previous = current;
        current = current->GetNext();
    }
    Node* pNew = new Node(val, ref);
    pNew->SetNext(current);
    pNew->SetPrev(previous);

    if(previous) previous->SetNext(pNew);
    else m_pRoot = pNew;

    if(current) current->SetPrev(pNew);
    else m_pLast = pNew;

    ++m_nElements;
}

// CLASE LDEC (Lista Doble CIRCULAR) 

template <typename Traits>
class CDoublyLinkedListCircular : public CDoublyLinkedList<Traits> {
public:
    using Base = CDoublyLinkedList<Traits>;
    using typename Base::value_type;

    CDoublyLinkedListCircular() : Base() {}

    ~CDoublyLinkedListCircular() {
        // Romper el circulo antes de destruir
        if (this->m_pLast) {
            this->m_pLast->SetNext(nullptr);
            if(this->m_pRoot) this->m_pRoot->SetPrev(nullptr);
        }
    }

    void push_back(value_type val, ref_type ref) override {
        Base::push_back(val, ref); // Inserto normal
        ConectarCirculo();
    }

    void Insert(const value_type &val, ref_type ref) override {
        Base::Insert(val, ref); // Inserto normal
        ConectarCirculo();
    }

    void ConectarCirculo(){
        if (this->m_pLast && this->m_pRoot) {
            this->m_pLast->SetNext(this->m_pRoot); // Ultimo -> Primero
            this->m_pRoot->SetPrev(this->m_pLast); // Primero -> Ultimo (Doble enlace)
        }
    }

    bool EsCircular() {
        if (!this->m_pLast || !this->m_pRoot) return false;
        return (this->m_pLast->GetNext() == this->m_pRoot) && 
               (this->m_pRoot->GetPrev() == this->m_pLast);
    }
};

#endif // __DOUBLYLINKEDLIST_H__