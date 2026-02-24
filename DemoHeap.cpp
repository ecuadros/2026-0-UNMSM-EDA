#include <iostream>
#include "containers/heap.h"

using namespace std;

void DemoHeap(){
    cout << "===== MaxHeap =====" << endl;
    CHeap< MaxHeapTrait<T1> > maxH;

    maxH.push(10, 1);
    maxH.push(40, 2);
    maxH.push(20, 3);
    maxH.push(50, 4);
    maxH.push(30, 5);
    maxH.push(5,  6);
    cout << maxH;

    cout << "Pop: " << maxH.pop() << endl;
    cout << "Pop: " << maxH.pop() << endl;
    cout << maxH;

    cout << endl;
    cout << "===== MinHeap =====" << endl;
    CHeap< MinHeapTrait<T1> > minH;

    minH.push(10, 1);
    minH.push(40, 2);
    minH.push(20, 3);
    minH.push(50, 4);
    minH.push(30, 5);
    minH.push(5,  6);
    cout << minH;

    cout << "Pop: " << minH.pop() << endl;
    cout << "Pop: " << minH.pop() << endl;
    cout << minH;

    cout << endl;
    cout << "===== Constructor Copia =====" << endl;
    CHeap< MaxHeapTrait<T1> > copiaH(maxH);
    cout << "Copia: " << copiaH;

    cout << "===== Move Constructor =====" << endl;
    CHeap< MaxHeapTrait<T1> > movedH(std::move(copiaH));
    cout << "Moved: " << movedH;
    cout << "Copia (vacio): size = " << copiaH.getSize() << endl;
}
