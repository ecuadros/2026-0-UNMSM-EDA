#include <iostream>
#include "util.h"
#include "containers/heap.h"
#include "pointers.h"
#include "containers/array.h"


//void DemoUtil();
//void DemoPointers1();
//void DemoArray();
//void DemoLists();
//void DemoStack();
//void DemoQueue();
void DemoHeap();

// g++ -std=c++17 -Wall -g -pthread main.cpp DemoStack.cpp DemoHeap.cpp -o main

int main() {
    std::cout << "Hello EDA-UNMSM!" << std::endl;
        
    // DemoUtil();
    // DemoPointers1();
    // DemoArray();
    // DemoLists();
    
    
    // DemoStack();
    // DemoQueue();
    
    
    DemoHeap();
    
    return 0;
}