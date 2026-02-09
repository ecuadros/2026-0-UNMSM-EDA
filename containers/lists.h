#ifndef __LISTS_H__
#define __LISTS_H__
#include "../general/types.h"

#include "linkedlist.h"
#include "circularlinkedlist.h"

// Type aliases to help IDEs infer concrete list types quickly.
template <typename T>
using AscendingList = CLinkedList<AscendingTrait<T>>;

template <typename T>
using DescendingList = CLinkedList<DescendingTrait<T>>;

// Generic alias with a trait template parameter, defaulting to ascending.
template <typename T, template <typename> class Trait = AscendingTrait>
using LinkedList = CLinkedList<Trait<T>>;

void DemoLists();

void DemoCircularLinkedLists();

#endif // __LISTS_H__
