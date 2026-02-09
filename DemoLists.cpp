#include <iostream>
#include "containers/lists.h"
#include "variadic-util.h"

using namespace std;

void DemoLists(){
    cout << "--- DemoLists:  ---" << endl;

    CLinkedList< AscendingTrait<T1> > listaA;
    
    listaA.Insert(15, 2);
    listaA.Insert(45, 7);
    listaA.Insert(5, 1);
    listaA.Insert(33, 4);

    cout << "Lista A :\n" << listaA << endl;

    // operator[]
    listaA[2] = 42;
    cout << "El contenido del nodo [2] ahora es: " << listaA[2] << endl;
    cout << listaA << endl;

    // Foreach
    listaA.Foreach([](int &v){ v = v * 2 + 1; });
    cout << "Lista A (tras lambda *2+1):\n" << listaA << endl;

    listaA.Foreach(&Mult<T1>, 2);
    cout << "Lista A (tras Mult *2):\n" << listaA << endl;

    // FirstThat 
    auto encontrado = listaA.FirstThat(EsMayorQue<T1>, 40);
    if (encontrado != listaA.end()) {
        cout << "El primer elemento >40 en Lista A: " << *encontrado << endl;
    } else {
        cout << "No hay elementos >40 en Lista A" << endl;
    }

    // Lista l2 con orden descendente
    CLinkedList< DescendingTrait<T1> > listaB;

    // Poblar listaB con valores diferentes
    listaB.Insert(88, 11);
    listaB.Insert(66, 22);
    listaB.Insert(77, 33);
    cout << "\nLista B (ejemplo): " << listaB << endl;

    listaB.Insert(44, 6);
    cout << listaB << endl;

    // Lista circular con pruebas
    CCircularLinkedList< AscendingTrait<T1> > circularA;
    circularA.push_back(101, 1);
    circularA.push_back(202, 2);
    circularA.Insert(151,5);
    cout << "Circular A (inicial): "<< circularA << endl;

    circularA[2] = 211;
    cout << circularA << endl;

    
    circularA.Foreach([](int &v){ v -= 3; });
    cout << "Circular A (tras lambda -3): " << circularA << endl;

    circularA.Foreach(&Suma<T1>, 4);
    cout << "Circular A (tras Suma +4): " << circularA << endl;

    auto encontrado2 = circularA.FirstThat(EsMenorQue<T1>, 210);
    if (encontrado2 != circularA.end()) {
        cout << "Primer elemento <210 en Circular A: " << *encontrado2 << endl;
    } else {
        cout << "No hay elementos <210 en Circular A" << endl;
    }

    cout << "--- fin DemoLists ---" << endl;
}