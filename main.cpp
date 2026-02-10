#include <iostream>
#include "util.h"
#include "pointers.h"
#include "containers/array.h"
#include "containers/lists.h"
#include "containers/doublelinkedlistcircular.h" 
#include "containers/doublelinkedlist.h"


using namespace std;

// g++ -std=c++17 -Wall -g -pthread main.cpp -o main

int main() {
    cout << "Hello EDA-UNMSM!" << endl;
    // DemoUtil();
    // DemoPointers1();
    // DemoArray();
    // DemoLists();
    DemoDoubleLinkedList();
    DemoDoubleLinkedListCircular(); 

    return 0;
}