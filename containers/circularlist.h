#ifndef __CIRCULARLIST_H__
#define __CIRCULARLIST_H__

#include "lists.h"
#include <iostream>
#include <mutex>

template <typename Traits>
class CCircularList : public CLinkedList<Traits> {
    using T    = typename Traits::value_type; 
    using Node = NodeLinkedList<Traits>;
    mutable std::mutex m_mutex;

public:
    CCircularList() : CLinkedList<Traits>() {}

    struct Iterator {
        Node* curr;
        Node* root;
        bool  is_done;
        Iterator(Node* n, Node* r, bool done) : curr(n), root(r), is_done(done) {}
        T& operator*() { 
            return const_cast<T&>(curr->GetValueRef());
        } 
        
        Iterator& operator++() {
            curr = curr->GetNext();
            if (curr == root) is_done = true; 
            return *this;
        }
        bool operator!=(const Iterator& other) const {
            if (is_done != other.is_done) return true;
            return curr != other.curr;
        }
    };

    Iterator begin() { return Iterator(this->m_pRoot, this->m_pRoot, this->m_pRoot == nullptr); }
    Iterator end()   { return Iterator(this->m_pRoot, this->m_pRoot, this->m_pRoot != nullptr); }

    T& operator[](T1 index) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (this->m_nElements == 0) throw std::out_of_range("Lista vacia");
        
        Node* pTmp = this->m_pRoot;
        T1 n = (T1)this->m_nElements;
        T1 realIndex = ((index % n) + n) % n; 
        
        for (T1 i = 0; i < realIndex; ++i) pTmp = pTmp->GetNext();

        return const_cast<T&>(pTmp->GetValueRef());
    }

    void Insert(const T& val, ref_type ref) {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (this->m_pRoot && this->m_nElements > 0) {
            Node* pLast = this->m_pRoot;
            for (size_t i = 0; i < (size_t)this->m_nElements - 1; ++i) pLast = pLast->GetNext();
            pLast->GetNextRef() = nullptr; 
        }

        this->InternalInsert(this->m_pRoot, val, ref);

        Node* pLast = this->m_pRoot;
        for (size_t i = 0; i < (size_t)this->m_nElements - 1; ++i) pLast = pLast->GetNext();
        pLast->GetNextRef() = this->m_pRoot;
    }
};

#endif