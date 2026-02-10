#ifndef __CIRCULAR_DOUBLEL_LINKED_LIST_H__
#define __CIRCULAR_DOUBLE_LINKED_LIST_H__

#include <iostream>
#include <mutex>
#include "../algorithms/sorting.h"
#include "../general/types.h"
#include "../util.h"
#include "doublelinkedlist.h"
#include "GeneralIterator.h"
using namespace std;

template< typename Traits>
class CCircularDoubleLinkedList : public CDoubleLinkedList<Traits> {
    using  value_type = typename Traits::value_type;
    using        Node = NodeDoubleLinkedList<Traits>;

public:
    CCircularDoubleLinkedList(){};

    CCircularDoubleLinkedList(const CCircularDoubleLinkedList<Traits>& another) {

        lock_guard<mutex> lock(another.mtx);
        Node* i = another.m_pRoot;
        if (!i) return;
        do {
            this->push_back(i->GetValueRef(), i->GetRef());
            i = i->GetNext();
        } while (i != another.m_pRoot);
    }

    CCircularDoubleLinkedList(CCircularDoubleLinkedList<Traits>&& another) noexcept
    {
        lock_guard<mutex> lock(another.mtx);

        this->m_pRoot = another.m_pRoot;
        this->m_pLast = another.m_pLast;
        this->m_nElements = another.m_nElements;

        another.m_pRoot = nullptr;
        another.m_pLast = nullptr;
        another.m_nElements = 0;
    }

    ~CCircularDoubleLinkedList() {
    if (this->m_pLast) {
        this->m_pLast->GetNextRef() = nullptr;  // Romper el ciclo
    }
}

    value_type &operator[](Size index) {

        Node *temp = this->m_pRoot;
        for(int i = 0; i < index; ++i) {
            temp = temp->GetNext();
        }
        return temp->GetValueRef();
    }

    template <typename ObjFunc, typename ...Args>
    void Foreach(ObjFunc of, Args... args){
        auto it = this->begin();
        for(size_t i = 0; i < this->m_nElements; ++i) {
            of(*it, args...);
            ++it;
        }
    }

    void push_back(value_type val, ref_type ref){
        Node *pNewNode = new Node(val, ref);
        if( !this->m_pRoot ) {
            this->m_pRoot = pNewNode;
            this->m_pLast = pNewNode;
        }
        else {
            this->m_pLast->GetNextRef() = pNewNode;
            this->m_pLast = pNewNode;
        }
        pNewNode->GetNextRef() = this->m_pRoot;
        ++this->m_nElements;
    }

    void Insert(const value_type &val, ref_type ref) {
        this->InternalInsert(this->m_pRoot, val, ref);
        this->m_pLast->GetNextRef() = this->m_pRoot;
    }
};

#endif