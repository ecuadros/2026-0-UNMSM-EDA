#ifndef __HEAP_H__
#define __HEAP_H__

#include <iostream>
#include <vector>
#include <mutex>
#include <functional>
#include "../general/types.h"
#include "../util.h"

// Traits para Max-Heap
template <typename T>
struct HeapTraitMax {
    using value_type  = T;
    using CompareFunc = std::less<T>; // padre debe ser mayor
};

// Traits para Min-Heap
template <typename T>
struct HeapTraitMin {
    using value_type  = T;
    using CompareFunc = std::greater<T>; // padre debe ser menor
};

template <typename Traits>
class CHeap {
public:
    using value_type  = typename Traits::value_type;
    using CompareFunc = typename Traits::CompareFunc;

private:
    std::vector<value_type> m_data;
    CompareFunc             m_comp;
    std::mutex              m_mutex;

    // Indice del padre, hijo izquierdo y derecho
    int parent(int i) { return (i - 1) / 2; }
    int left  (int i) { return 2 * i + 1;   }
    int right (int i) { return 2 * i + 2;   }

    // Sube el elemento en la posicion i hasta su lugar correcto
    void heapifyUp(int i) {
        while (i > 0 && m_comp(m_data[parent(i)], m_data[i])) {
            std::swap(m_data[i], m_data[parent(i)]);
            i = parent(i);
        }
    }

    // Baja el elemento en la posicion i hasta su lugar correcto
    void heapifyDown(int i) {
        int size = m_data.size();
        int target = i;
        int l = left(i);
        int r = right(i);

        if (l < size && m_comp(m_data[target], m_data[l]))
            target = l;
        if (r < size && m_comp(m_data[target], m_data[r]))
            target = r;

        if (target != i) {
            std::swap(m_data[i], m_data[target]);
            heapifyDown(target);
        }
    }

public:
    // Constructor por defecto
    CHeap() {}

    // Constructor copia
    CHeap(const CHeap& other) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_data = other.m_data;
    }

    // Move constructor
    CHeap(CHeap&& other) noexcept {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_data = std::move(other.m_data);
    }

    // Destructor seguro
    ~CHeap() {
        m_data.clear();
    }

    // Insertar elemento
    void Push(const value_type& val) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_data.push_back(val);
        heapifyUp(m_data.size() - 1);
    }

    // Eliminar la raiz (maximo o minimo segun trait)
    void Pop() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_data.empty()) return;
        m_data[0] = m_data.back();
        m_data.pop_back();
        if (!m_data.empty())
            heapifyDown(0);
    }

    // Ver la raiz sin eliminarla
    value_type Top() const {
        return m_data.front();
    }

    bool empty() const { return m_data.empty(); }
    int  size()  const { return m_data.size();  }

    // operator>> : extrae la raiz (como Pop pero retorna el valor)
    CHeap& operator>>(value_type& val) {
        std::lock_guard<std::mutex> lock(m_mutex);
        val = m_data.front();
        m_data[0] = m_data.back();
        m_data.pop_back();
        if (!m_data.empty())
            heapifyDown(0);
        return *this;
    }

    // operator<< : inserta un elemento
    CHeap& operator<<(const value_type& val) {
        Push(val);
        return *this;
    }

    // Imprimir el heap
    friend std::ostream& operator<<(std::ostream& os, const CHeap& h) {
        for (const auto& elem : h.m_data)
            os << elem << " ";
        return os;
    }
};

#endif // __HEAP_H__