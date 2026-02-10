#ifndef __DOUBLE_LINKED_LIST_H__
#define __DOUBLE_LINKED_LIST_H__

#include <iostream>
#include <cassert>
#include <utility>
#include <mutex>
#include "../general/types.h"
#include "../util.h"
#include "GeneralIterator.h"
using namespace std;

// Traits para listas doblemente enlazadas (misma idea que en linkedlist.h)
template <typename T, typename _Func>
struct DoubleListTrait{
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct DoubleAscendingTrait : public DoubleListTrait<T, std::greater<T> >{};

template <typename T>
struct DoubleDescendingTrait : public DoubleListTrait<T, std::less<T> >{};

// Iterador forward para listas doblemente enlazadas
template <typename Container>
class DoubleLinkedForwardIterator : public GeneralIterator<Container>
{
    using Parent = GeneralIterator<Container>;
    using Node   = typename Container::Node;

public:
    DoubleLinkedForwardIterator(Container *pContainer = nullptr, Node* pNode = nullptr, Node* pStart = nullptr)
        : Parent(pContainer, 0), m_pCurrent(pNode), m_pStart(pStart){}

    DoubleLinkedForwardIterator<Container> &operator++(){
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

    bool operator!=(const DoubleLinkedForwardIterator<Container>& another) const {
        return !(*this == another);
    }
    bool operator==(const DoubleLinkedForwardIterator<Container>& another) const {
        return m_pCurrent == another.m_pCurrent && this->m_pContainer == another.m_pContainer;
    }
private:
    Node* m_pCurrent = nullptr;
    Node* m_pStart = nullptr; // si no es null, delimitador para iteración circular
};

// Iterador backward para listas doblemente enlazadas
template <typename Container>
class DoubleLinkedBackwardIterator : public GeneralIterator<Container>
{
    using Parent = GeneralIterator<Container>;
    using Node   = typename Container::Node;

public:
    DoubleLinkedBackwardIterator(Container *pContainer = nullptr, Node* pNode = nullptr, Node* pStart = nullptr)
        : Parent(pContainer, 0), m_pCurrent(pNode), m_pStart(pStart){}

    // Avanzar el iterador recorre hacia prev 
    DoubleLinkedBackwardIterator<Container> &operator++(){
        if(m_pCurrent != nullptr){
            Node* prev = m_pCurrent->GetPrev();
            if(m_pStart != nullptr && prev == m_pStart) m_pCurrent = nullptr;
            else m_pCurrent = prev;
        }
        return *this;
    }

    typename Container::value_type& operator*(){
        return m_pCurrent->GetValueRef();
    }

    bool operator!=(const DoubleLinkedBackwardIterator<Container>& another) const {
        return !(*this == another);
    }
    bool operator==(const DoubleLinkedBackwardIterator<Container>& another) const {
        return m_pCurrent == another.m_pCurrent && this->m_pContainer == another.m_pContainer;
    }
private:
    Node* m_pCurrent = nullptr;
    Node* m_pStart = nullptr; // delimita el final en iteración backward circular
};

// Nodo doble para lista doblemente enlazada
template <typename Traits>
class NodeDoubleLinkedList{
public:
    using value_type = typename Traits::value_type;
    using ref_type = ::ref_type;

    NodeDoubleLinkedList(const value_type &v, ref_type r = -1, NodeDoubleLinkedList* prev = nullptr, NodeDoubleLinkedList* next = nullptr)
        : m_data(v), m_ref(r), m_pPrev(prev), m_pNext(next) {}

    NodeDoubleLinkedList() : m_data(), m_ref(-1), m_pPrev(nullptr), m_pNext(nullptr) {}

    value_type GetValue() const { return m_data; }
    value_type& GetValueRef() { return m_data; }
    ref_type GetRef() const { return m_ref; }
    ref_type& GetRefRef() { return m_ref; }

    NodeDoubleLinkedList* GetNext() const { return m_pNext; }
    NodeDoubleLinkedList*& GetNextRef() { return m_pNext; }

    NodeDoubleLinkedList* GetPrev() const { return m_pPrev; }
    NodeDoubleLinkedList*& GetPrevRef() { return m_pPrev; }

private:
    value_type m_data;
    ref_type  m_ref;
    NodeDoubleLinkedList* m_pPrev;
    NodeDoubleLinkedList* m_pNext;
};

// Base de lista doblemente enlazada
template <typename Traits>
class CDoubleLinkedList {
public:
    using value_type = typename Traits::value_type;
    using forward_iterator = DoubleLinkedForwardIterator< CDoubleLinkedList<Traits> >;
    using backward_iterator = DoubleLinkedBackwardIterator< CDoubleLinkedList<Traits> >;
    using Node = NodeDoubleLinkedList<Traits>;

    friend forward_iterator;
    friend backward_iterator;

protected:
    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;
    mutable recursive_mutex m_mutex;

public:
    CDoubleLinkedList(): m_pRoot(nullptr), m_pLast(nullptr), m_nElements(0) {}

    // Constructor copia
    CDoubleLinkedList(const CDoubleLinkedList<Traits> &another)
        : m_pRoot(nullptr), m_pLast(nullptr), m_nElements(0)
    {
        lock_guard<recursive_mutex> lock(another.m_mutex);
        Node* cur = another.m_pRoot;
        if (!cur) return;
        while (cur) {
            push_back(cur->GetValue(), cur->GetRef());
            cur = cur->GetNext();
            if (cur == another.m_pRoot) break; // protección por si es circular
        }
    }

    // Move constructor
    CDoubleLinkedList(CDoubleLinkedList<Traits> &&another) noexcept
        : m_pRoot(nullptr), m_pLast(nullptr), m_nElements(0)
    {
        lock_guard<recursive_mutex> lock(another.m_mutex);
        m_pRoot = exchange(another.m_pRoot, nullptr);
        m_pLast = exchange(another.m_pLast, nullptr);
        m_nElements = exchange(another.m_nElements, 0);
    }

    virtual ~CDoubleLinkedList(){
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
    backward_iterator rbegin(){ return backward_iterator(this, m_pLast); }
    backward_iterator rend(){ return backward_iterator(this, nullptr); }

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
        Node* node = new Node(val, ref, m_pLast, nullptr);
        if (!m_pRoot) {
            m_pRoot = node;
            m_pLast = node;
        } else {
            m_pLast->GetNextRef() = node;
            node->GetPrevRef() = m_pLast;
            m_pLast = node;
        }
        ++m_nElements;
    }

    // Inserción ordenada usando Traits
    void Insert(const value_type &val, ref_type ref = -1){
        lock_guard<recursive_mutex> lock(m_mutex);
        if (!m_pRoot) { push_back(val, ref); return; }
        Node* cur = m_pRoot;
        Node* prev = nullptr;
        while (cur) {
            if (typename Traits::Func()(cur->GetValue(), val)) break;
            prev = cur;
            cur = cur->GetNext();
            if (cur == m_pRoot) break; // si es circular
        }
        Node* inserted = new Node(val, ref, prev, cur);
        if (prev) prev->GetNextRef() = inserted;
        else m_pRoot = inserted;
        if (cur) cur->GetPrevRef() = inserted;
        if (!inserted->GetNext()) m_pLast = inserted;
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

    friend ostream &operator<<(ostream &os, CDoubleLinkedList<Traits> &container){
        lock_guard<recursive_mutex> lock(container.m_mutex);
        os << "CDoubleLinkedList: size = " << container.getSize() << endl;
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

    friend istream &operator>>(istream &is, CDoubleLinkedList<Traits> &container) {
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

// Variante circular: conecta primero y último
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
            if (this->m_pRoot) this->m_pRoot->GetPrevRef() = this->m_pLast;
        }
    }

    CCircularDoubleLinkedList(CCircularDoubleLinkedList<Traits> &&another) noexcept
        : CDoubleLinkedList<Traits>(move(another)) {}

    ~CCircularDoubleLinkedList(){
        lock_guard<recursive_mutex> lock(this->m_mutex);
        if (this->m_pLast) this->m_pLast->GetNextRef() = nullptr;
        if (this->m_pRoot) this->m_pRoot->GetPrevRef() = nullptr;
    }

    void push_back(const value_type &val, long ref = -1) {
        lock_guard<recursive_mutex> lock(this->m_mutex);
        CDoubleLinkedList<Traits>::push_back(val, ref);
        if (this->m_pLast && this->m_pRoot) {
            this->m_pLast->GetNextRef() = this->m_pRoot;
            this->m_pRoot->GetPrevRef() = this->m_pLast;
        }
    }

    // Overriding begin/end to support finite iteration over circular structure
    typename CDoubleLinkedList<Traits>::forward_iterator begin(){
        return typename CDoubleLinkedList<Traits>::forward_iterator(this, this->m_pRoot, this->m_pRoot);
    }
    typename CDoubleLinkedList<Traits>::forward_iterator end(){
        return typename CDoubleLinkedList<Traits>::forward_iterator(this, nullptr, nullptr);
    }

    typename CDoubleLinkedList<Traits>::backward_iterator rbegin(){
        return typename CDoubleLinkedList<Traits>::backward_iterator(this, this->m_pLast, this->m_pLast);
    }
    typename CDoubleLinkedList<Traits>::backward_iterator rend(){
        return typename CDoubleLinkedList<Traits>::backward_iterator(this, nullptr, nullptr);
    }
};

#endif // __DOUBLE_LINKED_LIST_H__