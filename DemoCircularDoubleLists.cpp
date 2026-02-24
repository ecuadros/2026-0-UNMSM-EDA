#include <iostream>
#include "containers/cdlists.h"
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

void DemoCircularDoubleLists(){
    cout << "=== DEMO CIRCULAR DOUBLE LINKED LIST ===" << endl << endl;
    
    // Test 1: Creación y push_back
    cout << "Test 1: Creación y push_back" << endl;
    CCircularDoubleLinkedList< CDAscendingTrait<T1> > cdl1;
    cdl1.push_back(10, 6);
    cdl1.push_back(50, 8);
    cdl1.push_back(70, 9);
    cout << cdl1 << endl;
    
    // Test 2: Insert ordenado
    cout << "Test 2: Insert ordenado" << endl;
    cdl1.Insert(20, 4);
    cdl1.Insert(30, 3);
    cdl1.Insert(5, 1);
    cout << cdl1 << endl;
    
    // Test 3: Foreach con Print (Forward)
    cout << "Test 3: Foreach con Print (Forward)" << endl;
    ::Foreach(cdl1.begin(), cdl1.end(), &Print<T1>);
    cout << endl << endl;
    
    // Test 4:  NUEVO - Backward Iterator
    cout << "Test 4: Backward Iterator (rbegin -> rend)" << endl;
    ::Foreach(cdl1.rbegin(), cdl1.rend(), &Print<T1>);
    cout << endl << endl;
    
    // Test 5: FirstThat
    cout << "Test 5: FirstThat - Primer múltiplo de 7" << endl;
    auto iter = cdl1.FirstThat(&Mult7<T1>);
    if (iter != cdl1.end()) {
        cout << "El primer múltiplo de 7 es: " << *iter << endl;
    } else {
        cout << "No se encontró ningún múltiplo de 7" << endl;
    }
    cout << endl;
    
    // Test 6: Operator[]
    cout << "Test 6: Operator[] - Acceso por índice" << endl;
    try {
        cout << "Elemento en índice 2: " << cdl1[2] << endl;
        cout << "Elemento en índice 4: " << cdl1[4] << endl;
        cdl1[3] = 100;
        cout << "Después de modificar índice 3 a 100:" << endl;
        cout << cdl1 << endl;
    } catch (const std::exception &e) {
        cout << "Error: " << e.what() << endl;
    }
    cout << endl;
    
    // Test 7: NUEVO - push_front
    cout << "Test 7: push_front (agregar al inicio)" << endl;
    cdl1.push_front(1, 0);
    cout << cdl1 << endl;
    
    // Test 8: Constructor copia
    cout << "Test 8: Constructor copia" << endl;
    CCircularDoubleLinkedList< CDAscendingTrait<T1> > cdl2(cdl1);
    cout << "Lista original cdl1:" << endl;
    cout << cdl1 << endl;
    cout << "Lista copiada cdl2:" << endl;
    cout << cdl2 << endl;
    
    // Test 9: Move constructor
    cout << "Test 9: Move constructor" << endl;
    CCircularDoubleLinkedList< CDAscendingTrait<T1> > cdl3(std::move(cdl2));
    cout << "Lista movida cdl3:" << endl;
    cout << cdl3 << endl;
    cout << "Lista original cdl2 (debería estar vacía):" << endl;
    cout << "Size de cdl2: " << cdl2.getSize() << endl << endl;
    
    // Test 10: Persistencia - Guardar en archivo
    cout << "Test 10: Persistencia - Guardar en archivo" << endl;
    try {
        cdl1.SaveToFile("circulardoublelist_data.txt");
        cout << "Lista circular doble guardada exitosamente en circulardoublelist_data.txt" << endl;
    } catch (const std::exception &e) {
        cout << "Error al guardar: " << e.what() << endl;
    }
    cout << endl;
    
    // Test 11: Persistencia - Cargar desde archivo
    cout << "Test 11: Persistencia - Cargar desde archivo" << endl;
    CCircularDoubleLinkedList< CDAscendingTrait<T1> > cdl4;
    try {
        cdl4.LoadFromFile("circulardoublelist_data.txt");
        cout << "Lista circular doble cargada exitosamente desde archivo:" << endl;
        cout << cdl4 << endl;
    } catch (const std::exception &e) {
        cout << "Error al cargar: " << e.what() << endl;
    }
    cout << endl;
    
    // Test 12: Foreach con incremento
    cout << "Test 12: Foreach con incremento" << endl;
    cdl1.Foreach(&IncBasico<T1>);
    cout << "Después de incrementar cada elemento:" << endl;
    cout << cdl1 << endl;
    
    // Test 13: Foreach con suma
    cout << "Test 13: Foreach con Suma" << endl;
    cdl1.Foreach(&Suma<T1>, 10);
    cout << "Después de sumar 10 a cada elemento:" << endl;
    cout << cdl1 << endl;
    
    // Test 14: Verificar circularidad (recorrer 2 vueltas forward)
    cout << "Test 14: Verificar circularidad forward (2 vueltas)" << endl;
    cout << "Elementos (2 vueltas): ";
    if (cdl1.getSize() > 0) {
        for (int i = 0; i < static_cast<int>(cdl1.getSize() * 2); ++i) {
            cout << cdl1[i % cdl1.getSize()] << " ";
        }
    }
    cout << endl << endl;
    
    // Test 15: Verificar circularidad backward
    cout << "Test 15: Verificar circularidad backward" << endl;
    cout << "Forward:  ";
    ::Foreach(cdl1.begin(), cdl1.end(), &Print<T1>);
    cout << endl;
    cout << "Backward: ";
    ::Foreach(cdl1.rbegin(), cdl1.rend(), &Print<T1>);
    cout << endl << endl;
    
    cout << "=== FIN DEMO CIRCULAR DOUBLE LINKED LIST ===" << endl;
}
