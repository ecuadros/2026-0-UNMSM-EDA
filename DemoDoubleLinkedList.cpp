#include <iostream>
#include "containers/doublelinkedlist.h"
#include "list_traits.h"

using namespace std;

void DemoDoubleLinkedList() {
    cout << "\n--- Demo CDoubleLinkedList ---" << endl;
    
    CDoubleLinkedList<AscendingTrait<int>> dlist;
    
    dlist.push_back(15, 1);
    dlist.push_back(30, 2);
    dlist.push_back(45, 3);
    dlist.push_back(60, 4);
    
    cout << "Lista doble:" << endl;
    cout << dlist;
    
    cout << "Navegación FORWARD: ";
    for(auto it = dlist.begin(); it != dlist.end(); ++it) {
        cout << *it << " -> ";
    }
    cout << "END" << endl;
    
    cout << "Navegación BACKWARD: ";
    for(auto it = dlist.rbegin(); it != dlist.rend(); ++it) {
        cout << *it << " <- ";
    }
    cout << "START" << endl;
    
    cout << "Acceso: dlist[2] = " << dlist[2] << endl;
    
    CDoubleLinkedList<AscendingTrait<int>> sorted;
    sorted.Insert(55, 1);
    sorted.Insert(25, 2);
    sorted.Insert(40, 3);
    
    cout << "Insert ordenado:" << endl;
    cout << sorted;
    
    auto result = dlist.FirstThat([](int val) { return val > 40; });
    cout << "FirstThat (val > 40): " << *result << endl;
}
