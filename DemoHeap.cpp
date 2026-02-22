

#include "containers/heap.h"
#include <cassert>
#include <sstream>
#include <iostream>

using namespace std;


using MaxPriorityHeap = CHeap<MaxHeapConfig<int>>;
using MinPriorityHeap = CHeap<MinHeapConfig<int>>;


void VerifyMaxHeapOrdering() {
    cout << "\n[PRUEBA] Verificar Ordenamiento MaxHeap\n";
    MaxPriorityHeap heap;
    
    heap.push(5, 0);
    heap.push(2, 0);
    heap.push(7, 0);
    heap.push(3, 0);

    int result1 = heap.pop();
    int result2 = heap.pop();
    int result3 = heap.pop();
    int result4 = heap.pop();

    assert(result1 == 7);
    assert(result2 == 5);
    assert(result3 == 3);
    assert(result4 == 2);
    cout << "APROBADO: Ordenamiento del Max heap verificado\n";
}


void VerifyMinHeapOrdering() {
    cout << "\n[PRUEBA] Verificar Ordenamiento MinHeap\n";
    MinPriorityHeap heap;
    
    heap.push(8, 0);
    heap.push(4, 0);
    heap.push(6, 0);
    heap.push(1, 0);

    int res1 = heap.pop();
    int res2 = heap.pop();
    int res3 = heap.pop();
    int res4 = heap.pop();

    assert(res1 == 1);
    assert(res2 == 4);
    assert(res3 == 6);
    assert(res4 == 8);
    cout << "APROBADO: Ordenamiento del Min heap verificado\n";
}


void ProcessDuplicateValues() {
    cout << "\n[PRUEBA] Procesar Valores Duplicados\n";
    MinPriorityHeap heap;
    
    heap.push(5, 1);
    heap.push(5, 2);
    heap.push(3, 3);
    heap.push(3, 4);

    assert(heap.pop() == 3);
    assert(heap.pop() == 3);
    assert(heap.pop() == 5);
    assert(heap.pop() == 5);
    cout << "APROBADO: Valores duplicados manejados correctamente\n";
}

void HandleEmptyHeapPop() {
    cout << "\n[PRUEBA] Manejar Pop en Heap Vacío\n";
    MaxPriorityHeap heap;
    
    int defaultValue = heap.pop();
    assert(defaultValue == 0);
    cout << "APROBADO: Heap vacío retorna valor por defecto\n";
}


void TestSerializationWithIntegers() {
    cout << "\n[PRUEBA] Serialización con Enteros\n";
    MaxPriorityHeap original;
    
    original.push(9, 100);
    original.push(2, 101);
    original.push(5, 102);

    stringstream buffer;
    buffer << original;

    MaxPriorityHeap restored;
    buffer >> restored;

    
    assert(restored.pop() == 9);
    assert(restored.pop() == 5);
    assert(restored.pop() == 2);
    cout << "APROBADO: Serialización/deserialización con enteros funciona\n";
}


void TestSerializationWithStrings() {
    cout << "\n[PRUEBA] Serialización con Strings\n";
    CHeap<MaxHeapConfig<string>> original;
    
    original.push("zebra", 10);
    original.push("apple", 20);
    original.push("mango", 30);

    stringstream buffer;
    buffer << original;

    CHeap<MaxHeapConfig<string>> restored;
    buffer >> restored;

    
    string top1 = restored.pop();
    string top2 = restored.pop();
    string top3 = restored.pop();

    assert(top1 == "zebra");
    assert(top2 == "mango");
    assert(top3 == "apple");
    cout << "APROBADO: Serialización/deserialización con strings funciona\n";
}

void VerifyHeapSize() {
    cout << "\n[PRUEBA] Verificar Tamaño del Heap\n";
    MaxPriorityHeap heap;
    
    assert(heap.isEmpty() == true);
    assert(heap.size() == 0);

    heap.push(10, 0);
    assert(heap.isEmpty() == false);
    assert(heap.size() == 1);

    heap.push(20, 0);
    heap.push(30, 0);
    assert(heap.size() == 3);

    heap.pop();
    assert(heap.size() == 2);
    cout << "APROBADO: Tamaño e isEmpty verificados\n";
}


void TestCopyAndMoveSemantics() {
    cout << "\n[PRUEBA] Semántica de Copia y Movimiento\n";
    MaxPriorityHeap original;
    
    original.push(15, 5);
    original.push(8, 6);
    original.push(22, 7);

    
    MaxPriorityHeap copiedHeap(original);
    assert(copiedHeap.pop() == 22);
    assert(copiedHeap.pop() == 15);
    
    
    assert(original.pop() == 22);
    assert(original.pop() == 15);
    assert(original.pop() == 8);
    
    
    MaxPriorityHeap temp;
    temp.push(100, 0);
    temp.push(50, 0);
    
    MaxPriorityHeap movedHeap(move(temp));
    assert(movedHeap.pop() == 100);
    assert(movedHeap.pop() == 50);
    
    cout << "APROBADO: Semántica de copia y movimiento verificada\n";
}


void TestPeekElement() {
    cout << "\n[PRUEBA] Función Peek\n";
    MinPriorityHeap heap;
    
    heap.push(42, 0);
    heap.push(17, 0);
    heap.push(58, 0);

   
    int peeked = heap.peek();
    assert(peeked == 17);
    
   
    assert(heap.size() == 3);
    
    
    int popped = heap.pop();
    assert(popped == 17);
    assert(heap.size() == 2);
    
    cout << "APROBADO: Funcionalidad de peek verificada\n";
}


void TestAssignmentOperator() {
    cout << "\n[PRUEBA] Operador de Asignación\n";
    MaxPriorityHeap heap1;
    heap1.push(11, 1);
    heap1.push(22, 2);

    MaxPriorityHeap heap2;
    heap2.push(99, 99);

    
    heap2 = heap1;

    assert(heap2.pop() == 22);
    assert(heap2.pop() == 11);
    assert(heap2.size() == 0);
    
    cout << "APROBADO: Operador de asignación verificado\n";
}


void DemoHeapInteractive() {
    cout << "\n" << string(70, '=') << "\n";
    cout << "DEMOSTRACIÓN INTERACTIVA DEL HEAP\n";
    cout << string(70, '=') << "\n\n";

    cout << "--- DEMOSTRACIÓN MAX HEAP ---\n";
    cout << "Creando un Max Heap e insertando valores: 15, 8, 23, 12, 30, 5, 18\n\n";
    
    MaxPriorityHeap maxHeap;
    vector<int> values = {15, 8, 23, 12, 30, 5, 18};
    
    for (int val : values) {
        cout << "Insertando: " << val << "\n";
        maxHeap.push(val, 0);
        cout << maxHeap << "\n";
    }

    cout << "\n--- Extrayendo del Max Heap (mayor primero) ---\n";
    while (maxHeap.size() > 0) {
        int top = maxHeap.pop();
        cout << "Extraído: " << top << "\n";
        if (maxHeap.size() > 0) {
            cout << maxHeap << "\n";
        }
    }
    cout << "El heap ahora está vacío\n\n";

   
    cout << "\n--- DEMOSTRACIÓN MIN HEAP ---\n";
    cout << "Creando un Min Heap e insertando valores: 42, 17, 58, 8, 31, 25, 9\n\n";
    
    MinPriorityHeap minHeap;
    vector<int> minValues = {42, 17, 58, 8, 31, 25, 9};
    
    for (int val : minValues) {
        cout << "Insertando: " << val << "\n";
        minHeap.push(val, 0);
        cout << minHeap << "\n";
    }

    cout << "\n--- Extrayendo del Min Heap (menor primero) ---\n";
    while (minHeap.size() > 0) {
        int top = minHeap.pop();
        cout << "Extraído: " << top << "\n";
        if (minHeap.size() > 0) {
            cout << minHeap << "\n";
        }
    }
    cout << "El heap ahora está vacío\n\n";

   
    cout << "\n--- DEMOSTRACIÓN MAX HEAP CON STRINGS ---\n";
    cout << "Creando un Max Heap de strings: manzana, zapato, pelota, casa, elefante\n\n";
    
    CHeap<MaxHeapConfig<string>> stringHeap;
    vector<string> fruits = {"manzana", "zapato", "pelota", "casa", "elefante"};
    
    for (const auto &fruit : fruits) {
        cout << "Insertando: \"" << fruit << "\"\n";
        stringHeap.push(fruit, 0);
        cout << stringHeap << "\n";
    }

    cout << "\n--- Extrayendo del Heap de Strings  ---\n";
    while (stringHeap.size() > 0) {
        string top = stringHeap.pop();
        cout << "Extraído: \"" << top << "\"\n";
        if (stringHeap.size() > 0) {
            cout << stringHeap << "\n";
        }
    }
    cout << "El heap ahora está vacío\n\n";
    cout << "\n--- DEMOSTRACIÓN DE SEMÁNTICA DE COPIA Y MOVIMIENTO ---\n";
    cout << "Creando heap original con valores: 100, 50, 75, 25\n";
    
    MaxPriorityHeap original;
    original.push(100, 1);
    original.push(50, 2);
    original.push(75, 3);
    original.push(25, 4);
    cout << original << "\n";

    cout << "Creando copia del heap usando constructor copia...\n";
    MaxPriorityHeap copied(original);
    cout << "Original: " << original << "\n";
    cout << "Copia:    " << copied << "\n";

    cout << "Extrayendo de la copia para verificar independencia...\n";
    cout << "Tamaño original: " << original.size() << ", Tamaño copia: " << copied.size() << "\n";
    int val = copied.pop();
    cout << "Se extrajo " << val << " de la copia\n";
    cout << "Tamaño original: " << original.size() << ", Tamaño copia: " << copied.size() << "\n\n";

    
    cout << "\n--- DEMOSTRACIÓN DE PEEK ---\n";
    cout << "Creando heap con valores: 99, 11, 55, 33, 77\n";
    
    MaxPriorityHeap peekHeap;
    peekHeap.push(99, 0);
    peekHeap.push(11, 0);
    peekHeap.push(55, 0);
    peekHeap.push(33, 0);
    peekHeap.push(77, 0);
    cout << peekHeap << "\n";

    cout << "Usando peek() para ver el tope sin remover...\n";
    cout << "Tamaño antes de peek: " << peekHeap.size() << "\n";
    int peeked = peekHeap.peek();
    cout << "Valor obtenido con peek: " << peeked << "\n";
    cout << "Tamaño después de peek: " << peekHeap.size() << " (sin cambios)\n";
    cout << peekHeap << "\n";

    cout << "\n" << string(70, '=') << "\n";
    cout << "FIN DE LA DEMOSTRACIÓN INTERACTIVA\n";
    cout << string(70, '=') << "\n\n";
}


void DemoHeap() {
    cout << "\n" << string(60, '=') << "\n";
    cout << "SUITE DE DEMOSTRACIÓN DEL HEAP\n";
    cout << string(60, '=') << "\n";

    try {
        VerifyMaxHeapOrdering();
        VerifyMinHeapOrdering();
        ProcessDuplicateValues();
        HandleEmptyHeapPop();
        TestSerializationWithIntegers();
        TestSerializationWithStrings();
        VerifyHeapSize();
        TestCopyAndMoveSemantics();
        TestPeekElement();
        TestAssignmentOperator();

        cout << "\n" << string(60, '=') << "\n";
        cout << "¡TODAS LAS PRUEBAS PASARON EXITOSAMENTE!\n";
        cout << string(60, '=') << "\n\n";
        
       
        DemoHeapInteractive();
    } catch (const exception &e) {
        cerr << "\nTEST FAILED: " << e.what() << "\n\n";
    }
}
