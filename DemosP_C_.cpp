#include <iostream>
#include "containers/StaQue.h"
void DemoPila(){
    cout<<"Prueba de pila de strings"<<endl;

    CStack<string> pila;
    pila.Push("uno");
    pila.Push("dos");
    pila.Push("tres");
    cout<<"Pila de String: "<<endl<<pila<<endl;
    cout<<"Tope: "<<pila.Tope()<<endl;
    //depilar
    pila.Pop();
    pila.Pop();
    cout<<"Pila de String: "<<endl<<pila<<endl;


    cout<<"Prueba de pila de enteros"<<endl;
    //enpilar
    CStack<int> p1;
    p1.Push(1);
    p1.Push(2);
    p1.Push(3);
    cout<<"Pila de enteros: "<<endl<<p1<<endl;
    cout<<"Tope: "<<p1.Tope()<<endl;
    cout<<endl;
    //depilar
    p1.Pop();
    p1.Pop();
    cout<<"Pila de enteros: "<<endl<<p1<<endl;

    Escribir("Prueba de pila de Alumno");
    CStack<Alumno> p2;
    p2.Push(Alumno(1, "Juan Perez", 9.5));
    p2.Push(Alumno(2, "Maria Mercedez", 10.5));
    p2.Push(Alumno(3, "Pedro Fernandez", 7.5));
    p2.Push(Alumno(4, "Ana Flores", 16.0));
    cout << "   Indice  | Nombres              |   Nota | Estado " << endl;
    cout<<p2<<endl;

    cout<<"Tope: "<<endl<<p2.Tope()<<endl;
    Escribir ("Depilando");
    Escribir("sacamos a Ana Flores");
    p2.Pop();
    cout<<p2<<endl;
    p2.Pop();
    cout<<"Pila de Alumno: "<<endl<<p2<<endl;
    cout<<"Tope: "<<endl<<p2.Tope()<<endl;

}
void DemoP_C_(){
    DemoPila();
}