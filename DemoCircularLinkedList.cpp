#include <iostream>
#include <fstream>
#include "containers/lists.h"

using namespace std;

// Helper local para evitar conflictos
static bool EsMultiploDe3(int &n){
    return n % 3 == 0;
}

void DemoCircularLists(){
    cout << "Prueba Circular" << endl;

    // Crear lista Circular e Insertar
    CCircularLinkedList< AscendingTrait<int> > lista1;
    lista1.Insert(29, 1);
    lista1.Insert(74, 2); 
    lista1.push_back(12, 4); 
    lista1.Insert(55, 3);
    lista1.Insert(100, 5);
    cout << lista1 << endl;

    // PRUEBA DE CIRCULARIDAD: Recorrer mas elementos que el tamaño
    cout << "Verificando Circularidad (Imprimiendo 15 nodos):" << endl;
    auto it = lista1.begin();
    for(int i=0; i<15; ++i){
        if(it.m_pNode) { // Acceso directo al nodo solo para demostracion
             cout << "(" << it.m_pNode->GetValue() << ") -> ";
             it.m_pNode = it.m_pNode->GetNext(); // Avance manual circular
        }
    }
    cout << "..." << endl << endl;

    // Mover lista
    CCircularLinkedList< AscendingTrait<int> > lista2(move(lista1));
    cout << lista2 << endl;

    // Copiar lista y modificar
    CCircularLinkedList< AscendingTrait<int> > lista3(lista2);
    lista3[2] = 88; // Modificar tercer elemento
    cout << lista3 << endl;

    // Mover de regreso
    lista1 = move(lista3);
    cout << lista1 << endl;

    // Probar FirstThat (Multiplo de 3)
    auto ptr = lista1.FirstThat(EsMultiploDe3);
    if(ptr != lista1.end()) cout << "Multiplo de 3: " << *ptr << endl;

    // Guardar en archivo
    lista1.Save("circular.txt");
    
    // Cargar de archivo
    CCircularLinkedList< AscendingTrait<int> > lista4;
    lista4.Load("circular.txt");
    cout << lista4 << endl;
}
