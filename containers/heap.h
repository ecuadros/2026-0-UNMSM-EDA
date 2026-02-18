#ifndef __HEAP_H__
#define __HEAP_H__

#include <iostream>
#include "../general/types.h"
#include "../util.h"
#include "array.h"


template <typename T, typename _Func>
struct HeapTrait{
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct AscendingTrait : HeapTrait<T, std::greater<T> >{
};

template <typename T>
struct DescendingTrait : HeapTrait<T, std::less<T> >{
};

template <typename T>
class CHeap{
    // ...
    using value_type = T;
    CArray<Trait1<T>> heap;

    struct Node {
        value_type m_value;
        ref_type m_ref;

        Node(){}
        Node( value_type _value, ref_type _ref = -1)
            : m_value(_value), m_ref(_ref){   }

        value_type  GetValue   () const { return m_value; }
        value_type &GetValueRef() { return m_value; }

        ref_type    GetRef     () const { return m_ref;   }
        ref_type   &GetRefRef  () { return m_ref;   }
        Node &operator=(const Node &another){
            m_value = another.GetValue();
            m_ref   = another.GetRef();
            return *this;
        }
        bool operator==(const Node &another) const
        { return m_value == another.GetValue();   }
        bool operator<(const Node &another) const
        { return m_value < another.GetValue();   }
    };

public:
    CHeap() = default;
    CHeap(const CHeap &to_copy);
    CHeap(CHeap &&to_move);
    virtual ~CHeap() = default;
    CHeap &operator=(const CHeap &to_copy);

private:
    void _clear();
};


#endif // __HEAP_H__