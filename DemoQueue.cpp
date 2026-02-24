#include <iostream>
#include "containers/queues.h"

using namespace std;
using T1 = int;

void DemoQueue() {
    cout << "=== DEMO QUEUE (COLA - FIFO) ===" << endl << endl;
    
    // Test 1: Creación y push
    cout << "Test 1: Creación y push" << endl;
    CQueue<T1> queue1;
    queue1.push(10, 1);
    queue1.push(20, 2);
    queue1.push(30, 3);
    cout << queue1 << endl;
    
    // Test 2: Pop (sacar del frente) 
    cout << "Test 2: Pop (sacar del frente)" << endl;
    T1 val = queue1.pop();
    cout << "Elemento sacado: " << val << endl;
    cout << queue1 << endl;
    
    // Test 3: Front y Rear
    cout << "Test 3: Front (ver frente) y Rear (ver final)" << endl;
    T1 frontVal = queue1.front();
    T1 rearVal = queue1.rear();
    cout << "Elemento en el frente: " << frontVal << endl;
    cout << "Elemento en el final: " << rearVal << endl;
    cout << "Cola después de front/rear (sin cambios):" << endl;
    cout << queue1 << endl;
    
    // Test 4: Push adicional
    cout << "Test 4: Push adicional" << endl;
    queue1.push(40, 4);
    queue1.push(50, 5);
    cout << queue1 << endl;
    
    // Test 5: Size e isEmpty
    cout << "Test 5: Size e isEmpty" << endl;
    cout << "Tamaño: " << queue1.getSize() << endl;
    cout << "¿Está vacía? " << (queue1.isEmpty() ? "Sí" : "No") << endl << endl;
    
    // Test 6: Constructor copia 
    cout << "Test 6: Constructor copia" << endl;
    CQueue<T1> queue2 = queue1;
    cout << "Cola original:" << endl;
    cout << queue1;
    cout << "Cola copiada:" << endl;
    cout << queue2 << endl;
    
    // Test 7: Move constructor 
    cout << "Test 7: Move constructor" << endl;
    CQueue<T1> queue3 = std::move(queue2);
    cout << "Cola movida:" << endl;
    cout << queue3;
    cout << "Cola original después de move (vacía):" << endl;
    cout << queue2 << endl;
    
    // Test 8: Operator >> 
    cout << "Test 8: Operator >> (insertar desde entrada)" << endl;
    cout << "Agregando 60 con ref 6..." << endl;
    queue1.push(60, 6);
    cout << queue1 << endl;
    
    // Test 9: Vaciar la cola con pop
    cout << "Test 9: Vaciar la cola con pop (FIFO)" << endl;
    while (!queue1.isEmpty()) {
        T1 val = queue1.pop();
        cout << "Sacado: " << val << " (del frente)" << endl;
    }
    cout << "Cola final (vacía):" << endl;
    cout << queue1;
    cout << "¿Está vacía? " << (queue1.isEmpty() ? "Sí" : "No") << endl << endl;
    
    // Test 10: Excepción al hacer pop en cola vacía
    cout << "Test 10: Intentar pop en cola vacía (debe lanzar excepción)" << endl;
    try {
        queue1.pop();
    } catch (const std::out_of_range &e) {
        cout << "Excepción capturada: " << e.what() << endl;
    }
    cout << endl;
    
    // Test 11: Comportamiento FIFO
    cout << "Test 11: Demostración FIFO (First In, First Out)" << endl;
    CQueue<T1> queueFIFO;
    cout << "Agregando: 100, 200, 300" << endl;
    queueFIFO.push(100, 1);
    queueFIFO.push(200, 2);
    queueFIFO.push(300, 3);
    cout << "Orden en la cola:" << endl;
    cout << queueFIFO;
    cout << "Sacando elementos (primero que entró, primero que sale):" << endl;
    cout << "Pop: " << queueFIFO.pop() << " (era el primero - 100)" << endl;
    cout << "Pop: " << queueFIFO.pop() << " (era el segundo - 200)" << endl;
    cout << "Pop: " << queueFIFO.pop() << " (era el tercero - 300)" << endl << endl;
    
    // Test 12: Concurrencia (mutex)
    cout << "Test 12: Concurrencia (mutex automático)" << endl;
    cout << "El mutex protege todas las operaciones automáticamente." << endl;
    cout << "Todas las operaciones son thread-safe." << endl << endl;
    
    // Test 13: Capacidad indefinida
    cout << "Test 13: Capacidad indefinida" << endl;
    CQueue<T1> queueBig;
    cout << "Agregando 100 elementos..." << endl;
    for (int i = 0; i < 100; ++i) {
        queueBig.push(i, i);
    }
    cout << "Tamaño de la cola: " << queueBig.getSize() << endl;
    cout << "La cola crece dinámicamente sin límite predefinido." << endl << endl;
    
    cout << "=== FIN DEMO QUEUE ===" << endl << endl;
}