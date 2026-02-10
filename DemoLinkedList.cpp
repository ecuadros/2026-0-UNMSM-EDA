#include <iostream>
#include "containers/linkedlist.h"
#include "list_traits.h"

using namespace std;

void DemoLinkedList() {
    cout << "\n--- Demo CLinkedList ---" << endl;
    
    CLinkedList<AscendingTrait<int>> list;
    
    list.push_back(10, 1);
    list.push_back(25, 2);
    list.push_back(15, 3);
    list.push_back(30, 4);
    
    cout << "Lista con push_back:" << endl;
    cout << list;
    
    cout << "Acceso: list[2] = " << list[2] << endl;
    
    CLinkedList<AscendingTrait<int>> sorted;
    sorted.Insert(50, 1);
    sorted.Insert(20, 2);
    sorted.Insert(35, 3);
    
    cout << "Lista con Insert (ordenada):" << endl;
    cout << sorted;
    
    auto iter = sorted.begin();
    cout << "Primer elemento: " << *iter << endl;
    
    sorted.sort();
    cout << "Después de sort:" << endl;
    cout << sorted;
}
