#include <iostream>
#include "containers/lists.h"
#include "containers/heap.h"
#include <iostream>

using namespace std;

void DemoHeap() {
    // Max Heap
    CHeap<AscendingHeapTrait<int>> maxH;
    maxH.push(10,0);
    maxH.push(20,1);
    maxH.push(5,2);
    
    cout << "-------------------------------------" << endl;
    cout << "\tMax. Heap" << endl;
    cout << "-------------------------------------" << endl;
    cout << "Tras el push 10, 20, 5: " << endl;
    cout << maxH << endl;
    cout << "Pop: " << maxH.pop() << endl;

    // Min Heap
    CHeap<DescendingHeapTrait<int>> minH;
    minH.push(10,0);
    minH.push(20,1);
    minH.push(5,2);
    cout << "\n-------------------------------------" << endl;
    cout << "\tMin. Heap" << endl;
    cout << "-------------------------------------" << endl;
    cout << "Tras el push 10, 20, 5: "<< endl;
    cout << minH << endl;
    cout << "Pop: " << minH.pop() << endl;
}