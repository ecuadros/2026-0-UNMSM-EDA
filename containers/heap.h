#ifndef __HEAP_H__
#define __HEAP_H__
#include <iostream>
#include <vector>
#include <mutex>
#include <stdexcept>
#include "../general/types.h"
#include "../util.h"
using namespace std;

// Heap (Max y Min)
template <typename T, typename Compare>
class CHeap {
private:
    vector<T> m_data;
    Compare m_comp;
    mutable std::mutex m_mutex; // Concurrencia

    void heapifyUp(size_t index) {
        while (index > 0) {
            size_t parent = (index - 1) / 2;
            if (m_comp(m_data[index], m_data[parent])) {
                swap(m_data[index], m_data[parent]);
                index = parent;
            } else {
                break;
            }
        }
    }

    void heapifyDown(size_t index) {
        size_t size = m_data.size();
        while (true) {
            size_t largest = index;
            size_t left = 2 * index + 1;
            size_t right = 2 * index + 2;

            if (left < size && m_comp(m_data[left], m_data[largest])) {
                largest = left;
            }
            if (right < size && m_comp(m_data[right], m_data[largest])) {
                largest = right;
            }

            if (largest != index) {
                swap(m_data[index], m_data[largest]);
                index = largest;
            } else {
                break;
            }
        }
    }

public:
    // Constructor
    CHeap() {}

    // Constructor copia
    CHeap(const CHeap& other) {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        m_data = other.m_data;
        m_comp = other.m_comp;
    }

    // Move Constructor
    CHeap(CHeap&& other) noexcept {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        m_data = std::move(other.m_data);
        m_comp = std::move(other.m_comp);
    }

    // Destructor seguro
    virtual ~CHeap() {
        clear();
    }

    CHeap& operator=(const CHeap& other) {
        if (this != &other) {
            std::lock(m_mutex, other.m_mutex);
            std::lock_guard<std::mutex> lock1(m_mutex, std::adopt_lock);
            std::lock_guard<std::mutex> lock2(other.m_mutex, std::adopt_lock);
            
            m_data = other.m_data;
            m_comp = other.m_comp;
        }
        return *this;
    }

    CHeap& operator=(CHeap&& other) noexcept {
        if (this != &other) {
            std::lock(m_mutex, other.m_mutex);
            std::lock_guard<std::mutex> lock1(m_mutex, std::adopt_lock);
            std::lock_guard<std::mutex> lock2(other.m_mutex, std::adopt_lock);
            
            m_data = std::move(other.m_data);
            m_comp = std::move(other.m_comp);
        }
        return *this;
    }
    
    // Push
    void push(const T& val) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_data.push_back(val);
        heapifyUp(m_data.size() - 1);
    }

    // Pop (retorna valor)
    T pop() {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (m_data.empty()) {
            throw std::runtime_error("Heap vacío");
        }

        T root = m_data[0];
        m_data[0] = m_data.back();
        m_data.pop_back();

        if (!m_data.empty()) {
            heapifyDown(0);
        }

        return root;
    }

    T top() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (m_data.empty()) {
            throw std::runtime_error("Heap vacío");
        }
        
        return m_data[0];
    }

    bool isEmpty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_data.empty();
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_data.size();
    }

    void clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_data.clear();
    }

    // Operator <<
    friend ostream& operator<<(ostream& os, CHeap<T, Compare>& heap) {
        std::lock_guard<std::mutex> lock(heap.m_mutex);
        
        os << "CHeap: size = " << heap.m_data.size() << endl;
        os << "[";
        
        for (size_t i = 0; i < heap.m_data.size(); ++i) {
            os << heap.m_data[i];
            if (i < heap.m_data.size() - 1) os << ", ";
        }
        
        os << "]" << endl;
        return os;
    }

    // Operator >>
    friend istream& operator>>(istream& is, CHeap<T, Compare>& heap) {
        std::lock_guard<std::mutex> lock(heap.m_mutex);
        
        heap.clear();
        
        size_t n;
        is >> n;
        
        for (size_t i = 0; i < n; ++i) {
            T val;
            is >> val;
            heap.m_data.push_back(val);
        }
        
        for (int i = (heap.m_data.size() / 2) - 1; i >= 0; --i) {
            heap.heapifyDown(i);
        }
        
        return is;
    }
};

// MaxHeap
template <typename T>
using MaxHeap = CHeap<T, std::greater<T>>;

// MinHeap
template <typename T>
using MinHeap = CHeap<T, std::less<T>>;

#endif // __HEAP_H__

