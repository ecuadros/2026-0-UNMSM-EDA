#ifndef __HEAP_H__
#define __HEAP_H__

#include <iostream>
#include <utility>
#include <mutex>
#include <stdexcept>
#include "../general/types.h"
#include "../util.h"

using namespace std;

// Traits
template <typename T, typename _Func>
struct HeapTrait{
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct MaxHeapTrait : 
    public HeapTrait<T, std::less<T> >{
};

template <typename T>
struct MinHeapTrait : 
    public HeapTrait<T, std::greater<T> >{
};

// Forward declarations
template <typename Traits> class CHeap;
template <typename Traits> ostream &operator<<(ostream &os, CHeap<Traits> &container);
template <typename Traits> istream &operator>>(istream &is, CHeap<Traits> &container);

// Heap (Max/Min)
template <typename Traits>
class CHeap {
public:
    using  value_type  = typename Traits::value_type;
    using  Func        = typename Traits::Func;

private:
    value_type *m_pData  = nullptr;
    ref_type   *m_pRefs  = nullptr;
    size_t m_nCapacity   = 0;
    size_t m_nElements   = 0;
    mutable mutex m_mtx;
    Func m_comp;

    static const size_t INITIAL_CAPACITY = 16;

public:
    CHeap() : m_pData(nullptr), m_pRefs(nullptr), 
              m_nCapacity(0), m_nElements(0) {}

    // constructor copia
    CHeap(const CHeap &otro);

    // move constructor
    CHeap(CHeap &&otro) noexcept;

    // destructor
    virtual ~CHeap();

    // push
    void push(value_type val, ref_type ref = -1);

    // pop
    value_type pop();

    // top
    value_type top() const;

    size_t getSize() const { return m_nElements; }
    bool   empty()   const { return m_nElements == 0; }

    // operator<<
    friend ostream &operator<< <>(ostream &os, CHeap<Traits> &container);

    // operator>>
    friend istream &operator>> <>(istream &is, CHeap<Traits> &container);

private:
    // heapifyUp
    void heapifyUp(size_t idx);

    // heapifyDown
    void heapifyDown(size_t idx);

    // resize
    void resize();
};

// constructor copia
template <typename Traits>
CHeap<Traits>::CHeap(const CHeap &otro){
    lock_guard<mutex> lock(otro.m_mtx);
    m_nCapacity = otro.m_nCapacity;
    m_nElements = otro.m_nElements;
    if( m_nCapacity > 0 ){
        m_pData = new value_type[m_nCapacity];
        m_pRefs = new ref_type[m_nCapacity];
        for( size_t i = 0; i < m_nElements; ++i ){
            m_pData[i] = otro.m_pData[i];
            m_pRefs[i] = otro.m_pRefs[i];
        }
    }
}

// move constructor
template <typename Traits>
CHeap<Traits>::CHeap(CHeap &&otro) noexcept {
    lock_guard<mutex> lock(otro.m_mtx);
    m_pData     = otro.m_pData;
    m_pRefs     = otro.m_pRefs;
    m_nCapacity = otro.m_nCapacity;
    m_nElements = otro.m_nElements;
    otro.m_pData     = nullptr;
    otro.m_pRefs     = nullptr;
    otro.m_nCapacity = 0;
    otro.m_nElements = 0;
}

// destructor
template <typename Traits>
CHeap<Traits>::~CHeap(){
    lock_guard<mutex> lock(m_mtx);
    delete[] m_pData;
    delete[] m_pRefs;
}

// resize
template <typename Traits>
void CHeap<Traits>::resize(){
    size_t newCapacity = (m_nCapacity == 0) ? INITIAL_CAPACITY : m_nCapacity * 2;
    value_type *pNewData = new value_type[newCapacity];
    ref_type   *pNewRefs = new ref_type[newCapacity];
    for( size_t i = 0; i < m_nElements; ++i ){
        pNewData[i] = m_pData[i];
        pNewRefs[i] = m_pRefs[i];
    }
    delete[] m_pData;
    delete[] m_pRefs;
    m_pData = pNewData;
    m_pRefs = pNewRefs;
    m_nCapacity = newCapacity;
}

// heapifyUp
template <typename Traits>
void CHeap<Traits>::heapifyUp(size_t idx){
    while( idx > 0 ){
        size_t parent = (idx - 1) / 2;
        if( m_comp(m_pData[parent], m_pData[idx]) ){
            intercambiar(m_pData[parent], m_pData[idx]);
            intercambiar(m_pRefs[parent], m_pRefs[idx]);
            idx = parent;
        } else {
            break;
        }
    }
}

// heapifyDown
template <typename Traits>
void CHeap<Traits>::heapifyDown(size_t idx){
    while( true ){
        size_t best  = idx;
        size_t left  = 2 * idx + 1;
        size_t right = 2 * idx + 2;
        if( left < m_nElements && m_comp(m_pData[best], m_pData[left]) )
            best = left;
        if( right < m_nElements && m_comp(m_pData[best], m_pData[right]) )
            best = right;
        if( best != idx ){
            intercambiar(m_pData[best], m_pData[idx]);
            intercambiar(m_pRefs[best], m_pRefs[idx]);
            idx = best;
        } else {
            break;
        }
    }
}

// push
template <typename Traits>
void CHeap<Traits>::push(value_type val, ref_type ref){
    lock_guard<mutex> lock(m_mtx);
    if( m_nElements >= m_nCapacity )
        resize();
    m_pData[m_nElements] = val;
    m_pRefs[m_nElements] = ref;
    heapifyUp(m_nElements);
    ++m_nElements;
}

// pop
template <typename Traits>
typename CHeap<Traits>::value_type CHeap<Traits>::pop(){
    lock_guard<mutex> lock(m_mtx);
    if( m_nElements == 0 ) throw runtime_error("Heap vacio");
    value_type val = m_pData[0];
    --m_nElements;
    m_pData[0] = m_pData[m_nElements];
    m_pRefs[0] = m_pRefs[m_nElements];
    heapifyDown(0);
    return val;
}

// top
template <typename Traits>
typename CHeap<Traits>::value_type CHeap<Traits>::top() const {
    lock_guard<mutex> lock(m_mtx);
    if( m_nElements == 0 ) throw runtime_error("Heap vacio");
    return m_pData[0];
}

// operator<<
template <typename Traits>
ostream &operator<<(ostream &os, CHeap<Traits> &container){
    os << "CHeap: size = " << container.getSize() << endl;
    os << "[";
    for( size_t i = 0; i < container.m_nElements; ++i ){
        os << "(" << container.m_pData[i] << ":" << container.m_pRefs[i] << ")";
        if( i + 1 < container.m_nElements ) os << ",";
    }
    os << "]" << endl;
    return os;
}

// operator>>
template <typename Traits>
istream &operator>>(istream &is, CHeap<Traits> &container){
    using value_type = typename Traits::value_type;
    size_t size;
    if( is >> size ){
        value_type value;
        ref_type ref;
        for( size_t i = 0; i < size; ++i ){
            is >> value >> ref;
            container.push(value, ref);
        }
    }
    return is;
}

#endif // __HEAP_H__