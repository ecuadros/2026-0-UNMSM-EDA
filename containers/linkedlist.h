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

// Traits para listas enlazadas
template <typename T, typename _Func>
struct ListTrait{
    using value_type = T;
    using Func       = _Func;
};

// AscendingTrait
template <typename T>
struct AscendingTrait : public ListTrait<T, std::greater<T> >{};

//DescendingTrait
template <typename T>
struct DescendingTrait : public ListTrait<T, std::less<T> >{};

// Iterador forward para listas enlazadas
template <typename Container>
class LinkedListForwardIterator : public GeneralIterator<Container>
{
    using Parent = GeneralIterator<Container>;
    using Node   = typename Container::Node;

public:
    LinkedListForwardIterator(Container *pContainer = nullptr, Node* pNode = nullptr, Node* pStart = nullptr)
        : Parent(pContainer, 0), m_pCurrent(pNode), m_pStart(pStart){}

    LinkedListForwardIterator<Container> &operator++(){
        if(m_pCurrent != nullptr){
            Node* next = m_pCurrent->GetNext();
            if(m_pStart != nullptr && next == m_pStart) m_pCurrent = nullptr;
            else m_pCurrent = next;
        }
        return *this;
    }

    typename Container::value_type& operator*(){
        return m_pCurrent->GetValueRef();
    }

    bool operator!=(const LinkedListForwardIterator<Container>& another) const {
        return !(*this == another);
    }
    bool operator==(const LinkedListForwardIterator<Container>& another) const {
        return m_pCurrent == another.m_pCurrent && this->m_pContainer == another.m_pContainer;
    }
private:
    Node* m_pCurrent = nullptr;
    Node* m_pStart = nullptr;
};

// Nodo simple para lista enlazada
template <typename Traits>
class NodeLinkedList{
public:
    using value_type = typename Traits::value_type;
    using ref_type = ::ref_type;

    NodeLinkedList(const value_type &v, ref_type r = -1, NodeLinkedList* next = nullptr)
        : m_data(v), m_ref(r), m_pNext(next) {}

    NodeLinkedList() : m_data(), m_ref(-1), m_pNext(nullptr) {}

    value_type GetValue() const { return m_data; }
    value_type& GetValueRef() { return m_data; }
    ref_type GetRef() const { return m_ref; }
    ref_type& GetRefRef() { return m_ref; }

    NodeLinkedList* GetNext() const { return m_pNext; }
    NodeLinkedList*& GetNextRef() { return m_pNext; }

private:
    value_type m_data;
    ref_type  m_ref;
    NodeLinkedList* m_pNext;
};

// Base de Lista enlazada 
template <typename Traits>
class CLinkedList {
public:
    using value_type = typename Traits::value_type;
    using forward_iterator = LinkedListForwardIterator< CLinkedList<Traits> >;
    using Node = NodeLinkedList<Traits>;

    friend forward_iterator;

protected:
    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;
    mutable recursive_mutex m_mutex;

public:
    CLinkedList(): m_pRoot(nullptr), m_pLast(nullptr), m_nElements(0) {}

    // Constructor copia
    CLinkedList(const CLinkedList<Traits> &another)
        : m_pRoot(nullptr), m_pLast(nullptr), m_nElements(0)
    {
        lock_guard<recursive_mutex> lock(another.m_mutex);
        Node* cur = another.m_pRoot;
        if (!cur) return;
        do {
            push_back(cur->GetValue(), cur->GetRef());
            cur = cur->GetNext();
        } while (cur && cur != another.m_pRoot);
    }

    // Move constructor
    CLinkedList(CLinkedList<Traits> &&another) noexcept
        : m_pRoot(nullptr), m_pLast(nullptr), m_nElements(0)
    {
        lock_guard<recursive_mutex> lock(another.m_mutex);
        m_pRoot = exchange(another.m_pRoot, nullptr);
        m_pLast = exchange(another.m_pLast, nullptr);
        m_nElements = exchange(another.m_nElements, 0);
    }

    virtual ~CLinkedList(){
        lock_guard<recursive_mutex> lock(m_mutex);
        Node* cur = m_pRoot;
        while (cur) {
            Node* next = cur->GetNext();
            delete cur;
            cur = next;
            if (cur == m_pRoot) break; // protección por si es circular
        }
        m_pRoot = nullptr;
        m_pLast = nullptr;
        m_nElements = 0;
    }

    // Iterators
    forward_iterator begin(){ return forward_iterator(this, m_pRoot); }
    forward_iterator end(){ return forward_iterator(this, nullptr); }

    // Acceso por índice (lineal)
    value_type& operator[](size_t index){
        lock_guard<recursive_mutex> lock(m_mutex);
        assert(index < m_nElements);
        Node* cur = m_pRoot;
        for (size_t i = 0; i < index; ++i) cur = cur->GetNext();
        return cur->GetValueRef();
    }

    const value_type& operator[](size_t index) const{
        lock_guard<recursive_mutex> lock(m_mutex);
        assert(index < m_nElements);
        Node* cur = m_pRoot;
        for (size_t i = 0; i < index; ++i) cur = cur->GetNext();
        return cur->GetValue();
    }

    void push_back(const value_type &val, ref_type ref = -1){
        lock_guard<recursive_mutex> lock(m_mutex);
        Node* node = new Node(val, ref, nullptr);
        if (!m_pRoot) {
            m_pRoot = node;
            m_pLast = node;
        } else {
            m_pLast->GetNextRef() = node;
            m_pLast = node;
        }
        ++m_nElements;
    }

    // Inserción ordenada usando Traits
    void Insert(const value_type &val, ref_type ref = -1){
        lock_guard<recursive_mutex> lock(m_mutex);
        if (!m_pRoot) { push_back(val, ref); return; }
        
        Node **pp = &m_pRoot;
        Node *prev = nullptr;
        while (*pp) {
            if (typename Traits::Func()((*pp)->GetValue(), val)) break;
            prev = *pp;
            pp = &((*pp)->GetNextRef());
            if (*pp == m_pRoot) break; // en caso circular
        }
        Node* inserted = new Node(val, ref, *pp);
        if (prev) prev->GetNextRef() = inserted;
        else m_pRoot = inserted;
        
        if (inserted->GetNext() == nullptr) m_pLast = inserted;
        ++m_nElements;
    }

    size_t getSize() const { lock_guard<recursive_mutex> lock(m_mutex); return m_nElements; }

    template <typename ObjFunc, typename ...Args>
    void Foreach(ObjFunc of, Args... args){
        lock_guard<recursive_mutex> lock(m_mutex);
        Node* cur = m_pRoot;
        while (cur) {
            of(cur->GetValueRef(), args...);
            cur = cur->GetNext();
            if (cur == m_pRoot) break; // si es circular
        }
    }

    template <typename ObjFunc, typename ...Args>
    auto FirstThat(ObjFunc of, Args... args){
        lock_guard<recursive_mutex> lock(m_mutex);
        Node* cur = m_pRoot;
        while (cur) {
            if (of(cur->GetValueRef(), args...))
                return forward_iterator(this, cur);
            cur = cur->GetNext();
            if (cur == m_pRoot) break;
        }
        return end();
    }

    
    friend ostream &operator<<(ostream &os, CLinkedList<Traits> &container){
        lock_guard<recursive_mutex> lock(container.m_mutex);
        os << "CLinkedList: size = " << container.getSize() << endl;
        os << "[";
        Node* pTemp = container.m_pRoot;
        bool first = true;
        while (pTemp) {
            if (!first) os << ",";
            first = false;
            os << "(" << pTemp->GetValue() << ":" << pTemp->GetRef() << ")";
            pTemp = pTemp->GetNext();
            if (pTemp == container.m_pRoot) break; // protección circular
        }
        os << "]" << endl;
        return os;
    }

    friend istream &operator>>(istream &is, CLinkedList<Traits> &container) {
        lock_guard<recursive_mutex> lock(container.m_mutex);
        char c;
        value_type val;
        ref_type ref;
        // limpiamos
        Node* cur = container.m_pRoot;
        while (cur) {
            Node* next = cur->GetNext();
            delete cur;
            cur = next;
            if (cur == container.m_pRoot) break;
        }
        container.m_pRoot = container.m_pLast = nullptr;
        container.m_nElements = 0;

        while (is >> c && c != '[') {}
        while (is >> c && c != ']') {
            if (c == '(') {
                if (!(is >> val)) break;
                if (!(is >> c)) break; // ':'
                if (c == ':') {
                    is >> ref;
                }
                container.Insert(val, ref);
                // consume until comma or ]
                while (is >> c && c != ',' && c != ']');
                if (c == ']') break;
            }
        }
        return is;
    }
};

// Lista Circular simple 
template <typename Traits>
class CCircularLinkedList : public CLinkedList<Traits> {
    using Node = typename CLinkedList<Traits>::Node;
    using value_type = typename Traits::value_type;
public:
    CCircularLinkedList() : CLinkedList<Traits>() {}
    CCircularLinkedList(const CCircularLinkedList<Traits> &another)
        : CLinkedList<Traits>(another)
    {
        lock_guard<recursive_mutex> lock(this->m_mutex);
        if (this->m_pLast) this->m_pLast->GetNextRef() = this->m_pRoot;
    }

    CCircularLinkedList(CCircularLinkedList<Traits> &&another) noexcept
        : CLinkedList<Traits>(move(another)) {}

    ~CCircularLinkedList(){
        lock_guard<recursive_mutex> lock(this->m_mutex);
        if (this->m_pLast) this->m_pLast->GetNextRef() = nullptr;
    }

    void push_back(const value_type &val, long ref = -1) {
        lock_guard<recursive_mutex> lock(this->m_mutex);
        CLinkedList<Traits>::push_back(val, ref);
        if (this->m_pLast) this->m_pLast->GetNextRef() = this->m_pRoot;
    }

    
    typename CLinkedList<Traits>::forward_iterator begin(){
        return typename CLinkedList<Traits>::forward_iterator(this, this->m_pRoot, this->m_pRoot);
    }
    typename CLinkedList<Traits>::forward_iterator end(){
        return typename CLinkedList<Traits>::forward_iterator(this, nullptr, nullptr);
    }
};

#endif // __LINKEDLIST_H__
