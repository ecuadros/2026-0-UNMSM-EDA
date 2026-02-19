#ifndef __HEAP_H__
#define __HEAP_H__

#include <iostream>
#include "../general/types.h"
#include "../util.h"
template <typename T, typename _Func>
struct ListTrait{
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct MinHeap : 
    public ListTrait<T, std::greater<T> >{
};

template <typename T>
struct MaxHeap : 
    public ListTrait<T, std::less<T> >{
};

template <typename Trait>
class CHeap{
    public:
    using value_type=typename Traits::value_type;
    private:

    public:
    CHeap(){};
    //COPY CONSTRUCTOR
    CHeap(const CHeap &another){

    }
    //MOVE CONSTRUCTOR
     CHeap(CHeap &&another) noexcept{

     }
    //DESTRUCTOR
    virtual ~CHeap(){};
    value_type  Pop();
    void   Push(value_type &Val);
};
template <typename Traits>
void  CHeap<Traits>::Push(value_type &val){

}
template <typename Traits>
typename  CHeap<Traits>::value_type CHeap<Traits>::Pop(){

}


template <typename Traits>
CHeap<Traits>::~CHeap() {
    
}



#endif // __HEAP_H__