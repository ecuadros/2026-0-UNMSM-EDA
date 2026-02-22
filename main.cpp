#include <iostream>
#include "util.h"
#include "pointers.h"
#include "containers/array.h"
#include "containers/lists.h"
#include "containers/heap.h"
#include "containers/binarytree.h"
#include "containers/avl.h"

void DemoHeap();
void DemoBinaryTree();
void DemoAVL();

// g++ -std=c++17 -Wall -g -pthread main.cpp -o main

int main() {
    std::cout << "Hello EDA-UNMSM!" << std::endl;
    // DemoUtil();
    // DemoPointers1();
    // DemoArray();
    // DemoLists();
    // DemoCircularLists();
    //DemoDoubleLists();
    //DemoCircularDoubleLists();
    //DemoStack();
    //DemoQueue();
    //DemoHeap();
    DemoBinaryTree();
    DemoAVL();
    return 0;
}
