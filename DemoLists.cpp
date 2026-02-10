#include <iostream>
#include <fstream>
#include "containers/doublelinkedlist.h"
#include "variadic-util.h"
#include "containers/linkedlist.h"

using namespace std;

typedef int T1;

void DemoLists(){
    cout << "--- DEMO LDE & LDEC ---" << endl;
    
    CDoubleLinkedList<AscendingTrait<T1>> lde;
    lde.Insert(50, 8);
    lde.Insert(20, 5);
    lde.Insert(30, 3);

    cout << "LDE inicial (Ordenada): " << lde << endl;
    
    lde[1] = 25; 
    cout << "LDE tras modificar indice [1] a 25: " << lde << endl;

    cout << "Recorrido Forward manual: ";
    for (auto it = lde.begin(); it != lde.end(); ++it) {
        cout << *it << " <-> ";
    }
    cout << "NULL" << endl;

    cout << "Aplicando Foreach (Suma +10)..." << endl;
    lde.Foreach(Suma<T1>, 10);
    cout << "Resultado: " << lde << endl;

    cout << "\n--- Probando LDEC ---" << endl;
    CDoubleCircularLinkedList<DescendingTrait<T1>> ldec;
    ldec.Insert(100, 10);
    ldec.Insert(200, 20);
    ldec.Insert(150, 15);
    ldec.MakeCircular();

    cout << "LDEC (Descendente): " << ldec << endl;

}