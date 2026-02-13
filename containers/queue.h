#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <iostream>
#include <mutex>
#include <cassert>
#include <utility>


template <typename T, typename _Func>
struct QueueTrait{
    using value_type = T;
    using Func       = _Func;
};

/* Se implemetara una cola circular aprovechando :
    -Rendimiento constante O(1)
    -Uso eficiente de memoria
    -Mejor uso de caché(memoria contigua)
   Además es el modelo natural de “buffer rotatorio” */


template <typename Traits>
class CircularQueue{

public:
    using value_type = typename Traits::value_type;

private:
    value_type*      m_pData    = nullptr;
    size_t           m_capacity = 0;
    size_t           m_size     = 0;
    size_t           m_front    = 0;
    size_t           m_tail_v   = 0;
    mutable std::mutex m_mutex;

public:
    explicit CircularQueue(size_t capacity)
        : m_capacity(capacity)
    {
        assert(capacity > 0);
        m_pData = new value_type[m_capacity];
    }

    // Copy Constructor
  
    CircularQueue(const CircularQueue& another){
        std::lock_guard<std::mutex> lock(another.m_mutex);

        m_capacity = another.m_capacity;
        m_size     = another.m_size;
        m_front    = another.m_front;
        m_tail_v   = another.m_tail_v;

        m_pData = new value_type[m_capacity];

        for (size_t i = 0; i < m_capacity; ++i)
            m_pData[i] = another.m_pData[i];
    }

    // Move Constructor
    CircularQueue(CircularQueue&& another) noexcept{
        std::lock_guard<std::mutex> lock(another.m_mutex);

        m_pData    = std::exchange(another.m_pData, nullptr);
        m_capacity = std::exchange(another.m_capacity, 0);
        m_size     = std::exchange(another.m_size, 0);
        m_front    = std::exchange(another.m_front, 0);
        m_tail_v   = std::exchange(another.m_tail_v, 0);
    }

    // Destructor
    ~CircularQueue(){
        delete[] m_pData;
    }

    // Push sin rezize

    void push(const value_type& val){
        std::lock_guard<std::mutex> lock(m_mutex);

        assert(m_size < m_capacity);

        m_pData[m_tail_v] = val;
        m_tail_v = (m_tail_v + 1) % m_capacity;
        ++m_size;

    }

    // Pop sin resize
 
    value_type pop(){
        std::lock_guard<std::mutex> lock(m_mutex);

        assert(m_size > 0);

        value_type temp = std::move(m_pData[m_front]);  // mover en vez de copiar
        m_front = (m_front + 1) % m_capacity;
        --m_size;

        return temp;
    }

    // Front

    value_type& front(){
        std::lock_guard<std::mutex> lock(m_mutex);
        assert(m_size > 0);
        return m_pData[m_front];
    }

    const value_type& front() const{
        std::lock_guard<std::mutex> lock(m_mutex);
        assert(m_size > 0);
        return m_pData[m_front];
    }

    // Estado

    bool empty() const{
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_size == 0;
    }

    bool full() const{
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_size == m_capacity;
    }

    size_t getSize() const{
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_size;
    }

    size_t capacity() const{
        return m_capacity;
    }

    void clear(){
        std::lock_guard<std::mutex> lock(m_mutex);
        m_size    = 0;
        m_front   = 0;
        m_tail_v  = 0;
    }

    //  Operator <<

    friend std::ostream& operator<<(std::ostream& os, const CircularQueue& queue){
        std::lock_guard<std::mutex> lock(queue.m_mutex);

        os << "CircularQueue: size = "
           << queue.m_size << " -- capacity = "
           << queue.m_capacity << "\n";

        os << "Front -> ";

        for (size_t i = 0; i < queue.m_size; ++i){
            os << queue.m_pData[(queue.m_front + i) % queue.m_capacity];
            if (i + 1 < queue.m_size)
                os << " -> ";
        }

        return os;
    }
    // Operator >>
    
    friend std::istream& operator>>(std::istream& is, CircularQueue& queue){

        queue.clear();

        std::cout << "Capacidad de la cola: "
                << queue.m_capacity << "\n";

        std::cout << "¿Cuántos elementos desea añadir? ";
        size_t count;
        is >> count;

        if (count > queue.m_capacity){
            std::cout << "La cantidad excede la capacidad.\n";
            std::cout << "Solo se insertarán "
                    << queue.m_capacity << " elementos.\n";
            count = queue.m_capacity;
        }

        for (size_t i = 0; i < count; ++i){
            value_type val;

            std::cout << "Elemento " << (i + 1) << ": ";
            is >> val;

            queue.push(val);
        }

        return is;
    }

};

void DemoCircularQueue();


#endif // __QUEUE_H__
