#ifndef __DOUBLYLINKEDLIST_H__
#define __DOUBLYLINKEDLIST_H__

#include <iostream>
#include <mutex>
#include "../general/types.h"
#include "util.h"

template <typename Traits>
struct NodeLDE {
    using value_type = typename Traits::T; 
    value_type m_data;
    ref_type   m_ref;
    NodeLDE* pNext;
    NodeLDE* pPrev;

    NodeLDE(value_type v, ref_type r = -1) 
        : m_data(v), m_ref(r), pNext(nullptr), pPrev(nullptr) {}
    
    value_type& GetValueRef() { return m_data; }
    ref_type    GetRef()      { return m_ref;  }
};

template <typename Traits>
class CDoublyLinkedList {
    using Node = NodeLDE<Traits>;
    using T    = typename Traits::T;

    Node* m_pRoot = nullptr;
    Node* m_pLast = nullptr;
    size_t m_nElements = 0;
    mutable std::mutex m_mtx;

private:
    void InternalInsert(Node*& pRoot, T val, ref_type ref) {
        Node* pNew = new Node(val, ref);
        
        // 1. Caso lista vacía
        if (!pRoot) {
            pRoot = m_pLast = pNew;
            return;
        }

        // 2. Insertar antes de la raíz según el Trait
        if (Traits::Compare(val, pRoot->m_data)) {
            pNew->pNext = pRoot;
            pRoot->pPrev = pNew;
            pRoot = pNew;
            return;
        }

        // 3. Buscar posición intermedia o final
        Node* pActual = pRoot;
        while (pActual->pNext && !Traits::Compare(val, pActual->pNext->m_data)) {
            pActual = pActual->pNext;
        }

        pNew->pNext = pActual->pNext;
        pNew->pPrev = pActual;
        
        if (pActual->pNext) 
            pActual->pNext->pPrev = pNew;
        else
            m_pLast = pNew;
            
        pActual->pNext = pNew;
    }

public:
    CDoublyLinkedList() = default;

    void Insert(T val, ref_type ref = -1) {
        std::lock_guard<std::mutex> lock(m_mtx);
        InternalInsert(this->m_pRoot, val, ref);
        m_nElements++;
    }

    // Constructor Copia: Usamos Insert para mantener la integridad
    CDoublyLinkedList(const CDoublyLinkedList& other) {
        std::lock_guard<std::mutex> lock(other.m_mtx);
        for (Node* p = other.m_pRoot; p != nullptr; p = p->pNext) {
            this->Insert(p->m_data, p->m_ref);
        }
    }

    CDoublyLinkedList(CDoublyLinkedList&& other) noexcept {
        std::lock_guard<std::mutex> lock(other.m_mtx);
        m_pRoot = other.m_pRoot;
        m_pLast = other.m_pLast;
        m_nElements = other.m_nElements;
        other.m_pRoot = other.m_pLast = nullptr;
        other.m_nElements = 0;
    }

    ~CDoublyLinkedList() {
        std::lock_guard<std::mutex> lock(m_mtx);
        Node* p = m_pRoot;
        while (p) {
            Node* next = p->pNext;
            delete p;
            p = next;
        }
    }

    // Iteradores
    struct Iterator {
        Node* curr;
        Iterator(Node* n) : curr(n) {}
        T& operator*() { return curr->GetValueRef(); }
        Iterator& operator++() { if(curr) curr = curr->pNext; return *this; }
        bool operator!=(const Iterator& other) const { return curr != other.curr; }
    };

    struct ReverseIterator {
        Node* curr;
        ReverseIterator(Node* n) : curr(n) {}
        T& operator*() { return curr->GetValueRef(); }
        ReverseIterator& operator++() { if(curr) curr = curr->pPrev; return *this; }
        bool operator!=(const ReverseIterator& other) const { return curr != other.curr; }
    };

    Iterator begin() { return Iterator(m_pRoot); }
    Iterator end()   { return Iterator(nullptr); }
    ReverseIterator rbegin() { return ReverseIterator(m_pLast); }
    ReverseIterator rend()   { return ReverseIterator(nullptr); }

    size_t getSize() const { 
        std::lock_guard<std::mutex> lock(m_mtx);
        return m_nElements; 
    }

    friend std::ostream& operator<<(std::ostream& os, CDoublyLinkedList<Traits>& list) {
        os << "LDE size=" << list.getSize() << ": [";
        for (auto val : list) {
            os << val << ",";
        }
        os << "]" << std::endl;
        return os;
    }
    
    friend std::istream& operator>>(std::istream& is, CDoublyLinkedList<Traits>& list) {
    T val;
    ref_type ref;
        if (is >> val>> ref) 
            list.Insert(val, ref);
        return is;
    }
};

#endif