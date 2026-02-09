#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__
#include <iostream>
#include <functional>
#include "../general/types.h"
#include "../util.h"
#include <stdexcept>
#include <mutex>
using namespace std;

template <typename T, typename _Func>
struct ListTrait{
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct AscendingTrait : 
    public ListTrait<T, std::greater<T> >{
};

template <typename T>
struct DescendingTrait : 
    public ListTrait<T, std::less<T> >{
};

template <typename Traits>
class NodeLinkedList{

    using  value_type  = typename Traits::value_type;
    using  Node        = NodeLinkedList<Traits>;
private:
    value_type m_data;
    ref_type   m_ref;
    Node *m_pNext = nullptr;

public:
    NodeLinkedList(){}
    NodeLinkedList( value_type _value, ref_type _ref, Node* next=nullptr)
        : m_data(_value), m_ref(_ref), m_pNext(next){   }
    value_type  GetValue   () const { return m_data; }
    value_type &GetValueRef() { return m_data; }

    ref_type    GetRef     () const { return m_ref;   }
    ref_type   &GetRefRef  () { return m_ref;   }

    Node      * GetNext     () const { return m_pNext;   }
    Node      *&GetNextRef  () { return m_pNext;   }

    Node &operator=(const Node &another){
        m_data = another.GetValue();
        m_ref   = another.GetRef();
        return *this;
    }
    bool operator==(const Node &another) const
    { return m_data == another.GetValue();   }
    bool operator<(const Node &another) const
    { return m_data < another.GetValue();   }
};

template <typename Traits>
class CCircularLinkedList {
    using  value_type  = typename Traits::value_type;
    using  Node = NodeLinkedList<Traits>;

    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;
    mutable std::mutex m_mtx;
public:
    CCircularLinkedList() = default;
    CCircularLinkedList(const CCircularLinkedList& other);
    CCircularLinkedList(CCircularLinkedList&& other) noexcept;
    ~CCircularLinkedList();


    void clear();

    class iterator {
        Node* m_curr = nullptr;
        Node* m_root = nullptr;
        bool  m_firstPass = true;

    public:
        iterator(Node* curr=nullptr, Node* root=nullptr, bool firstPass=true)
            : m_curr(curr), m_root(root), m_firstPass(firstPass) {}

        iterator& operator++() {
            if(!m_curr) return *this;
            m_curr = m_curr->GetNext();
            if (m_curr == m_root) m_firstPass = false;
            return *this;
        }

        value_type& operator*() const {
            return m_curr->GetValueRef();
        }

        bool operator!=(const iterator& other) const {
            return m_curr != other.m_curr || m_firstPass != other.m_firstPass;
        }
    };
    iterator begin() {
        if (!m_pRoot) return iterator(nullptr, nullptr, false);
        return iterator(m_pRoot, m_pRoot, true);
    }

    iterator end() {
        if (!m_pRoot) return iterator(nullptr, nullptr, false);
        return iterator(m_pRoot, m_pRoot, false); // mismo nodo, pero firstPass=false
    }

    void push_back(const value_type &val, ref_type ref);
    void Insert(const value_type &val, ref_type ref);
    size_t getSize(){ return m_nElements;  }

    value_type& operator[](Size index);
    const value_type& operator[](Size index) const;

private:
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref);
    void clear_nolock();


    friend ostream &operator<<(ostream &os, const CCircularLinkedList<Traits> &container){
    os << "CCircularLinkedList(size=" << container.m_nElements << ") [";

    if (!container.m_pRoot) {
        os << "]";
        return os;
    }

    Node *curr = container.m_pRoot;
    bool first = true;
    do {
        if (!first) os << ", ";
        os << "(" << curr->GetValue() << ":" << curr->GetRef() << ")";
        first = false;
        curr = curr->GetNext();
    } while (curr != container.m_pRoot);

    os << "]";
    return os;
}

friend istream &operator>>(istream &is, CCircularLinkedList<Traits> &container) {
    container.clear();
    value_type v;
    ref_type r;

    while (is >> v >> r) {
        container.Insert(v, r);
    }
    return is;
}
};

template <typename Traits>
void CCircularLinkedList<Traits>::push_back(const value_type &val, ref_type ref){
    std::lock_guard<std::mutex> lock(m_mtx);
    Node *pNewNode = new Node(val, ref);
    if( !m_pRoot ){
        m_pRoot = pNewNode;
        m_pLast = pNewNode;
        pNewNode->GetNextRef() = pNewNode;  
    } else {
        pNewNode->GetNextRef() = m_pRoot;  
        m_pLast->GetNextRef() = pNewNode;
        m_pLast = pNewNode;
    }
    ++m_nElements;
}

template <typename Traits>
void CCircularLinkedList<Traits>::InternalInsert(Node *&rParent, const value_type &val, ref_type ref){
    typename Traits::Func cmp;
    if( !rParent || cmp(val, rParent->GetValue()) ){
        Node *pNew = new Node(val, ref, rParent);
        rParent = pNew;
        ++m_nElements;
        return;
    }
    InternalInsert(rParent->GetNextRef(), val, ref);
}

template <typename Traits>
void CCircularLinkedList<Traits>::Insert(const value_type &val, ref_type ref){
    std::lock_guard<std::mutex> lock(m_mtx);

    typename Traits::Func cmp;
    Node* pNew = new Node(val, ref);

    if (!m_pRoot) {
        m_pRoot = m_pLast = pNew;
        pNew->GetNextRef() = pNew;  
        m_nElements = 1;
        return;
    }

    if (cmp(val, m_pRoot->GetValue())) {
        pNew->GetNextRef() = m_pRoot;
        m_pRoot = pNew;
        m_pLast->GetNextRef() = m_pRoot;
        ++m_nElements;
        return;
    }

    Node* curr = m_pRoot;
    while (curr->GetNext() != m_pRoot && !cmp(val, curr->GetNext()->GetValue())) {
        curr = curr->GetNext();
    }

    pNew->GetNextRef() = curr->GetNext();
    curr->GetNextRef() = pNew;

    if (curr == m_pLast) {
        m_pLast = pNew;
    }

    m_pLast->GetNextRef() = m_pRoot;

    ++m_nElements;
}

template <typename Traits>
CCircularLinkedList<Traits>::CCircularLinkedList(const CCircularLinkedList& other) {
    m_pRoot = nullptr;
    m_pLast = nullptr;
    m_nElements = 0;

    if (!other.m_pRoot) return;

    Node* curr = other.m_pRoot;
    do {
        push_back(curr->GetValue(), curr->GetRef());
        curr = curr->GetNext();
    } while (curr != other.m_pRoot);
}


template <typename Traits>
CCircularLinkedList<Traits>::CCircularLinkedList(CCircularLinkedList&& other) noexcept {
    m_pRoot = other.m_pRoot;
    m_pLast = other.m_pLast;
    m_nElements = other.m_nElements;

    other.m_pRoot = nullptr;
    other.m_pLast = nullptr;
    other.m_nElements = 0;
}

template <typename Traits>
void CCircularLinkedList<Traits>::clear() {
    std::lock_guard<std::mutex> lock(m_mtx);
    clear_nolock();
}

template <typename Traits>
CCircularLinkedList<Traits>::~CCircularLinkedList() {
    clear_nolock();
}

template <typename Traits>
typename CCircularLinkedList<Traits>::value_type&
CCircularLinkedList<Traits>::operator[](Size index) {
    if (index < 0 || static_cast<size_t>(index) >= m_nElements)
        throw std::out_of_range("CCircularLinkedList<TraiCCircularLinkedListts>::::operator[] out of range");

    Node* curr = m_pRoot;
    for (Size i = 0; i < index; ++i)
        curr = curr->GetNext();

    return curr->GetValueRef();
}

template <typename Traits>
const typename CCircularLinkedList<Traits>::value_type&
CCircularLinkedList<Traits>::operator[](Size index) const {
    if (index < 0 || static_cast<size_t>(index) >= m_nElements)
        throw std::out_of_range("CCircularLinkedList<TraiCCircularLinkedListts>::::operator[] out of range");

    Node* curr = m_pRoot;
    for (Size i = 0; i < index; ++i)
        curr = curr->GetNext();

    return curr->GetValue();
}

template <typename Traits>
void CCircularLinkedList<Traits>::clear_nolock() {
    if (!m_pRoot) return;

    Node* curr = m_pRoot->GetNext();
    while (curr != m_pRoot) {
        Node* next = curr->GetNext();
        delete curr;
        curr = next;
    }
    delete m_pRoot;

    m_pRoot = nullptr;
    m_pLast = nullptr;
    m_nElements = 0;
}

#endif // __LINKEDLIST_H__
