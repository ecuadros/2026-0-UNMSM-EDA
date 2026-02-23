#ifndef __HEAP_H__
#define __HEAP_H__

#include <iostream>
#include <utility>
#include <mutex>
#include <cassert>
#include <fstream>
#include "../general/types.h"
#include "../util.h"

template <typename _T>
struct MinHeapTrait{
    using  T = _T;
    static bool Compare(const T &parent, const T &child){
        return parent <= child;
    }

};
template <typename _T>
struct MaxHeapTrait{
    using  T = _T;
    static bool Compare(const T &parent, const T &child){
        return parent >= child;
    }
};  

template<typename Traits>
class CHeap{
    using value_type = typename Traits::T;

private:
    Size m_capacity = 0;
    value_type* m_data = nullptr;
    Size m_last = 0;
    Size parent(Size i) const { return (i - 1) / 2; }
    Size left  (Size i) const { return 2 * i + 1; }
    Size right (Size i) const { return 2 * i + 2; }   

    mutable std::recursive_mutex m_mtx;
public:
    CHeap(){}
    CHeap(Size capacity);
    CHeap(const CHeap<Traits>& other);
    CHeap(CHeap<Traits>&& other) noexcept
        :   m_capacity(std::exchange(other.m_capacity, 0)), 
            m_data(std::exchange(other.m_data, nullptr)),
            m_last(std::exchange(other.m_last,0)){}
    
    virtual ~CHeap(){
        std::lock_guard<std::recursive_mutex> lock(m_mtx);
        delete[] m_data;
        m_data = nullptr;
        m_capacity = 0;
        m_last = 0;
    }
    void Push(const value_type& value);
    value_type Pop();
    void push(const value_type& v){ Push(v); }
    value_type pop(){ return Pop(); }

    Size size() const{
    std::lock_guard<std::recursive_mutex> lock(m_mtx);
    return m_last;
    }
    bool empty() const{
    return size() == 0;
    }
    value_type top() const{
    std::lock_guard<std::recursive_mutex> lock(m_mtx);
    if (m_last == 0){
        std::cout << "Heap vacio (top)\n";
        return value_type();
    }
    return m_data[0];
    }
    void resize();
    CHeap<Traits>& operator=(const CHeap<Traits>& other);
    CHeap<Traits>& operator=(CHeap<Traits>&& other) noexcept;

    friend std::ostream& operator<<(std::ostream& os, const CHeap<Traits>& heap){
        std::lock_guard<std::recursive_mutex> lock(heap.m_mtx);
        os << "CHeap: size = " << heap.m_last << std::endl;
        os << "[";
        for (Size i = 0; i < heap.m_last; ++i){
            os << heap.m_data[i];
            if (i + 1 < heap.m_last) os << ",";
        }
        os << "]";
        return os;
    }
    friend std::istream& operator>>(std::istream& is, CHeap<Traits>& heap){
    std::lock_guard<std::recursive_mutex> lock(heap.m_mtx);

    Size n = 0;
    if(!(is >> n)) return is;

    value_type x;
    for (Size i = 0; i < n; ++i){
        if(!(is >> x)) break;
        heap.Push(x); 
    }
    return is;
    }

    private:
    void HeapifyUp();
    void HeapifyDown();
    void HeapifyUpRecursive(Size index);
    void HeapifyDownRecursive(Size index);
};
    template<typename Traits>
    CHeap<Traits>::CHeap(Size capacity)
        : m_capacity(capacity), m_data(nullptr), m_last(0)
    {
    if (m_capacity > 0)
    m_data = new value_type[m_capacity];
    }

    template<typename Traits>
    CHeap<Traits>::CHeap(const CHeap<Traits>& other){
        std::lock_guard<std::recursive_mutex> lock(other.m_mtx);
        m_capacity = other.m_capacity;
        m_last = other.m_last;
        m_data = new value_type[m_capacity];
        for (Size i = 0; i < m_last; ++i){
            m_data[i] = other.m_data[i];
        }

    }
    template<typename Traits>
    void CHeap<Traits>::resize(){
        std::lock_guard<std::recursive_mutex> lock(m_mtx);
        Size new_capacity=0;
        if(m_capacity == 0)
            new_capacity = 5;
        else
            new_capacity = m_capacity * 2;
        value_type* new_data = new value_type[new_capacity];
        for (Size i = 0; i < m_last; ++i){
            new_data[i] = m_data[i];
        }
        delete[] m_data;
        m_data = new_data;
        m_capacity = new_capacity;
    }
template<typename Traits>
void CHeap<Traits>::Push(const value_type& value){
    std::lock_guard<std::recursive_mutex> lock(m_mtx);
    if (m_last >= m_capacity)
        resize();
    m_data[m_last++] = value;
    HeapifyUp();
    }
template<typename Traits>
void CHeap<Traits>::HeapifyUp(){
    if (m_last == 0) return;
    HeapifyUpRecursive(m_last - 1);
}

template<typename Traits>
void CHeap<Traits>::HeapifyDown(){
    if (m_last == 0) return;
    HeapifyDownRecursive(0);
}

template<typename Traits>
void CHeap<Traits>::HeapifyUpRecursive(Size index){
    if (index == 0) return;

    Size p = parent(index);

    if (!Traits::Compare(m_data[p], m_data[index])){
        intercambiar(m_data[p], m_data[index]);
        HeapifyUpRecursive(p);
    }
}

template<typename Traits>
typename CHeap<Traits>::value_type CHeap<Traits>::Pop(){
    std::lock_guard<std::recursive_mutex> lock(m_mtx);

    if (m_last == 0){
        std::cout << "Heap vacio (pop)\n";
        return value_type();
    }

    value_type ans = m_data[0];

    // subir el último a la raíz
    m_data[0] = m_data[m_last - 1];
    --m_last;

    if (m_last > 0)
        HeapifyDown();

    return ans;
}

template<typename Traits>
void CHeap<Traits>::HeapifyDownRecursive(Size index){

    Size l = left(index);
    if (l >= m_last) return; // no hay hijos

    Size r = right(index);
    Size best =l;

    if (r < m_last && !Traits::Compare(m_data[l], m_data[r])){    
        best = r;
    }if (Traits::Compare(m_data[index], m_data[best])) return;
    intercambiar(m_data[index], m_data[best]);
    HeapifyDownRecursive(best);
}

template<typename Traits>
CHeap<Traits>& CHeap<Traits>::operator=(const CHeap<Traits>& other){
    if (this == &other) return *this;
    
    std::scoped_lock lock(m_mtx, other.m_mtx);
    
    value_type* new_data = nullptr;
    if(other.m_capacity > 0){
        new_data = new value_type[other.m_capacity];
        for (Size i = 0; i < other.m_last; ++i)
            new_data[i] = other.m_data[i];
        
    }
    delete[] m_data;
    m_data = new_data;
    m_capacity = other.m_capacity;
    m_last = other.m_last;
    return *this;
}

template<typename Traits>
CHeap<Traits>& CHeap<Traits>::operator=(CHeap<Traits>&& other) noexcept{
    if (this == &other) return *this;

    std::scoped_lock lock(m_mtx, other.m_mtx);

    delete[] m_data;

    m_data     = std::exchange(other.m_data, nullptr);
    m_capacity = std::exchange(other.m_capacity, 0);
    m_last     = std::exchange(other.m_last, 0);
    
    return *this;
}
#endif // __HEAP_H__