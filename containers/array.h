#ifndef __ARRAY_H__
#define __ARRAY_H__
#include <iostream>
#include <assert.h>
using namespace std;
#include <stddef.h>
#include "../algorithms/sorting.h"

template <typename T>
class CArray {
    using value_type = T;
    using CompareFunc = bool (*)(const T &, const T &);

  private:
    size_t m_capacity = 0, m_count = 0;
    value_type *m_data = nullptr;

  public:
    CArray(size_t size);
    ~CArray();

    void push_back(value_type value);
    value_type &operator[](size_t index);
    size_t getSize() const
    {   return m_count;  };
    void resize(size_t delta = 10);
    void sort( CompareFunc pComp );
    void Foreach(void (*pf)(T &, T), T p1);
};

template <typename T>
CArray<T>::CArray(size_t size) {
  m_capacity = size;
  m_data = new value_type[size];
}
template <typename T>
CArray<T>::~CArray() { delete[] m_data; }

template <typename T>
typename CArray<T>::value_type &CArray<T>::operator[](size_t index) {
    // cout << "XResizing from " << m_capacity << " to at least " << index + 5 << endl;
    if (index > m_capacity) {
        cout << "Resizing from " << m_capacity << " to at least " << index + 5 << endl;
        resize(index - m_count + 5);
    }
    assert(index < m_capacity);
    if (index >= m_count)
        m_count = index + 1;
    return m_data[index];
}

template <typename T>
void CArray<T>::push_back(value_type value) {
    if (m_count >= m_capacity)
      resize();
    m_data[m_count++] = value;
}

template <typename T>
void CArray<T>::resize(size_t delta) {
    size_t new_capacity = m_capacity + delta;
    value_type *new_data = new value_type[new_capacity];
    for (auto i = 0; i < m_capacity; ++i)
      new_data[i] = m_data[i];
    delete[] m_data;
    m_data = new_data;
    m_capacity = new_capacity;
}

template <typename T>
void CArray<T>::sort( CompareFunc pComp ){
    BurbujaRecursivo(m_data, m_count, pComp);
}

template <typename Q>
auto Test(Q elem) { return elem;  }

template <typename Q, typename ...Args>
auto Test(Q elem, Args ...args){
    return elem + Test(args...);
}

template <typename Q>
void Suma(Q &elem, Q p1){    elem += p1;     }

template <typename Q>
void Mult(Q &elem, Q p1){    elem *= p1;     }

template <typename T>
void CArray<T>::Foreach(void (*pf)(T &, T), T p1 ){
    for (auto i = 0; i < getSize(); ++i)
        pf(m_data[i], p1);
}

template <typename T>
ostream &operator<<(ostream &os, CArray<T> &arr) {
  os << "CArray: size = " << arr.getSize() << endl;
  os << "[";
  for (auto i = 0; i < arr.getSize(); ++i)
    os << arr[i] << ",";
  os << "]" << endl;
  return os;
}

void DemoArray();

#endif // __ARRAY_H__
