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


#endif // __QUEUE_H__
