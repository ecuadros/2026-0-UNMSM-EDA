#ifndef __STACK_H__
#define __STACK_H__

#include "doublelinkedlist.h"
#include <stdexcept>

template <typename Traits>
class CStack {
private:
    using  value_type = typename Traits::value_type;
    CDoubleLinkedList<Traits> m_list;

public:
    CStack(){}

    void push(const value_type &val, ref_type ref = -1){ m_list.push_back(val, ref); }
    
    value_type pop(){
        if (isEmpty()) throw std::out_of_range("stack vacio");
        value_type val = m_list[m_list.getSize() - 1];
        m_list.pop_back();
        return val;
    }
    
    value_type &top(){
        if (isEmpty()) throw std::out_of_range("stack vacio");
        return m_list[m_list.getSize() - 1];
    }
    
    bool isEmpty() const { return m_list.getSize() == 0; }
    size_t size () const { return m_list.getSize();      }
    
    friend std::ostream &operator<<(std::ostream &os, CStack<Traits> &stack){
        os << "CStack (LIFO): size = " << stack.m_list.getSize() << std::endl;
        os << stack.m_list;
        return os;
    }
};

#endif // __STACK_H__