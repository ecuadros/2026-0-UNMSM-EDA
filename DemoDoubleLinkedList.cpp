#include <iostream>
#include "containers/lists.h"
using namespace std;

void DemoDoubleLists(){
    cout << "--- Demo Double Linked List ---" << endl;

    // Insert ordenado
    CDoubleLinkedList< DAscendingTrait<int> > lista1;
    lista1.Insert(30, 1);
    lista1.Insert(10, 2);
    lista1.Insert(50, 3);
    lista1.Insert(20, 4);
    lista1.Insert(40, 5);
    cout << "Insert ordenado: " << lista1 << endl;

    // push_front y push_back
    lista1.push_front(5, 6);
    lista1.push_back(60, 7);
    cout << "push_front(5) + push_back(60): " << lista1 << endl;

    // Forward
    cout << "Forward:  [";
    for(auto it = lista1.begin(); it != lista1.end(); ++it)
        cout << *it << " ";
    cout << "]" << endl;

    // Backward
    cout << "Backward: [";
    for(auto it = lista1.rbegin(); it != lista1.rend(); ++it)
        cout << *it << " ";
    cout << "]" << endl << endl;

    // Copy
    CDoubleLinkedList< DAscendingTrait<int> > lista2(lista1);
    cout << "Copia: " << lista2 << endl;

    // Move
    CDoubleLinkedList< DAscendingTrait<int> > lista3(move(lista2));
    cout << "Movida: " << lista3 << endl;
}
