#include <iostream>
#include "containers/StaQue.h"

void DemoPila(){
    Titulo("Prueba de Pilas");

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
    cout << "   Indice  |        Nombres        |   Nota | Estado " << endl;
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

void DemoCola(){
    Titulo("Prueba de Colas");

        cout<<"Prueba de cola de enteros"<<endl;
        CQueue<int> cola;
        cola.Push(1);
        cola.Push(2);
        cola.Push(3);
        cola.Push(4);
        cout<<"Cola: "<<endl<<cola<<endl;
        cout<<"Frente: "<<cola.Front()<<endl;
        cola.Pop();
        cola.Pop();
        cout<<"Cola depilando: "<<endl<<cola<<endl;
        cout<<"Frente: "<<cola.Front()<<endl;
        cout<<endl;

        Escribir("Prueba de cola de String");
        CQueue<string>cola2;
        cola2.Push("uno");
        cola2.Push("dos");
        cola2.Push("tres");
        cola2.Push("cuatro");
        cout<<"Cola: "<<endl<<cola2<<endl;
        cout<<"Frente: "<<cola2.Front()<<endl;
        cola2.Pop();
        cola2.Pop();
        cout<<"Cola depilando: "<<endl<<cola2<<endl;
        cout<<"Frente: "<<cola2.Front()<<endl;
        cout<<endl;

        cout<<"Prueba de cola de Alumno"<<endl;
        cout<<endl;
        CQueue<Alumno> cola3;
        cola3.Push(Alumno(1, "Juan Perez", 9.5));
        cola3.Push(Alumno(2, "Maria Mercedez", 10.5));
        cola3.Push(Alumno(3, "Pedro Fernandez", 7.5));
        cola3.Push(Alumno(4, "Ana Flores", 16.0));
        cout << "   Indice  |        Nombres        |   Nota | Estado " << endl;
        cout<<cola3<<endl;
        cout<<"Frente: "<<endl<<cola3.Front()<<endl;
        cola3.Pop();
        cola3.Pop();
        cout<<"Cola depilando: "<<endl<<cola3<<endl;
        cout<<"Frente: "<<endl<<cola3.Front()<<endl;
        cout<<endl;

}
void DemoHeap(){
    Titulo("Prueba de Heap");

    Escribir("MAX HEAP de eteros");
    CHeap<MaxHeap<int>> maxHeap;
    maxHeap.Push(1);
    maxHeap.Push(2);
    maxHeap.Push(3);
    maxHeap.Push(4);
    cout<< maxHeap;
    cout<<"Tope MAX: "<<maxHeap.Top()<<endl;
    //haciendo pop sacando al mayor
    Escribir("Haciendo POP sacando al mayor");
    maxHeap.Pop();
    cout<< maxHeap;
    cout<<"Nuevo Tope MAX: "<<maxHeap.Top()<<endl;
    cout<<endl;

    Escribir("MIN HEAP de eteros");
    CHeap<MinHeap<int> > minHeap;
    minHeap.Push(4);
    minHeap.Push(3);
    minHeap.Push(2); //sube como raiz
    minHeap.Push(8);
    Escribir("Ingrese un valor");
    cin>>minHeap;
    cout<< minHeap;
    cout<<"Tope MIN: "<<minHeap.Top()<<endl;
    //haciendo pop sacando al menor
    Escribir("Haciendo POP sacando al menor: ");
    while(!minHeap.IsEmpty()){
        cout<<"sacando al menor: "<<minHeap.Top()<<endl;
        minHeap.Pop();
        cout<<minHeap<<endl;
    }
    if(minHeap.IsEmpty()){
        cout<<"Heap vacio"<<endl;
    }
    cout<<endl;
    //Constructor copia y move

    //copia
    CHeap<MaxHeap<int> > copiaMax(maxHeap);
    maxHeap.Push(4444);
    cout<<" MAX Original + 4444 : "<<maxHeap<<endl;
    cout<<"copia MAX: "<<copiaMax<<endl;
    //move
    CHeap<MaxHeap<int> > moveMax(std::move(copiaMax));
    cout<<"Heap Movido(copia)"<<moveMax<<endl;
    cout<<"Heap Original: "<<maxHeap<<endl;
    cout<<endl;

}
void DemoP_C_(){
    DemoPila();
    DemoCola();
    DemoHeap();
}