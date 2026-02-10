#ifndef __DOUBLE_LINKED_LIST_H__
#define __DOUBLE_LINKED_LIST_H__
#include <iostream>
#include "../general/types.h"
#include "../util.h"
#include <mutex>
#include <utility> 
#include "linkedlist.h"
using namespace std;

template <typename Traits>
class NodeDoubleLinkedList{
     using  value_type  = typename Traits::value_type;
     using  Node        = NodeDoubleLinkedList<Traits>;
private:
    value_type m_data;
    ref_type   m_ref;
    Node      *m_pNext = nullptr;
    Node      *m_pPrev = nullptr;
    public:
    NodeDoubleLinkedList() {}
    NodeDoubleLinkedList(value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref) {}

    value_type  GetValue() const   { return m_data; }
    value_type &GetValueRef()      { return m_data; }

    ref_type    GetRef() const     { return m_ref; }
    ref_type   &GetRefRef()        { return m_ref; }

    Node       *GetNext() const    { return m_pNext; }
    Node      *&GetNextRef()       { return m_pNext; }

    Node       *GetPrev() const    { return m_pPrev; }
    Node      *&GetPrevRef()       { return m_pPrev; }

    bool operator==(const Node &another) const { return m_data == another.GetValue(); }
    bool operator<(const Node &another) const  { return m_data < another.GetValue(); }
};

template <typename Container>
class DoubleLinkedListForwardIterator {
    using value_type = typename Container::value_type;
    using Node       = typename Container::Node;

private:
    Node *m_pCurrent;

public:
    DoubleLinkedListForwardIterator(Node *pNode) : m_pCurrent(pNode) {}

    bool operator!=(const DoubleLinkedListForwardIterator &another) const {
        return m_pCurrent != another.m_pCurrent;
    }

    bool operator==(const DoubleLinkedListForwardIterator &another) const {
        return m_pCurrent == another.m_pCurrent;
    }

    DoubleLinkedListForwardIterator &operator++() {
        if (m_pCurrent) { m_pCurrent = m_pCurrent->GetNext(); }
        return *this;
    }

    value_type &operator*() {
        return m_pCurrent->GetValueRef();
    }
};

template <typename Container>
class DoubleLinkedListBackwardIterator {
    using value_type = typename Container::value_type;
    using Node       = typename Container::Node;

private:
    Node *m_pCurrent;

public:
    DoubleLinkedListBackwardIterator(Node *pNode) : m_pCurrent(pNode) {}

    bool operator!=(const DoubleLinkedListBackwardIterator &another) const {
        return m_pCurrent != another.m_pCurrent;
    }

    bool operator==(const DoubleLinkedListBackwardIterator &another) const {
        return m_pCurrent == another.m_pCurrent;
    }

    DoubleLinkedListBackwardIterator &operator++() {
        if (m_pCurrent) { m_pCurrent = m_pCurrent->GetPrev(); }
        return *this;
    }

    value_type &operator*() {
        return m_pCurrent->GetValueRef();
    }
};

template <typename Traits>
class CDoubleLinkedList {
public:
    using value_type       = typename Traits::value_type;
    using Node             = NodeDoubleLinkedList<Traits>; 
    using forward_iterator = DoubleLinkedListForwardIterator<CDoubleLinkedList<Traits>>;
    using backward_iterator = DoubleLinkedListBackwardIterator<CDoubleLinkedList<Traits>>;
    
    friend forward_iterator;
    friend backward_iterator;

private:
    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;
    mutable std::mutex m_Block;

public:
    CDoubleLinkedList() {}

    CDoubleLinkedList(const CDoubleLinkedList &another) {
        std::lock_guard<std::mutex> lock(another.m_Block);
        Node *pTemp = another.m_pRoot;
        while (pTemp) {
            this->push_back(pTemp->GetValue(), pTemp->GetRef());
            pTemp = pTemp->GetNext();
        }
    }

    CDoubleLinkedList(CDoubleLinkedList &&another) noexcept {
        std::lock_guard<std::mutex> lock(another.m_Block);
        m_pRoot     = std::exchange(another.m_pRoot, nullptr);
        m_pLast     = std::exchange(another.m_pLast, nullptr);
        m_nElements = std::exchange(another.m_nElements, 0);
    }

    virtual ~CDoubleLinkedList();

    void push_back(const value_type &val, ref_type ref);
    void Insert(const value_type &val, ref_type ref); 
    
    size_t getSize() const { return m_nElements; }

    forward_iterator begin() { return forward_iterator(m_pRoot); }
    forward_iterator end()   { return forward_iterator(nullptr); }

    backward_iterator rbegin() { return backward_iterator(m_pLast); }
    backward_iterator rend()   { return backward_iterator(nullptr); }

    template <typename ObjFunc, typename ...Args>
    void Foreach(ObjFunc of, Args... args) {
        std::lock_guard<std::mutex> lock(m_Block);
        ::Foreach(*this, of, args...);
    }

    template <typename ObjFunc, typename ...Args>
    auto FirstThat(ObjFunc of, Args... args) {
        std::lock_guard<std::mutex> lock(m_Block);
        return ::FirstThat(*this, of, args...);
    }

    friend ostream &operator<<(ostream &os, CDoubleLinkedList<Traits> &container) {
        std::lock_guard<std::mutex> lock(container.m_Block);
        os << "[";
        Node *pTemp = container.m_pRoot;
        while (pTemp != nullptr) {
            os << "(" << pTemp->GetValue() << "|" << pTemp->GetRef() << ")";
            if (pTemp->GetNext() != nullptr) {
                os << " <-> "; 
            }
            pTemp = pTemp->GetNext();
        }
        os << "]" << endl;
        return os;
    }

    friend istream &operator>>(istream &is, CDoubleLinkedList<Traits> &container) {
        value_type val;
        ref_type   ref;
        is >> val >> ref;
        container.Insert(val, ref);
        return is;
    }

    value_type &operator[](size_t index) {
        std::lock_guard<std::mutex> lock(m_Block);
        Node *pTemp = m_pRoot;
        for (size_t i = 0; i < index && pTemp; ++i) {
            pTemp = pTemp->GetNext();
        }
        return pTemp->GetValueRef(); 
    }
};

template <typename Traits>
CDoubleLinkedList<Traits>::~CDoubleLinkedList() {
    Node *pTemp = m_pRoot;
    while (pTemp) {
        Node *pNext = pTemp->GetNext();
        delete pTemp;
        pTemp = pNext;
    }
    m_pRoot = nullptr;
    m_pLast = nullptr;
}

template <typename Traits>
void CDoubleLinkedList<Traits>::push_back(const value_type &val, ref_type ref) {
    std::lock_guard<std::mutex> lock(m_Block);
    Node *pNew = new Node(val, ref);

    if (!m_pRoot) {
        m_pRoot = pNew;
        m_pLast = pNew;
    } else {
        m_pLast->GetNextRef() = pNew; 
        pNew->GetPrevRef()    = m_pLast; 
        m_pLast               = pNew; 
    }
    ++m_nElements;
}

template <typename Traits>
void CDoubleLinkedList<Traits>::Insert(const value_type &val, ref_type ref) {
    std::lock_guard<std::mutex> lock(m_Block);
    typename Traits::Func compare;
    Node *pNew = new Node(val, ref);

    if (!m_pRoot) {
        m_pRoot = pNew;
        m_pLast = pNew;
        ++m_nElements;
        return;
    }

    if (compare(m_pRoot->GetValue(), val)) {
        pNew->GetNextRef() = m_pRoot;
        m_pRoot->GetPrevRef() = pNew; 
        m_pRoot = pNew;
        ++m_nElements;
        return;
    }

    Node *pTemp = m_pRoot;
    while (pTemp->GetNext() && !compare(pTemp->GetNext()->GetValue(), val)) {
        pTemp = pTemp->GetNext();
    }

    if (pTemp == m_pLast) {
        m_pLast->GetNextRef() = pNew;
        pNew->GetPrevRef()    = m_pLast;
        m_pLast               = pNew;
    } 
    else {
        Node *pNextNode = pTemp->GetNext();
        
        pTemp->GetNextRef() = pNew;
        pNew->GetNextRef()  = pNextNode;
        pNew->GetPrevRef()      = pTemp;
        pNextNode->GetPrevRef() = pNew;
    }
    ++m_nElements;
}
#endif