#include <iostream>
#include "containers/circularlinkedlist.h"
#include "list_traits.h"

using namespace std;

void DemoCircularLinkedList() {
    cout << "\n--- Demo CCircularLinkedList ---" << endl;
    
    CCircularLinkedList<AscendingTrait<int>> clist;
    
    clist.push_back(100, 1);
    clist.push_back(200, 2);
    clist.push_back(300, 3);
    
    cout << "Lista circular:" << endl;
    cout << clist;
    
    cout << "Tamaño: " << clist.getSize() << endl;
    cout << "Acceso circular clist[0]: " << clist[0] << endl;
    cout << "Acceso circular clist[4]: " << clist[4] << " (da la vuelta)" << endl;
    cout << "Acceso circular clist[7]: " << clist[7] << endl;
    
    CCircularLinkedList<AscendingTrait<int>> sorted;
    sorted.Insert(80, 1);
    sorted.Insert(20, 2);
    sorted.Insert(50, 3);
    
    cout << "Insert ordenado circular:" << endl;
    cout << sorted;
    
    cout << "Foreach (un recorrido): ";
    clist.Foreach([](int val) { cout << val << " "; });
    cout << endl;
}
