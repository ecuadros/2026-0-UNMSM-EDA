#include <iostream>
#include "util.h"
#include "pointers.h"
#include "containers/array.h"
#include "containers/lists.h"
#include "containers/circularlist.h"
#include "containers/circular_doubly_linked_list.h"

// g++ -std=c++17 -Wall -g -pthread main.cpp -o main
void DemoLists();
void DemoCircular();
void DemoLDE();
void DemoCircularEnlazada();

int main() {
    std::cout << "Hello EDA-UNMSM!" << std::endl;
    // DemoUtil();
    // DemoPointers1();
    // DemoArray();
    DemoLists();
    //DemoCircular();
    //DemoLDE();
    //DemoCircularEnlazada();

    return 0;
}