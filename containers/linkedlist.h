#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include <iostream>
#include <mutex>
#include <functional>
#include "../general/types.h"
#include "GeneralIterator.h"

using namespace std;

template <typename T, typename _Func>
struct ListTrait {
    using value_type = T;
    using Func = _Func;
};

template <typename T>
struct AscendingTrait : public ListTrait<T, std::less<T>> {};

template <typename T>
struct DescendingTrait : public ListTrait<T, std::greater<T>> {};

template <typename Traits>
struct Node {
    typename Traits::value_type m_data;
    ref_type m_ref;
    Node* m_pNext = nullptr;
    Node(typename Traits::value_type v, ref_type r = -1) : m_data(v), m_ref(r) {}
};

template <typename Traits, bool IS_CIRCULAR = false>
class CLinkedList {
public:
    using value_type = typename Traits::value_type;
    using NodePtr = Node<Traits>*;

    // --- 5. Forward Iterator ---
    class iterator : public GeneralIterator<CLinkedList> {
    public:
        NodePtr m_pCurrent;
        iterator(CLinkedList* pC, NodePtr pN, size_t pos) 
            : GeneralIterator<CLinkedList>(pC, pos), m_pCurrent(pN) {}

        value_type& operator*() override { return m_pCurrent->m_data; }

        iterator& operator++() override {
            if (m_pCurrent) {
                m_pCurrent = m_pCurrent->m_pNext;
                this->m_pos++;
                if (IS_CIRCULAR && m_pCurrent == this->m_pContainer->m_pRoot) 
                    this->m_pos = this->m_pContainer->m_nElements;
            }
            return *this;
        }
    };

private:
    NodePtr m_pRoot = nullptr;
    size_t m_nElements = 0;
    // --- 11. Concurrencia (mutex) ---
    mutable std::mutex m_mtx;

public:
    CLinkedList() = default;

    // --- 1. Constructor Copia ---
    CLinkedList(const CLinkedList& other) {
        lock_guard<mutex> lock(other.m_mtx);
        NodePtr temp = other.m_pRoot;
        for(size_t i=0; i<other.m_nElements; ++i) {
            Insert(temp->m_data, temp->m_ref);
            temp = temp->m_pNext;
        }
    }

    // --- 2. Move Constructor ---
    CLinkedList(CLinkedList&& other) noexcept {
        m_pRoot = other.m_pRoot;
        m_nElements = other.m_nElements;
        other.m_pRoot = nullptr;
        other.m_nElements = 0;
    }

    // --- 3. Destructor Seguro ---
    ~CLinkedList() {
        while (m_nElements > 0) {
            NodePtr temp = m_pRoot;
            m_pRoot = m_pRoot->m_pNext;
            delete temp;
            m_nElements--;
        }
    }

    // --- 4. Operator [] ---
    value_type& operator[](size_t idx) {
        lock_guard<mutex> lock(m_mtx);
        NodePtr temp = m_pRoot;
        for(size_t i=0; i<idx; ++i) temp = temp->m_pNext;
        return temp->m_data;
    }

    void Insert(value_type val, ref_type ref) {
        lock_guard<mutex> lock(m_mtx);
        NodePtr pNew = new Node<Traits>(val, ref);
        typename Traits::Func comp;
        if (!m_pRoot) {
            m_pRoot = pNew;
            if (IS_CIRCULAR) pNew->m_pNext = m_pRoot;
        } else {
            NodePtr* pp = &m_pRoot;
            size_t count = 0;
            while (count < m_nElements && comp((*pp)->m_data, val)) {
                pp = &((*pp)->m_pNext);
                count++;
            }
            pNew->m_pNext = *pp;
            *pp = pNew;
            if (IS_CIRCULAR) {
                NodePtr last = m_pRoot;
                for(size_t i=0; i<m_nElements; ++i) last = last->m_pNext;
                last->m_pNext = m_pRoot;
            }
        }
        m_nElements++;
    }

    iterator begin() { return iterator(this, m_pRoot, 0); }
    iterator end() { return iterator(this, nullptr, m_nElements); }

    // --- 7. Foreach ---
    void Foreach(function<void(value_type&)> f) {
        lock_guard<mutex> lock(m_mtx);
        NodePtr t = m_pRoot;
        for(size_t i=0; i<m_nElements; ++i) { f(t->m_data); t = t->m_pNext; }
    }

    // --- 8. FirstThat ---
    value_type* FirstThat(function<bool(value_type)> f) {
        lock_guard<mutex> lock(m_mtx);
        NodePtr t = m_pRoot;
        for(size_t i=0; i<m_nElements; ++i) {
            if(f(t->m_data)) return &(t->m_data);
            t = t->m_pNext;
        }
        return nullptr;
    }

    // --- 9. Operator << (Persistencia Write) ---
    friend ostream& operator<<(ostream& os, const CLinkedList& l) {
        NodePtr t = l.m_pRoot;
        os << (IS_CIRCULAR ? "LEC: " : "LE: ");
        for(size_t i=0; i<l.m_nElements; ++i) {
            os << t->m_data << " ";
            t = t->m_pNext;
        }
        return os;
    }

    // --- 10. Operator >> (Persistencia Read) ---
    friend istream& operator>>(istream& is, CLinkedList& l) {
        value_type val;
        while (is >> val) {
            l.Insert(val, 0);
        }
        return is;
    }
};

#endif