#include <iostream>
#include "containers/lists.h"
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

void DemoLists(){
    cout << "=== DEMO LINKED LIST ===" << endl << endl;
    
    // Test 1: Creación y push_back
    cout << "Test 1: Creación y push_back" << endl;
    CLinkedList< AscendingTrait<T1> > l1;
    l1.push_back(10, 6);
    l1.push_back(50, 8);
    l1.push_back(70, 9);
    cout << l1 << endl;
    
    // Test 2: Insert ordenado
    cout << "Test 2: Insert ordenado" << endl;
    l1.Insert(20, 4);
    l1.Insert(30, 3);
    l1.Insert(5, 1);
    cout << l1 << endl;
    
    // Test 3: Foreach con Print
    cout << "Test 3: Foreach con Print" << endl;
    ::Foreach(l1.begin(), l1.end(), &Print<T1>);
    cout << endl << endl;
    
    // Test 4: Foreach con Mult7
    cout << "Test 4: FirstThat - Buscar múltiplos de 7" << endl;
    ::Foreach(l1.begin(), l1.end(), &Mult7<T1>);
    cout << endl << endl;
    
    // Test 5: FirstThat
    cout << "Test 5: FirstThat - Primer múltiplo de 7" << endl;
    auto iter = l1.FirstThat(&Mult7<T1>);
    if (iter != l1.end()) {
        cout << "El primer múltiplo de 7 es: " << *iter << endl;
    } else {
        cout << "No se encontró ningún múltiplo de 7" << endl;
    }
    cout << endl;
    
    // Test 6: Operator[]
    cout << "Test 6: Operator[] - Acceso por índice" << endl;
    try {
        cout << "Elemento en índice 2: " << l1[2] << endl;
        cout << "Elemento en índice 4: " << l1[4] << endl;
        l1[3] = 100;
        cout << "Después de modificar índice 3 a 100:" << endl;
        cout << l1 << endl;
    } catch (const std::exception &e) {
        cout << "Error: " << e.what() << endl;
    }
    cout << endl;
    
    // Test 7: Constructor copia
    cout << "Test 7: Constructor copia" << endl;
    CLinkedList< AscendingTrait<T1> > l2(l1);
    cout << "Lista original l1:" << endl;
    cout << l1 << endl;
    cout << "Lista copiada l2:" << endl;
    cout << l2 << endl;
    
    // Test 8: Move constructor
    cout << "Test 8: Move constructor" << endl;
    CLinkedList< AscendingTrait<T1> > l3(std::move(l2));
    cout << "Lista movida l3:" << endl;
    cout << l3 << endl;
    cout << "Lista original l2 (debería estar vacía):" << endl;
    cout << "Size de l2: " << l2.getSize() << endl << endl;
    
    // Test 9: Persistencia - Guardar en archivo
    cout << "Test 9: Persistencia - Guardar en archivo" << endl;
    try {
        l1.SaveToFile("linkedlist_data.txt");
        cout << "Lista guardada exitosamente en linkedlist_data.txt" << endl;
    } catch (const std::exception &e) {
        cout << "Error al guardar: " << e.what() << endl;
    }
    cout << endl;
    
    // Test 10: Persistencia - Cargar desde archivo
    cout << "Test 10: Persistencia - Cargar desde archivo" << endl;
    CLinkedList< AscendingTrait<T1> > l4;
    try {
        l4.LoadFromFile("linkedlist_data.txt");
        cout << "Lista cargada exitosamente desde archivo:" << endl;
        cout << l4 << endl;
    } catch (const std::exception &e) {
        cout << "Error al cargar: " << e.what() << endl;
    }
    cout << endl;
    
    // Test 11: Foreach con modificación
    cout << "Test 11: Foreach con incremento" << endl;
    l1.Foreach(&IncBasico<T1>);
    cout << "Después de incrementar cada elemento:" << endl;
    cout << l1 << endl;
    
    // Test 12: Foreach con suma
    cout << "Test 12: Foreach con Suma" << endl;
    l1.Foreach(&Suma<T1>, 10);
    cout << "Después de sumar 10 a cada elemento:" << endl;
    cout << l1 << endl;
    
    cout << "=== FIN DEMO LINKED LIST ===" << endl;
}

