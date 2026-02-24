#ifndef __HEAP_H__
#define __HEAP_H__

#include <iostream>
#include "../general/types.h"
#include "../util.h"
#include <mutex>
#include <stdexcept>
#include <vector>

    //traits
    template <typename T>
    struct MaxHeap{
        using value_type = T;
        using CompareFunc = std::greater<T>;
    };
    template <typename T>
    struct MinHeap{
        using value_type = T;
        using CompareFunc = std::less<T>;
    };

template <typename Traits>
class CHeap{
public:
    using value_type = typename Traits::value_type;
    using CompareFunc = typename Traits::CompareFunc;
    using size_type = size_t;
private:
    vector <value_type> m_data;
    mutable std::mutex mutex;
    CompareFunc comp;

    size_t Parent(size_t i){ return (i-1)/2;}
    size_t Left(size_t i){ return (2*i)+1;}
    size_t Right(size_t i){ return (2*i)+2;}

    //reordena hacia arriiba (PUSH)
    void HeapPush(size_t index){
        while (index != 0 && comp(m_data[index], m_data[Parent(index)])) {
            swap(m_data[index], m_data[Parent(index)]);
            index = Parent(index);
        }
    }
    //reordena hacia abajo (POP)
    void HeapPop(size_t index){
        size_t largest = index;
        size_t left = Left(index);
        size_t right = Right(index);
        size_t h_size = m_data.size();
        if (left < h_size && comp(m_data[left], m_data[largest])) {
            largest = left;}
        if (right < h_size && comp(m_data[right], m_data[largest])) {
            largest = right;}
        if (largest != index) {
            swap(m_data[index], m_data[largest]);
            HeapPop(largest);
        }
    }
public:
    CHeap() = default;

    //constructor copia
    CHeap(const CHeap& other) {
        std::lock_guard <std::mutex> lock(other.mutex);
        m_data = other.m_data;
    }
    //move constructor
    CHeap(CHeap&& other) noexcept {
        std::lock_guard <std::mutex> lock(other.mutex);
        m_data = std::move(other.m_data);
    }
    //destructor seguro
    virtual ~CHeap(){
        std::lock_guard <std::mutex> lock(mutex);
        m_data.clear();
    }
    //push //reordenaar hacia arriba
    void Push(const value_type& val){
        std::lock_guard <std::mutex> lock(mutex);
        m_data.push_back(val);
        HeapPush(m_data.size()-1);
    }
    //POP //reordenar hacia abajo
    void Pop(){
        std::lock_guard <std::mutex> lock(mutex);
        if(m_data.empty()) return;

        m_data[0]=m_data.back();
        m_data.pop_back();

        if(!m_data.empty()) HeapPop(0);
    }
    value_type Top() const {
        std::lock_guard <std::mutex> lock(mutex);
        if(m_data.empty()) throw std::out_of_range("Heap vacio");
        return m_data[0];
    }
    bool IsEmpty() const { return m_data.empty(); }

    //operadores << (write)
    friend ostream& operator<<(ostream& os, CHeap<Traits>& heap) {
    std::lock_guard <std::mutex> lock(heap.mutex);
    os<<"Heap: [";
    for(const auto& val : heap.m_data){
        os<<val<<" ";
    }
    os<<"]"<<endl;
    return os;
    }
    //operadores >> (read)
    friend istream& operator>>(istream& is, CHeap<Traits>& heap) {
    value_type val;
    cout<<"Ingrese un valor para el heap: ";
    is>>val;
    heap.Push(val);
    return is;
    }


};



#endif // __HEAP_H__