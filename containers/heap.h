#ifndef __HEAP_H__
#define __HEAP_H__

#include <iostream>
#include <mutex>
#include <stdexcept>
#include <vector>
#include <functional>
#include <utility> 
#include <type_traits> 
#include "../general/types.h"

template <typename Traits> class CHeap;
template <typename Traits> std::ostream& operator<<(std::ostream& os, CHeap<Traits>& heap);
template <typename Traits> std::istream& operator>>(std::istream& is, CHeap<Traits>& heap);

template <typename T, typename _Compare>
struct HeapTrait {
    using value_type = T;
    using Compare = _Compare;
};

template <typename T>
struct MaxHeapTrait : public HeapTrait<T, std::less<T>> {};

template <typename T>
struct MinHeapTrait : public HeapTrait<T, std::greater<T>> {};

template <typename Traits>
class CHeap {
public:
    using value_type = typename Traits::value_type;
    using Compare = typename Traits::Compare;
    
private:
    std::vector<value_type> m_data;
    
    // Concurrencia (mutex) 
    mutable std::mutex m_mutex;

    size_t indicePadre(size_t i) const { return (i - 1) / 2; }
    size_t hijoIzquierdo(size_t i) const { return 2 * i + 1; }
    size_t hijoDerecho(size_t i) const { return 2 * i + 2; }

    bool tieneHijoIzquierdo(size_t i) const {
        if (hijoIzquierdo(i) < m_data.size()) {
            return true;
        } else {
            return false;
        }
    }

    bool tieneHijoDerecho(size_t i) const {
        if (hijoDerecho(i) < m_data.size()) {
            return true;
        } else {
            return false;
        }
    }

    bool tienePadre(size_t i) const {
        if (i > 0) {
            return true;
        } else {
            return false;
        }
    }

    void subirElemento(size_t indice) {
        Compare comp;
        while (tienePadre(indice)) {
            size_t padre = indicePadre(indice);
            
            if (comp(m_data[padre], m_data[indice])) {
                std::swap(m_data[padre], m_data[indice]);
                indice = padre;
            } else {
                break;
            }
        }
    }

    void bajarElemento(size_t indice) {
        Compare comp;
        while (tieneHijoIzquierdo(indice)) {
            size_t candidato = hijoIzquierdo(indice);

            if (tieneHijoDerecho(indice)) {
                size_t derecho = hijoDerecho(indice);
                if (comp(m_data[candidato], m_data[derecho])) {
                    candidato = derecho;
                }
            }

            if (comp(m_data[indice], m_data[candidato])) {
                std::swap(m_data[indice], m_data[candidato]);
                indice = candidato;
            } else {
                break;
            }
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
        m_data = std::exchange(other.m_data, std::vector<value_type>());
    }
    
    void clear() {
        
        std::lock_guard<std::mutex> lock(m_mutex);
        m_data.clear();
    }
    
    //Destructor seguro
    virtual ~CHeap() {
        clear();
    }
    
    // Push
    void push(const value_type &valor) {
        
        std::lock_guard<std::mutex> lock(m_mutex);
        m_data.push_back(valor);
        subirElemento(m_data.size() - 1);
    }
    
    // Pop
    value_type pop() {
        
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (m_data.empty()) {
            throw std::out_of_range("El Heap esta vacio");
        }
        
        value_type resultado = m_data[0];
        
        if (m_data.size() > 1) {
            m_data[0] = std::exchange(m_data.back(), value_type());
            m_data.pop_back();
            bajarElemento(0);
        } else {
            m_data.pop_back();
        }
        
        return resultado;
    }
    
    const value_type& top() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_data.empty()) {
            throw std::out_of_range("El Heap esta vacio");
        }
        return m_data[0];
    }
  
    bool isEmpty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_data.empty()) {
            return true;
        } else {
            return false;
        }
    }
    
    size_t getSize() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_data.size();
    }
    
    bool operator<(const CHeap &other) const {
        std::scoped_lock lock(m_mutex, other.m_mutex);
        if (m_data.size() < other.m_data.size()) {
            return true;
        } else {
            return false;
        }
    }
    
    bool operator>(const CHeap &other) const {
        std::scoped_lock lock(m_mutex, other.m_mutex);
        if (m_data.size() > other.m_data.size()) {
            return true;
        } else {
            return false;
        }
    }
    
    bool operator==(const CHeap &other) const {
        std::scoped_lock lock(m_mutex, other.m_mutex);
        if (m_data.size() == other.m_data.size()) {
            return true;
        } else {
            return false;
        }
    }

    // Declaraciones de los operadores de flujo
    friend std::ostream& operator<< <Traits>(std::ostream& os, CHeap<Traits>& heap);
    friend std::istream& operator>> <Traits>(std::istream& is, CHeap<Traits>& heap);
};

// operator >> 
template <typename Traits>
std::istream& operator>>(std::istream& is, CHeap<Traits>& heap) {
    typename Traits::value_type valor;
    if (is >> valor) {
        heap.push(valor);
    }
    return is;
}

//  Operator << 
template <typename Traits>
std::ostream& operator<<(std::ostream& os, CHeap<Traits>& heap) {
    
    std::lock_guard<std::mutex> lock(heap.m_mutex);
    
    std::string tipoHeap;
    if (std::is_same<typename Traits::Compare, std::less<typename Traits::value_type>>::value) {
        tipoHeap = "Max-Heap";
    } else if (std::is_same<typename Traits::Compare, std::greater<typename Traits::value_type>>::value) {
        tipoHeap = "Min-Heap";
    } else {
        tipoHeap = "Heap Custom";
    }
    
    os << "CHeap (" << tipoHeap << "): size = " << heap.m_data.size() << "\n[";
    
    for (size_t i = 0; i < heap.m_data.size(); ++i) {
        os << heap.m_data[i];
        
        if (i < heap.m_data.size() - 1) {
            os << ", ";
        }
    }
    
    os << "]\n";
    return os;
}

template <typename T>
using CMaxHeap = CHeap<MaxHeapTrait<T>>;

template <typename T>
using CMinHeap = CHeap<MinHeapTrait<T>>;

#endif // __HEAP_H__