#ifndef __STACK_H__
#define __STACK_H__

#include <iostream>
#include <mutex>
#include "../general/types.h"
#include "../util.h"
using namespace std;

// Nodo para el Stack
template <typename Traits>
class NodeStack{
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeStack<Traits>;
private:
    value_type m_data;
    ref_type   m_ref;
    Node      *m_pNext = nullptr;
public:
    NodeStack(){}
    NodeStack(value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref){}
    NodeStack(value_type _value, ref_type _ref, Node* next)
        : m_data(_value), m_ref(_ref), m_pNext(next){}
    
    value_type  GetValue() const { return m_data; }
    value_type &GetValueRef() { return m_data; }
    ref_type    GetRef() const { return m_ref; }
    Node       *GetNext() const { return m_pNext; }
    
    void SetNext(Node* next) { m_pNext = next; }
};

// CLASE STACK (LIFO - Last In First Out)
template <typename Traits>
class CStack {
public:
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeStack<Traits>;
    
private:
    Node   *m_pTop = nullptr;
    size_t  m_nElements = 0;
    std::mutex m_mutex;
    
public:
    CStack(){}
    
    // Constructor copia
    CStack(const CStack &other) {
        if(other.m_pTop == nullptr) return;
        
        Node* tmp = other.m_pTop;
        Node* prev = nullptr;
        Node* newNode = nullptr;
        
        while(tmp != nullptr){
            newNode = new Node(tmp->GetValue(), tmp->GetRef());
            if(prev == nullptr) {
                m_pTop = newNode;
            } else {
                prev->SetNext(newNode);
            }
            prev = newNode;
            tmp = tmp->GetNext();
        }
        m_nElements = other.m_nElements;
    }
    
    // Move Constructor
    CStack(CStack &&other) noexcept {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        m_pTop = other.m_pTop;
        m_nElements = other.m_nElements;
        
        other.m_pTop = nullptr;
        other.m_nElements = 0;
    }
    
    // Destructor seguro
    virtual ~CStack() { 
        clear(); 
    }
    
    // Limpiar la pila
    void clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        while (m_pTop != nullptr) {
            Node* temp = m_pTop;
            m_pTop = m_pTop->GetNext();
            delete temp;
        }
        m_nElements = 0;
    }
    
    // Push - agregar al tope
    void push(value_type val, ref_type ref = -1) {
        std::lock_guard<std::mutex> lock(m_mutex);
        Node* pNew = new Node(val, ref, m_pTop);
        m_pTop = pNew;
        ++m_nElements;
    }
    
    // Pop - sacar del tope
    value_type pop() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if(m_pTop == nullptr) {
            throw std::runtime_error("Stack vacio");
        }
        
        Node* temp = m_pTop;
        value_type val = temp->GetValue();
        m_pTop = m_pTop->GetNext();
        delete temp;
        --m_nElements;
        
        return val;
    }
    
    // Ver el tope sin sacarlo
    value_type top() const {
        if(m_pTop == nullptr) {
            throw std::runtime_error("Stack vacio");
        }
        return m_pTop->GetValue();
    }
    
    // Verificar si esta vacio
    bool isEmpty() const {
        return m_pTop == nullptr;
    }
    
    // Obtener tamaño
    size_t getSize() const { 
        return m_nElements;  
    }
    
    // Operator<< para imprimir
    friend ostream &operator<<(ostream &os, const CStack<Traits> &stack){
        os << "CStack (LIFO): size = " << stack.getSize() << endl;
        os << "TOP -> [";
        
        Node* tmp = stack.m_pTop;
        while(tmp != nullptr){
            os << tmp->GetValue();
            if(tmp->GetNext() != nullptr) os << " -> ";
            tmp = tmp->GetNext();
        }
        os << "]" << endl;
        return os;
    }
    
    // Operator>> para leer desde input
    friend istream &operator>>(istream &is, CStack<Traits> &stack){
        value_type val;
        is >> val;
        if(is.good()){
            stack.push(val);
        }
        return is;
    }
};

#endif // __STACK_H__