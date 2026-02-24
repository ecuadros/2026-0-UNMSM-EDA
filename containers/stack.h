#ifndef __STACK_H__
#define __STACK_H__

#include <iostream>
#include <mutex>
#include <stdexcept>
#include <utility>  
#include "../general/types.h"




template <typename T>
class CStack;

template <typename T>
std::ostream& operator<<(std::ostream& os, CStack<T>& stack);

template <typename T>
std::istream& operator>>(std::istream& is, CStack<T>& stack);


// NODO PARA STACK

template <typename T>
class NodeStack {
    friend class CStack<T>;
    
    template <typename U>
    friend std::ostream& operator<<(std::ostream& os, CStack<U>& stack);
    
private:
    T m_data;
    ref_type m_ref;
    NodeStack *m_pNext = nullptr;

public:
    NodeStack() : m_ref(-1), m_pNext(nullptr) {}
    
    NodeStack(T value, ref_type ref = -1) 
        : m_data(value), m_ref(ref), m_pNext(nullptr) {}
    
    NodeStack(T value, ref_type ref, NodeStack *pNext) 
        : m_data(value), m_ref(ref), m_pNext(pNext) {}
    
    T GetValue() const { return m_data; }
    ref_type GetRef() const { return m_ref; }
    NodeStack *GetNext() const { return m_pNext; }
};


// CLASE STACK (LIFO - Last In First Out)

template <typename T>
class CStack {
    using Node = NodeStack<T>;
    
private:
    Node *m_pTop = nullptr;
    size_t m_nElements = 0;
    mutable std::mutex m_mutex;

public:
    // Constructor por defecto
    CStack() = default;
    
    
    // Constructor Copia 
    
    CStack(const CStack &other) {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        
        
        if (!other.m_pTop) {
            m_pTop = nullptr;
            m_nElements = 0;
            return;
        }
        
        
        m_pTop = new Node(other.m_pTop->m_data, other.m_pTop->m_ref);
        
        
        Node *pDest = m_pTop;              
        Node *pSrc  = other.m_pTop->m_pNext; 
        
        // Bucle: Leer -> Crear -> Enlazar
        while (pSrc) {
            Node *pNew = new Node(pSrc->m_data, pSrc->m_ref);
            pDest->m_pNext = pNew;  // Enlazar
            pDest = pNew;           // Avanzar destino
            pSrc = pSrc->m_pNext;   // Avanzar origen
        }
        
        // Copiar el tamaño
        m_nElements = other.m_nElements;
    }
    
    
    // Move Constructor
    
    CStack(CStack &&other) noexcept {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        
        m_pTop = std::exchange(other.m_pTop, nullptr);
        m_nElements = std::exchange(other.m_nElements, 0);
    }
    
    
    // Destructor seguro y virtual
    
    virtual ~CStack() {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        Node *pCurrent = m_pTop;
        while (pCurrent) {
            Node *pNext = pCurrent->m_pNext;
            delete pCurrent;
            pCurrent = pNext;
        }
        
        m_pTop = nullptr;
        m_nElements = 0;
    }
    
    
    // Push: Agregar elemento al tope
    
    void push(const T &value, ref_type ref = -1) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        Node *pNew = new Node(value, ref, m_pTop);
        m_pTop = pNew;
        ++m_nElements;
    }
    
    
    // Pop: Remover y retornar elemento del tope
    
    T pop() {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (!m_pTop) {
            throw std::out_of_range("Stack is empty");
        }
        
        Node *pTemp = m_pTop;
        T value = pTemp->m_data;
        m_pTop = m_pTop->m_pNext;
        delete pTemp;
        --m_nElements;
        
        return value;
    }
    
    
    // Top: Ver elemento del tope sin removerlo
    
    T& top() {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (!m_pTop) {
            throw std::out_of_range("Stack is empty");
        }
        
        return m_pTop->m_data;
    }
    
    const T& top() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (!m_pTop) {
            throw std::out_of_range("Stack is empty");
        }
        
        return m_pTop->m_data;
    }
    
    
    
    bool isEmpty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_pTop == nullptr;
    }
    
    size_t getSize() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_nElements;
    }
    
    
    // Operadores friend
    
    friend std::ostream& operator<< <T>(std::ostream& os, CStack<T>& stack);
    friend std::istream& operator>> <T>(std::istream& is, CStack<T>& stack);
};


// OPERADOR << 

template <typename T>
std::ostream& operator<<(std::ostream& os, CStack<T>& stack) {
    std::lock_guard<std::mutex> lock(stack.m_mutex);
    
    os << "CStack (LIFO): size = " << stack.m_nElements << std::endl;
    os << "TOP -> [";
    
    auto pCurrent = stack.m_pTop;
    while (pCurrent) {
        os << pCurrent->m_data;  
        pCurrent = pCurrent->m_pNext;
        if (pCurrent) os << ", ";
    }
    
    os << "]" << std::endl;
    return os;
}


// OPERADOR >> 

template <typename T>
std::istream& operator>>(std::istream& is, CStack<T>& stack) {
    T value;
    ref_type ref;
    
    if (is >> value >> ref) {
        stack.push(value, ref);
    }
    
    return is;
}

#endif // __STACK_H__
