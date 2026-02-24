#include <iostream>
#include "containers/heap.h"

using namespace std;

void DemoHeap() {
    // MaxHeap
    MaxHeap<int> maxh;
    maxh.push(10);
    maxh.push(30);
    maxh.push(20);
    maxh.push(5);
    cout << maxh << endl;
    
    cout << "Pop (max): " << maxh.pop() << endl;
    cout << maxh << endl;

    // MinHeap
    MinHeap<int> minh;
    minh.push(10);
    minh.push(30);
    minh.push(20);
    minh.push(5);
    cout << minh << endl;
    
    cout << "Pop (min): " << minh.pop() << endl;
    cout << minh << endl;
}
