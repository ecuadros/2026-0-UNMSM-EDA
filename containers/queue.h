#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <iostream>
#include "../general/types.h"
#include "../util.h"

template <typename T>
class CQueue{
    private: 
        CLinkedList<AscendingTrait<T>> m_list;
        std::mutex m_mutex; // Mutex para proteger el acceso a la cola
    public:
        CQueue() = default;
        CQueue(const CQueue &other);
        CQueue(CQueue &&other) noexcept;
        ~CQueue() = default;

        void push(const T& value);
        void pop();

        T front() const;

        bool operator<(const CQueue &other) const;
        bool operator>(const CQueue &other) const;
        
};

// Implementación de los métodos de CQueue
// El método push agrega un nuevo elemento al final de la lista, que es el final de la cola
template <typename T>
void CQueue<T>::push(const T& value){
    std::lock_guard<std::mutex> lock(m_mutex); // Bloquea el mutex durante la operación
    m_list.push_back(value, -1); // Agrega al final de la lista

}

// El método pop elimina el primer elemento de la lista, que es el frente de la cola
template <typename T>
void CQueue<T>::pop(){
    std::lock_guard<std::mutex> lock(m_mutex);
    if(m_list.getSize() > 0) {
        m_list.pop_front(); // Elimina el primer elemento de la lista
    }
}

// Constructor copia
template <typename T>
CQueue<T>::CQueue(const CQueue &other) : m_list(other.m_list){}

//constructor move
template <typename T>
CQueue<T>::CQueue(CQueue &&other) noexcept : m_list(std::move(other.m_list)) {}

// Operador <
template <typename T>
bool CQueue<T>::operator<(const CQueue &other) const {
    return m_list.getSize() < other.m_list.getSize(); // Compara por tamaño
}

// Operador >
template <typename T>
bool CQueue<T>::operator>(const CQueue &other) const {
    return m_list.getSize() > other.m_list.getSize(); // Compara por tamaño
}

// El método front retorna el primer elemento de la lista, que es el frente de la cola
template <typename T>
T CQueue<T>::front() const {
    return m_list.get_front(); // Retorna el primer elemento de la lista, que es el frente de la cola
}


#endif // __QUEUE_H__