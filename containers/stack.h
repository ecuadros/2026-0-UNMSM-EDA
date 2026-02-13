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


#endif // __STACK_H__
