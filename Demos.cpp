#include <iostream>
#include "containers/heap.h"
#include "containers/traits.h"
#include "general/types.h"

using namespace std;

void Demos() {
    //datos de prueba
    T1 datos[]      = {45, 12, 89, 33, 5, 76, 21, 99, 1, 64, 70, 85, 14, 2, 17};
    auto numDatos   = sizeof(datos) / sizeof(datos[0]);

    //MinHeap
    cout << "*** MinHeap : DescendingTrait ***" << endl;
    
    CHeap<DescendingTrait<T1>> minHeap; 
    
    cout << "1. push datos ..." << endl;
    for (size_t i=0; i<numDatos; ++i) { minHeap.push(datos[i], i); }
    cout << "MinHeap : " << minHeap << "\n";
    cout << "SizeHeap: " << minHeap.size() << endl;

    cout << "\n2. pop raiz ..." << endl;
    if (!minHeap.isEmpty()) {
        auto valor = minHeap.top(); 
        minHeap.pop();
        cout << " -> extraido: "        << valor 
            << " | quedan: "            << minHeap.size() 
            << " | arreglo parcial : "   << minHeap << endl;
    }

    T1 valExtracc = 33;
    cout << "\nextraer: " << valExtracc << "..." << endl;
    
    bool exito = minHeap.remove(valExtracc);

    if (exito) { cout << "nuevo heap: " << minHeap << endl; } 
        else { cout << "-> error: " << valExtracc << " no esta en el heap" << endl; }
    cout << "\n";

    //MaxHeap
    cout << "*** MaxHeap : AscendingTrait ***" << endl;
    
    CHeap<AscendingTrait<T1>> maxHeap; 
    
    cout << "1. push datos ..." << endl;
    for (size_t i=0; i<numDatos; ++i) { maxHeap.push(datos[i], i); }
    cout << "MaxHeap: " << maxHeap << "\n";
    cout << "SizeHeap: " << maxHeap.size() << endl;

    cout << "\n2. pop raiz ..." << endl;
    if (!maxHeap.isEmpty()) {
        auto valor = maxHeap.top(); 
        maxHeap.pop();
        cout << " -> extraido: " << valor 
            << " | quedan: " << maxHeap.size() 
            << " | arreglo parcial: " << maxHeap << endl;
    }

    valExtracc = 17;
    cout << "\nextraer: " << valExtracc << "..." << endl;
    
    exito = maxHeap.remove(valExtracc);

    if (exito) { cout << "nuevo heap: " << maxHeap << endl; } 
        else { cout << "-> error: " << valExtracc << " no esta en el heap" << endl; }
    cout << "\n";
}
