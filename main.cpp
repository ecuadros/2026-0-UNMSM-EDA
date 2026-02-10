#include <iostream>
#include "util.h"
#include "pointers.h"
#include "containers/array.h"
#include "containers/lists.h"

void DemoLinkedList();
void DemoCircularLinkedList();
void DemoDoubleLinkedList();
void DemoCircularDoubleLinkedList();

// g++ -std=c++17 -Wall -g -pthread main.cpp -o main

int main() {
    std::cout << "Hello EDA-UNMSM!" << std::endl;
    // DemoUtil();
    // DemoPointers1();
    // DemoArray();
    // DemoLists();
    // DemoCircularLists();
    DemoLinkedList();
    DemoCircularLinkedList();
    DemoDoubleLinkedList();
    DemoCircularDoubleLinkedList();
    
    return 0;
}