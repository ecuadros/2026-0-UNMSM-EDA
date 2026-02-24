#include <iostream>
#include "containers/heap.h"

using namespace std;
using T1 = int;

void DemoHeaps() {
    cout << " DEMO HEAPS " << endl << endl;
    

    
    cout << "MAX HEAP " << endl << endl;
    
 
    cout << "Test: Creacion y push en MaxHeap" << endl;
    CMaxHeap<T1> maxHeap;  
    maxHeap.push(10);
    maxHeap.push(30);
    maxHeap.push(20);
    maxHeap.push(50);
    maxHeap.push(40);
    cout << maxHeap << endl;
    

    cout << "Test: Pop en MaxHeap " << endl;
    T1 val = maxHeap.pop();
    cout << "Elemento sacado: " << val << endl;
    cout << maxHeap << endl;
    
    
    cout << "Test 3: Top en MaxHeap" << endl;
    T1 topVal = maxHeap.top();
    cout << "Elemento maximo: " << topVal << endl;
    cout << "MaxHeap despues de top :" << endl;
    cout << maxHeap << endl;
    
    // Test: Constructor copia
    cout << "Test 4: Constructor copia en MaxHeap" << endl;
    CMaxHeap<T1> maxHeap2 = maxHeap;
    cout << "MaxHeap original:" << endl;
    cout << maxHeap;
    cout << "MaxHeap copiado:" << endl;
    cout << maxHeap2 << endl;
    
    // Test: Move constructor 
    cout << "Test 5: Move constructor en MaxHeap" << endl;
    CMaxHeap<T1> maxHeap3 = std::move(maxHeap2);
    cout << "MaxHeap movido:" << endl;
    cout << maxHeap3;
    cout << "MaxHeap original despues de move:" << endl;
    cout << maxHeap2 << endl;
    
    // Test: Vaciar heap
    cout << "Test 6: Vaciar MaxHeap con pop" << endl;
    while (!maxHeap.isEmpty()) {
        T1 sacado = maxHeap.pop(); 
        cout << "Sacado (maximo): " << sacado << endl;
    }
    cout << "MaxHeap vacio:" << endl;
    cout << maxHeap;
    
    cout << "¿Esta vacio? ";
    if (maxHeap.isEmpty()) {
        cout << "Si" << endl << endl;
    } else {
        cout << "No" << endl << endl;
    }
    
    
    cout << "Test: Intentar pop en MaxHeap vacio" << endl;
    try {
        maxHeap.pop();
    } catch (const std::out_of_range &e) {
        cout << "Excepcion capturada: " << e.what() << endl;
    }
    cout << endl;
    
    

    
    cout << "MIN HEAP " << endl << endl;
    
   
    cout << "Test: Creacion en MinHeap" << endl;
    CMinHeap<T1> minHeap;  
    minHeap.push(10);
    minHeap.push(30);
    minHeap.push(20);
    minHeap.push(50);
    minHeap.push(40);
    cout << minHeap << endl;
    
    
    cout << "Test: Pop en MinHeap " << endl;
    val = minHeap.pop();
    cout << "Elemento sacado: " << val << endl;
    cout << minHeap << endl;
    
    
    cout << "Test: Top en MinHeap " << endl;
    topVal = minHeap.top();
    cout << "Elemento minimo: " << topVal << endl;
    cout << "MinHeap despues de top:" << endl;
    cout << minHeap << endl;
    
    
    cout << "Test: Constructor copia en MinHeap" << endl;
    CMinHeap<T1> minHeap2 = minHeap;
    cout << "MinHeap original:" << endl;
    cout << minHeap;
    cout << "MinHeap copiado:" << endl;
    cout << minHeap2 << endl;
    
 
    cout << "Test: Move constructor en MinHeap " << endl;
    CMinHeap<T1> minHeap3 = std::move(minHeap2);
    cout << "MinHeap movido:" << endl;
    cout << minHeap3;
    cout << "MinHeap original despues de move :" << endl;
    cout << minHeap2 << endl;
    
    
    cout << "Test: Vaciar MinHeap con pop" << endl;
    while (!minHeap.isEmpty()) {
        T1 sacado = minHeap.pop();
        cout << "Sacado (minimo): " << sacado << endl;
    }
    cout << "MinHeap vacio:" << endl;
    cout << minHeap;
    
    cout << "¿Esta vacio? ";
    if (minHeap.isEmpty()) {
        cout << "Si" << endl << endl;
    } else {
        cout << "No" << endl << endl;
    }
    
  
    cout << "Test: Comparacion MaxHeap vs MinHeap" << endl;
    CMaxHeap<T1> maxDemo;
    CMinHeap<T1> minDemo;
    
    cout << "Insertando mismos valores: 15, 10, 20, 8, 25" << endl;
    int values[] = {15, 10, 20, 8, 25};
    
    for (int v : values) {
        maxDemo.push(v);
        minDemo.push(v);
    }
    
    cout << "\nMaxHeap :" << endl;
    cout << maxDemo;
    cout << "Pop en MaxHeap: " << maxDemo.pop() << " (era el maximo)" << endl << endl;
    
    cout << "MinHeap :" << endl;
    cout << minDemo;
    cout << "Pop en MinHeap: " << minDemo.pop() << " (era el minimo)" << endl << endl;



    cout << "Test : Operadores de comparacion (<, >, ==) por tamano" << endl;
    CMaxHeap<T1> h1;
    CMaxHeap<T1> h2;
    
    h1.push(10);
    h1.push(20);
    
    h2.push(10);
    h2.push(20);
    h2.push(30);
    
    cout << "Tamano h1: " << h1.getSize() << endl;
    cout << "Tamano h2: " << h2.getSize() << endl;
    
    cout << " es ¿h1 == h2 ? ";
    if (h1 == h2) {
        cout << "Si" << endl;
    } else {
        cout << "No" << endl;
    }
    
    cout << "es ¿h1 < h2 ? ";
    if (h1 < h2) {
        cout << "Si" << endl;
    } else {
        cout << "No" << endl;
    }
    
    cout << "es ¿h1 > h2 ? ";
    if (h1 > h2) {
        cout << "Si" << endl;
    } else {
        cout << "No" << endl;
    }
    cout << endl;

    cout << "=== FIN DEMO HEAPS ===" << endl << endl;
}