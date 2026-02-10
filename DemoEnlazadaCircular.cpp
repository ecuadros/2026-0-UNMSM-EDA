#include <iostream>
#include <algorithm>
#include "containers/circular_doubly_linked_list.h"
#include "general/types.h" 
#include "containers/array.h"

void DemoCircularEnlazada() {
    CCircularDoublyLinkedList<Trait1<T1>> cde;

    std::cout << "Entrada de Datos" << std::endl;
    std::cout << "Ingrese [Valor] y [Referencia] para el primer nodo: ";
    std::cin >> cde;
    
    cde.Insert(20, 200);
    cde.Insert(10, 100);
    cde.Insert(30, 300);
    cde.Insert(5,  50);

    std::cout << "\n1. Orden Descendente (Trait Mayor a Menor):" << std::endl;
    for (auto it = cde.begin(); it != cde.end(); ++it) 
        std::cout << "[" << it.curr->m_data << ":" << it.curr->m_ref << "] ";
    std::cout << std::endl;

    std::cout << "\n2. Orden Ascendente (Viceversa con ReverseIterator):" << std::endl;
    for (auto it = cde.rbegin(); it != cde.rend(); ++it) 
        std::cout << "[" << it.curr->m_data << ":" << it.curr->m_ref << "] ";
    std::cout << std::endl;

    std::cout << "ORDENAMIENTO BIEN" << std::endl;

    //  Prueba de Operator<<
    std::cout << "--- Test Lista Circular Doble ---" << std::endl;
    std::cout << "Lista: " << cde << std::endl;

    // Prueba de Foreach (Columna Azul de la tabla)
    std::cout << "\nPrueba Foreach (Multiplicar cada valor por 2):" << std::endl;
    for (auto& val : cde) {
        val *= 2; 
        std::cout << "[" << val << "] ";
    }
    std::cout << std::endl;

    // Prueba de FirstThat (Columna Azul de la tabla)
    std::cout << "\nPrueba FirstThat (Buscando primero > 30):" << std::endl;
    auto it = std::find_if(cde.begin(), cde.end(), [](T1 v) { return v > 30; });
    
    if (it != cde.end())
        std::cout << "Encontrado: " << *it << std::endl;
    else 
        std::cout << "No encontrado" << std::endl;

    // Prueba de acceso infinito (operator[])
    std::cout << "\nPrueba de acceso infinito (operator[]):" << std::endl;
    T1 indices[] = {0, 1, 4, 5, 10};
    
    for(T1 i : indices) 
        std::cout << "Indice [" << i << "]: " << cde[i] << std::endl;

    std::cout << "\n FINALIZADO" << std::endl;
}