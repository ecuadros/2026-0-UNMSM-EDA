#ifndef __HEAP_H__
#define __HEAP_H__

#include <iostream>
#include <mutex>
#include "vector.h"
#include "../util.h"

template <typename T, typename _Compare>
struct HeapTrait {
    using value_type = T;
    using Compare = _Compare;
};

template <typename T>
struct MinHeapTrait : public HeapTrait<T, std::greater<T>> {};

template <typename T>
struct MaxHeapTrait : public HeapTrait<T, std::less<T>> {};

template <typename Traits>
class CHeap{

    public:
    using value_type     = typename Traits::value_type;
    using Compare        = typename Traits::Compare;
    using Vector         = Vector<Traits>;

private:
    Vector                m_data;
    Compare               m_comp;
    mutable std::mutex    m_mutex;

public:
    CHeap();
    CHeap(const CHeap&);
    CHeap(CHeap&&) noexcept;
    virtual ~CHeap();

    void push(const value_type&);
    value_type pop();

    value_type& top();
    const value_type& top() const;

    size_t size() const;
    bool empty() const;

private:
    void heapifyUp(size_t idx);
    void heapifyDown(size_t idx);

    friend std::ostream& operator<<(std::ostream&, const CHeap<Traits>&);
    friend std::istream& operator>>(std::istream&, CHeap<Traits>&);
};

template <typename Traits>
CHeap<Traits>::CHeap() : m_data(), m_comp(Compare()) {
    // No se utilizará el primer elemento para mejor legibilidad
    m_data.pushback(value_type());
}

template <typename Traits>
CHeap<Traits>::CHeap(const CHeap& other){
    std::lock_guard<std::mutex> lock(other.m_mutex);
    m_data = other.m_data;
}

template <typename Traits>
CHeap<Traits>::CHeap(CHeap&& other) noexcept{
    std::lock_guard<std::mutex> lock(other.m_mutex);
    m_data = std::move(other.m_data);
}

template <typename Traits>
CHeap<Traits>::~CHeap() {}  // El destructor del vector se encargará de liberar la memoria

template <typename Traits>
void CHeap<Traits>::push(const value_type& value){
    std::lock_guard<std::mutex> lock(m_mutex);
    m_data.pushback(value);
    heapifyUp(m_data.getSize() - 1);
}

template <typename Traits>
typename CHeap<Traits>::value_type CHeap<Traits>::pop(){
    std::lock_guard<std::mutex> lock(m_mutex);

    if(size() == 0) return value_type();

    value_type root = m_data[1];
    m_data[1] = m_data[m_data.getSize()-1];
    m_data.popback();

    if(size() > 0)
        heapifyDown(1);
    return root;
}

template <typename Traits>
typename CHeap<Traits>::value_type& CHeap<Traits>::top(){
    return m_data[1];
}

template <typename Traits>
const typename CHeap<Traits>::value_type& CHeap<Traits>::top() const{
    return m_data[1];
}

template <typename Traits>
size_t CHeap<Traits>::size() const{
    return (m_data.getSize() == 0 ? 0 : m_data.getSize() - 1);
}

template <typename Traits>
bool CHeap<Traits>::empty() const{
    return size() == 0;
}

template <typename Traits>
void CHeap<Traits>::heapifyUp(size_t idx){
    if(idx <= 1) return;

    size_t parent = idx / 2;

    if(m_comp(m_data[parent], m_data[idx])){
        std::swap(m_data[parent], m_data[idx]);
        heapifyUp(parent);
    }
}

template <typename Traits>
void CHeap<Traits>::heapifyDown(size_t idx){
    size_t left  = idx * 2;
    size_t right = idx * 2 + 1;
    size_t aux = idx;

    if(left < m_data.getSize() && m_comp(m_data[aux], m_data[left]))
        aux = left;

    if(right < m_data.getSize() && m_comp(m_data[aux], m_data[right]))
        aux = right;

    if(aux != idx){
        std::swap(m_data[idx], m_data[aux]);
        heapifyDown(aux);
    }
}

template <typename Traits>
std::ostream& operator<<(std::ostream& os, const CHeap<Traits>& heap){
    std::lock_guard<std::mutex> lock(heap.m_mutex);
    os << "CHeap size=" << heap.size() << " -> [ ";
    for(size_t i = 1; i < heap.m_data.getSize(); ++i)
        os << heap.m_data[i] << " ";
    os << "]";
    return os;
}

template <typename Traits>
std::istream& operator>>(std::istream& is, CHeap<Traits>& heap){
    std::lock_guard<std::mutex> lock(heap.m_mutex);

    heap = CHeap<Traits>(); // Limpiar el heap antes de cargar nuevos datos
    heap.m_data.pushback(typename Traits::value_type()); // Reservar el primer elemento
    size_t n;
    std::cout << "¿Cuántos elementos?: ";
    std::cin >> n;

    for(size_t i=0;i<n;++i){
        typename Traits::value_type val;
        std::cout << "Elemento " << i+1 << ": ";
        std::cin >> val;
        heap.push(val);
    }
    return is;
}

void DemoHeap();

#endif // __HEAP_H__