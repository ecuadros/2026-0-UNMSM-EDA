#ifndef __STACK_H__
#define __STACK_H__
#include <iostream>
#include <mutex>
#include <assert.h>
#include <stdexcept>
#include <string>
#include "../general/types.h"

struct Registro {
    TT id; 
    std::string url;      
    friend std::ostream& operator<<(std::ostream& os, const Registro& r) {
        os << "[" << r.id << ":" << r.url << "]";
        return os;
    }

    friend std::istream& operator>>(std::istream& is, Registro& r) {
        is >> r.url;
        return is;
    }
};

template <typename _T>
struct StackTraits {
    using T = _T;
};

template <typename Traits>
class Stack {
public:
    using value_type = typename Traits::T;
private:
    value_type* m_data;
    T1 m_capacity;
    T1 m_top; 
    mutable std::mutex mtx;

public:
    Stack(T1 capacity = 10);
    Stack(const Stack<Traits>& other); 
    Stack(Stack<Traits>&& other) noexcept; 
    virtual ~Stack();

    void push(value_type value);
    void pop(); 

    // Regla de los 5: Asignaciones
    Stack<Traits>& operator=(const Stack<Traits>& other) {
        if (this != &other) {
            std::lock_guard<std::mutex> lock1(mtx);
            std::lock_guard<std::mutex> lock2(other.mtx);
            delete[] m_data; 
            m_capacity = other.m_capacity;
            m_top = other.m_top;
            m_data = new value_type[m_capacity];
            for (T1 i = 0; i <= m_top; ++i) m_data[i] = other.m_data[i];
        }
        return *this;
    }

    Stack<Traits>& operator=(Stack<Traits>&& other) noexcept {
        if (this != &other) {
            std::lock_guard<std::mutex> lock1(mtx);
            std::lock_guard<std::mutex> lock2(other.mtx);
            delete[] m_data; 
            m_data = other.m_data;
            m_capacity = other.m_capacity;
            m_top = other.m_top;
            other.m_data = nullptr;
            other.m_top = -1;
        }
        return *this;
    }

    void pop(value_type& outValue) {
        std::lock_guard<std::mutex> lock(mtx);
        if (m_top < 0) throw std::runtime_error("Stack empty");
        outValue = m_data[m_top];
        m_top--;
    }

    value_type& top() {
        if (m_top < 0) throw std::runtime_error("Stack empty");
        return m_data[m_top];
    }

    T1 getSize() const { return m_top + 1; }

    friend std::ostream& operator<<(std::ostream& os, const Stack<Traits>& s) {
        std::lock_guard<std::mutex> lock(s.mtx);
        os << "[";
        for (T1 i = 0; i <= s.m_top; ++i) {
            os << s.m_data[i] << (i == s.m_top ? "" : ", ");
        }
        os << "]";
        return os;
    }

    friend std::istream& operator>>(std::istream& is, Stack<Traits>& s) {
        value_type temp;
        if (is >> temp) { 
            s.push(temp);
        }
        return is;
    }
};

#include "Stack.cpp" 
#endif