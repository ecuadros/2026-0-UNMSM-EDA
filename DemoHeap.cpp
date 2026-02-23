#include <iostream>
#include <utility>
#include "containers/heap.h"
using T1 = int;

void DemoHeap() {
    CHeap< MaxHeap<T1> > h1;
    std::cout << "\n--DEMO MAX HEAP--\n";
    std::cout << "Ingresamos valores al MaxHeap (Push):\n";
    h1.Push(89);
    h1.Push(90);
    h1.Push(70);
    h1.Push(84);
    h1.Push(95);
    h1.Push(17);
    h1.Push(9);
    h1.Push(23);
    h1.Push(24);
    h1.Push(5);
    
    std::cout << "Mostramos el MaxHeap h1:\n" << h1;

    std::cout << "\nHacemos dos Pop (deberian salir los dos mas grandes):\n";
    std::cout << "Primer pop : " << h1.Pop() << "\n" << h1;
    std::cout << "Segundo pop: " << h1.Pop() << "\n" << h1;

    std::cout << "\nCOPY CONSTRUCTOR: Creamos h2 y copiamos h1\n";
    CHeap< MaxHeap<T1> > h2 = h1;
    std::cout << "h1 (original):\n" << h1;
    std::cout << "h2 (copia):\n" << h2;

    std::cout << "\nMOVE CONSTRUCTOR: Creamos h3 robando los datos de h2\n";
    CHeap< MaxHeap<T1> > h3 = std::move(h2);
    std::cout << "h2 (original vaciado):\n" << h2; 
    std::cout << "h3 (movido):\n" << h3;

    std::cout << "\nAhora agregamos un valor a h3 usando 'cin >>' : ";
    std::cin >> h3;
    std::cout << "Heap actualizado:\n" << h3;
    std::cout << "El valor actual en la raiz (Top) es: " << h3.Top() << "\n";

    std::cout << "\n--DEMO MIN HEAP--\n";
    CHeap< MinHeap<T1> > hMin;
    
    std::cout << "Ingresamos los mismos valores al MinHeap: \n";
    hMin.Push(89);
    hMin.Push(90);
    hMin.Push(70);
    hMin.Push(84);
    hMin.Push(95);
    hMin.Push(17);
    hMin.Push(9);
    hMin.Push(23);
    hMin.Push(24);
    hMin.Push(5);
    
    std::cout << "Mostramos el MinHeap (el menor estara primero):\n" << hMin;
    std::cout << "Hacemos el primer pop (sale el menor): " << hMin.Pop() << "\n";
    std::cout<<hMin;
    std::cout << "El nuevo Top del MinHeap es: " << hMin.Top() << "\n\n";
}
