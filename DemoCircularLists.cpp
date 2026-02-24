#include <iostream>
#include "containers/clists.h"
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

void DemoCircularLinkedList(){
    cout << "=== DEMO CIRCULAR LINKED LIST ===" << endl << endl;
    
    // Test 1: Creación y push_back
    cout << "Test 1: Creación y push_back" << endl;
    CCircularLinkedList< CAscendingTrait<T1> > cl1;
    cl1.push_back(10, 6);
    cl1.push_back(50, 8);
    cl1.push_back(70, 9);
    cout << cl1 << endl;
    
    // Test 2: Insert ordenado
    cout << "Test 2: Insert ordenado" << endl;
    cl1.Insert(20, 4);
    cl1.Insert(30, 3);
    cl1.Insert(5, 1);
    cout << cl1 << endl;
    
    // Test 3: Foreach con Print
    cout << "Test 3: Foreach con Print" << endl;
    ::Foreach(cl1.begin(), cl1.end(), &Print<T1>);
    cout << endl << endl;
    
    // Test 4: Foreach con Mult7
    cout << "Test 4: Foreach - Mostrar múltiplos de 7" << endl;
    ::Foreach(cl1.begin(), cl1.end(), &Mult7<T1>);
    cout << endl << endl;
    
    // Test 5: FirstThat
    cout << "Test 5: FirstThat - Primer múltiplo de 7" << endl;
    auto iter = cl1.FirstThat(&Mult7<T1>);
    if (iter != cl1.end()) {
        cout << "El primer múltiplo de 7 es: " << *iter << endl;
    } else {
        cout << "No se encontró ningún múltiplo de 7" << endl;
    }
    cout << endl;
    
    // Test 6: Operator[]
    cout << "Test 6: Operator[] - Acceso por índice" << endl;
    try {
        cout << "Elemento en índice 2: " << cl1[2] << endl;
        cout << "Elemento en índice 4: " << cl1[4] << endl;
        cl1[3] = 100;
        cout << "Después de modificar índice 3 a 100:" << endl;
        cout << cl1 << endl;
    } catch (const std::exception &e) {
        cout << "Error: " << e.what() << endl;
    }
    cout << endl;
    
    // Test 7: Constructor copia
    cout << "Test 7: Constructor copia" << endl;
    CCircularLinkedList< CAscendingTrait<T1> > cl2(cl1);
    cout << "Lista original cl1:" << endl;
    cout << cl1 << endl;
    cout << "Lista copiada cl2:" << endl;
    cout << cl2 << endl;
    
    // Test 8: Move constructor
    cout << "Test 8: Move constructor" << endl;
    CCircularLinkedList< CAscendingTrait<T1> > cl3(std::move(cl2));
    cout << "Lista movida cl3:" << endl;
    cout << cl3 << endl;
    cout << "Lista original cl2 (debería estar vacía):" << endl;
    cout << "Size de cl2: " << cl2.getSize() << endl << endl;
    
    // Test 9: Persistencia - Guardar en archivo
    cout << "Test 9: Persistencia - Guardar en archivo" << endl;
    try {
        cl1.SaveToFile("circularlist_data.txt");
        cout << "Lista circular guardada exitosamente en circularlist_data.txt" << endl;
    } catch (const std::exception &e) {
        cout << "Error al guardar: " << e.what() << endl;
    }
    cout << endl;
    
    // Test 10: Persistencia - Cargar desde archivo
    cout << "Test 10: Persistencia - Cargar desde archivo" << endl;
    CCircularLinkedList< CAscendingTrait<T1> > cl4;
    try {
        cl4.LoadFromFile("circularlist_data.txt");
        cout << "Lista circular cargada exitosamente desde archivo:" << endl;
        cout << cl4 << endl;
    } catch (const std::exception &e) {
        cout << "Error al cargar: " << e.what() << endl;
    }
    cout << endl;
    
    // Test 11: Foreach con modificación
    cout << "Test 11: Foreach con incremento" << endl;
    cl1.Foreach(&IncBasico<T1>);
    cout << "Después de incrementar cada elemento:" << endl;
    cout << cl1 << endl;
    
    // Test 12: Foreach con suma
    cout << "Test 12: Foreach con Suma" << endl;
    cl1.Foreach(&Suma<T1>, 10);
    cout << "Después de sumar 10 a cada elemento:" << endl;
    cout << cl1 << endl;
    
    // Test 13: Verificar circularidad
    cout << "Test 13: Verificar circularidad (recorrer 2 vueltas completas)" << endl;
    cout << "Elementos (2 vueltas): ";
    if (cl1.getSize() > 0) {
        for (int i = 0; i < static_cast<int>(cl1.getSize() * 2); ++i) {
            cout << cl1[i % cl1.getSize()] << " ";
        }
    }
    cout << endl << endl;
    
    cout << "=== FIN DEMO CIRCULAR LINKED LIST ===" << endl;
}
