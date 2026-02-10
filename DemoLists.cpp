#include <iostream>
#include "containers/lists.h"

using namespace std;

void DemoLists() {
    // Lista Enlazada Simple (LE)
    CLinkedList<AscendingTrait<int>> l1;
    l1.Insert(30, 3);
    l1.Insert(10, 1);
    l1.Insert(20, 2);
    cout << l1 << endl;

    // Lista Enlazada Circular (LEC)
    CLinkedList<AscendingTrait<int>> l2(true);
    l2.push_back(5, 1);
    l2.push_back(15, 2);
    l2.push_back(25, 3);
    cout << l2 << endl;

    // Foreach
    cout << "3. Usando Foreach (suma 10):" << endl;
    l1.Foreach(&Suma<int>, 10);
    cout << l1 << endl;
}
