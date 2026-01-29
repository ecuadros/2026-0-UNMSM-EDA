#ifndef __ARRAY_H__
#define __ARRAY_H__
#include <iostream>
#include <assert.h>
using namespace std;
#include <stddef.h>
#include "../algorithms/sorting.h"

template <typename _T>
struct Trait1
{
    using T = _T;
    using CompareFunc = bool (*)(const T &, const T &);
};

template <typename Container>
class ArrayForwardIterator
{ private:
    using value_type  = typename Container::value_type;

    Container  *m_pContainer = nullptr;
    value_type *m_data       = nullptr;
    size_t      m_pos        = -1;
  public:
    ArrayForwardIterator(Container *pContainer, size_t pos=0) 
         : m_pContainer(pContainer), m_pos(pos) {
          m_data = m_pContainer->m_data;
         }
    ArrayForwardIterator(ArrayForwardIterator<Container> &another)
         :  m_pContainer(another.m_pContainer),
            m_data (another.m_data),
            m_pos  (another.m_pos)
    {}
    virtual ~ArrayForwardIterator(){};
    ArrayForwardIterator<Container> &operator++();
    bool operator!=(ArrayForwardIterator<Container> &another){
        return m_pContainer != another.m_pContainer ||
               m_pos        != another.m_pos;         
    }
    value_type &operator*(){
      return m_data[m_pos];
    }
};

template <typename Container>
ArrayForwardIterator<Container>& ArrayForwardIterator<Container>::operator++(){
    if( m_pos < m_pContainer->getSize() )
        ++m_pos;
    return *this;
}

template <typename Traits>
class CArray {
    using value_type  = typename Traits::T;
    using CompareFunc = typename Traits::CompareFunc;
    using forward_iterator = ArrayForwardIterator< CArray<Traits> >;
    friend forward_iterator;
  private:
    size_t m_capacity = 0, m_last = 0;
    value_type *m_data = nullptr;

  public:
    CArray(size_t size);
    virtual ~CArray();

    void push_back(value_type value);
    value_type &operator[](size_t index);
    size_t getSize() const
    {   return m_last + 1;  };
    void resize(size_t delta = 10);
    void sort( CompareFunc pComp );

    forward_iterator begin()
    { return forward_iterator(this);  }
    forward_iterator end()
    { return forward_iterator(this, getSize());  }

    template <typename ObjFunc, typename ...Args>
    void Foreach(ObjFunc of, Args... args){
      for (auto i = 0; i < getSize(); ++i)
          of(m_data[i], args...);
    }
};

template <typename Traits>
CArray<Traits>::CArray(size_t size) {
  m_capacity = size;
  m_data = new value_type[size];
}
template <typename Traits>
CArray<Traits>::~CArray() { delete[] m_data; }

template <typename Traits>
typename CArray<Traits>::value_type &CArray<Traits>::operator[](size_t index) {
    // cout << "XResizing from " << m_capacity << " to at least " << index + 5 << endl;
    if (index > m_capacity) {
      cout << "Resizing from " << m_capacity << " to at least " << index + 5 << endl;
      resize(index - m_last + 5);
    }
    assert(index < m_capacity);
    if (index > m_last)
      m_last = index;
    return m_data[index];
}

template <typename Traits>
void CArray<Traits>::push_back(value_type value) {
    if (m_last >= m_capacity)
      resize();
    m_data[m_last++] = value;
}

template <typename Traits>
void CArray<Traits>::resize(size_t delta) {
    size_t new_capacity = m_capacity + delta;
    value_type *new_data = new value_type[new_capacity];
    for (auto i = 0; i < m_capacity; ++i)
      new_data[i] = m_data[i];
    delete[] m_data;
    m_data = new_data;
    m_capacity = new_capacity;
}

template <typename Traits>
void CArray<Traits>::sort( CompareFunc pComp ){
    BurbujaRecursivo(m_data, m_last, pComp);
}

template <typename Traits>
ostream &operator<<(ostream &os, CArray<Traits> &arr) {
  os << "CArray: size = " << arr.getSize() << endl;
  os << "[";
  for (auto i = 0; i < arr.getSize(); ++i)
    os << arr[i] << ",";
  os << "]" << endl;
  return os;
}

void DemoArray();

#endif // __ARRAY_H__
