#include <iostream>
#include "containers/lists.h"

using namespace std;

void DemoLists(){
    CLinkedList< AscendingTrait<T1> > l1;

    l1.Insert(20, 5);
    l1.Insert(30, 3);
    l1.Insert(10, 13);
    cout << l1 << endl;
}

