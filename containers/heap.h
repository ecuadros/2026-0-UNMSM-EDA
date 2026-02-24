#ifndef __HEAP_H__
#define __HEAP_H__
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <functional>
#include "../general/types.h"
#include "../util.h"
using namespace std;

template <typename T, typename _Comp>
struct HeapTrait {
    using value_type = T;
    using Comp       = _Comp;
};

// MaxHeap
template <typename T>
struct MaxHeapTrait : public HeapTrait<T, std::less<T> > {};

// MinHeap
template <typename T>
struct MinHeapTrait : public HeapTrait<T, std::greater<T> > {};

template <typename Traits>
class CHeap {

    using value_type = typename Traits::value_type;
    using Comp       = typename Traits::Comp;

    struct Node {
        value_type m_value;
        ref_type   m_ref;

        Node() {}
        Node(value_type _value, ref_type _ref = -1)
            : m_value(_value), m_ref(_ref) {}

        value_type  GetValue   () const { return m_value; }
        value_type &GetValueRef()       { return m_value; }
        ref_type    GetRef     () const { return m_ref;   }
        ref_type   &GetRefRef  ()       { return m_ref;   }

        Node &operator=(const Node &another) {
            m_value = another.GetValue();
            m_ref   = another.GetRef();
            return *this;
        }
    };

    Node  *m_data      = nullptr;
    size_t m_capacity  = 0;
    size_t m_nElements = 0;
    Comp   m_comp;  // Comparador (less para MaxHeap, greater para MinHeap)
    mutable mutex m_mutex;

    size_t parent(size_t i) const { return i / 2; }
    size_t leftChild(size_t i)  const { return 2 * i; }
    size_t rightChild(size_t i) const { return 2 * i + 1; }

    // Reordena hacia arriba después de insertar
    void heapifyUp(size_t i);

    // Reordena hacia abajo después de extraer la raíz
    void heapifyDown(size_t i);

    void resize(size_t newCapacity);

public:
    CHeap(size_t initialCapacity = 10);

    // Constructor copia
    CHeap(const CHeap<Traits> &another);

    // Move Constructor
    CHeap(CHeap<Traits> &&another) noexcept;

    // Destructor seguro
    ~CHeap();

    void       push(const value_type &val, ref_type ref = -1);
    value_type pop();

    size_t getSize() const { return m_nElements; }

    friend ostream &operator<<(ostream &os, CHeap<Traits> &container) {
        lock_guard<mutex> lock(container.m_mutex);
        os << "CHeap: size = " << container.m_nElements << endl;
        os << "[";

        for (size_t i = 1; i <= container.m_nElements; ++i)
            os << "(" << container.m_data[i].GetValue() << ":" 
               << container.m_data[i].GetRef() << "),";
        os << "]" << endl;
        return os;
    }

    friend istream &operator>>(istream &is, CHeap<Traits> &container) {
        value_type val;
        ref_type   ref;
        is >> val >> ref;
        container.push(val, ref);
        return is;
    }
};

// Constructor
template <typename Traits>
CHeap<Traits>::CHeap(size_t initialCapacity) 
    : m_capacity(initialCapacity + 1) {
    m_data = new Node[m_capacity];
    m_nElements = 0;
}

// Constructor copia
template <typename Traits>
CHeap<Traits>::CHeap(const CHeap<Traits> &another) {
    lock_guard<mutex> lock(another.m_mutex);
    m_capacity  = another.m_capacity;
    m_nElements = another.m_nElements;
    m_data      = new Node[m_capacity];

    for (size_t i = 1; i <= m_nElements; ++i)
        m_data[i] = another.m_data[i];
}

// Move Constructor
template <typename Traits>
CHeap<Traits>::CHeap(CHeap<Traits> &&another) noexcept
    : m_data(another.m_data), m_capacity(another.m_capacity),
      m_nElements(another.m_nElements) {
    another.m_data      = nullptr;
    another.m_capacity  = 0;
    another.m_nElements = 0;
}

// Destructor seguro
template <typename Traits>
CHeap<Traits>::~CHeap() {
    delete[] m_data;
}

template <typename Traits>
void CHeap<Traits>::resize(size_t newCapacity) {
    Node *newData = new Node[newCapacity];
    for (size_t i = 1; i <= m_nElements; ++i)
        newData[i] = m_data[i];
    delete[] m_data;
    m_data     = newData;
    m_capacity = newCapacity;
}

template <typename Traits>
void CHeap<Traits>::heapifyUp(size_t i) {

    while (i > 1 && m_comp(m_data[parent(i)].GetValue(), m_data[i].GetValue())) {
        intercambiar(m_data[i], m_data[parent(i)]);
        i = parent(i);
    }
}

template <typename Traits>
void CHeap<Traits>::heapifyDown(size_t i) {
    size_t extremeIndex = i;
    size_t left  = leftChild(i);
    size_t right = rightChild(i);

    if (left <= m_nElements && m_comp(m_data[extremeIndex].GetValue(), m_data[left].GetValue()))
        extremeIndex = left;
    if (right <= m_nElements && m_comp(m_data[extremeIndex].GetValue(), m_data[right].GetValue()))
        extremeIndex = right;

    if (extremeIndex != i) {
        intercambiar(m_data[i], m_data[extremeIndex]);
        heapifyDown(extremeIndex);
    }
}

template <typename Traits>
void CHeap<Traits>::push(const value_type &val, ref_type ref) {
    lock_guard<mutex> lock(m_mutex);
    
    if (m_nElements + 1 >= m_capacity)
        resize(m_capacity * 2);
    
    ++m_nElements;
    m_data[m_nElements] = Node(val, ref);
    
    heapifyUp(m_nElements);
}

template <typename Traits>
typename CHeap<Traits>::value_type CHeap<Traits>::pop() {
    lock_guard<mutex> lock(m_mutex);
    
    if (m_nElements == 0)
        throw runtime_error("CHeap: pop() en heap vacío");
    
    value_type rootValue = m_data[1].GetValue();

    m_data[1] = m_data[m_nElements];
    --m_nElements;
    
    if (m_nElements > 0)
        heapifyDown(1);
    
    return rootValue;
}

void DemoHeap();

#endif // __HEAP_H__