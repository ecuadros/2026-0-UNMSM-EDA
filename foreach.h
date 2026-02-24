#ifndef __FOREACH_H__
#define __FOREACH_H__

#include <utility> // std::forward

template <typename Iterator, typename FuncObj, typename... Args>
void Foreach(Iterator begin, Iterator end, FuncObj&& fn, Args&&... args){
    for(auto iter = begin; iter != end; ++iter){
        fn(*iter, std::forward<Args>(args)...);
    }
}

template <typename Container, typename FuncObj, typename... Args>
void Foreach(Container &container, FuncObj&& fn, Args&&... args){
    Foreach(container.begin(), container.end(),
            std::forward<FuncObj>(fn),
            std::forward<Args>(args)...);
}

template <typename Iterator, typename FuncObj, typename... Args>
Iterator FirstThat(Iterator begin, Iterator end, FuncObj&& fn, Args&&... args){
    for(auto iter = begin; iter != end; ++iter){
        if(fn(*iter, std::forward<Args>(args)...))
            return iter;
    }
    return end;
}

template <typename Container, typename FuncObj, typename... Args>
auto FirstThat(Container &container, FuncObj&& fn, Args&&... args){
    return FirstThat(container.begin(), container.end(),
                     std::forward<FuncObj>(fn),
                     std::forward<Args>(args)...);
}

#endif // __FOREACH_H__
