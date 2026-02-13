#include <iostream>
#include "containers/lists.h"
#include "containers/stack.h"
#include <utility>
using namespace std;
using T1 = int;
void DemoStack(){
    CStack< StackTrait<T1> >l1;
    cout<< "Creamos una pila llamada l1 "       << endl;
    cout<< "Ingresamos valores a la pila (Push)"<< endl;
    l1.Push(89);
    l1.Push(90);
    l1.Push(70);
    l1.Push(84);
    l1.Push(95);
    cout<< " Mostramos la pila:        "     << endl;
    cout<<l1;
    cout<< "Quitamos valores a la pila (Pop)"<< endl;
    cout<< "Hacemos dos Pop"                 << endl;
    cout<<" Hacemos el primer pop :  "<<l1.Pop();
    cout<<endl;
    cout<<l1;
    cout<<" Hacemos el segundo pop :  "<<l1.Pop();
    cout<<endl;
    cout<<l1;
    cout<<"Creamos otra pila l2 y hacemos que copie la pila l1 (COPY CONSTRUCTOR) "<<endl;
    CStack< StackTrait<T1> >l2=l1;
    cout<<" l1 (original):     "             << endl;
    cout<<l1;
    cout<<" l2 (copia):        "             << endl;
    cout<<l2;
    cout<<"Ahora creamos otra pila l3  que tome todoss los valores de l2 y l2 quede vacia (MOVE CONSTRUCTOR )"<<endl;
    CStack< StackTrait<T1> >l3=std::move(l2);
    cout<<" l2 (original):     "             << endl;
    cout<<l2;
    cout<<endl;
    cout<<" l3 (copia):         "            << endl;
    cout<<l3;
    cout<<" Ahora agregamos un valor a la pila : ";
    cin>>l3;
    cout<< endl;
    cout<<l3;
}
