#ifndef __LIST_TRAITS_H__
#define __LIST_TRAITS_H__

#include <functional>

// TODO: Traits para listas enlazadas
template <typename T, typename _Func>
struct ListTrait{
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct AscendingTrait : public ListTrait<T, std::greater<T> >{};

template <typename T>
struct DescendingTrait : public ListTrait<T, std::less<T> >{};

#endif // __LIST_TRAITS_H__
