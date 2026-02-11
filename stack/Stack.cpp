#include "Stack.h"

template <typename Traits>
Stack<Traits>::Stack(T1 capacity) : m_capacity(capacity), m_top(-1) {
    m_data = new value_type[m_capacity];
}

template <typename Traits>
Stack<Traits>::Stack(const Stack<Traits>& other) : m_capacity(other.m_capacity), m_top(other.m_top) {
    m_data = new value_type[m_capacity];
    for (T1 i = 0; i <= m_top; ++i) m_data[i] = other.m_data[i];
}

template <typename Traits>
Stack<Traits>::Stack(Stack<Traits>&& other) noexcept 
    : m_data(other.m_data), m_capacity(other.m_capacity), m_top(other.m_top) {
    other.m_data = nullptr;
    other.m_top = -1;
}

template <typename Traits>
Stack<Traits>::~Stack() {
    delete[] m_data; 
}

template <typename Traits>
void Stack<Traits>::push(value_type value) {
    std::lock_guard<std::mutex> lock(mtx);
    if (m_top + 1 >= m_capacity) throw std::runtime_error("Stack overflow");
    m_data[++m_top] = value;
}

template <typename Traits>
void Stack<Traits>::pop() {
    std::lock_guard<std::mutex> lock(mtx);
    if (m_top < 0) throw std::runtime_error("Stack empty");
    m_top--;
}