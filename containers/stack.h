#ifndef __STACK_H__
#define __STACK_H__

#include <iostream>
#include "../general/types.h"
#include "../util.h"
#include <utility>
using namespace std;
template <typename T>
struct StackTrait
{
    using Value_type = T;
};

template <typename Traits>
class NodeStack{
using Value_type = typename Traits::Value_type;
using Node       = NodeStack<Traits>;
private:
Value_type m_data;
Node *m_pNext = nullptr;
public:
NodeStack(){};
NodeStack( Value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref){   };
Value_type  GetValue   () const { return m_data; }
Value_type &GetValueRef() { return m_data; }


Node      * GetNext     () const { return m_pNext;   }
Node      *&GetNextRef  () { return m_pNext;   }

};

template <typename Traits>
class CStack{
using Value_type = typename Traits::Value_type;
using Node       = NodeStack<Traits>;    
private:
Node* n_Top=nullptr;
size_t m_nElements = 0;
mutable std::mutex m_Block;
public:
CStack ();
void Push(const Value_type &Val );
void Pop();
size_t getSize(){ return m_nElements;  }
virtual ~CStack();



 friend ostream &operator<<(ostream &os, CStack<Traits> &container){
        std::lock_guard<std::mutex> lock(container.m_Block);
        
        return os;
    }

    friend istream &operator>>(istream &is, CStack<Traits> &container) {
        
        return is;
    }    
};


#endif // __STACK_H__