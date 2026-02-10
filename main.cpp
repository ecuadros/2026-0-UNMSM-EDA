#include <iostream>
#include "util.h"
#include "pointers.h"
#include "containers/array.h"
#include "containers/lists.h"

// g++ -std=c++17 -Wall -g -pthread main.cpp DemoLinkedList.cpp DemoLinkedListCircular.cpp Demos.cpp -o main

int main() {
    std::cout << "Hello EDA-UNMSM!" << std::endl << std::endl;
    
    // Demos disponibles:
    // DemoUtil();
    // DemoPointers1();
    // DemoArray();
    // DemoLists();  // Demo original del profesor
    
    // Nuevos demos completos:
    DemoLinkedList();           
    DemoLinkedListCircular();  
    return 0;
}
