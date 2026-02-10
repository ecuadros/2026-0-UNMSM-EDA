#include <iostream>
#include "containers/lists.h"
#include "variadic-util.h"

using namespace std;

typedef int T1;

void DemoLists(){
    cout << "--- DEMO 26: LE y LEC ---" << endl;
    CLinkedList< AscendingTrait<T1> > l1;
    l1.Insert(50, 8);
    l1.Insert(20, 5);
    l1.Insert(30, 3);
    cout << "Lista inicial ordenada: " << l1;

    l1[1] = 24; 
    cout << "Nodo [1] modificado a 24: " << l1;

    l1.Foreach(Suma<T1>, 5); 
    cout << "Tras Foreach(Suma +5): " << l1;

    l1.Foreach(Mult<T1>, 2);
    cout << "Tras Foreach(Mult *2): " << l1;

    CCircularLinkedList<AscendingTrait<T1>> l_circ;
    l_circ.Insert(100, 1);
    l_circ.Insert(200, 2);
    l_circ.MakeCircular();
}