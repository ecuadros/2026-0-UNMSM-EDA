#include <iostream>
#include "containers/lists.h"

using namespace std;

void DemoLists(){

    CDoubleLinkedList< AscendingTrait<T1> > l1;

    l1.Insert(20, 5);
    l1.Insert(30, 3);
    l1.Insert(50, 8);
    cout << l1 << endl;

    l1[1] = 24;

    cout<<"El contenido del nodo [1] ahora es: "<<l1[1]<<endl;
    cout<< l1 <<endl;
    
    l1.Foreach(&Suma<T1>, 5);
    cout << l1 << endl;

    l1.Foreach(&Mult<T1>, 2);
    cout << l1 << endl;

    auto it = l1.FirstThat(EsMayorQue<T1>, 55);
    if (it != l1.end()) {
        cout << "El primer elemento mayor que 55 de la lista 1 es: " << *it <<endl;
    } else {
        cout << "No hay elementos mayores a 55 en la lista 1" <<endl;
    }

    cout<<endl;
    CDoubleLinkedList< DescendingTrait<T1> > l2;
    cout<<"Ingrese el contenido de la lista l2 de este modo [(a:b), ... ]:"<<endl;
    cin>>l2;
    cout<<endl<<"Lista leida: "<<l2<<endl;

    l2.Insert(40, 6);
    cout<<l2<<endl;

    CCircularDoubleLinkedList< AscendingTrait<T1> > l3;
    l3.push_back(100, 1);
    l3.push_back(200, 2);
    l3.Insert(150,5);
    cout << "Lista 3 (Circular): "<<l3<<endl;

    l3[2] = 210;
    cout<<l3<<endl;

    l3.Foreach(&Suma<T1>, 2);
    cout << l3 << endl;

    l3.Foreach(&Mult<T1>, 3);
    cout << l3 << endl;

    auto it2 = l3.FirstThat(EsMenorQue<T1>, 376);
    if (it2 != l3.end()) {
        cout << "El primer elemento menor que 376 de la lista 3 es: " << *it2 <<endl;
    } else {
        cout << "No hay elementos menores a 376 en la lista 3" << endl;
    }
}
