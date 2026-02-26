#include <iostream>
#include "containers/heap.h"

using namespace std;

void DemoHeap() {
    // Prueba Max-Heap
    CHeap<HeapTraitMax<int>> maxHeap;
    maxHeap << 10 << 30 << 20 << 5 << 50;
    cout << "Max-Heap: " << maxHeap << endl;

    int val;
    maxHeap >> val;
    cout << "Extraido (debe ser 50): " << val << endl;
    cout << "Max-Heap tras Pop: " << maxHeap << endl;

    // Prueba Min-Heap
    CHeap<HeapTraitMin<int>> minHeap;
    minHeap << 10 << 30 << 20 << 5 << 50;
    cout << "Min-Heap: " << minHeap << endl;

    minHeap >> val;
    cout << "Extraido (debe ser 5): " << val << endl;
    cout << "Min-Heap tras Pop: " << minHeap << endl;
}