#ifndef __ARRAY_H__
#define __ARRAY_H__
#include <iostream>
#include <assert.h>
using namespace std;
#include <stddef.h>
#include "../algorithms/sorting.h"
#include "GeneralIterator.h"

using namespace std;

template <typename _T>
struct Trait1
{
    using T = _T;
};

template <typename Container>
class ArrayForwardIterator : public GeneralIterator<Container>
{ 
    using Parent = GeneralIterator<Container>;
public:
    ArrayForwardIterator(Container *pContainer, size_t pos = 0) : Parent(pContainer, pos) {}
    ArrayForwardIterator(const ArrayForwardIterator<Container> &another) : Parent(another) {}

    ArrayForwardIterator<Container> &operator++() {
        if (Parent::m_pos < Parent::m_pContainer->getSize())
            ++Parent::m_pos;
        return *this;
    }
};

template <typename Container>
class ArrayBackwardIterator : public GeneralIterator<Container>
{ 
  using Parent = GeneralIterator<Container>;
  public:
    ArrayBackwardIterator(Container *pContainer, Size pos=0)          : Parent(pContainer, pos){}
    ArrayBackwardIterator(ArrayBackwardIterator<Container> &another)  :  Parent(another){}

    ArrayBackwardIterator<Container> &operator++(){
        if( Parent::m_pos > -1 )
            --Parent::m_pos;
        return *this;
    }
};

template <typename Traits>
class CArray {
    using value_type  = typename Traits::T;
    using  forward_iterator  = ArrayForwardIterator < CArray<Traits> >;
    friend forward_iterator;
    using  backward_iterator = ArrayBackwardIterator< CArray<Traits> >;
    friend backward_iterator;
    friend GeneralIterator< CArray<Traits> >;
  private:
    Size m_capacity = 0, m_last = 0;
    value_type *m_data = nullptr;

  public:
    CArray(size_t Capacity=10);
    virtual ~CArray();

    void push_back(const value_type &value);
    void pop_back();
    value_type &back();
    value_type &operator[](Size index);
    Size getSize() const
    {   return m_last ;  };
    void resize(Size delta = 10);
    void sort( CompareFunc pComp );

    forward_iterator begin()
    { return forward_iterator(this);  }
    forward_iterator end()
    { return forward_iterator(this, getSize());  }

    backward_iterator rbegin()
    { return backward_iterator(this, getSize()-1);  }
    backward_iterator rend()
    { return backward_iterator(this, -1);  }

    template <typename ObjFunc, typename ...Args>
    void Foreach(ObjFunc of, Args... args){
        ::Foreach(*this, of, args...);
        // for (auto i = 0; i < getSize(); ++i)
        //     of(m_data[i], args...);
    }
    template <typename ObjFunc, typename ...Args>
    auto FirstThat(ObjFunc of, Args... args){
        return ::FirstThat(*this, of, args...);
    }
    friend ostream &operator<<(ostream &os, CArray<Traits> &container){
        os << "CArray: size = " << container.getSize() << endl;
        os << "[ ";
        if (container.m_last != (size_t)-1) {
            for (size_t i = 0; i < container.m_last; ++i)
              os << container.m_data[i] << " ";
        }
        os << "]" << endl;
        return os;
    }
};

template <typename Traits>
CArray<Traits>::CArray(size_t capacity) {
    if (capacity == 0) capacity = 10;
    m_capacity = capacity;
   m_last = 0;
    m_data = new value_type[capacity];
}
template <typename Traits>
CArray<Traits>::~CArray() { delete[] m_data; }

template <typename Traits>
typename CArray<Traits>::value_type &CArray<Traits>::operator[](Size index) {
    if (index >= m_capacity) {
        resize(index - m_last + 5);
    }
    assert(index < m_capacity);
    if (index >= m_capacity) {
        m_last = index ;
    }
    return m_data[index];
}

template <typename Traits>
void CArray<Traits>::push_back(const value_type &value) {
    if (m_last >= m_capacity)
      resize(m_capacity); 
    m_data[m_last++] = value;
}

template <typename Traits>
void CArray<Traits>::pop_back() {
    if (m_last > 0)
        m_last--;
}

template <typename Traits>
typename CArray<Traits>::value_type &CArray<Traits>::back() {
    assert(m_last > 0);
    return m_data[m_last - 1];
}

template <typename Traits>
void CArray<Traits>::resize(Size delta) {
    size_t new_capacity = m_capacity + delta;
    value_type *new_data = new value_type[new_capacity];
    for (size_t i = 0; i < m_capacity; ++i)
      new_data[i] = m_data[i];
    delete[] m_data;
    m_data = new_data;
    m_capacity = new_capacity;
}




void DemoArray();

#endif // __ARRAY_H__
