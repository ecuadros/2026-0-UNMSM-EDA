#ifndef __DOUBLE_LINKED_LIST_H__
#define __DOUBLE_LINKED_LIST_H__

#include <iostream>
#include <mutex>
#include <assert.h>
#include <functional>

#include "../general/types.h"
#include "../util.h"
#include "GeneralIterator.h"
using namespace std;

// TODO: ForwardIterator para listas doblemente enlazadas

// TODO: BackwardIterator para listas doblemente enlazadas
template <typename T, typename _Func>
struct ListTrait {
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct AscendingTrait : public ListTrait<T, std::greater<T>> {};

template <typename T>
struct DescendingTrait : public ListTrait<T, std::less<T>> {};

template <typename Traits>
class NodeDoubleLinkedList{
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeDoubleLinkedList<Traits>;
private:
    value_type m_data;
    ref_type   m_ref;
    Node *m_pNext = nullptr;
    Node *m_pPrev = nullptr;
public: 
    NodeDoubleLinkedList(value_type _value, ref_type _ref = -1, Node *_pNext = nullptr, Node *_pPrev = nullptr)
        :m_data(_value), m_ref(_ref), m_pNext(_pNext), m_pPrev(_pPrev){}
    
    value_type GetValue() const{ return m_data; }
    value_type &GetValueRef()       { return m_data; }
    ref_type    GetRef()      const { return m_ref;  }

    Node* GetNext()     const { return m_pNext; }
    Node*& GetNextRef()        { return m_pNext; }
    
    Node* GetPrev()     const { return m_pPrev; }
    Node*& GetPrevRef()        { return m_pPrev; }

    Node &operator=(const Node &another) {
        m_data = another.m_data;
        m_ref  = another.m_ref;
        return *this;
    }
};

template <typename Container>
class DoubleLinkedListForwardIterator : public GeneralIterator<Container> {
    using Parent = GeneralIterator<Container>; 
    using Node   = typename Container::Node;
public:
    DoubleLinkedListForwardIterator(Container *pContainer, Node* pNode = nullptr)
        : Parent(pContainer, 0), m_pCurrent(pNode) {}

    DoubleLinkedListForwardIterator& operator++() {
        if (m_pCurrent) {
            m_pCurrent = m_pCurrent->GetNext();
            if (m_pCurrent == this->m_pContainer->getRoot()) 
                m_pCurrent = nullptr;
            this->m_pos++;
        }
        return *this;
    }

    typename Container::value_type& operator*() { return m_pCurrent->GetValueRef(); }
    
    bool operator!=(const DoubleLinkedListForwardIterator& other) const { 
        return m_pCurrent != other.m_pCurrent; 
    }
private:
    Node *m_pCurrent;
};

template <typename Container>
class DoubleLinkedListBackwardIterator : public GeneralIterator<Container> {
    using Parent = GeneralIterator<Container>; // Estilo del profesor
    using Node   = typename Container::Node;
public:
    DoubleLinkedListBackwardIterator(Container *pContainer, Node* pNode = nullptr, int pos = -1)
        : Parent(pContainer, pos), m_pCurrent(pNode) {}

    DoubleLinkedListBackwardIterator& operator++() {
        if (m_pCurrent) {
            m_pCurrent = m_pCurrent->GetPrev();
            if (m_pCurrent == this->m_pContainer->getLast()) 
                m_pCurrent = nullptr;
            this->m_pos--;
        }
        return *this;
    }

    typename Container::value_type& operator*() { return m_pCurrent->GetValueRef(); }
    
    bool operator!=(const DoubleLinkedListBackwardIterator& other) const { 
        return m_pCurrent != other.m_pCurrent; 
    }
private:
    Node *m_pCurrent;
};

template <typename Traits>
class CDoubleLinkedList {
    using value_type = typename Traits::value_type;
    using Node       = NodeDoubleLinkedList<Traits>;
    using forward_iterator  = DoubleLinkedListForwardIterator<CDoubleLinkedList<Traits>>;
    using backward_iterator = DoubleLinkedListBackwardIterator<CDoubleLinkedList<Traits>>;

    friend forward_iterator;
    friend backward_iterator;
    friend class GeneralIterator<CDoubleLinkedList<Traits>>;

private:
    Node   *m_pRoot     = nullptr;
    Node   *m_pLast     = nullptr;
    size_t  m_nElements = 0;
    mutable std::mutex m_mtx;

public:
    CDoubleLinkedList() {}
    
    CDoubleLinkedList(const CDoubleLinkedList<Traits>& other);
    
    CDoubleLinkedList(CDoubleLinkedList<Traits>&& other) noexcept;

    virtual ~CDoubleLinkedList();

    size_t getElements() const { return m_nElements; }
    Node* getRoot()     const { return m_pRoot; }
    Node* getLast()     const { return m_pLast; }

    forward_iterator  begin()  { return forward_iterator(this, m_pRoot); }
    forward_iterator  end()    { return forward_iterator(this, nullptr); }
    backward_iterator rbegin() { return backward_iterator(this, m_pLast, (int)m_nElements - 1); }
    backward_iterator rend()   { return backward_iterator(this, nullptr, -1); }

    void push_back(const value_type &val, ref_type ref);

    void Insert(const value_type &val, ref_type ref);

    template <typename ObjFunc, typename ...Args>
    void Foreach(ObjFunc of, Args... args) { ::Foreach(*this, of, args...); }

    template <typename ObjFunc, typename ...Args>
    auto FirstThat(ObjFunc of, Args... args) { return ::FirstThat(*this, of, args...); }

private:
    friend ostream &operator<<(ostream &os, CDoubleLinkedList<Traits> &container) {
        os << "LDEC: size = " << container.m_nElements << endl << "[";
        Node *pCurrent = container.m_pRoot;
        for (size_t i = 0; i < container.m_nElements; ++i) {
            os << "(" << pCurrent->GetValue() << ")";
            pCurrent = pCurrent->GetNext();
            if (i < container.m_nElements - 1) os << " <-> ";
        }
        os << "]";
        return os;
    }

    void InternalInsert(Node *&rParent, Node *pPrev, const value_type &val, ref_type ref) {
        typename Traits::Func compare;

        if (!rParent || compare(rParent->GetValue(), val) || (rParent == m_pRoot && pPrev != nullptr)) {
            Node *pNew = new Node(val, ref, rParent, pPrev);
            if (rParent) 
                rParent->GetPrevRef() = pNew;
            rParent = pNew; 
            if (pNew->GetNext() == m_pRoot || pNew->GetNext() == nullptr)
                m_pLast = pNew;
            m_pLast->GetNextRef() = m_pRoot;
            m_pRoot->GetPrevRef() = m_pLast;

            m_nElements++;
            return;
        }
        InternalInsert(rParent->GetNextRef(), rParent, val, ref);
    }
};


template <typename Traits>
CDoubleLinkedList<Traits>::CDoubleLinkedList(const CDoubleLinkedList<Traits>& other) {
    lock_guard<mutex> lock(other.m_mtx);
    
    m_pRoot = nullptr;
    m_pLast = nullptr;
    m_nElements = 0;

    Node* pTemp = other.m_pRoot;
    for (size_t i = 0; i < other.m_nElements; ++i) {
        if (!pTemp) break;
        this->push_back(pTemp->GetValue(), pTemp->GetRef());
        pTemp = pTemp->GetNext();
    }
}

template <typename Traits>
CDoubleLinkedList<Traits>::CDoubleLinkedList(CDoubleLinkedList<Traits>&& other) noexcept {
    lock_guard<mutex> lock(other.m_mtx);
    
    this->m_pRoot = other.m_pRoot;
    this->m_pLast = other.m_pLast;
    this->m_nElements = other.m_nElements;

    other.m_pRoot = nullptr;
    other.m_pLast = nullptr;
    other.m_nElements = 0;
}

template <typename Traits>
CDoubleLinkedList<Traits>::~CDoubleLinkedList() {
    lock_guard<mutex> lock(m_mtx);
    
    if (!m_pRoot) 
        return;
    if (m_pLast) 
        m_pLast->GetNextRef() = nullptr;
    if (m_pRoot)
         m_pRoot->GetPrevRef() = nullptr;
    Node *pCurrent = m_pRoot;
    while (pCurrent) {
        Node *pNext = pCurrent->GetNext();
        delete pCurrent; 
        pCurrent = pNext;
    }
    m_pRoot = nullptr;
    m_pLast = nullptr;
    m_nElements = 0;
}

template <typename Traits>
void CDoubleLinkedList<Traits>::push_back(const value_type &val, ref_type ref) {
    lock_guard<mutex> lock(m_mtx);
    Node *pNew = new Node(val, ref);
    if (!m_pRoot) 
        m_pRoot = m_pLast = pNew;
    else {
        pNew->GetPrevRef()    = m_pLast;
        m_pLast->GetNextRef() = pNew;
        m_pLast               = pNew;
    }
    m_pLast->GetNextRef() = m_pRoot;
    m_pRoot->GetPrevRef() = m_pLast;
    
    m_nElements++;
}

template <typename Traits>
void CDoubleLinkedList<Traits>::Insert(const value_type &val, ref_type ref) {
    lock_guard<mutex> lock(m_mtx);
    InternalInsert(m_pRoot, nullptr, val, ref);
}

#endif // __DOUBLE_LINKED_LIST_H__