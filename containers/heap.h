#ifndef __HEAP_H__
#define __HEAP_H__

#include <mutex>
#include <iostream>
#include "../general/types.h"
#include "../util.h"

template <typename T>
class CHeap{
    private:
        T *m_data = nullptr;
        int size = 0;
        int capacity = 0;

        void heapifyUp(int index);
        void heapifyDown(int index);
        void resize();

        std::mutex mtx; // Mutex para operaciones concurrentes
        
    public:
        // Constructor normal
        CHeap(int cap = 10);

        // Constructor copia
        CHeap (const CHeap &other);

        // Destructor
        ~CHeap();

        //Move Constructor
        CHeap(CHeap &&other);

        //Push
        void push(const T &value);

        //Pop
        T pop();
        bool empty() const { return size == 0; };

        // Operador de acceso <<
        template <typename U>
        friend std::ostream &operator<<(std::ostream &os, const CHeap<U> &heap);

        // Operador de acceso >>
        template <typename U>
        friend CHeap<U> &operator>>(CHeap<U> &heap, const U &value);
    
};

// Constructor normal
template <typename T>
CHeap<T>::CHeap(int cap){
    capacity = cap;
    size = 0;
    m_data = new T[capacity];
}

// Constructor copia
template <typename T>
CHeap<T>::CHeap (const CHeap &other){
    size = other.size;
    capacity = other.capacity;

    m_data = new T[capacity];

    for (int i = 0; i < size; ++i){
        m_data[i] = other.m_data[i];
    }
}

// Destructor
template <typename T>
CHeap<T>::~CHeap(){
    delete[] m_data;
}

// Move Constructor
template <typename T>
CHeap<T>::CHeap(CHeap &&other){
    m_data = other.m_data;
    size = other.size;
    capacity = other.capacity;

    other.m_data = nullptr;
    other.size = 0;
    other.capacity = 0;
}

// Resize
template <typename T>
void CHeap<T>::resize(){
    capacity *= 2;
    T *newData = new T[capacity];

    for (int i = 0; i < size; ++i){
        newData[i] = m_data[i];
    }

    delete[] m_data;
    m_data = newData;
}

//Push
template <typename T>
void CHeap<T>::push(const T &value){
    std::lock_guard<std::mutex> lock(mtx); // Bloquea el mutex durante la operación
    if (size == capacity){
        resize();
    }

    m_data[size] = value;
    heapifyUp(size);
    size++;
}

// Heapify Up
template <typename T>
void CHeap<T>::heapifyUp(int index){
    while (index > 0){
        int parent = (index - 1) / 2;

        if (m_data[index] > m_data[parent]){
            std::swap(m_data[index], m_data[parent]);
            index = parent;
        } else {
            break;
        }
    }
}

//Pop
template <typename T>
T CHeap<T>::pop(){
    std::lock_guard<std::mutex> lock(mtx);
    if (empty()){
        throw std::out_of_range("Heap vacío");
    }

    T root = m_data[0];
    m_data[0] = m_data[size - 1];
    size--;

    heapifyDown(0);
    return root;
}

// Heapify Down
template <typename T>
void CHeap<T>::heapifyDown(int index){
    while (true){
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int largest = index;

        if (left < size && m_data[left] > m_data[largest]){
            largest = left;
        }

        if (right < size && m_data[right] > m_data[largest]){
            largest = right;
        }

        if (largest != index){
            std::swap(m_data[index], m_data[largest]);
            index = largest;
        } else {
            break;
        }
    }
}

// Operador de acceso <<
template <typename T>
std::ostream &operator<<(std::ostream &os, const CHeap<T> &heap){
    os << "Heap: [";

    for (int i = 0; i < heap.size; ++i){
        os << heap.m_data[i] << " ";
    }

    os << "]";
    return os;
}

// Operador de acceso >>
template <typename T>
CHeap<T> &operator>>(CHeap<T> &heap, const T &value){
    heap.push(value);
    return heap;
}

#endif // __HEAP_H__