#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <iostream>
#include <mutex>
#include "../general/types.h"
#include "../util.h"
using namespace std;

// Nodo para la Queue
template <typename Traits>
class NodeQueue{
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeQueue<Traits>;
private:
    value_type m_data;
    ref_type   m_ref;
    Node      *m_pNext = nullptr;
public:
    NodeQueue(){}
    NodeQueue(value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref){}
    
    value_type  GetValue() const { return m_data; }
    value_type &GetValueRef() { return m_data; }
    ref_type    GetRef() const { return m_ref; }
    Node       *GetNext() const { return m_pNext; }
    
    void SetNext(Node* next) { m_pNext = next; }
};

// CLASE QUEUE 
template <typename Traits>
class CQueue {
public:
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeQueue<Traits>;
    
private:
    Node   *m_pFront = nullptr;
    Node   *m_pRear = nullptr;
    size_t  m_nElements = 0;
    std::mutex m_mutex;
    
public:
    CQueue(){}
    
    // Constructor copia
    CQueue(const CQueue &other) {
        Node* tmp = other.m_pFront;
        while(tmp != nullptr){
            this->push(tmp->GetValue(), tmp->GetRef());
            tmp = tmp->GetNext();
        }
    }
    
    // Move Constructor
    CQueue(CQueue &&other) noexcept {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        m_pFront = other.m_pFront;
        m_pRear = other.m_pRear;
        m_nElements = other.m_nElements;
        
        other.m_pFront = nullptr;
        other.m_pRear = nullptr;
        other.m_nElements = 0;
    }
    
    // Destructor seguro
    virtual ~CQueue() { 
        clear(); 
    }
    
    // Limpiar la cola
    void clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        while (m_pFront != nullptr) {
            Node* temp = m_pFront;
            m_pFront = m_pFront->GetNext();
            delete temp;
        }
        m_pRear = nullptr;
        m_nElements = 0;
    }
    
    // Push - agregar al final
    void push(value_type val, ref_type ref = -1) {
        std::lock_guard<std::mutex> lock(m_mutex);
        Node* pNew = new Node(val, ref);
        
        if(m_pRear == nullptr) {
            m_pFront = pNew;
            m_pRear = pNew;
        } else {
            m_pRear->SetNext(pNew);
            m_pRear = pNew;
        }
        ++m_nElements;
    }
    
    // Pop - sacar del frente
    value_type pop() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if(m_pFront == nullptr) {
            throw std::runtime_error("Queue vacia");
        }
        
        Node* temp = m_pFront;
        value_type val = temp->GetValue();
        m_pFront = m_pFront->GetNext();
        
        if(m_pFront == nullptr) {
            m_pRear = nullptr;
        }
        
        delete temp;
        --m_nElements;
        
        return val;
    }
    
    // Ver el frente sin sacarlo
    value_type front() const {
        if(m_pFront == nullptr) {
            throw std::runtime_error("Queue vacia");
        }
        return m_pFront->GetValue();
    }
    
    // Ver el final sin sacarlo
    value_type back() const {
        if(m_pRear == nullptr) {
            throw std::runtime_error("Queue vacia");
        }
        return m_pRear->GetValue();
    }
    
    // Verificar si esta vacia
    bool isEmpty() const {
        return m_pFront == nullptr;
    }
    
    // Obtener tamaño
    size_t getSize() const { 
        return m_nElements;  
    }
    
    // Operator<< para imprimir
    friend ostream &operator<<(ostream &os, const CQueue<Traits> &queue){
        os << "CQueue (FIFO): size = " << queue.getSize() << endl;
        os << "FRONT -> [";
        
        Node* tmp = queue.m_pFront;
        while(tmp != nullptr){
            os << tmp->GetValue();
            if(tmp->GetNext() != nullptr) os << " -> ";
            tmp = tmp->GetNext();
        }
        os << "] <- REAR" << endl;
        return os;
    }
    
    // Operator>> para leer desde input
    friend istream &operator>>(istream &is, CQueue<Traits> &queue){
        value_type val;
        is >> val;
        if(is.good()){
            queue.push(val);
        }
        return is;
    }
};

#endif // __QUEUE_H__