#include <iostream>
#include "containers/doublelinkedlist.h"

using namespace std;
using T1 = int;

bool Mod8 (const T1 &v){ return v % 8  == 0; }
bool Mod16(const T1 &v){ return v % 16 == 0; }

void DemoDoubleLists(){


    // push_back 
    CDoubleLinkedList< AscendingTrait<T1> > l1;

    l1.push_back(5,  0);
    l1.push_back(10, 2);
    l1.push_back(68, 8);
    l1.push_back(80, 4);

    cout << "---> l1: " << l1 << endl << endl;

    // Insert ordenado
    CDoubleLinkedList< AscendingTrait<T1> > l2;

    l2.Insert(40, 4);
    l2.Insert(10, 1);
    l2.Insert(70, 7);
    l2.Insert(30, 3);
    l2.Insert(20, 2);
    l2.Insert(60, 6);
    l2.Insert(50, 5);

    cout << "---> l2: " << l2 << endl << endl;

    // operator[]
    cout << "Size l2 = " << l2.getSize() << endl;
    for (Size i = 0; i < l2.getSize(); ++i){
        cout << "l2[" << i << "] = " << l2[i] << endl;
    }
    cout << endl;


    // Iteración forward
    for (auto it = l2.begin(); it != l2.end(); ++it){
        cout << *it << " ";
    }
    cout << endl << endl;


    //Iteración backward
    for (auto it = l2.rbegin(); it != l2.rend(); ++it){
        cout << *it << " ";
    }
    cout << endl << endl;


    // FirstThat
    auto it8 = l2.FirstThat(&Mod8);
    if (it8 != l2.end())
        cout << "Primer múltiplo de 8: " << *it8 << endl;
    else
        cout << "No hay múltiplos de 8\n";

    auto it16 = l2.FirstThat(&Mod16);
    if (it16 != l2.end())
        cout << "Primer múltiplo de 16: " << *it16 << endl;
    else
        cout << "No hay múltiplos de 16\n";
    cout << endl;


    // Copy constructor
    CDoubleLinkedList< AscendingTrait<T1> > l3 = l2;

    cout << "l2: " << l2 << endl;
    cout << "l3: " << l3 << endl;

    cout << "Modificando l3[0]" << endl;
    l3[0] = 11;

    cout << "l2 (sin cambios): " << l2 << endl;
    cout << "l3 (modificada): " << l3 << endl << endl;


    // Move constructor
    CDoubleLinkedList< AscendingTrait<T1> > l4 = std::move(l1);

    cout << "l1 (tras move): " << l1 << endl;
    cout << "l4 (recursos de l1): " << l4 << endl << endl;

}