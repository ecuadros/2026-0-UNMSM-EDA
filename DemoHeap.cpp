#include <iostream>
#include <stdexcept>
using namespace std;
#include "containers/heap.h"

using T1 = int;

void DemoHeap() {
    cout << "\nDemoHeap" << endl;
    cout << "-------------" << endl;

    // MaxHeap
    CHeap< MaxHeapTrait<T1> > maxH;
    maxH.push(10, 0);
    maxH.push(30, 1);
    maxH.push(20, 2);
    maxH.push(50, 3);
    maxH.push(40, 4);
    cout << "MaxHeap:" << endl;
    cout << maxH;
    
    cout << "pop() = " << maxH.pop() << endl;
    cout << "pop() = " << maxH.pop() << endl;
    cout << maxH;

    // MinHeap
    CHeap< MinHeapTrait<T1> > minH;
    minH.push(10, 0);
    minH.push(30, 1);
    minH.push(20, 2);
    minH.push(50, 3);
    minH.push(40, 4);
    cout << "\nMinHeap:" << endl;
    cout << minH;
    
    cout << "pop() = " << minH.pop() << endl;
    cout << "pop() = " << minH.pop() << endl;
    cout << minH;

    // Constructor copia
    CHeap< MaxHeapTrait<T1> > maxH2(maxH);
    cout << "\nCopia de MaxHeap:" << endl;
    cout << maxH2;

    // Move Constructor
    CHeap< MinHeapTrait<T1> > minH2(move(minH));
    cout << "\nMove de MinHeap:" << endl;
    cout << minH2;
    cout << "Original size = " << minH.getSize() << endl;

    // Excepción
    CHeap< MaxHeapTrait<T1> > empty;
    try {
        empty.pop();
    } catch (const runtime_error &e) {
        cout << "\nExcepción: " << e.what() << endl;
    }

    cout << "----------------" << endl; 
    cout << "Fin DemoHeap\n" << endl;
}