#include <iostream>
#include "util.h"
#include "pointers.h"
#include "containers/array.h"
#include "containers/lists.h"
#include "containers/clists.h" 
#include "containers/dlists.h"
#include "containers/cdlists.h"
#include "containers/stacks.h"   
#include "containers/queues.h"   

// g++ -std=c++17 -Wall -g -pthread main.cpp -o main


int main() {
    std::cout << "Hello EDA-UNMSM!" << std::endl;
    // DemoUtil();
    // DemoPointers1();
    // DemoArray();
    DemoLists();
    std::cout << std::endl << std::endl;
    DemoCircularLinkedList();
    std::cout << std::endl << std::endl;
    DemoDoubleLists();
    std::cout << std::endl << std::endl;
    DemoCircularDoubleLists();
   std::cout << std::endl << std::endl;
    
    DemoStack();
    std::cout << std::endl << std::endl;
    
    //DemoQueue();
    
    return 0;
}