#include <iostream>
#include "containers/doublelinkedlist.h"

using namespace std;

using T1 = int;

void DemoDoubleList() {
    // Instancia circular segun constructor
    CDoubleLinkedList< DoubleAscendingTrait<T1> > l2(true);

    l2.Insert(100, 10);
    l2.Insert(50, 20);
    l2.push_front(25, 5); // Inserción al inicio
    l2.push_back(150, 30); // Inserción al final

    cout << "--- CDoubleLinkedList (Circular) ---" << endl;
    cout << l2 << endl;

    // Prueba de Foreach con lambda simple
    cout << "Recorrido Foreach: ";
    l2.Foreach([](T1 &val) {
        cout << val << " ";
    });
    cout << endl;

    // Verificacion de estado
    cout << "Size: " << l2.getSize() << endl;
    cout << "Es circular: " << (l2.isCircular() ? "Si" : "No") << endl;
}

int main() {
    DemoDoubleList();
    return 0;
}

