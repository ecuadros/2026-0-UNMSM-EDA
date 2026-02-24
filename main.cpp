#include <iostream>
#include "util.h"
#include "pointers.h"
#include "containers/array.h"
#include "containers/lists.h"
#include "containers/NewBTree.h"
#include "containers/2-3Tree.h"
// g++ -std=c++17 -Wall -g -pthread main.cpp -o main

int main() {
    std::cout << "Hello EDA-UNMSM!" << std::endl;
    // DemoUtil();
    // DemoPointers1();
    // DemoArray();
    // DemoLists();
      //DemoNewBTree();
    DemoTwoThreeTree();
    return 0;
}