#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__
#include "../general/types.h"
#include "../util.h"
#include <mutex>
#include <iostream>
using namespace std;

// TODO: Traits para listas enlazadas
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

// Iterators para listas enlazadas

template <typename Node>
class ForwardIterator {
    Node* m_ptr;
public:
    ForwardIterator(Node* p=nullptr): m_ptr(p) {}

    auto& operator*() { return m_ptr->GetValueRef(); }

    ForwardIterator& operator++() {
        m_ptr = m_ptr->GetNext();
        return *this;
    }

    bool operator!=(const ForwardIterator& other) const {
        return m_ptr != other.m_ptr;
    }
};

template <typename Traits>
class CLinkedList {
    using value_type = typename Traits::value_type;
    using Node = NodeLinkedList<Traits>;

    Node* m_pRoot = nullptr;
    Node* m_pLast = nullptr;
    size_t m_nElements = 0;
    std::mutex mtx;

public:
    using iterator = ForwardIterator<Node>;

    CLinkedList() {}

    CLinkedList(const CLinkedList& other){
        Node* cur = other.m_pRoot;
        while(cur){
            push_back(cur->GetValueRef(), cur->GetRef());
            cur = cur->GetNext();
        }
    }

    CLinkedList(CLinkedList&& other){
        m_pRoot = other.m_pRoot;
        m_pLast = other.m_pLast;
        m_nElements = other.m_nElements;
        other.m_pRoot = other.m_pLast = nullptr;
        other.m_nElements = 0;
    }

    virtual ~CLinkedList(){
        Node* cur = m_pRoot;
        while(cur){
            Node* tmp = cur;
            cur = cur->GetNext();
            delete tmp;
        }
    }

    iterator begin(){ return iterator(m_pRoot); }
    iterator end(){ return iterator(nullptr); }

    size_t getSize(){ return m_nElements; }

    value_type& operator[](size_t idx){
        Node* cur = m_pRoot;
        while(idx-- && cur)
            cur = cur->GetNext();
        return cur->GetValueRef();
    }

    void push_back(value_type &val, ref_type ref){
        std::lock_guard<std::mutex> lock(mtx);

        Node *pNewNode = new Node(val, ref);

        if(!m_pRoot){
            m_pRoot = m_pLast = pNewNode;
        } else {
            m_pLast->GetNextRef() = pNewNode;
            m_pLast = pNewNode;
        }
        ++m_nElements;
    }

    void Insert(const value_type &val, ref_type ref);

private:
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref);

    friend ostream &operator<<(ostream &os, CLinkedList &c){
        Node* cur = c.m_pRoot;
        os << "[";
        while(cur){
            os << "(" << cur->GetValue() << ":" << cur->GetRef() << ")";
            if(cur->GetNext()) os << ",";
            cur = cur->GetNext();
        }
        os << "]";
        return os;
    }
};

#endif // __LINKEDLIST_H__
