#ifndef __CIRCULAR_LINKED_LIST_H__
#define __CIRCULAR_LINKED_LIST_H__

#include <iostream>
#include <mutex>
#include "../algorithms/sorting.h"
#include "../general/types.h"
#include "../util.h"
#include "linkedlist.h"
#include "GeneralIterator.h"
using namespace std;

template< typename Traits>
class CCircularLinkedList : public CLinkedList<Traits> {
    using  value_type = typename Traits::value_type;
    using        Node = NodeLinkedList<Traits>;

public:
    using CLinkedList<Traits>::CLinkedList;

    CCircularLinkedList(){};

    CCircularLinkedList(const CCircularLinkedList<Traits>& another) {

        lock_guard<mutex> lock(another.mtx);
        Node* i = another.m_pRoot;
        if (!i) return;
        do {
            this->push_back(i->GetValueRef(), i->GetRef());
            i = i->GetNext();
        } while (i != another.m_pRoot);
    }

    CCircularLinkedList(CCircularLinkedList<Traits>&& another) noexcept
        : CLinkedList<Traits>(std::move(another)) { }

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