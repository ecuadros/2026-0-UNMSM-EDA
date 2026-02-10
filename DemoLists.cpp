#include <iostream>
#include "containers/lists.h"
#include "containers/linkedlist.h"

using namespace std;

using T1 = int;

void DemoSimpleList() {
    CLinkedList< AscendingTrait<T1> > l1;

    // Inserción ordenada usando InternalInsert
    l1.Insert(20, 5);
    l1.Insert(30, 3);
    l1.Insert(10, 1);

    // Uso de push_back directo
    T1 val = 40;
    l1.push_back(val, 10);

    cout << "--- CLinkedList Simple ---" << endl;
    cout << l1 << endl;

    // Acceso por indice (operator[])
    try {
        cout << "Elemento en indice 2: " << l1[2] << endl;
    } catch (const std::out_of_range& e) {
        cerr << e.what() << endl;
    }
}

int main() {
    DemoSimpleList();
    return 0;
}