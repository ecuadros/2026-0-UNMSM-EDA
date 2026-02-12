#ifndef __HEAP_H__
#define __HEAP_H__

#include "doublelinkedlist.h"

template <typename Traits>
class CHeap {
protected:
    using  value_type = typename Traits::value_type;
    CDoubleLinkedList<Traits> m_list;

public:
    CHeap(){}

    void insert(const value_type &val, ref_type ref = -1){ m_list.push_back(val, ref); }
    
    value_type extractFirst(){
        if (isEmpty()) { 
            std::cout << "queue vacia" << std::endl; 
            return value_type(); 
        }
        value_type val = m_list[0];
        
        CDoubleLinkedList<Traits> temp;
        if (size() > 1) {
            auto it = m_list.begin();
            ++it; 
            for (; it != m_list.end(); ++it) { temp.push_back(it.m_pNode->GetValue(), it.m_pNode->GetRef()); }
        }
        m_list = std::move(temp);
        return val;
    }
    
    value_type &first(){
        if (isEmpty()) throw std::out_of_range("heap vacio");
        return m_list[0];
    }
    
    bool isEmpty() const { return m_list.getSize() == 0; }
    size_t size () const { return m_list.getSize();      }
    
    friend std::ostream &operator<<(std::ostream &os, CHeap<Traits> &heap){
        os << "CHeap: size = " << heap.m_list.getSize() << std::endl;
        os << heap.m_list;
        return os;
    }
};

#endif // __HEAP_H__