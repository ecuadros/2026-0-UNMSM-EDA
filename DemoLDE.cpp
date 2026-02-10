#include "containers/doublylinkedlist.h"
#include "general/types.h"
#include "containers/array.h"

void DemoLDE() {
    CDoublyLinkedList<TraitDescendente<T1>> lde;
    
    lde.Insert(10, 100);
    lde.Insert(30, 300);
    lde.Insert(20, 200);
    lde.Insert(5,  50);

    std::cout << "Lista actual:" << std::endl;
    for (auto it = lde.begin(); it != lde.end(); ++it) 
        std::cout << "[" << it.curr->m_data << ":" << it.curr->m_ref << "] ";
    std::cout << "\n" << std::endl;

    std::cout << "Ingrese un valor entero para insertar: ";
    std::cout << "Ingrese [Valor] y [Referencia] separados por un espacio :) : ";
    std::cin >> lde;

    std::cout << "1. Orden Descendente:" << std::endl;
    for (auto it = lde.begin(); it != lde.end(); ++it) 
        std::cout << "[" << it.curr->m_data << ":" << it.curr->m_ref << "] ";
    std::cout << std::endl;

    std::cout << "2. Orden Ascendente:" << std::endl;
    for (auto it = lde.rbegin(); it != lde.rend(); ++it)
        std::cout << "[" << it.curr->m_data << ":" << it.curr->m_ref << "] ";

    std::cout << "\n" << std::endl;
}