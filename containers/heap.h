#ifndef __HEAP_H__
#define __HEAP_H__

#include <iostream>
#include <mutex>
#include <stdexcept>
#include <utility>
#include "../general/types.h"
#include "../util.h"


template <typename _T>
struct MinHeapTrait {
    using T = _T;
    static bool Compare(const T& parent, const T& child) {
        return parent <= child;
    }
};

template <typename _T>
struct MaxHeapTrait {
    using T = _T;
    static bool Compare(const T& parent, const T& child) {
        return parent >= child;
    } 
};
template <typename Traits>
class CHeap{
    using value_type = typename Traits::T;

private:
    Size m_capacity  = 0;
    value_type *m_data = nullptr;
    Size m_last      = 0;

    mutable std::recursive_mutex m_mtx;
    
public:
    explicit CHeap(Size capacity = 10);
    CHeap(const CHeap<Traits>& other);
    CHeap(CHeap<Traits>&& other) noexcept;
    virtual ~CHeap();

    void Push(const value_type& value);
    value_type Pop();

    CHeap& operator=(const CHeap& other);

    friend std::ostream& operator<<(std::ostream& os, const CHeap<Traits>& heap) {
        std::lock_guard<std::recursive_mutex> lock(heap.m_mtx);
        os << "[";
        if (heap.m_last > 0) {      
            os << heap.m_data[0]; 
            for (Size i = 1; i < heap.m_last; ++i) 
                os << ", " << heap.m_data[i];
        }
        os << "]";
        return os;
    }

    friend std::istream& operator>>(std::istream& is, CHeap<Traits>& heap) {
        std::lock_guard<std::recursive_mutex> lock(heap.m_mtx);
        value_type val;
        if (is >> val) 
            heap.Push(val);
        return is;
    }

private:
    void resize();
    void HeapifyUp(Size index);
    void HeapifyDown(Size index);
    
};

template <typename Traits>
CHeap<Traits>::CHeap(Size capacity)
    : m_capacity(capacity), m_data(new value_type[m_capacity]), m_last(0){}

template <typename Traits>
CHeap<Traits>::CHeap(const CHeap<Traits>& other) {
    std::lock_guard<std::recursive_mutex> lock(other.m_mtx);
    m_capacity = other.m_capacity;
    m_last = other.m_last;
    m_data = new value_type[m_capacity];
    for (Size i = 0; i < m_last; ++i) 
        m_data[i] = other.m_data[i];
}

template <typename Traits>
CHeap<Traits>::CHeap(CHeap<Traits>&& other) noexcept {
    std::lock_guard<std::recursive_mutex> lock(other.m_mtx);
    m_capacity = std::exchange(other.m_capacity, 0);
    m_data = std::exchange(other.m_data, nullptr);
    m_last = std::exchange(other.m_last, 0);
}

template <typename Traits>
CHeap<Traits>::~CHeap() {
    std::lock_guard<std::recursive_mutex> lock(m_mtx);    
    delete[] m_data;
}

template <typename Traits>
void CHeap<Traits>::Push(const value_type& value) {
    std::lock_guard<std::recursive_mutex> lock(m_mtx);
    if (m_last >= m_capacity) 
        resize();
    m_data[m_last++] = value;
    HeapifyUp(m_last-1);    
}

template <typename Traits>
typename Traits::T CHeap<Traits>::Pop() {
    std::lock_guard<std::recursive_mutex> lock(m_mtx);
    if (m_last == 0) 
        throw std::out_of_range("Heap vacío");
    value_type topValue = std::move(m_data[0]);
    m_last--;
    if (m_last > 0) {
        m_data[0] = std::move(m_data[m_last]);
        HeapifyDown(0);
    }
    return topValue;
}

template <typename Traits>
CHeap<Traits>& CHeap<Traits>::operator=(const CHeap& other) {
    if (this != &other) {   
        std::scoped_lock lock(m_mtx, other.m_mtx);
        if (m_capacity < other.m_last){
            delete[] m_data;
            m_capacity = other.m_capacity;
            m_data = new value_type[m_capacity];
        }
        m_last = other.m_last;
        std::copy(other.m_data, other.m_data + m_last, m_data);
    }
    return *this;
}

template <typename Traits>
void CHeap<Traits>::resize() {
    Size new_capacity = 0;
    if (m_capacity == 0)
        new_capacity = 10;
    else
        new_capacity = m_capacity*2;
    value_type* new_data = new value_type[new_capacity];
    for (Size i = 0; i < m_last; ++i)
        new_data[i] = std::move(m_data[i]);
    delete[] m_data;
    m_data = new_data;
    m_capacity = new_capacity;
}

template <typename Traits>
void CHeap<Traits>::HeapifyUp(Size index) {
    if (index == 0) 
        return;
    value_type movingValue = std::move(m_data[index]);
    
    while (index > 0) {
        Size parent = (index - 1) / 2;
        if (Traits::Compare(m_data[parent], movingValue)) 
            break; 
        m_data[index] = std::move(m_data[parent]);
        index = parent;
    }
    m_data[index] = std::move(movingValue);
}

template <typename Traits>
void CHeap<Traits>::HeapifyDown(Size index) {
    value_type movingValue = std::move(m_data[index]);
    Size current = index;

    while (true) {
        Size left = 2 * current + 1;
        Size right = 2 * current + 2;

        if (left >= m_last) 
            break;
        Size childToSwap = left;
        if (right < m_last && !Traits::Compare(m_data[left], m_data[right])) 
            childToSwap = right; 
        if (Traits::Compare(movingValue, m_data[childToSwap])) 
            break; 
        m_data[current] = std::move(m_data[childToSwap]);
        current = childToSwap;  
    }
    m_data[current] = std::move(movingValue);
}


#endif // __HEAP_H__