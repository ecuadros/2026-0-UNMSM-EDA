#include <iostream>
#include "containers/dlists.h"
#include "variadic-util.h"

using namespace std;
using T1 = int;

template <typename T>
bool Mult7(T &elem){
    return elem % 7 == 0;
}

template <typename Q>
void Print(Q &elem) { 
    cout << elem << ","; 
}

void DemoDoubleLists(){
    cout << "=== DEMO DOUBLE LINKED LIST ===" << endl << endl;
    
    // Test 1: Creación y push_back
    cout << "Test 1: Creación y push_back" << endl;
    CDoubleLinkedList< DAscendingTrait<T1> > dl1;
    dl1.push_back(10, 6);
    dl1.push_back(50, 8);
    dl1.push_back(70, 9);
    cout << dl1 << endl;
    
    // Test 2: Insert ordenado
    cout << "Test 2: Insert ordenado" << endl;
    dl1.Insert(20, 4);
    dl1.Insert(30, 3);
    dl1.Insert(5, 1);
    cout << dl1 << endl;
    
    // Test 3: Foreach con Print (Forward)
    cout << "Test 3: Foreach con Print (Forward)" << endl;
    ::Foreach(dl1.begin(), dl1.end(), &Print<T1>);
    cout << endl << endl;
    
    // Test 4: Backward Iterator
    cout << "Test 4: Backward Iterator (rbegin -> rend)" << endl;
    ::Foreach(dl1.rbegin(), dl1.rend(), &Print<T1>);
    cout << endl << endl;
    
    // Test 5: FirstThat
    cout << "Test 5: FirstThat - Primer múltiplo de 7" << endl;
    auto iter = dl1.FirstThat(&Mult7<T1>);
    if (iter != dl1.end()) {
        cout << "El primer múltiplo de 7 es: " << *iter << endl;
    } else {
        cout << "No se encontró ningún múltiplo de 7" << endl;
    }
    cout << endl;
    
    // Test 6: Operator[]
    cout << "Test 6: Operator[] - Acceso por índice" << endl;
    try {
        cout << "Elemento en índice 2: " << dl1[2] << endl;
        cout << "Elemento en índice 4: " << dl1[4] << endl;
        dl1[3] = 100;
        cout << "Después de modificar índice 3 a 100:" << endl;
        cout << dl1 << endl;
    } catch (const std::exception &e) {
        cout << "Error: " << e.what() << endl;
    }
    cout << endl;
    
    // Test 7: push_front
    cout << "Test 7: push_front (agregar al inicio)" << endl;
    dl1.push_front(1, 0);
    cout << dl1 << endl;
    
    // Test 8: Constructor copia
    cout << "Test 8: Constructor copia" << endl;
    CDoubleLinkedList< DAscendingTrait<T1> > dl2(dl1);
    cout << "Lista original dl1:" << endl;
    cout << dl1 << endl;
    cout << "Lista copiada dl2:" << endl;
    cout << dl2 << endl;
    
    // Test 9: Move constructor
    cout << "Test 9: Move constructor" << endl;
    CDoubleLinkedList< DAscendingTrait<T1> > dl3(std::move(dl2));
    cout << "Lista movida dl3:" << endl;
    cout << dl3 << endl;
    cout << "Lista original dl2 (debería estar vacía):" << endl;
    cout << "Size de dl2: " << dl2.getSize() << endl << endl;
    
    // Test 10: Persistencia - Guardar en archivo
    cout << "Test 10: Persistencia - Guardar en archivo" << endl;
    try {
        dl1.SaveToFile("doublelinkedlist_data.txt");
        cout << "Lista guardada exitosamente en doublelinkedlist_data.txt" << endl;
    } catch (const std::exception &e) {
        cout << "Error al guardar: " << e.what() << endl;
    }
    cout << endl;
    
    // Test 11: Persistencia - Cargar desde archivo
    cout << "Test 11: Persistencia - Cargar desde archivo" << endl;
    CDoubleLinkedList< DAscendingTrait<T1> > dl4;
    try {
        dl4.LoadFromFile("doublelinkedlist_data.txt");
        cout << "Lista cargada exitosamente desde archivo:" << endl;
        cout << dl4 << endl;
    } catch (const std::exception &e) {
        cout << "Error al cargar: " << e.what() << endl;
    }
    cout << endl;
    
    // Test 12: Foreach con incremento
    cout << "Test 12: Foreach con incremento" << endl;
    dl1.Foreach(&IncBasico<T1>);
    cout << "Después de incrementar cada elemento:" << endl;
    cout << dl1 << endl;
    
    // Test 13: Foreach con suma
    cout << "Test 13: Foreach con Suma" << endl;
    dl1.Foreach(&Suma<T1>, 10);
    cout << "Después de sumar 10 a cada elemento:" << endl;
    cout << dl1 << endl;
    
    // Test 14: Verificar backward iterator con datos modificados
    cout << "Test 14: Backward Iterator después de modificaciones" << endl;
    cout << "Forward:  ";
    ::Foreach(dl1.begin(), dl1.end(), &Print<T1>);
    cout << endl;
    cout << "Backward: ";
    ::Foreach(dl1.rbegin(), dl1.rend(), &Print<T1>);
    cout << endl << endl;
    
    cout << "=== FIN DEMO DOUBLE LINKED LIST ===" << endl;
}
