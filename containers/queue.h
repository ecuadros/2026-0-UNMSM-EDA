#ifndef __QUEUE_H__
#define __QUEUE_H__
#include <iostream>
#include <mutex>
#include <stdexcept>
#include "../general/types.h"
#include "../util.h"
using namespace std;

// Queue (FIFO)
template <typename T>
class CQueue {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& val) : data(val), next(nullptr) {}
    };

    Node* m_pFront;
    Node* m_pRear;
    size_t m_nElements;
    mutable std::mutex m_mutex; // Concurrencia

public:
    // Constructor
    CQueue() : m_pFront(nullptr), m_pRear(nullptr), m_nElements(0) {}

    // Constructor copia
    CQueue(const CQueue& other) : m_pFront(nullptr), m_pRear(nullptr), m_nElements(0) {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        
        Node* curr = other.m_pFront;
        while (curr) {
            push(curr->data);
            curr = curr->next;
        }
    }

    // Move Constructor
    CQueue(CQueue&& other) noexcept
        : m_pFront(other.m_pFront), m_pRear(other.m_pRear), m_nElements(other.m_nElements) {
        other.m_pFront = nullptr;
        other.m_pRear = nullptr;
        other.m_nElements = 0;
    }

    // Destructor seguro
    virtual ~CQueue() {
        clear();
    }

    CQueue& operator=(const CQueue& other) {
        if (this != &other) {
            std::lock(m_mutex, other.m_mutex);
            std::lock_guard<std::mutex> lock1(m_mutex, std::adopt_lock);
            std::lock_guard<std::mutex> lock2(other.m_mutex, std::adopt_lock);
            
            clear();
            
            Node* curr = other.m_pFront;
            while (curr) {
                push(curr->data);
                curr = curr->next;
            }
        }
        return *this;
    }

    CQueue& operator=(CQueue&& other) noexcept {
        if (this != &other) {
            clear();
            m_pFront = other.m_pFront;
            m_pRear = other.m_pRear;
            m_nElements = other.m_nElements;
            
            other.m_pFront = nullptr;
            other.m_pRear = nullptr;
            other.m_nElements = 0;
        }
        return *this;
    }

    // Push
    void push(const T& val) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        Node* pNew = new Node(val);
        
        if (!m_pRear) {
            m_pFront = m_pRear = pNew;
        } else {
            m_pRear->next = pNew;
            m_pRear = pNew;
        }
        
        ++m_nElements;
    }

    // Pop (retorna valor)
    T pop() {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (!m_pFront) {
            throw std::runtime_error("Queue vacío");
        }
        
        Node* temp = m_pFront;
        T val = temp->data;
        m_pFront = m_pFront->next;
        
        if (!m_pFront) {
            m_pRear = nullptr;
        }
        
        delete temp;
        --m_nElements;
        
        return val;
    }

    T front() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (!m_pFront) {
            throw std::runtime_error("Queue vacío");
        }
        
        return m_pFront->data;
    }

    bool isEmpty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_pFront == nullptr;
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_nElements;
    }

    void clear() {
        Node* curr = m_pFront;
        while (curr) {
            Node* next = curr->next;
            delete curr;
            curr = next;
        }
        m_pFront = nullptr;
        m_pRear = nullptr;
        m_nElements = 0;
    }



#endif // __QUEUE_H__
