#ifndef __HEAP_H__
#define __HEAP_H__

#include <iostream>
#include <mutex>
#include <vector>
#include <cassert>
#include <fstream>
#include "../general/types.h"
#include "../util.h"
#include "../general/traits.h"

using namespace std;

template <typename T>
struct MinHeapTrait {
    using value_type = T;
    using Compare = less<T>; 
};

template <typename T>
struct MaxHeapTrait {
    using value_type = T;
    using Compare = greater<T>;
};

template <typename Traits>
class CHeap{
public:
    using value_type = typename Traits::value_type;
    using Compare    = typename Traits::Compare;

private:
    vector<value_type> m_data;
    mutable mutex m_mutex;

    size_t parent(size_t i) { return (i - 1) / 2; }
    size_t left(size_t i)   { return (2 * i) + 1; }
    size_t right(size_t i)  { return (2 * i) + 2; }
    void heapify_up(size_t index);
    void heapify_down(size_t index);

public:
    CHeap() {}
    CHeap(const CHeap<Traits> &another);
    CHeap(CHeap<Traits> &&another) noexcept;
    virtual ~CHeap();

    void push(const value_type &val);
    value_type pop();
    value_type& top();
    bool empty() const;
    size_t size() const;

    template <typename U>
    friend ostream& operator<<(ostream &os, CHeap<U> &heap);
    template <typename U>
    friend istream& operator>>(istream &is, CHeap<U> &heap);
};

template <typename Traits>
void CHeap<Traits>::heapify_up(size_t index) {
    while (index > 0) {
        size_t p = parent(index);
        if (Compare()(m_data[index], m_data[p])) {
            value_type temp = m_data[index];
            m_data[index]   = m_data[p];
            m_data[p]       = temp;
            index = p;
        } else 
            break;
    }
}

template <typename Traits>
void CHeap<Traits>::heapify_down(size_t index) {
    size_t size = m_data.size();

    while(true) {
        size_t extreme = index; 
        size_t l = left(index);
        size_t r = right(index);

        if (l < size && Compare()(m_data[l], m_data[extreme]))
            extreme = l;
            
        if (r < size && Compare()(m_data[r], m_data[extreme]))
            extreme = r;
            
        if (extreme != index) {
            value_type temp = m_data[index];
            m_data[index]   = m_data[extreme];
            m_data[extreme] = temp;
            index = extreme;
        } else
            break;
    }
}

template <typename Traits>
CHeap<Traits>::CHeap(const CHeap<Traits> &another) {
    lock_guard<mutex> lock(another.m_mutex);
    m_data = another.m_data; 
}
template <typename Traits>
CHeap<Traits>::CHeap(CHeap<Traits> &&another) noexcept {
    lock_guard<mutex> lock(another.m_mutex);
    m_data = move(another.m_data);
}

template <typename Traits>
CHeap<Traits>::~CHeap() {
    lock_guard<mutex> lock(m_mutex);
    m_data.clear(); 
}

template <typename Traits>
void CHeap<Traits>::push(const value_type &val) {
    lock_guard<mutex> lock(m_mutex);
    m_data.push_back(val);
    heapify_up(m_data.size() - 1);
}

template <typename Traits>
typename CHeap<Traits>::value_type CHeap<Traits>::pop() {
    lock_guard<mutex> lock(m_mutex);
    assert(!m_data.empty() && "Heap is empty");

    value_type root = m_data[0];

    m_data[0] = m_data.back();
    m_data.pop_back();

    if (!m_data.empty())
        heapify_down(0);

    return root;
}

template <typename Traits>
typename CHeap<Traits>::value_type& CHeap<Traits>::top() {
    lock_guard<mutex> lock(m_mutex);
    assert(!m_data.empty() && "Heap is empty");
    return m_data[0];
}

template <typename Traits>
bool CHeap<Traits>::empty() const {
    lock_guard<mutex> lock(m_mutex);
    return m_data.empty();
}

template <typename Traits>
size_t CHeap<Traits>::size() const {
    lock_guard<mutex> lock(m_mutex);
    return m_data.size();
}

template <typename Traits>
ostream& operator<<(ostream &os, CHeap<Traits> &heap) {
    lock_guard<mutex> lock(heap.m_mutex);
    os << "Heap Size: " << heap.m_data.size() << endl;
    os << "[ ";
    for (const auto &val : heap.m_data) {
        os << val << " ";
    }
    os << "]" << endl;
    return os;
}

template <typename Traits>
istream& operator>>(istream &is, CHeap<Traits> &heap) {
    typename Traits::value_type val;
    while (is >> val) {
        heap.push(val);
    }
    is.clear();
    return is;
}

void DemoHeap();

#endif // __HEAP_H__