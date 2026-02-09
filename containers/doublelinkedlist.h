#ifndef __DOUBLE_LINKED_LIST_H__
#define __DOUBLE_LINKED_LIST_H__

#include <iostream>
#include <mutex>
#include "../general/types.h"
#include "../util.h"
using namespace std;

template <typename Traits>
class NodeDoubleLinkedList{
public:
    using value_type = typename Traits::value_type;
    using Node = NodeDoubleLinkedList<Traits>;

private:
    value_type m_data;
    ref_type   m_ref;
    Node* m_next = nullptr;
    Node* m_prev = nullptr;

public:
    NodeDoubleLinkedList() {}

    NodeDoubleLinkedList(value_type v, ref_type r,
                         Node* p=nullptr, Node* n=nullptr)
        : m_data(v), m_ref(r), m_prev(p), m_next(n) {}

    value_type& GetValueRef(){ return m_data; }
    value_type  GetValue() const { return m_data; }

    ref_type GetRef() const { return m_ref; }

    Node*& Next(){ return m_next; }
    Node*& Prev(){ return m_prev; }
};
template <typename Node>
class DLForwardIterator {
    Node* p;
public:
    DLForwardIterator(Node* x=nullptr): p(x) {}

    auto& operator*(){ return p->GetValueRef(); }

    DLForwardIterator& operator++(){
        p = p->Next();
        return *this;
    }

    bool operator!=(const DLForwardIterator& other) const {
        return p != other.p;
    }
};

template <typename Node>
class DLBackwardIterator {
    Node* p;
public:
    DLBackwardIterator(Node* x=nullptr): p(x) {}

    auto& operator*(){ return p->GetValueRef(); }

    DLBackwardIterator& operator++(){
        p = p->Prev();
        return *this;
    }

    bool operator!=(const DLBackwardIterator& other) const {
        return p != other.p;
    }
};

template <typename Traits>
class CDoubleLinkedList {

    using value_type = typename Traits::value_type;
    using Node = NodeDoubleLinkedList<Traits>;

    Node* m_root = nullptr;
    Node* m_last = nullptr;
    size_t m_size = 0;
    std::mutex mtx;

public:

    using iterator = DLForwardIterator<Node>;
    using riterator = DLBackwardIterator<Node>;

    CDoubleLinkedList(){}

    CDoubleLinkedList(const CDoubleLinkedList& other){
        Node* cur = other.m_root;
        while(cur){
            push_back(cur->GetValue(), cur->GetRef());
            cur = cur->Next();
        }
    }

    CDoubleLinkedList(CDoubleLinkedList&& other){
        m_root = other.m_root;
        m_last = other.m_last;
        m_size = other.m_size;

        other.m_root = other.m_last = nullptr;
        other.m_size = 0;
    }

    ~CDoubleLinkedList(){
        clear();
    }

    void clear(){
        Node* cur = m_root;
        while(cur){
            Node* t = cur;
            cur = cur->Next();
            delete t;
        }
        m_root = m_last = nullptr;
        m_size = 0;
    }

    void push_back(value_type v, ref_type r){
        std::lock_guard<std::mutex> lock(mtx);

        Node* n = new Node(v,r,m_last,nullptr);

        if(!m_root){
            m_root = m_last = n;
        } else {
            m_last->Next() = n;
            m_last = n;
        }
        m_size++;
    }

    value_type& First(){ return m_root->GetValueRef(); }
    value_type& Last(){ return m_last->GetValueRef(); }

    iterator begin(){ return iterator(m_root); }
    iterator end(){ return iterator(nullptr); }

    riterator rbegin(){ return riterator(m_last); }
    riterator rend(){ return riterator(nullptr); }

    bool operator<(const CDoubleLinkedList& other){
        return m_size < other.m_size;
    }

    bool operator>(const CDoubleLinkedList& other){
        return m_size > other.m_size;
    }


    friend ostream& operator<<(ostream& os, CDoubleLinkedList& l){
        os << "[";
        Node* cur = l.m_root;
        while(cur){
            os << cur->GetValue();
            if(cur->Next()) os << ",";
            cur = cur->Next();
        }
        os << "]";
        return os;
    }

    size_t size() const { return m_size; }
};

#endif