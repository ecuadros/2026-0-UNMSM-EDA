#include <iostream>
#include "util.h"
#include "pointers.h"
#include "containers/array.h"
//#include "containers/lists.h"
#include "containers/heap.h"

// g++ -std=c++17 -Wall -g -pthread main.cpp -o main

void DemoHeap();

int main() {
    std::cout << "Hello EDA-UNMSM!" << std::endl;
    // DemoUtil();
    // DemoPointers1();
    // DemoArray();
    //DemoLists();
    DemoHeap();
    return 0;
}