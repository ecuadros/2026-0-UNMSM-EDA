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
class CLinkedList {
    using  value_type  = typename Traits::value_type;
    using  Node = NodeLinkedList<Traits>;

    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;
    mutable std::mutex m_mtx;
public:
    CLinkedList(){}
    CLinkedList(const CLinkedList& other);
    CLinkedList(CLinkedList&& other) noexcept;
    ~CLinkedList(); 

    void clear();

    class iterator {
        Node* m_curr = nullptr;
    public:
        iterator(Node* p=nullptr) : m_curr(p) {}

        iterator& operator++() {
            if (m_curr) m_curr = m_curr->GetNext();
            return *this;
        }

        value_type& operator*() const {
            return m_curr->GetValueRef();
        }

        bool operator!=(const iterator& other) const {
            return m_curr != other.m_curr;
        }
    };
    iterator begin() { return iterator(m_pRoot); }
    iterator end()   { return iterator(nullptr); }

    void push_back(const value_type &val, ref_type ref);
    void Insert(const value_type &val, ref_type ref);
    size_t getSize(){ return m_nElements;  }

    value_type& operator[](Size index);
    const value_type& operator[](Size index) const;

private:
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref);
    void clear_nolock();


    friend ostream &operator<<(ostream &os, const CLinkedList<Traits> &container){
    os << "CLinkedList(size=" << container.m_nElements << ") ";
    os << "[";
    Node *curr = container.m_pRoot;
    bool first = true;
    while(curr){
        if(!first) os << ", ";
        os << "(" << curr->GetValue() << ":" << curr->GetRef() << ")";
        first = false;
        curr = curr->GetNext();
    }
    os << "]";
    return os;
}

    friend istream &operator>>(istream &is, CLinkedList<Traits> &container) {
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
void CLinkedList<Traits>::push_back(const value_type &val, ref_type ref){
    std::lock_guard<std::mutex> lock(m_mtx);
    Node *pNewNode = new Node(val, ref);
    if( !m_pRoot ){
        m_pRoot = pNewNode;
        m_pLast = pNewNode;
    } else {
        m_pLast->GetNextRef() = pNewNode;
        m_pLast = pNewNode;
    }
    ++m_nElements;
}

template <typename Traits>
void CLinkedList<Traits>::InternalInsert(Node *&rParent, const value_type &val, ref_type ref){
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
void CLinkedList<Traits>::Insert(const value_type &val, ref_type ref){
    std::lock_guard<std::mutex> lock(m_mtx);
    InternalInsert(m_pRoot, val, ref);
}

template <typename Traits>
CLinkedList<Traits>::CLinkedList(const CLinkedList& other) {
    m_pRoot = nullptr;
    m_pLast = nullptr;
    m_nElements = 0;

    Node* curr = other.m_pRoot;
    while (curr) {
        value_type v = curr->GetValue();
        ref_type r   = curr->GetRef();
        push_back(v, r);
        curr = curr->GetNext();
    }
}

template <typename Traits>
CLinkedList<Traits>::CLinkedList(CLinkedList&& other) noexcept {
    m_pRoot = other.m_pRoot;
    m_pLast = other.m_pLast;
    m_nElements = other.m_nElements;

    other.m_pRoot = nullptr;
    other.m_pLast = nullptr;
    other.m_nElements = 0;
}

template <typename Traits>
void CLinkedList<Traits>::clear() {
    std::lock_guard<std::mutex> lock(m_mtx);
    clear_nolock();
}

template <typename Traits>
CLinkedList<Traits>::~CLinkedList() {
    clear_nolock();
}

template <typename Traits>
typename CLinkedList<Traits>::value_type&
CLinkedList<Traits>::operator[](Size index) {
    if (index < 0 || static_cast<size_t>(index) >= m_nElements)
        throw std::out_of_range("CLinkedList::operator[] out of range");

    Node* curr = m_pRoot;
    for (Size i = 0; i < index; ++i)
        curr = curr->GetNext();

    return curr->GetValueRef();
}

template <typename Traits>
const typename CLinkedList<Traits>::value_type&
CLinkedList<Traits>::operator[](Size index) const {
    if (index < 0 || static_cast<size_t>(index) >= m_nElements)
        throw std::out_of_range("CLinkedList::operator[] out of range");

    Node* curr = m_pRoot;
    for (Size i = 0; i < index; ++i)
        curr = curr->GetNext();

    return curr->GetValue();
}

template <typename Traits>
void CLinkedList<Traits>::clear_nolock() {
    Node* curr = m_pRoot;
    while (curr) {
        Node* next = curr->GetNext();
        delete curr;
        curr = next;
    }
    m_pRoot = nullptr;
    m_pLast = nullptr;
    m_nElements = 0;
}

#endif // __LINKEDLIST_H__
