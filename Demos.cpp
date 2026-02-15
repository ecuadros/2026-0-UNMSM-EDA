#include <iostream>
#include "containers/heap.h"

using namespace std;

void DemoHeap(){

    cout << "PROBANDO MIN HEAP " << endl;
    CHeap<MinHeapTrait<int>> minHeap;

    minHeap.push(50);
    minHeap.push(10);
    minHeap.push(30);
    minHeap.push(5);

    cout << "Estructura interna (Vector): " << minHeap << endl;

    cout << "Top (Raiz): " << minHeap.top() << endl;

    cout << "Hacemos Pop -> Sale: " << minHeap.pop() << endl;
    cout << "Hacemos Pop -> Sale: " << minHeap.pop() << endl;
    cout << "Hacemos Pop -> Sale: " << minHeap.pop() << endl;

    cout << endl <<"PROBANDO MAXHEAP" << endl;
    CHeap<MaxHeapTrait<int>> maxHeap;

    maxHeap.push(50);
    maxHeap.push(10);
    maxHeap.push(30);
    maxHeap.push(100);

    cout << "Estructura interna (Vector): " << maxHeap << endl;

    cout << "Top (Raiz): " << maxHeap.top() << endl;

    cout << "Hacemos Pop -> Sale: " << maxHeap.pop() << endl;
    cout << "Hacemos Pop -> Sale: " << maxHeap.pop() << endl;

    cout << endl << "PROBANDO PERSISTENCIA" << endl;
    ofstream archivoOut("datos_heap.txt");
    archivoOut << "20 60 10 40 5";
    archivoOut.close();

    CHeap<MinHeapTrait<int>> heapArchivo;
    ifstream archivoIn("datos_heap.txt");
    if (archivoIn.is_open()) {
        archivoIn >> heapArchivo;
        archivoIn.close();
        cout << "Heap cargado del archivo: " << heapArchivo << endl;
        cout << "La raiz (el menor) es: " << heapArchivo.top() << endl;
    }

    cout << endl << "PROBANDO CONSTRUCTOR COPIA" << endl;
    CHeap<MinHeapTrait<int>> heapCopia(heapArchivo);
    cout << "Modificando la copia (Push -999)..." << endl;
    heapCopia.push(-999);
    cout << "Original (Size " << heapArchivo.size() << "): " << heapArchivo.top() << " (Intacto)" << endl;
    cout << "Copia    (Size " << heapCopia.size() << "): " << heapCopia.top()   << " (Modificado)" << endl;
    
    cout << endl << "PROBANDO MOVE CONSTRUCTOR" << endl;
    CHeap<MinHeapTrait<int>> heapMovido(std::move(heapArchivo));
    
    cout << "Original vacio? " << (heapArchivo.empty() ? "SI" : "NO") << endl;
    cout << "Heap Movido: " << heapMovido << endl;

}


