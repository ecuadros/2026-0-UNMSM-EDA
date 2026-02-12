#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "heap.h"

template <typename Traits>
class CQueue : public CHeap<Traits> {
private:
    using  value_type   = typename Traits::value_type;
    using  Parent       = CHeap<Traits>;

public:
    CQueue() : Parent(){}
    
    void enqueue(const value_type &val, ref_type ref = -1){ Parent::insert(val, ref); }
    
    value_type dequeue(){ return Parent::extractFirst(); }
    
    value_type &front(){
        if (isEmpty())
            std::cout << "queue vacia" << std::endl;
        return Parent::first();
    }
    
    bool isEmpty() const { return Parent::isEmpty(); }
    size_t size () const { return Parent::size();    }
    
    friend std::ostream &operator<<(std::ostream &os, CQueue<Traits> &queue){
        os << "CQueue : size = " << queue.m_list.getSize() << std::endl;
        os << queue.m_list;
        return os;
    }
};

#endif // __QUEUE_H__