#ifndef __CIRCULAR_DOUBLY_LINKED_LIST_H__
#define __CIRCULAR_DOUBLY_LINKED_LIST_H__

#include <iostream>
#include <mutex>
#include "../general/types.h"
#include "doublylinkedlist.h"

template <typename Traits>
class CCircularDoublyLinkedList {
    using Node = NodeLDE<Traits>;
    using T    = typename Traits::T;

    Node* m_pRoot = nullptr;
    Node* m_pLast = nullptr;
    size_t m_nElements = 0;
    mutable std::mutex m_mtx;

public:
    //ITERADOR
    struct Iterator {
        Node* curr;
        Node* root;
        bool  is_done;

        Iterator(Node* n, Node* r, bool done) : curr(n), root(r), is_done(done) {}

        T& operator*() { return curr->m_data; }
        
        Iterator& operator++() {
            curr = curr->pNext;
            if (curr == root) is_done = true; // Se detiene al completar la vuelta
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            if (is_done != other.is_done) return true;
            return curr != other.curr;
        }
    };
    
    struct ReverseIterator {
    Node* curr;
    Node* last;
    bool  is_done;

    ReverseIterator(Node* n, Node* l, bool done) : curr(n), last(l), is_done(done) {}

    T& operator*() { return curr->m_data; }
    
    ReverseIterator& operator++() {
        curr = curr->pPrev; // Hacia atrás
        if (curr == last) is_done = true; 
        return *this;
    }

    bool operator!=(const ReverseIterator& other) const {
        if (is_done != other.is_done) return true;
        return curr != other.curr;
    }
};

    ReverseIterator rbegin() { return ReverseIterator(m_pLast, m_pLast, m_pLast == nullptr); }
    ReverseIterator rend()   { return ReverseIterator(m_pLast, m_pLast, m_pLast != nullptr); }
    Iterator        begin() { return Iterator(m_pRoot, m_pRoot, m_pRoot == nullptr); }
    Iterator        end()   { return Iterator(m_pRoot, m_pRoot, m_pRoot != nullptr); }

    CCircularDoublyLinkedList() = default;

    // Destructor Seguro
    ~CCircularDoublyLinkedList() {
        if (!m_pRoot) return;
        std::lock_guard<std::mutex> lock(m_mtx);
        m_pLast->pNext = nullptr; // Rompemos el círculo
        Node* p = m_pRoot;
        while (p) {
            Node* next = p->pNext;
            delete p;
            p = next;
        }
    }

    void Insert(T val, ref_type ref = -1) {
        std::lock_guard<std::mutex> lock(m_mtx);
        Node* pNew = new Node(val, ref);
        if (!m_pRoot) {
            m_pRoot = m_pLast = pNew;
            pNew->pNext = pNew->pPrev = pNew;
        } else if (Traits::Compare(val, m_pRoot->m_data)) {
            pNew->pNext = m_pRoot;
            pNew->pPrev = m_pLast;
            m_pRoot->pPrev = m_pLast->pNext = pNew;
            m_pRoot = pNew;
        } else {
            Node* p = m_pRoot;
            while (p->pNext != m_pRoot && !Traits::Compare(val, p->pNext->m_data))
                p = p->pNext;
            pNew->pNext = p->pNext;
            pNew->pPrev = p;
            p->pNext->pPrev = pNew;
            p->pNext = pNew;
            if (p == m_pLast) m_pLast = pNew;
        }
        m_nElements++;
    }

    // Operator[]
    T& operator[](T1 index) {
        if (!m_pRoot) throw std::out_of_range("Lista vacia");
        T1 real_index = index % (T1)m_nElements;
        if (real_index < 0) real_index += m_nElements;
        Node* p = m_pRoot;
        for(T1 i = 0; i < real_index; ++i) p = p->pNext;
        return p->m_data;
    }

    friend std::ostream& operator<<(std::ostream& os, CCircularDoublyLinkedList& list) {
        if (!list.m_pRoot) return os << "Empty";
        Node* p = list.m_pRoot;
        do {
            os << "[" << p->m_data << ":" << p->m_ref << "] ";
            p = p->pNext;
        } while (p != list.m_pRoot);
        return os;
    }

    friend std::istream& operator>>(std::istream& is, CCircularDoublyLinkedList<Traits>& list) {
    T val;
    ref_type ref;
    if (is >> val >> ref) {
        list.Insert(val, ref);
    }
    return is;
}

};
#endif