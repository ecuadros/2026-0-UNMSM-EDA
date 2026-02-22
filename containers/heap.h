#ifndef __HEAP_H__
#define __HEAP_H__

#include <iostream>
#include <vector>
#include <mutex>
#include <stdexcept>
#include "../containers/traits.h"
#include "../general/types.h"

template <typename Traits>
class CHeap {
public:
    using value_type = typename Traits::value_type;
    using Compare    = typename Traits::Func;

    struct Element {
        value_type data;
        ref_type ref;
        Element(value_type d, ref_type r) : data(d), ref(r) {}
    };

private:
    std::vector<Element> m_data;
    mutable std::mutex m_mtx;
    using LOCK = std::lock_guard<std::mutex>;

    //metodo para flotar lo que se inserta, calcula el nodo padre y compara
    void heapifyUp(size_t index) {
        if (index == 0) return;
        size_t parent = (index - 1) / 2;

        if (Compare()(m_data[index].data, m_data[parent].data)) {
            std::swap(m_data[index], m_data[parent]);
            heapifyUp(parent);
        }
    }

    void heapifyDown(size_t index) {
        size_t leftChild    = 2 * index + 1;
        size_t rightChild   = 2 * index + 2;
        size_t target       = index;

        //left & right
        if (leftChild  < m_data.size() && Compare() (m_data[leftChild].data, m_data[target].data))  { target = leftChild; }
        if (rightChild < m_data.size() && Compare()(m_data[rightChild].data, m_data[target].data))  { target = rightChild; }

        //si el objetivo cambia, intercambiamos y seguimos hundiendo
        //se usa swap para no usar una variable temporal al intercambiar
        if (target != index) {
            std::swap(m_data[index], m_data[target]);
            heapifyDown(target);
        }
    }

public:
    CHeap() {}

    void push(const value_type& val, ref_type ref = -1) {
        LOCK lock(m_mtx);
        m_data.emplace_back(val, ref); //insertar al final del array dinamico (heap)
        heapifyUp(m_data.size() - 1);  //flotar el elemento
    }

    value_type pop() {
        LOCK lock(m_mtx);
        if (m_data.empty()) throw std::out_of_range("el heap esta vacio");
        
        value_type rootVal = m_data[0].data; //el min o max siempre está en el índice 0
        
        //intercambiar el ultimo por el primero
        m_data[0] = m_data.back();
        m_data.pop_back();
        
        //hundir el elemento raiz hasta donde corresponda
        if (!m_data.empty()) { heapifyDown(0); }
        return rootVal;
    }

    value_type top() const {
        LOCK lock(m_mtx);
        if (m_data.empty()) throw std::out_of_range("el heap esta vacio");
        return m_data[0].data; //min o max siempre estara en index 0
    }

    bool isEmpty() const {
        LOCK lock(m_mtx);
        return m_data.empty();
    }

    size_t size() const {
        LOCK lock(m_mtx);
        return m_data.size();
    }

    //imprimir el arreglo
    friend std::ostream& operator<<(std::ostream& os, CHeap<Traits>& heap) {
        LOCK lock(heap.m_mtx);
        os << "[ ";
        for (const auto& el : heap.m_data) {
            os << "(" << el.data << ":" << el.ref << ") ";
        }
        os << "]";
        return os;
    }

    //extraer un dato del heap
    bool remove(const value_type& val) {
        LOCK lock(m_mtx);
        if (m_data.empty()) return false;

        auto indexToRemove = -1;
        for (size_t i=0; i<m_data.size(); ++i) {
            if (m_data[i].data == val) { 
                indexToRemove = i;
                break;
            }
        }

        if (indexToRemove == -1) return false; 
        
        //si es hoja
        if (indexToRemove == static_cast<int>(m_data.size() - 1)) {
            m_data.pop_back();
            return true;
        }

        //si es rama, remplazar el q se va a borrar con el ultimo
        m_data[indexToRemove] = m_data.back();
        m_data.pop_back();

        //flotar o hundir
        size_t actual = indexToRemove;
        size_t parent  = (actual - 1) / 2;

        //si tiene padre y cumple la regla para subir, lo flotamos
        if (actual>0 && Compare()(m_data[actual].data, m_data[parent].data)) { heapifyUp(actual); } 
            else { heapifyDown(actual); }

        return true;
    }
};

#endif // __HEAP_H__