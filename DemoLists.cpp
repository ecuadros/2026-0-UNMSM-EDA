#include <iostream>
#include "containers/lists.h"

using namespace std;
using T1 = int;

void DemoLists(){

    // push_back
    CLinkedList< AscendingTrait<T1> > l1;
    
    l1.push_back(10, 1);
    l1.push_back(20, 2);
    l1.push_back(30, 3);
    cout << "---> " << l1 << endl;

    // Insert

    CLinkedList< AscendingTrait<T1> > l2;
    cout << "Insertando valores de forma ordenada:\n";
    
    l2.Insert(50, 5);
    l2.Insert(20, 2);
    l2.Insert(80, 8);
    l2.Insert(35, 3);
    l2.Insert(15, 1);
    l2.Insert(60, 6);
    cout << "---> " << l2 << endl;

    // Acceso con operator[]
    cout << "Size l2: " << l2.getSize() << endl;
    for (Size i = 0; i < l2.getSize(); ++i) {
        cout << "l2[" << i << "] = " << l2[i] << endl;
    }

    // Iteradores y operator*;
    for (auto it = l2.begin(); it != l2.end(); ++it) {
        cout << *it << " ";
    }
    cout << endl;

    // Copy Constructor
    CLinkedList< AscendingTrait<T1> > l3 = l2;
    cout << "l3 = l2 (copia profunda)" << endl;
    cout << "l2: " << l2 << endl;
    cout << "l3: " << l3 << endl;
    

    // Move Constructor
    CLinkedList< AscendingTrait<T1> > l4 = std::move(l1);
    cout << "l1 (Vacío): " << l1 << endl;
    cout << "l4 (Recursos de l1): " << l4 << endl;


    // Foreach
    cout << "Elementos de l2 usando Foreach: ";
    l2.Foreach(&multiplicar, 8); // Multiplica cada elemento por 2
    cout << l2 << endl;
   
    // FirstThat
    CLinkedList< AscendingTrait<T1> > l5;
    l5.Insert(5, 1);
    l5.Insert(16, 2);
    l5.Insert(32, 3);
    l5.Insert(10, 4);
    cout << "Lista l5: " << l5 << endl;
    
    auto resultado = l5.FirstThat(&Mod16);
    if (resultado != l5.end()) {
        cout << "Primer elemento divisible por 16: " << *resultado << endl;
    } else {
        cout << "No hay elementos divisibles por 16" << endl;
    }

    // 13. operator>> 
    CLinkedList< AscendingTrait<T1> > l6;
    cout << "Ingresando elementos en l6:\n";
    cin >> l6;
    cout << "l6 tras entrada: " << l6 << endl;

}