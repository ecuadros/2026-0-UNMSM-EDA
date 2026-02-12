/*#include <iostream>
#include "util.h"
#include "pointers.h"
#include "containers/array.h"
#include "containers/lists.h"

// g++ -std=c++17 -Wall -g -pthread main.cpp -o main

int main() {
    std::cout << "Hello EDA-UNMSM!" << std::endl;
    // DemoUtil();
    // DemoPointers1();
    // DemoArray();
    DemoLists();
    return 0;
}*/

#include <iostream>
using namespace std;

#include "containers/stack.h"

int main() {
    CStack<int> s;

    // Push básico
    s.push(10);
    s.push(20);
    s.push(30);

    cout << "Stack despues de push: ";
    s.print();
    cout << endl;

    // Pop clásico
    s.pop();
    cout << "Stack despues de pop: ";
    s.print();
    cout << endl;

    // Operador >>
    int x;
    s >> x;
    cout << "Valor extraido con >> : " << x << endl;

    cout << "Stack actual: ";
    s.print();
    cout << endl;

    // Operador <<
    s << 99;
    cout << "Stack despues de << 99: ";
    s.print();
    cout << endl;

    return 0;
}
