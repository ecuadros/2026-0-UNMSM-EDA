#ifndef __LIST_TRAITS_H__
#define __LIST_TRAITS_H__

// Usaremos los mismos Traits para todas las listas enlazadas
// Nos ayudan a definir el tipo de dato a lo largo de la estructura
// de forma más sencilla. Además de la función greater o less 
// para comprobar que uno es descendente o ascendete

template <typename T, typename _Func>
struct ListTrait{
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct AscendingTrait : public ListTrait<T, std::greater<T> >{ };

template <typename T>
struct DescendingTrait : public ListTrait<T, std::less<T> >{ };

#endif