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

#endif // __STACK_H__
