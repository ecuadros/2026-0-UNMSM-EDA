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


#endif // __QUEUE_H__
