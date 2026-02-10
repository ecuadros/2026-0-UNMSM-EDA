#include <iostream>
#include "containers/lists.h"
#include "variadic-util.h" 

using namespace std;
using T1 = int;

void DemoLists(){
    // --- PARTE 1: LDE ---
    cout << "--TEST 1: LDE (Doble)--" << endl;
    CDoublyLinkedList< AscendingTrait<T1> > listaDoble;
    listaDoble.Insert(100, 1);
    listaDoble.Insert(50, 2); 
    listaDoble.push_back(200, 3);

    cout << "Normal: " << listaDoble << "NULL" << endl;

    cout << "Al reves: ";
    auto it = listaDoble.rbegin(); 
    while(it != listaDoble.rend()) {
        cout << *it << " <-> ";
        --it; 
    }
    cout << "NULL" << endl << endl;

    // --- PARTE 2: LDEC (Circular) ---
    cout << "--LDEC (Circular Doble)--" << endl;
    CDoublyLinkedListCircular< AscendingTrait<T1> > listaCircular;
    listaCircular.push_back(10, 1);
    listaCircular.push_back(20, 2);
    listaCircular.push_back(30, 3);

    cout << "Lista: " << listaCircular << "(inicio)" << endl;

    if (listaCircular.EsCircular()){
        cout << "[OK] Es Circular: El ultimo conecta con el primero." << endl;
    }
    
    cout << "FIN" << endl;
}