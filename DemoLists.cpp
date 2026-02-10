#include <iostream>
#include "containers/lists.h"
#include "variadic-util.h" 

using namespace std;
using T1 = int;

// Funcion para imprimir
template <typename T>
void Print(T &val){
    cout << val << " "; 
}

void DemoLists(){
    // --- Parte 1: LE ---
    cout << "Lista Enlazada" << endl;
    
    CLinkedList< AscendingTrait<T1> > lista;
    lista.Insert(100, 1);
    lista.Insert(50, 2); 
    lista.push_back(300, 3);

    // Muestro la lista
    cout << lista << "NULL" << endl;

    // Pruebo Foreach
    cout << "Foreach: "; 
    lista.Foreach(&Print<T1>); 
    cout << endl << endl;


    // --- Parte 2: LEC ---
    cout << "Lista Circular" << endl;

    CLinkedListCircular< AscendingTrait<T1> > listaCircular;
    listaCircular.push_back(10, 1);
    listaCircular.push_back(20, 2);
    listaCircular.push_back(30, 3);

    cout << listaCircular << "(inicio)" << endl;

    // Confirmacion simple
    if (listaCircular.EsCircular()) {
        cout << "Es circular: SI" << endl;
        auto it = listaCircular.begin(); 
        cout << "Inicio: " << *it << endl;
    }
    
    cout << "FIN" << endl;
}