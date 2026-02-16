#include <assert.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "containers/heap.h" 

using namespace std;

using T1 = int;
using T2 = string;

template <typename Q>
void Print(Q &elem){    cout << elem << " ";     }

void DemoHeap() {
    const int N = 10;

    cout << "MinHeap (Enteros) " << endl;
    
    CHeap< MinHeapTrait<T1> > heap1(5);

    for (auto i = 0; i < N; ++i) {
        int val = (N - i) * 10;
        heap1.Push(val);
        cout << "Push: " << val << endl;
    }

    cout << "Estado actual del Heap:" << endl;
    cout << heap1 << endl;

    ofstream of("heap.txt");
    of << heap1;
    of.close();
    cout << "Guardado en 'heap.txt'." << endl;


    cout << "Extrayendo elementos (Pop - Orden Logico): " << endl;
    while (true) {
        try {
            T1 val = heap1.Pop(); 
            Print(val);
        } catch (const std::out_of_range& e) {
            break; 
        }
    }
    
    CHeap< MaxHeapTrait<T2> > heapStr;
    
    heapStr.Push("UNMSM");
    heapStr.Push("Sistemas");
    heapStr.Push("Algoritmos");
    heapStr.Push("Facultad");
    heapStr.Push("Codigo");

    cout << "MaxHeap Strings: " << heapStr << endl;

    cout << "Top (Max): " << heapStr.Pop() << endl; 
    cout << "Nuevo Max: " << heapStr.Pop() << endl;
    
    CHeap< MaxHeapTrait<T1> > heapSrc;
    heapSrc.Push(1);
    heapSrc.Push(100);
    heapSrc.Push(50);

    CHeap< MaxHeapTrait<T1> > heapCopy = heapSrc; 
    cout << "Original: " << heapSrc << endl;
    cout << "Copia   : " << heapCopy << endl;


    CHeap< MaxHeapTrait<T1> > heapMoved = std::move(heapSrc);
    
    cout << "Moved   : " << heapMoved << endl;

}
