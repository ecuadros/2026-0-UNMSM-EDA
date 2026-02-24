#include <iostream>
#include "containers/heap.h"

using namespace std;

void DemoHeap() {
    cout << "========================================" << endl;
    cout << "         DEMO: HEAP (MAX Y MIN)" << endl;
    cout << "========================================" << endl << endl;
    
   
    cout << "========================================" << endl;
    cout << "         PARTE 1: MAX-HEAP" << endl;
    cout << "========================================" << endl << endl;
    
    // 1. Crear Max-Heap y hacer Push
    cout << "1. Crear Max-Heap y hacer Push:" << endl;
    MaxHeap<int> maxHeap;
    maxHeap.Push(10);
    maxHeap.Push(30);
    maxHeap.Push(20);
    maxHeap.Push(50);
    maxHeap.Push(40);
    maxHeap.Push(15);
    cout << maxHeap << endl;
    
    // 2. Top - Ver la raíz (máximo)
    cout << "2. Ver la raiz (maximo) con Top():" << endl;
    cout << "Top (maximo): " << maxHeap.Top() << endl << endl;
    
    // 3. Pop - Extraer el máximo
    cout << "3. Extraer elementos (siempre el maximo):" << endl;
    cout << "Elemento extraido: " << maxHeap.Pop() << endl;
    cout << "Elemento extraido: " << maxHeap.Pop() << endl;
    cout << "Max-Heap despues de 2 Pops: " << maxHeap << endl;
    
    // 4. getSize
    cout << "4. Tamanio del Max-Heap:" << endl;
    cout << "maxHeap.getSize() = " << maxHeap.getSize() << endl << endl;
    
    // 5. Demostrar propiedad Max-Heap
    cout << "5. Demostracion Max-Heap (extrae en orden descendente):" << endl;
    MaxHeap<int> maxHeap2;
    maxHeap2.Push(5);
    maxHeap2.Push(15);
    maxHeap2.Push(10);
    maxHeap2.Push(30);
    maxHeap2.Push(20);
    
    cout << "Insertados: 5, 15, 10, 30, 20" << endl;
    cout << "Extrayendo (siempre el mayor): ";
    while (!maxHeap2.isEmpty()) {
        cout << maxHeap2.Pop() << " ";
    }
    cout << endl;
    cout << "Orden: 30, 20, 15, 10, 5 (descendente)" << endl << endl;
    
    
    cout << "========================================" << endl;
    cout << "         PARTE 2: MIN-HEAP" << endl;
    cout << "========================================" << endl << endl;
    
    // 6. Crear Min-Heap y hacer Push
    cout << "6. Crear Min-Heap y hacer Push:" << endl;
    MinHeap<int> minHeap;
    minHeap.Push(10);
    minHeap.Push(30);
    minHeap.Push(20);
    minHeap.Push(50);
    minHeap.Push(40);
    minHeap.Push(15);
    cout << minHeap << endl;
    
    // 7. Top - Ver la raíz (mínimo)
    cout << "7. Ver la raiz (minimo) con Top():" << endl;
    cout << "Top (minimo): " << minHeap.Top() << endl << endl;
    
    // 8. Pop - Extraer el mínimo
    cout << "8. Extraer elementos (siempre el minimo):" << endl;
    cout << "Elemento extraido: " << minHeap.Pop() << endl;
    cout << "Elemento extraido: " << minHeap.Pop() << endl;
    cout << "Min-Heap despues de 2 Pops: " << minHeap << endl;
    
    // 9. Demostrar propiedad Min-Heap
    cout << "9. Demostracion Min-Heap (extrae en orden ascendente):" << endl;
    MinHeap<int> minHeap2;
    minHeap2.Push(5);
    minHeap2.Push(15);
    minHeap2.Push(10);
    minHeap2.Push(30);
    minHeap2.Push(20);
    
    cout << "Insertados: 5, 15, 10, 30, 20" << endl;
    cout << "Extrayendo (siempre el menor): ";
    while (!minHeap2.isEmpty()) {
        cout << minHeap2.Pop() << " ";
    }
    cout << endl;
    cout << "Orden: 5, 10, 15, 20, 30 (ascendente)" << endl << endl;
    
  
    cout << "========================================" << endl;
    cout << "      COMPARACION MAX-HEAP vs MIN-HEAP" << endl;
    cout << "========================================" << endl << endl;
    
    cout << "Max-Heap:" << endl;
    cout << "  - Raiz: MAYOR elemento" << endl;
    cout << "  - Pop: Extrae el MAXIMO" << endl;
    cout << "  - Orden: Descendente" << endl;
    cout << "  - Uso: Priority Queue (mayor prioridad primero)" << endl << endl;
    
    cout << "Min-Heap:" << endl;
    cout << "  - Raiz: MENOR elemento" << endl;
    cout << "  - Pop: Extrae el MINIMO" << endl;
    cout << "  - Orden: Ascendente" << endl;
    cout << "  - Uso: Dijkstra, Heap Sort ascendente" << endl << endl;
    
    // 10. Constructor Copia
    cout << "10. Constructor Copia (Max-Heap):" << endl;
    MaxHeap<int> maxHeap3;
    maxHeap3.Push(100);
    maxHeap3.Push(50);
    maxHeap3.Push(75);
    
    MaxHeap<int> maxHeapCopy(maxHeap3);
    cout << "Heap original: " << maxHeap3;
    cout << "Heap copiado:  " << maxHeapCopy << endl;
    
    // 11. Move Constructor
    cout << "11. Move Constructor (Min-Heap):" << endl;
    MinHeap<int> minHeap3;
    minHeap3.Push(10);
    minHeap3.Push(5);
    minHeap3.Push(8);
    
    MinHeap<int> minHeapMoved(std::move(minHeap3));
    cout << "Heap movido: " << minHeapMoved;
    cout << "Heap original (vacio): " << minHeap3 << endl;
    
    // 12. Operator < y >
    cout << "12. Comparacion de heaps (operator < y >):" << endl;
    MaxHeap<int> h1;
    h1.Push(1);
    h1.Push(2);
    
    MaxHeap<int> h2;
    h2.Push(1);
    h2.Push(2);
    h2.Push(3);
    h2.Push(4);
    
    cout << "h1 size: " << h1.getSize() << endl;
    cout << "h2 size: " << h2.getSize() << endl;
    cout << "h1 < h2: " << (h1 < h2 ? "true" : "false") << endl;
    cout << "h1 > h2: " << (h1 > h2 ? "true" : "false") << endl << endl;
    
    // 13. Uso práctico: Priority Queue
    cout << "13. Uso practico: Priority Queue (Max-Heap):" << endl;
    MaxHeap<int> priorityQueue;
    
    cout << "Tareas con prioridad:" << endl;
    priorityQueue.Push(5);  cout << "  - Tarea A (prioridad 5)" << endl;
    priorityQueue.Push(10); cout << "  - Tarea B (prioridad 10)" << endl;
    priorityQueue.Push(3);  cout << "  - Tarea C (prioridad 3)" << endl;
    priorityQueue.Push(8);  cout << "  - Tarea D (prioridad 8)" << endl;
    
    cout << "\nEjecutando tareas (mayor prioridad primero):" << endl;
    int taskNum = 1;
    while (!priorityQueue.isEmpty()) {
        cout << "  " << taskNum++ << ". Ejecutando tarea con prioridad: " 
             << priorityQueue.Pop() << endl;
    }
    cout << endl;
    
    // 14. Clear
    cout << "14. Limpiar heap con Clear():" << endl;
    maxHeap.Clear();
    cout << "Despues de Clear(): " << maxHeap;
    cout << "Esta vacio? " << (maxHeap.isEmpty() ? "SI" : "NO") << endl << endl;
    
    cout << "========================================" << endl;
    cout << "           FIN DEMO HEAP" << endl;
    cout << "========================================" << endl << endl;
}
