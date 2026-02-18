#include "containers/heap.h"
#include <thread>
#include <vector>

void DemoHeap() {
    cout << "1. MIN HEAP DEMO " << endl;
    CHeap<MinHeapTrait<int>> minHeap;
    minHeap.push(50); minHeap.push(10); minHeap.push(30); minHeap.push(5);
    cout << "MinHeap despues de push(50, 10, 30, 5): " << minHeap << endl;
    cout << "Pop (debe ser 5): " << minHeap.pop() << endl;

    cout << "\n 2. MAX HEAP DEMO " << endl;
    CHeap<MaxHeapTrait<int>> maxHeap;
    maxHeap.push(50); maxHeap.push(10); maxHeap.push(30); maxHeap.push(5);
    cout << "MaxHeap despues de push(50, 10, 30, 5): " << maxHeap << endl;
    cout << "Pop (debe ser 50): " << maxHeap.pop() << endl;

    cout << "\n 3. CONSTRUCTOR COPIA Y MOVE " << endl;
    CHeap<MaxHeapTrait<int>> original;
    original.push(100);
    
    CHeap<MaxHeapTrait<int>> copia(original); // Copy
    cout << "Copia del original: " << copia << endl;
    
    CHeap<MaxHeapTrait<int>> movido(std::move(original)); // Move
    cout << "Objeto movido: " << movido << endl;
    cout << "Original post-move (size): " << original.size() << endl;

    cout << "\n 4. CONCURRENCIA (Simulacion simple) " << endl;
    CHeap<MinHeapTrait<int>> concurrentHeap;
    
    auto t1 = thread([&]() {
        for(int i=0; i<10; ++i) concurrentHeap.push(i);
    });
    
    auto t2 = thread([&]() {
        for(int i=10; i<20; ++i) concurrentHeap.push(i);
    });

    t1.join(); t2.join();
    cout << "Size final tras 2 hilos insertando: " << concurrentHeap.size() << " (esperado 20)" << endl;

    cout << "\n 5. OPERADOR >> (Input Stream)" << endl;
    cout << "Escribe un numero para insertar en el heap: ";
    cin >> concurrentHeap;
    cout << "Heap actualizado: " << concurrentHeap << endl;
}
