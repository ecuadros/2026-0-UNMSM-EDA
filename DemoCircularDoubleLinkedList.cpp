#include <iostream>
#include "containers/lists.h"
using namespace std;

void DemoCircularDoubleLists(){
    cout << "--- Demo Circular Double Linked List ---" << endl;

    // Insert ordenado
    CCircularDoubleLinkedList< CDAscendingTrait<int> > lista1;
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

    // Circularidad
    cout << "Circularidad Forward (10 nodos): ";
    auto *p = lista1.begin().m_pNode;
    for(int i = 0; i < 10 && p; ++i){
        cout << p->GetValue() << " -> ";
        p = p->GetNext();
    }
    cout << "..." << endl << endl;

    // Copy
    CCircularDoubleLinkedList< CDAscendingTrait<int> > lista2(lista1);
    cout << "Copia: " << lista2 << endl;

    // Move
    CCircularDoubleLinkedList< CDAscendingTrait<int> > lista3(move(lista2));
    cout << "Movida: " << lista3 << endl;
}
