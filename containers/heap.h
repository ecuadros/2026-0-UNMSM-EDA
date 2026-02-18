#ifndef __HEAP_H__
#define __HEAP_H__

#include <iostream>
#include <mutex>
#include <vector>
#include <cassert>
#include <algorithm>

using namespace std;

// Traits para MinHeap y MaxHeap
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
class CHeap {
public:
    using value_type = typename Traits::value_type;
    using Compare    = typename Traits::Compare;

private:
    vector<value_type> m_data;
    mutable mutex m_mutex;

    size_t parent(size_t i) { return (i - 1) / 2; }
    size_t left(size_t i)   { return (2 * i) + 1; }
    size_t right(size_t i)  { return (2 * i) + 2; }

    void heap_up(size_t index) {
        while (index > 0) {
            size_t p = parent(index);
            if (Compare()(m_data[index], m_data[p])) {
                swap(m_data[index], m_data[p]);
                index = p;
            } else
               break;
        }
    }

    void heap_down(size_t index) {
        size_t size = m_data.size();
        while (true) {
            size_t extreme = index;
            size_t l = left(index);
            size_t r = right(index);

            if (l < size && Compare()(m_data[l], m_data[extreme])) extreme = l;
            if (r < size && Compare()(m_data[r], m_data[extreme])) extreme = r;

            if (extreme != index) {
                swap(m_data[index], m_data[extreme]);
                index = extreme;
            } else
              break;
        }
    }

public:
    CHeap() = default;

    // Constructor de Copia (Thread-safe)
    CHeap(const CHeap<Traits> &another) {
        lock_guard<mutex> lock(another.m_mutex);
        m_data = another.m_data;
    }

    // Move Constructor (noexcept)
    CHeap(CHeap<Traits> &&another) noexcept {
        lock_guard<mutex> lock(another.m_mutex);
        m_data = move(another.m_data);
    }

    // Destructor seguro
    virtual ~CHeap() {
        lock_guard<mutex> lock(m_mutex);
        m_data.clear();
    }

    // Operadores de Asignación 
    CHeap& operator=(const CHeap& other) {
        if (this != &other) {
            scoped_lock lock(m_mutex, other.m_mutex);
            m_data = other.m_data;
        }
        return *this;
    }
    
    CHeap& operator=(CHeap&& other) noexcept {
    if (this != &other) {
        std::scoped_lock lock(m_mutex, other.m_mutex);
        m_data = std::move(other.m_data);
    }
    return *this;
    }  

    void push(const value_type &val) {
        lock_guard<mutex> lock(m_mutex);
        m_data.push_back(val);
        heap_up(m_data.size() - 1);
    }

    value_type pop() {
        lock_guard<mutex> lock(m_mutex);
        if (m_data.empty()) throw runtime_error("Heap vacío");
        
        value_type root = m_data[0];
        m_data[0] = m_data.back();
        m_data.pop_back();

        if (!m_data.empty()) heap_down(0);
        return root;
    }

    value_type top() const {
        lock_guard<mutex> lock(m_mutex);
        if (m_data.empty()) throw runtime_error("Heap vacío");
        return m_data[0];
    }

    bool empty() const {
        lock_guard<mutex> lock(m_mutex);
        return m_data.empty();
    }

    size_t size() const {
        lock_guard<mutex> lock(m_mutex);
        return m_data.size();
    }

    template <typename U>
    friend ostream& operator<<(ostream &os, const CHeap<U> &heap);

    template <typename U>
    friend istream& operator>>(istream &is, CHeap<U> &heap);
};

template <typename Traits>
ostream& operator<<(ostream &os, const CHeap<Traits> &heap) {
    lock_guard<mutex> lock(heap.m_mutex);
    os << "[ ";
    for (const auto &val : heap.m_data) os << val << " ";
    os << "]";
    return os;
}

template <typename Traits>
istream& operator>>(istream &is, CHeap<Traits> &heap) {
    typename Traits::value_type val;
    if (is >> val) heap.push(val);
    return is;
}

#endif