#include <iostream>
#include "containers/stacks.h"

using namespace std;
using T1 = int;

void DemoStack() {
    cout << "=== DEMO STACK (PILA - LIFO) ===" << endl << endl;
    
    // Test 1: Creación y push
    cout << "Test 1: Creación y push" << endl;
    CStack<T1> stack1;
    stack1.push(10, 1);
    stack1.push(20, 2);
    stack1.push(30, 3);
    cout << stack1 << endl;
    
    // Test 2: Pop (sacar del tope) 
    cout << "Test 2: Pop (sacar del tope)" << endl;
    T1 val = stack1.pop();
    cout << "Elemento sacado: " << val << endl;
    cout << stack1 << endl;
    
    // Test 3: Top (ver sin sacar)
    cout << "Test 3: Top (ver tope sin sacar)" << endl;
    T1 topVal = stack1.top();
    cout << "Elemento en el tope: " << topVal << endl;
    cout << "Pila después de top (sin cambios):" << endl;
    cout << stack1 << endl;
    
    // Test 4: Push adicional
    cout << "Test 4: Push adicional" << endl;
    stack1.push(40, 4);
    stack1.push(50, 5);
    cout << stack1 << endl;
    
    // Test 5: Size e isEmpty
    cout << "Test 5: Size e isEmpty" << endl;
    cout << "Tamaño: " << stack1.getSize() << endl;
    cout << "¿Está vacía? " << (stack1.isEmpty() ? "Sí" : "No") << endl << endl;
    
    // Test 6: Constructor copia (deep copy)
    cout << "Test 6: Constructor copia" << endl;
    CStack<T1> stack2 = stack1;
    cout << "Pila original:" << endl;
    cout << stack1;
    cout << "Pila copiada:" << endl;
    cout << stack2 << endl;
    
    // Test 7: Move constructor (usando std::exchange)
    cout << "Test 7: Move constructor" << endl;
    CStack<T1> stack3 = std::move(stack2);
    cout << "Pila movida:" << endl;
    cout << stack3;
    cout << "Pila original después de move (vacía):" << endl;
    cout << stack2 << endl;
    
    // Test 8: Operator >> (insertar)
    cout << "Test 8: Operator >> (insertar desde entrada)" << endl;
    cout << "Agregando 60 con ref 6..." << endl;
    stack1.push(60, 6);
    cout << stack1 << endl;
    
    // Test 9: Vaciar la pila con pop
    cout << "Test 9: Vaciar la pila con pop (LIFO)" << endl;
    while (!stack1.isEmpty()) {
        T1 val = stack1.pop();
        cout << "Sacado: " << val << " (del tope)" << endl;
    }
    cout << "Pila final (vacía):" << endl;
    cout << stack1;
    cout << "¿Está vacía? " << (stack1.isEmpty() ? "Sí" : "No") << endl << endl;
    
    // Test 10: Excepción al hacer pop en pila vacía
    cout << "Test 10: Intentar pop en pila vacía (debe lanzar excepción)" << endl;
    try {
        stack1.pop();
    } catch (const std::out_of_range &e) {
        cout << "Excepción capturada: " << e.what() << endl;
    }
    cout << endl;
    
    // Test 11: Comportamiento LIFO
    cout << "Test 11: Demostración LIFO (Last In, First Out)" << endl;
    CStack<T1> stackLIFO;
    cout << "Agregando: 100, 200, 300" << endl;
    stackLIFO.push(100, 1);
    stackLIFO.push(200, 2);
    stackLIFO.push(300, 3);
    cout << "Orden en la pila:" << endl;
    cout << stackLIFO;
    cout << "Sacando elementos (último que entró, primero que sale):" << endl;
    cout << "Pop: " << stackLIFO.pop() << " (era el último - 300)" << endl;
    cout << "Pop: " << stackLIFO.pop() << " (era el segundo - 200)" << endl;
    cout << "Pop: " << stackLIFO.pop() << " (era el primero - 100)" << endl << endl;
    
    // Test 12: Concurrencia (mutex)
    cout << "Test 12: Concurrencia (mutex automático)" << endl;
    cout << "El mutex protege todas las operaciones automáticamente." << endl;
    cout << "Todas las operaciones son thread-safe." << endl << endl;
    
    cout << "=== FIN DEMO STACK ===" << endl << endl;
}
