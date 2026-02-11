#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>
#include "../general/types.h"

// Estructura con el apartado 'referencia' solicitado
struct Registro {
    ref_type id;      // Usando ref_type de tu types.h
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
struct QueueTraits {
    using T = _T;
};

template <typename Traits>
class Queue {
public:
    using value_type = typename Traits::T;
private:
    value_type* m_data;
    T1 m_capacity;
    T1 m_front;
    T1 m_back;
    T1 m_size;
    mutable std::mutex mtx;

public:
    Queue(T1 capacity = 10);
    virtual ~Queue();

    void enqueue(value_type value);
    void dequeue(value_type& outValue);
    
    T1 getSize() const { return m_size; }
    bool isEmpty() const { return m_size == 0; }

    friend std::ostream& operator<<(std::ostream& os, const Queue<Traits>& q) {
        std::lock_guard<std::mutex> lock(q.mtx);
        os << "FRENTE < ";
        for (T1 i = 0; i < q.m_size; ++i) {
            T1 index = (q.m_front + i) % q.m_capacity;
            os << q.m_data[index] << (i == q.m_size - 1 ? "" : " | ");
        }
        os << " < FINAL";
        return os;
    }
};

#include "../queue.cpp"
#endif // __QUEUE_H__