#ifndef __HEAP_H__
#define __HEAP_H__

#include <iostream>
#include <vector>
#include <mutex>
#include <memory>
#include <stdexcept>
#include <functional>
#include <iomanip>
#include <type_traits>
#include <utility>
#include "../general/types.h"
#include "../util.h"

using namespace std;

// Configuraciones para Max y Min Heap
template <typename T, typename Comparator>
struct HeapConfiguration {
    using element_type = T;
    using compare_type = Comparator;
};

template <typename T>
struct MaxHeapConfig : HeapConfiguration<T, std::greater<T>> {};

template <typename T>
struct MinHeapConfig : HeapConfiguration<T, std::less<T>> {};

template <typename Config>
class CHeap;

template <typename Config>
ostream& operator<<(ostream &os, CHeap<Config> &h);

template <typename Config>
istream& operator>>(istream &is, CHeap<Config> &h);

template <typename Config>
class CHeap {
public:
    using element_type = typename Config::element_type;
    using compare_type = typename Config::compare_type;
    
private:
    struct Element {
        element_type data;
        ref_type reference;

        Element() : data(), reference(-1) {}
        Element(element_type d, ref_type r = -1) : data(d), reference(r) {}
        
        // Rule of 5 para Element
        Element(const Element &other) = default;
        Element(Element &&other) noexcept = default;
        Element &operator=(const Element &other) = default;
        Element &operator=(Element &&other) noexcept = default;
    };

    mutable mutex access_lock; // 8. Punto: Concurrencia
    vector<Element> elements;

    // Métodos de navegación del árbol
    static size_t getParentIdx(size_t i) noexcept { return i == 0 ? 0 : (i - 1) / 2; }
    static size_t getLeftChildIdx(size_t i) noexcept { return 2 * i + 1; }
    static size_t getRightChildIdx(size_t i) noexcept { return 2 * i + 2; }

    bool shouldSwap(const element_type &parent, const element_type &child) const {
        return compare_type{}(child, parent);
    }

    void percolateUp(size_t idx) noexcept {
        while (idx > 0) {
            size_t parent = getParentIdx(idx);
            if (shouldSwap(elements[parent].data, elements[idx].data)) {
                swap(elements[idx], elements[parent]);
                idx = parent;
            } else break;
        }
    }

    void percolateDown(size_t idx) noexcept {
        size_t size = elements.size();
        while (true) {
            size_t left = getLeftChildIdx(idx);
            size_t right = getRightChildIdx(idx);
            if (left >= size) break;

            size_t target = left;
            if (right < size && shouldSwap(elements[left].data, elements[right].data)) {
                target = right;
            }
            if (!shouldSwap(elements[idx].data, elements[target].data)) break;
            swap(elements[idx], elements[target]);
            idx = target;
        }
    }

public:
    CHeap() = default;

    // 1. Constructor Copia
    CHeap(const CHeap &other) {
        lock_guard<mutex> lock(other.access_lock);
        elements = other.elements;
    }

    // 2. Move Constructor
    CHeap(CHeap &&other) noexcept {
        lock_guard<mutex> lock(other.access_lock);
        elements = move(other.elements);
    }

    // 3. Destructor Seguro
    virtual ~CHeap() {
        lock_guard<mutex> lock(access_lock);
        elements.clear();
    }

    // 4. Push con Mutex
    void push(element_type value, ref_type ref = -1) {
        lock_guard<mutex> lock(access_lock);
        elements.emplace_back(Element(value, ref));
        percolateUp(elements.size() - 1);
    }

    // 5. Pop con Mutex
    element_type pop() {
        lock_guard<mutex> lock(access_lock);
        if (elements.empty()) return element_type{};
        
        element_type top = elements.front().data;
        elements[0] = move(elements.back());
        elements.pop_back();
        
        if (!elements.empty()) percolateDown(0);
        return top;
    }

    // 6. Operator << (Imprimir con Lock)
    friend ostream& operator<< <Config>(ostream &os, CHeap<Config> &h) {
        lock_guard<mutex> lock(h.access_lock);
        os << "Heap[" << h.elements.size() << "]: {";
        for (size_t i = 0; i < h.elements.size(); ++i) {
            os << h.elements[i].data << (i == h.elements.size() - 1 ? "" : ", ");
        }
        os << "}";
        return os;
    }

    // 7. Operator >> (Cargar con Lock)
    friend istream& operator>> <Config>(istream &is, CHeap<Config> &h) {
        element_type val;
        if (is >> val) h.push(val);
        return is;
    }

    // Bonus: Operador de asignación (Rule of 3/5)
    CHeap &operator=(const CHeap &other) {
        if (this != &other) {
            unique_lock<mutex> lock1(access_lock, defer_lock);
            unique_lock<mutex> lock2(other.access_lock, defer_lock);
            lock(lock1, lock2);
            elements = other.elements;
        }
        return *this;
    }
};

#endif