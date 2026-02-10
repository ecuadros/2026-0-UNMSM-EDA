#ifndef __DOUBLELINKEDLIST_H__
#define __DOUBLELINKEDLIST_H__

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

template <typename Traits>
struct DNode {
    typename Traits::value_type m_data;
    ref_type m_ref;
    DNode *m_pNext = nullptr, *m_pPrev = nullptr;
    DNode(typename Traits::value_type v, ref_type r = -1) : m_data(v), m_ref(r) {}
};

template <typename Traits, bool IS_CIRCULAR = false>
class CDoubleLinkedList {
public:
    using value_type = typename Traits::value_type;
    using NodePtr = DNode<Traits>*;

    // --- 5. Forward Iterator (begin, end) ---
    class iterator : public GeneralIterator<CDoubleLinkedList> {
    public:
        NodePtr m_pCurr;
        iterator(CDoubleLinkedList* pC, NodePtr pN, size_t pos) 
            : GeneralIterator<CDoubleLinkedList>(pC, pos), m_pCurr(pN) {}
        value_type& operator*() override { return m_pCurr->m_data; }
        iterator& operator++() override {
            if (m_pCurr) {
                m_pCurr = m_pCurr->m_pNext;
                this->m_pos++;
                if (IS_CIRCULAR && m_pCurr == this->m_pContainer->m_pRoot) this->m_pos = this->m_pContainer->m_nElements;
            }
            return *this;
        }
    };

    // --- 6. Backward Iterator (rbegin, rend) ---
    class reverse_iterator : public GeneralIterator<CDoubleLinkedList> {
    public:
        NodePtr m_pCurr;
        reverse_iterator(CDoubleLinkedList* pC, NodePtr pN, size_t pos) 
            : GeneralIterator<CDoubleLinkedList>(pC, pos), m_pCurr(pN) {}
        value_type& operator*() override { return m_pCurr->m_data; }
        reverse_iterator& operator++() override {
            if (m_pCurr) {
                m_pCurr = m_pCurr->m_pPrev;
                this->m_pos++;
                if (IS_CIRCULAR && m_pCurr == this->m_pContainer->m_pLast) this->m_pos = this->m_pContainer->m_nElements;
            }
            return *this;
        }
    };

private:
    NodePtr m_pRoot = nullptr, m_pLast = nullptr;
    size_t m_nElements = 0;
    // --- 12. Concurrencia ---
    mutable std::mutex m_mtx;

public:
    CDoubleLinkedList() = default;

    // --- 1. Constructor Copia ---
    CDoubleLinkedList(const CDoubleLinkedList& other) {
        lock_guard<mutex> lock(other.m_mtx);
        NodePtr temp = other.m_pRoot;
        for(size_t i=0; i<other.m_nElements; ++i) {
            Insert(temp->m_data, temp->m_ref);
            temp = temp->m_pNext;
        }
    }

    // --- 2. Move Constructor ---
    CDoubleLinkedList(CDoubleLinkedList&& other) noexcept {
        m_pRoot = other.m_pRoot; m_pLast = other.m_pLast;
        m_nElements = other.m_nElements;
        other.m_pRoot = other.m_pLast = nullptr; other.m_nElements = 0;
    }

    // --- 3. Destructor Seguro ---
    ~CDoubleLinkedList() {
        while (m_nElements > 0) {
            NodePtr temp = m_pRoot;
            m_pRoot = m_pRoot->m_pNext;
            delete temp;
            m_nElements--;
        }
    }

    // --- 4. operator[] ---
    value_type& operator[](size_t idx) {
        lock_guard<mutex> lock(m_mtx);
        NodePtr t = m_pRoot;
        for(size_t i=0; i<idx; ++i) t = t->m_pNext;
        return t->m_data;
    }

    void Insert(value_type val, ref_type ref) {
        lock_guard<mutex> lock(m_mtx);
        NodePtr pNew = new DNode<Traits>(val, ref);
        typename Traits::Func comp;
        if (!m_pRoot) {
            m_pRoot = m_pLast = pNew;
            if (IS_CIRCULAR) { pNew->m_pNext = pNew->m_pPrev = pNew; }
        } else {
            NodePtr curr = m_pRoot;
            size_t count = 0;
            while (count < m_nElements && comp(curr->m_data, val)) {
                curr = curr->m_pNext;
                count++;
            }
            if (count == 0) { // Inicio
                pNew->m_pNext = m_pRoot; pNew->m_pPrev = m_pLast;
                m_pRoot->m_pPrev = pNew; m_pRoot = pNew;
                if (IS_CIRCULAR) m_pLast->m_pNext = m_pRoot;
            } else if (count == m_nElements) { // Final
                pNew->m_pPrev = m_pLast; pNew->m_pNext = m_pRoot;
                m_pLast->m_pNext = pNew; m_pLast = pNew;
                if (IS_CIRCULAR) m_pRoot->m_pPrev = m_pLast;
            } else { // Medio
                pNew->m_pNext = curr; pNew->m_pPrev = curr->m_pPrev;
                curr->m_pPrev->m_pNext = pNew; curr->m_pPrev = pNew;
            }
        }
        m_nElements++;
    }

    iterator begin() { return iterator(this, m_pRoot, 0); }
    iterator end() { return iterator(this, nullptr, m_nElements); }
    reverse_iterator rbegin() { return reverse_iterator(this, m_pLast, 0); }
    reverse_iterator rend() { return reverse_iterator(this, nullptr, m_nElements); }

    // --- 8. Foreach ---
    void Foreach(function<void(value_type&)> f) {
        lock_guard<mutex> lock(m_mtx);
        NodePtr t = m_pRoot;
        for(size_t i=0; i<m_nElements; ++i) { f(t->m_data); t = t->m_pNext; }
    }

    // --- 9. FirstThat ---
    value_type* FirstThat(function<bool(value_type)> f) {
        lock_guard<mutex> lock(m_mtx);
        NodePtr t = m_pRoot;
        for(size_t i=0; i<m_nElements; ++i) {
            if(f(t->m_data)) return &(t->m_data);
            t = t->m_pNext;
        }
        return nullptr;
    }

    // --- 10. operator<< ---
    friend ostream& operator<<(ostream& os, const CDoubleLinkedList& l) {
        NodePtr t = l.m_pRoot;
        os << (IS_CIRCULAR ? "LDEC: " : "LDE: ");
        for(size_t i=0; i<l.m_nElements; ++i) { os << t->m_data << " "; t = t->m_pNext; }
        return os;
    }

    // --- 11. operator>> ---
    friend istream& operator>>(istream& is, CDoubleLinkedList& l) {
        value_type v;
        while(is >> v) l.Insert(v, 0);
        return is;
    }
};

#endif