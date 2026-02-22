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



template <typename T, typename Comparator>
struct HeapConfiguration {
    using element_type = T;
    using compare_type = Comparator;
};

template <typename T>
struct MaxHeapConfig : HeapConfiguration<T, std::greater<T>> {
};

template <typename T>
struct MinHeapConfig : HeapConfiguration<T, std::less<T>> {
};


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
        Element(element_type d, ref_type r = -1) 
            : data(d), reference(r) {}

        Element(const Element &other) = default;
        Element(Element &&other) noexcept = default;
        Element &operator=(const Element &other) = default;
        Element &operator=(Element &&other) noexcept = default;

        bool operator<(const Element &other) const {
            return data < other.data;
        }
        bool operator>(const Element &other) const {
            return data > other.data;
        }
        bool operator==(const Element &other) const {
            return data == other.data;
        }
    };

    mutable mutex access_lock;
    vector<Element> elements;

   
    static size_t getParentIdx(size_t i) noexcept {
        return i == 0 ? 0 : (i - 1) / 2;
    }

    static size_t getLeftChildIdx(size_t i) noexcept {
        return 2 * i + 1;
    }

    static size_t getRightChildIdx(size_t i) noexcept {
        return 2 * i + 2;
    }

    bool shouldSwap(const element_type &parent, const element_type &child) const {
        return compare_type{}(child, parent);  // Retorna true si child "mejor" que parent
    }

    void swapElements(size_t idx1, size_t idx2) noexcept {
        swap(elements[idx1], elements[idx2]);
    }

    void percolateUp(size_t idx) noexcept {
        if (idx == 0) return;
        
        size_t parent_idx = getParentIdx(idx);
        if (shouldSwap(elements[parent_idx].data, elements[idx].data)) {
            swapElements(idx, parent_idx);
            percolateUp(parent_idx);
        }
    }

    void percolateDown(size_t idx) noexcept {
        size_t size = elements.size();
        while (true) {
            size_t left = getLeftChildIdx(idx);
            size_t right = getRightChildIdx(idx);

            if (left >= size) break;

            size_t target = left;
            if (right < size && 
                shouldSwap(elements[left].data, elements[right].data)) {
                target = right;
            }

            if (!shouldSwap(elements[idx].data, elements[target].data)) break;

            swapElements(idx, target);
            idx = target;
        }
    }

    void clearHeap() noexcept {
        elements.clear();
        elements.shrink_to_fit();
    }

public:
    // Constructor por defecto
    CHeap() = default;

    // Constructor copia
    CHeap(const CHeap &other) {
        lock_guard<mutex> lock(other.access_lock);
        elements = other.elements;
    }

    // Move constructor
    CHeap(CHeap &&other) noexcept {
        lock_guard<mutex> lock(other.access_lock);
        elements = move(other.elements);
        other.clearHeap();
    }

    // Destructor
    virtual ~CHeap() {
        lock_guard<mutex> lock(access_lock);
        clearHeap();
    }

    // Operador asignación copia
    CHeap &operator=(const CHeap &other) {
        if (this == &other) return *this;
        
        vector<Element> temp;
        {
            lock_guard<mutex> lock(other.access_lock);
            temp = other.elements;
        }
        {
            lock_guard<mutex> lock(access_lock);
            elements = move(temp);
        }
        return *this;
    }

    // Insertar elemento
    void push(element_type value, ref_type ref = -1) {
        lock_guard<mutex> lock(access_lock);
        elements.emplace_back(Element(value, ref));
        if (elements.size() > 1) {
            percolateUp(elements.size() - 1);
        }
    }

    // Extraer raíz
    element_type pop() {
        lock_guard<mutex> lock(access_lock);
        if (elements.empty()) {
            return element_type{};  // Retorna valor por defecto si está vacío
        }

        element_type top = elements.front().data;
        if (elements.size() == 1) {
            elements.pop_back();
            return top;
        }

        swapElements(0, elements.size() - 1);
        elements.pop_back();
        if (!elements.empty()) {
            percolateDown(0);
        }
        return top;
    }

    // Obtener tamaño
    size_t size() const {
        lock_guard<mutex> lock(access_lock);
        return elements.size();
    }

    // Verificar si está vacío
    bool isEmpty() const {
        lock_guard<mutex> lock(access_lock);
        return elements.empty();
    }

    // Obtener el tope sin remover
    element_type peek() const {
        lock_guard<mutex> lock(access_lock);
        if (elements.empty()) {
            throw runtime_error("Cannot peek at empty heap");
        }
        return elements.front().data;
    }

    // Operadores de flujo
    friend ostream& operator<< <Config>(ostream &os, CHeap<Config> &h);
    friend istream& operator>> <Config>(istream &is, CHeap<Config> &h);
};



template <typename Config>
ostream& operator<<(ostream &os, CHeap<Config> &h) {
    using element_type = typename Config::element_type;
    lock_guard<mutex> lock(h.access_lock);
    
    os << "CHeap[size=" << h.elements.size() << "]: {";
    for (size_t i = 0; i < h.elements.size(); ++i) {
        if (i > 0) os << ", ";
        if constexpr (is_same_v<element_type, string>) {
            os << quoted(h.elements[i].data) << ":" << h.elements[i].reference;
        } else {
            os << h.elements[i].data << ":" << h.elements[i].reference;
        }
    }
    os << "}" << endl;
    return os;
}

template <typename Config>
istream& operator>>(istream &is, CHeap<Config> &h) {
    using element_type = typename Config::element_type;
    
    vector<pair<element_type, ref_type>> items;
    
    try {
        string line;
        getline(is, line, '{');
        
        char ch;
        while (is.get(ch) && ch != '}') {
            if (ch == ':' || ch == ',' || ch == ' ') continue;
            
            is.unget();
            element_type val;
            ref_type ref = -1;
            
            if constexpr (is_same_v<element_type, string>) {
                is >> quoted(val);
            } else {
                is >> val;
            }
            
            if (is.peek() == ':') {
                is.ignore();
                is >> ref;
            }
            
            items.push_back({val, ref});
            
           
            while (is.peek() == ' ' || is.peek() == ',') {
                is.get();
            }
        }
    } catch (const exception&) {
        is.setstate(ios::failbit);
        return is;
    }

    if (is) {
        lock_guard<mutex> lock(h.access_lock);
        h.clearHeap();
        
        for (const auto &item : items) {
            h.elements.emplace_back(
                typename CHeap<Config>::Element(item.first, item.second)
            );
        }
        
        
        if (h.elements.size() > 1) {
            for (int i = h.elements.size() / 2 - 1; i >= 0; --i) {
                h.percolateDown(i);
            }
        }
    }
    
    return is;
}


void DemoHeap();

#endif 