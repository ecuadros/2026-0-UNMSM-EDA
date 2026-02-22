/*#include <iostream>
#include "util.h"
#include "pointers.h"
//#include "containers/array.h"
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
#include "containers/heap.h"
using namespace std;

int main() {
    cout << "Demo Heap" << endl;
    CHeap<int> heap(5);

    heap.push(10);
    heap.push(40);
    heap.push(20);
    heap.push(30);

    cout << heap << endl;

    //Insertar usando operador >>
    heap >> 50 >> 5 >> 100;

    cout << "Despues de insertar más elementos:" << endl;
    cout << heap << endl;

    //Probando pop
    cout << "Pop: " << heap.pop() << endl;
    cout << "Despues de hacer pop:" << endl;
    cout << heap << endl;

    // Probando copy constructor
    CHeap<int> heapCopy(heap);
    cout << "Heap original: " << heap << endl;
    cout << "Heap copia: " << heapCopy << endl;

    heapCopy.push(999);

    cout << "Despues de modificar la copia:" << endl;
    cout << "Heap original: " << heap << endl;
    cout << "Heap copia: " << heapCopy << endl;

    //Probando move constructor
    CHeap<int> heapMoved(std::move(heap));
    cout << "Heap movido: " << heapMoved << endl;

    cout << "Despues de mover el heap original:" << endl;
    cout << "Heap original: " << heap << endl; // Deberia estar vacio o en estado válido pero no definido

    //Probando excepcion
    cout << "Vaciando el heap" << endl;
    while (!heapMoved.empty()){
        cout << "Pop: " << heapMoved.pop() << endl;
    }

    try {
        cout << "Intentando hacer pop en heap vacio..." << endl;
        heapMoved.pop();
    } catch (const std::out_of_range &e) {
        cout << "Excepcion capturada: " << e.what() << endl;
    }

    return 0;
}