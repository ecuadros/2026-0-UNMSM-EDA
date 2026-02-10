#include <iostream>
#include <fstream>
#include "containers/lists.h"
#include "variadic-util.h"

using namespace std;

bool EsMultiploDe5(int &n){
    return n % 5 == 0;
}

void DemoLists(){
    cout << "Prueba" << endl;

    // Crear lista e insertar valores
    CLinkedList< AscendingTrait<int> > lista1;
    lista1.Insert(15, 1);
    lista1.Insert(10, 2); 
    lista1.push_back(5, 4);
    lista1.Insert(25, 3);
    cout << lista1 << endl;

    // Mover lista
    CLinkedList< AscendingTrait<int> > lista2 = move(lista1);
    cout << lista2 << endl;
    
    // Copiar lista y modificar
    CLinkedList< AscendingTrait<int> > lista3 = lista2;
    lista3[1] = 99;
    cout << lista3 << endl;
    
    // Mover de regreso
    lista1 = move(lista3);
    cout << lista1 << endl;

    // Probar FirstThat
    auto ptr = lista1.FirstThat(EsMultiploDe5);
    if(ptr != lista1.end()) cout << "Multiplo de 5: " << *ptr << endl;

    // Probar Foreach
    lista1.Foreach(Suma<int>, 10);
    cout << lista1 << endl;

    // Guardar en archivo
    lista1.Save("data.txt");
    
    // Cargar de archivo
    CLinkedList< AscendingTrait<int> > lista4;
    lista4.Load("data.txt");
    cout << lista4 << endl;

    // Leer stream
    ifstream file("list.txt");
    if(file.is_open()){
        CLinkedList< AscendingTrait<int> > lista5;
        file >> lista5;
        cout << lista5 << endl;
    }
}