#ifndef __STACK_H__
#define __STACK_H__
#include <iostream>
#include <mutex>
#include <stdexcept>
#include "../general/types.h"
#include "../util.h"
using namespace std;

// Stack (LIFO)
template <typename T>
class CStack {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& val) : data(val), next(nullptr) {}
    };

    Node* m_pTop;
    size_t m_nElements;
    mutable std::mutex m_mutex; // Concurrencia

public:
    // Constructor
    CStack() : m_pTop(nullptr), m_nElements(0) {}

    // Constructor copia
    CStack(const CStack& other) : m_pTop(nullptr), m_nElements(0) {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        
        if (!other.m_pTop) return;

        Node* temp[1000];
        size_t count = 0;
        Node* curr = other.m_pTop;
        
        while (curr && count < 1000) {
            temp[count++] = curr;
            curr = curr->next;
        }
        
        for (int i = count - 1; i >= 0; --i) {
            push(temp[i]->data);
        }
    }

    // Move Constructor
    CStack(CStack&& other) noexcept
        : m_pTop(other.m_pTop), m_nElements(other.m_nElements) {
        other.m_pTop = nullptr;
        other.m_nElements = 0;
    }

    // Destructor seguro
    virtual ~CStack() {
        clear();
    }

    CStack& operator=(const CStack& other) {
        if (this != &other) {
            std::lock(m_mutex, other.m_mutex);
            std::lock_guard<std::mutex> lock1(m_mutex, std::adopt_lock);
            std::lock_guard<std::mutex> lock2(other.m_mutex, std::adopt_lock);
            
            clear();
            
            if (!other.m_pTop) return *this;

            Node* temp[1000];
            size_t count = 0;
            Node* curr = other.m_pTop;
            
            while (curr && count < 1000) {
                temp[count++] = curr;
                curr = curr->next;
            }
            
            for (int i = count - 1; i >= 0; --i) {
                push(temp[i]->data);
            }
        }
        return *this;
    }

    CStack& operator=(CStack&& other) noexcept {
        if (this != &other) {
            clear();
            m_pTop = other.m_pTop;
            m_nElements = other.m_nElements;
            
            other.m_pTop = nullptr;
            other.m_nElements = 0;
        }
        return *this;
    }

    // Push
    void push(const T& val) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        Node* pNew = new Node(val);
        pNew->next = m_pTop;
        m_pTop = pNew;
        ++m_nElements;
    }

    // Pop (retorna valor)
    T pop() {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (!m_pTop) {
            throw std::runtime_error("Stack vacío");
        }
        
        Node* temp = m_pTop;
        T val = temp->data;
        m_pTop = m_pTop->next;
        delete temp;
        --m_nElements;
        
        return val;
    }

    T top() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (!m_pTop) {
            throw std::runtime_error("Stack vacío");
        }
        
        return m_pTop->data;
    }

    bool isEmpty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_pTop == nullptr;
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_nElements;
    }

    void clear() {
        Node* curr = m_pTop;
        while (curr) {
            Node* next = curr->next;
            delete curr;
            curr = next;
        }
        m_pTop = nullptr;
        m_nElements = 0;
    }

    // Operator <<
    friend ostream& operator<<(ostream& os, CStack<T>& stack) {
        std::lock_guard<std::mutex> lock(stack.m_mutex);
        
        os << "CStack: size = " << stack.m_nElements << endl;
        os << "[TOP -> ";
        
        Node* curr = stack.m_pTop;
        while (curr) {
            os << curr->data;
            if (curr->next) os << ", ";
            curr = curr->next;
        }
        
        os << "]" << endl;
        return os;
    }

    // Operator >>
    friend istream& operator>>(istream& is, CStack<T>& stack) {
        std::lock_guard<std::mutex> lock(stack.m_mutex);
        
        stack.clear();
        
        size_t n;
        is >> n;
        
        for (size_t i = 0; i < n; ++i) {
            T val;
            is >> val;
            stack.push(val);
        }
        
        return is;
    }
};

#endif // __STACK_H__
