#ifndef __BTREE_TRAITS_H__
#define __BTREE_TRAITS_H__

#include <functional>
#include "../general/types.h"

template <typename T>
struct BTreeTraitAscending {
    using value_type  = T;
    using CompareFunc = std::greater<T>; 
};

template <typename T>
struct BTreeTraitDescending {
    using value_type  = T;
    using CompareFunc = std::less<T>;
};

#endif //__BTREE_TRAITS_H__