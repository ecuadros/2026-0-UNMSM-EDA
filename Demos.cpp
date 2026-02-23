#include <iostream>
#include "containers/lists.h"

using namespace std;

void DemoHeap(){
    CHeap<MinHeapTrait<int>> heap(10);
    heap.Push(5);
    heap.Push(3);
    heap.Push(8);
    cout << heap << endl;
}


