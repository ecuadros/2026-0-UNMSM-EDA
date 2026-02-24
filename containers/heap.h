#ifndef __HEAP_H__
#define __HEAP_H__

#include <iostream>
#include <utility>
#include <mutex>
#include <stdexcept>
#include <vector>
#include "../general/types.h"
#include "../util.h"
template <typename T, typename _Func>
struct HeapTrait{
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct MinHeap : 
    public HeapTrait<T, std::greater<T> >{
};

template <typename T>
struct MaxHeap : 
    public HeapTrait<T, std::less<T> >{
};

template <typename Traits>
class CHeap{
    public:
    using value_type=typename Traits::value_type;
    using Func       = typename Traits::Func;
    private:
    std::vector<value_type> m_data; 
    mutable std::mutex Block;       

    size_t Father(size_t i)     { return (i - 1) / 2; }
    size_t Son_Left(size_t i)   { return (2 * i) + 1; }
    size_t Son_Right(size_t i)  { return (2 * i) + 2; }
    void Swap_up(size_t index);
    void Swap_down(size_t index);
    public:
    CHeap(){};
    //COPY CONSTRUCTOR
    CHeap(const CHeap &another){
     std::lock_guard<std::mutex> lock(another.Block);
     m_data = another.m_data;
    }
    //MOVE CONSTRUCTOR
     CHeap(CHeap &&another) noexcept{
     std::lock_guard<std::mutex> lock(another.Block);
     m_data = std::exchange(another.m_data, std::vector<value_type>());
     }
    //DESTRUCTOR
    virtual ~CHeap()=default;
    value_type  Pop();
    value_type Top() const;
    void   Push(const value_type& Val);
    //OPERATOR <<
    friend std::ostream& operator<<(std::ostream &os, CHeap<Traits> &container){
    std::lock_guard<std::mutex> lock(container.Block);
    os << "Heap [ ";
    for (size_t i = 0; i < container.m_data.size(); ++i) {
        os << container.m_data[i] << " ";
    }
    os << "]" << std::endl;
    return os;
    }
    //OPERATOR >>
    friend std::istream& operator>>(std::istream &is, CHeap<Traits> &container){
    value_type val;
        if (is >> val) {
            container.Push(val);   
         }
    return is;
    }
};
template <typename Traits>
void CHeap<Traits>::Swap_up(size_t index) {
    Func comp;
    while (index > 0) {
        size_t p = Father(index);
        
        if (comp(m_data[p], m_data[index])) {
            std::swap(m_data[index], m_data[p]);
            index = p;
        } else {
            break;
        }
    }
}
template <typename Traits>
void CHeap<Traits>::Swap_down(size_t index) {
    Func comp;
    while (true) {
        size_t extreme = index;
        size_t Izq = Son_Left(index);
        size_t Der = Son_Right(index);

        if (Izq < m_data.size() && comp(m_data[extreme], m_data[Izq]))
            extreme = Izq;
        
        if (Der < m_data.size() && comp(m_data[extreme], m_data[Der]))
            extreme = Der;
        
        if (extreme != index) {
            std::swap(m_data[index], m_data[extreme]);
            index = extreme;
        } else {
            break;
        }
    }
}
template <typename Traits>
void  CHeap<Traits>::Push(const value_type& val){
std::lock_guard<std::mutex> lock(Block);
    m_data.push_back(std::move(val)); 
    Swap_up(m_data.size() - 1);
}
template <typename Traits>
typename  CHeap<Traits>::value_type CHeap<Traits>::Pop(){
std::lock_guard<std::mutex> lock(Block);
if (m_data.size() == 0) {
        throw std::runtime_error("El heap esta vacio "); 
    }
    value_type root = m_data[0];

    if (m_data.size() > 1) {
        m_data[0] = m_data.back();
        m_data.pop_back();                    
        Swap_down(0);
    } else {
        m_data.pop_back();
    }
    return root;
}

template <typename Traits>
typename CHeap<Traits>::value_type CHeap<Traits>::Top() const {
    std::lock_guard<std::mutex> lock(Block);
    if (m_data.size() == 0) {
        throw std::runtime_error("El Heap esta vacio ");
    }
    return m_data[0]; 
}
#endif // __HEAP_H__