#include <iostream>
#include "containers/lists.h"
#include "DemoHeap.cpp"

using namespace std;

void DemoLists(){
    CLinkedList< AscendingTrait<T1> > l1;

    l1.Insert(20, 5);
    l1.Insert(30, 3);
    cout << l1 << endl;
}

void DemoHeap();

int main(){
    DemoHeap();
    return 0;
}