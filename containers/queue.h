#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <iostream>
#include <mutex>
#include <stdexcept>
#include <utility>  
#include "../general/types.h"




template <typename T>
class CQueue;

template <typename T>
std::ostream& operator<<(std::ostream& os, CQueue<T>& queue);

template <typename T>
std::istream& operator>>(std::istream& is, CQueue<T>& queue);


// NODO PARA QUEUE

template <typename T>
class NodeQueue {
    friend class CQueue<T>;
    
    template <typename U>
    friend std::ostream& operator<<(std::ostream& os, CQueue<U>& queue);
    
private:
    T m_data;
    ref_type m_ref;
    NodeQueue *m_pNext = nullptr;

public:
    NodeQueue() : m_ref(-1), m_pNext(nullptr) {}
    
    NodeQueue(T value, ref_type ref = -1) 
        : m_data(value), m_ref(ref), m_pNext(nullptr) {}
    
    NodeQueue(T value, ref_type ref, NodeQueue *pNext) 
        : m_data(value), m_ref(ref), m_pNext(pNext) {}
    
    T GetValue() const { return m_data; }
    ref_type GetRef() const { return m_ref; }
    NodeQueue *GetNext() const { return m_pNext; }
};


// CLASE QUEUE (FIFO - First In First Out)

template <typename T>
class CQueue {
    using Node = NodeQueue<T>;
    
private:
    Node *m_pFront = nullptr;  // Frente de la cola 
    Node *m_pRear = nullptr;   // Final de la cola 
    size_t m_nElements = 0;
    mutable std::mutex m_mutex;

public:
    // Constructor por defecto
    CQueue() = default;
    
    
    // Constructor Copia 
    
    CQueue(const CQueue &other) {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        
        
        if (!other.m_pFront) {
            m_pFront = nullptr;
            m_pRear = nullptr;
            m_nElements = 0;
            return;
        }
        
        
        m_pFront = new Node(other.m_pFront->m_data, other.m_pFront->m_ref);
        
        // punteros para recorrer y copiar el resto
        Node *pDest = m_pFront;              // Último nodo creado
        Node *pSrc  = other.m_pFront->m_pNext; // Siguiente a leer
        
        //  Bucle: Leer -> Crear -> Enlazar
        while (pSrc) {
            Node *pNew = new Node(pSrc->m_data, pSrc->m_ref);
            pDest->m_pNext = pNew;  // Enlazar
            pDest = pNew;           // Avanzar destino
            pSrc = pSrc->m_pNext;   // Avanzar origen
        }
        
        
        m_pRear = pDest;
        
        
        m_nElements = other.m_nElements;
    }
    
    
    // Move Constructor
    
    CQueue(CQueue &&other) noexcept {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        
        m_pFront = std::exchange(other.m_pFront, nullptr);
        m_pRear = std::exchange(other.m_pRear, nullptr);
        m_nElements = std::exchange(other.m_nElements, 0);
    }
    
    
    // Destructor seguro y virtual
    
    virtual ~CQueue() {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        Node *pCurrent = m_pFront;
        while (pCurrent) {
            Node *pNext = pCurrent->m_pNext;
            delete pCurrent;
            pCurrent = pNext;
        }
        
        m_pFront = nullptr;
        m_pRear = nullptr;
        m_nElements = 0;
    }
    
    // ----------------------------------------
    // Push: Agregar elemento al final
    // ----------------------------------------
    void push(const T &value, ref_type ref = -1) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        Node *pNew = new Node(value, ref);
        
        if (m_pRear == nullptr) {
            // Cola vacía
            m_pFront = pNew;
            m_pRear = pNew;
        } else {
            // Agregar al final
            m_pRear->m_pNext = pNew;
            m_pRear = pNew;
        }
        
        ++m_nElements;
    }
    
    // ----------------------------------------
    // Pop: Remover y retornar elemento del frente
    // ----------------------------------------
    T pop() {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (!m_pFront) {
            throw std::out_of_range("Queue is empty");
        }
        
        Node *pTemp = m_pFront;
        T value = pTemp->m_data;
        m_pFront = m_pFront->m_pNext;
        
        // Si la cola queda vacía, actualizar rear
        if (m_pFront == nullptr) {
            m_pRear = nullptr;
        }
        
        delete pTemp;
        --m_nElements;
        
        return value;
    }
    
    // ----------------------------------------
    // Front: Ver elemento del frente sin removerlo
    // ----------------------------------------
    T& front() {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (!m_pFront) {
            throw std::out_of_range("Queue is empty");
        }
        
        return m_pFront->m_data;
    }
    
    const T& front() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (!m_pFront) {
            throw std::out_of_range("Queue is empty");
        }
        
        return m_pFront->m_data;
    }
    
    // ----------------------------------------
    // Rear: Ver elemento del final sin removerlo
    // ----------------------------------------
    T& rear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (!m_pRear) {
            throw std::out_of_range("Queue is empty");
        }
        
        return m_pRear->m_data;
    }
    
    const T& rear() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (!m_pRear) {
            throw std::out_of_range("Queue is empty");
        }
        
        return m_pRear->m_data;
    }
    
    // ----------------------------------------
    // Utilidades
    // ----------------------------------------
    bool isEmpty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_pFront == nullptr;
    }
    
    size_t getSize() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_nElements;
    }
    
    // ----------------------------------------
    // Operadores friend
    // ----------------------------------------
    friend std::ostream& operator<< <T>(std::ostream& os, CQueue<T>& queue);
    friend std::istream& operator>> <T>(std::istream& is, CQueue<T>& queue);
};

// ============================================
// OPERADOR << (OUTPUT) - SIN MOSTRAR REF
// ============================================
template <typename T>
std::ostream& operator<<(std::ostream& os, CQueue<T>& queue) {
    std::lock_guard<std::mutex> lock(queue.m_mutex);
    
    os << "CQueue (FIFO): size = " << queue.m_nElements << std::endl;
    os << "FRONT -> [";
    
    auto pCurrent = queue.m_pFront;
    while (pCurrent) {
        os << pCurrent->m_data;  // Solo el valor, sin ref
        pCurrent = pCurrent->m_pNext;
        if (pCurrent) os << ", ";
    }
    
    os << "] <- REAR" << std::endl;
    return os;
}


// OPERADOR >> (INPUT)

template <typename T>
std::istream& operator>>(std::istream& is, CQueue<T>& queue) {
    T value;
    ref_type ref;
    
    if (is >> value >> ref) {
        queue.push(value, ref);
    }
    
    return is;
}

#endif // __QUEUE_H__
