#include "containers/queue.h"

template <typename Traits>
Queue<Traits>::Queue(T1 capacity) 
    : m_capacity(capacity), m_front(0), m_back(0), m_size(0) {
    m_data = new value_type[m_capacity];
}

template <typename Traits>
Queue<Traits>::~Queue() {
    delete[] m_data;
}

template <typename Traits>
void Queue<Traits>::enqueue(value_type value) {
    std::lock_guard<std::mutex> lock(mtx);
    if (m_size == m_capacity) throw std::runtime_error("Queue overflow");
    
    m_data[m_back] = value;
    m_back = (m_back + 1) % m_capacity;
    m_size++;
}

template <typename Traits>
void Queue<Traits>::dequeue(value_type& outValue) {
    std::lock_guard<std::mutex> lock(mtx);
    if (m_size == 0) throw std::runtime_error("Queue empty");
    
    outValue = m_data[m_front];
    m_front = (m_front + 1) % m_capacity;
    m_size--;
}