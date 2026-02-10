#include <iostream>
#include "containers/lists.h"

using namespace std;

void DemoLists() {
    // Lista Doblemente Enlazada (LDE)
    CDoubleLinkedList<DAscendingTrait<int>> dl1;
    dl1.push_back(100, 1);
    dl1.push_back(200, 2);
    dl1.push_back(300, 3);
    cout << dl1 << endl;

    // Lista Doblemente Enlazada Circular (LDEC)
    CDoubleLinkedList<DAscendingTrait<int>> dl2(true);
    dl2.push_back(50, 1);
    dl2.push_back(150, 2);
    dl2.push_back(250, 3);
    cout << dl2 << endl;
}
