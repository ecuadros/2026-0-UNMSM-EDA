#include <iostream>
#include <algorithm>
#include "containers/lists.h"
#include "general/types.h"


using namespace std;

void DemoInput(CLinkedList<AscendingTrait<T1>>& lista);
void DemoOperatorRef(CLinkedList<AscendingTrait<T1>>& lista);

void DemoLists(){
    CLinkedList< AscendingTrait<T1> > l1;

    l1.Insert(10, 5);
    l1.Insert(30, 3);
    l1.Insert(40, 5);
    l1.Insert(15, 1);
    l1.Insert(25, 2);
    l1.Insert(60, 6);
    l1.Insert(50, 4);

    cout << "DEMO" << endl;
    cout << l1 << endl;
    cout << "1. Recorrido con Foreach (+5):" << endl;

    for (T1& val : l1) { 
    val += 5;
    cout << "[" << val << "] ";
}
    cout << "\nLista tras modificacion: " << l1 << endl;
    // 2. Entrada de Datos
    DemoInput(l1);

    // 3. Prueba de Búsqueda
    cout << "\n3. Prueba de Busqueda (Rango 20-40):" << endl;
    auto condicion = [](T1 valor, T1 minimo, T1 maximo) {
        return valor >= minimo && valor <= maximo;
    };
    auto it = l1.FirstThat(condicion, 20, 40);

    if (it != l1.end()) 
       cout << "Encontrado con FirstThat: " << *it << endl;
    else 
       cout << "Nadie cumple con el rango." << endl;

    DemoOperatorRef(l1);

    cout << "\n--- FINALIZADO DEMO LISTS ---" << endl;
}
void DemoInput(CLinkedList<AscendingTrait<T1>>& lista) {
    Size n;
    cout << "\n2. Entrada de Datos Manual" << endl;
    cout << "Cuantos elementos quieres ingresar? "; cin >> n;
    cout << "IMPORTANTE: Formato (valor:referencia) ejemplo: (15:75)" << endl;
    for (T1 a = 0; a < n; ++a) 
    {
        cout << "Elemento " << a + 1 << ": ";
        if (!(cin >> lista)) {
            cout << "Error en formato. Limpiando buffer..." << endl;
            cin.clear();
            cin.ignore(1000, '\n');
        }
    }
    cout << "Lista actualizada: " << lista << endl;
}

void DemoOperatorRef(CLinkedList<AscendingTrait<T1>>& lista) {
    cout << "\n4. Acceso por Referencia (operator[])" << endl;
    try {
        ref_type refABuscar = 5;
        T1& valorEncontrado = lista[refABuscar]; 
        cout << "Exito: El valor para la ref " << refABuscar << " es: " << valorEncontrado << endl;
        
        valorEncontrado = 999;
        cout << "Cambiamos el valor a 999 via referencia." << endl;
        cout << "Lista final: " << lista << endl;

    } catch (const std::out_of_range& e) {
        cout << "Error: " << e.what() << " (La referencia no existe en la lista)" << endl;
    }
}