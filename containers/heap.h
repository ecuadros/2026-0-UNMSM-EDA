#ifndef __HEAP_H__
#define __HEAP_H__

#include <iostream>
#include <vector>
#include <mutex>
#include <stdexcept>
#include <algorithm>
#include "../general/types.h"
#include "../util.h"

// FORWARD DECLARATIONS

template <typename T, typename Comparator> class CHeap;

template <typename T, typename Comparator>
std::ostream& operator<<(std::ostream& os, CHeap<T, Comparator>& heap);

template <typename T, typename Comparator>
std::istream& operator>>(std::istream& is, CHeap<T, Comparator>& heap);


// COMPARADORES

// Max-Heap: El padre es MAYOR que los hijo
template <typename T>
struct MaxHeapComparator {
    bool operator()(const T& a, const T& b) const {
        return a < b; // Para max-heap, queremos el mayor arriba
    }
};

// Min-Heap: El padre es MENOR que los hijos
template <typename T>
struct MinHeapComparator {
    bool operator()(const T& a, const T& b) const {
        return a > b; // Para min-heap, queremos el menor arriba
    }
};


// CLASE HEAP 

template <typename T, typename Comparator = MaxHeapComparator<T>>
class CHeap {
private:
    std::vector<T> m_data;           
    Comparator m_compare;            // Comparador (Max o Min)
    mutable std::mutex m_mutex;      
    
    
    // Métodos auxiliares privados
    
    // Obtener índices de padre e hijos
    size_t GetParentIndex(size_t index) const {
        return (index - 1) / 2;
    }
    
    size_t GetLeftChildIndex(size_t index) const {
        return 2 * index + 1;
    }
    
    size_t GetRightChildIndex(size_t index) const {
        return 2 * index + 2;
    }
    
    // Verificar si tiene hijos
    bool HasLeftChild(size_t index) const {
        return GetLeftChildIndex(index) < m_data.size();
    }
    
    bool HasRightChild(size_t index) const {
        return GetRightChildIndex(index) < m_data.size();
    }
    
    bool HasParent(size_t index) const {
        return index > 0;
    }
    
    // Obtener valores de padre e hijos
    T GetLeftChild(size_t index) const {
        return m_data[GetLeftChildIndex(index)];
    }
    
    T GetRightChild(size_t index) const {
        return m_data[GetRightChildIndex(index)];
    }
    
    T GetParent(size_t index) const {
        return m_data[GetParentIndex(index)];
    }
    
    // Intercambiar elementos
    void Swap(size_t index1, size_t index2) {
        std::swap(m_data[index1], m_data[index2]);
    }
    
    // Heapify Up: Reorganizar hacia arriba (después de insertar)
    void HeapifyUp(size_t index) {
        while (HasParent(index) && m_compare(GetParent(index), m_data[index])) {
            Swap(GetParentIndex(index), index);
            index = GetParentIndex(index);
        }
    }
    
    // Heapify Down: Reorganizar hacia abajo (después de extraer)
    void HeapifyDown(size_t index) {
        while (HasLeftChild(index)) {
            size_t smallerChildIndex = GetLeftChildIndex(index);
            
            if (HasRightChild(index) && 
                m_compare(GetLeftChild(index), GetRightChild(index))) {
                smallerChildIndex = GetRightChildIndex(index);
            }
            
            if (!m_compare(m_data[index], m_data[smallerChildIndex])) {
                break;
            }
            
            Swap(index, smallerChildIndex);
            index = smallerChildIndex;
        }
    }

public:
    CHeap() = default;
    
    // Constructor Copia
    
     CHeap(const CHeap &other) {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        m_data = other.m_data;
    }
    
    // Move Constructor
    
    CHeap(CHeap &&other) noexcept {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        m_data = std::move(other.m_data);
    }
    
    
    // Destructor
    
    virtual ~CHeap() {
        Clear();
    }
    
    // Push 
    
    void Push(const T &value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_data.push_back(value);
        HeapifyUp(m_data.size() - 1);
    }
    
    // Pop 
    
    T Pop() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_data.empty()) throw std::out_of_range("Heap is empty");
        
        T root = m_data[0];
        m_data[0] = m_data.back();
        m_data.pop_back();
        
        if (!m_data.empty()) {
            HeapifyDown(0);
        }
        
        return root;
    }
    
    // Top 
    
    T &Top() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_data.empty()) throw std::out_of_range("Heap is empty");
        return m_data[0];
    }
    
    const T &Top() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_data.empty()) throw std::out_of_range("Heap is empty");
        return m_data[0];
    }
    
    
    // isEmpty 

    bool isEmpty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_data.empty();
    }
    
    // getSize 
    
    size_t getSize() const {
        return m_data.size();
    }
    
    
    // Clear 

    void Clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_data.clear();
    }
    
    
    // Operadores de Comparación
    
    bool operator<(const CHeap &other) const {
        return m_data.size() < other.m_data.size();
    }
    
    bool operator>(const CHeap &other) const {
        return m_data.size() > other.m_data.size();
    }
    
    bool operator==(const CHeap &other) const {
        return m_data.size() == other.m_data.size();
    }
    
    // Friends
    friend std::ostream& operator<< <T, Comparator>(std::ostream& os, CHeap<T, Comparator>& heap);
    friend std::istream& operator>> <T, Comparator>(std::istream& is, CHeap<T, Comparator>& heap);
};


// IMPLEMENTACIÓN DE OPERADORES


template <typename T, typename Comparator>
std::ostream& operator<<(std::ostream& os, CHeap<T, Comparator>& heap) {
    std::lock_guard<std::mutex> lock(heap.m_mutex);
    
    // Determinar tipo de heap
    std::string heapType;
    if (std::is_same<Comparator, MaxHeapComparator<T>>::value) {
        heapType = "Max-Heap";
    } else if (std::is_same<Comparator, MinHeapComparator<T>>::value) {
        heapType = "Min-Heap";
    } else {
        heapType = "Heap";
    }
    
    os << "CHeap (" << heapType << "): size = " << heap.m_data.size() << "\n";
    os << "[ ";
    
    for (size_t i = 0; i < heap.m_data.size(); ++i) {
        os << heap.m_data[i];
        if (i < heap.m_data.size() - 1) os << ", ";
    }
    
    os << " ]" << std::endl;
    return os;
}

template <typename T, typename Comparator>
std::istream& operator>>(std::istream& is, CHeap<T, Comparator>& heap) {
    T value;
    if (is >> value) {
        heap.Push(value);
    }
    return is;
}

template <typename T>
using MaxHeap = CHeap<T, MaxHeapComparator<T>>;

template <typename T>
using MinHeap = CHeap<T, MinHeapComparator<T>>;

#endif // __HEAP_H__
