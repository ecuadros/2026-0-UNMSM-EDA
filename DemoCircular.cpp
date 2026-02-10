#include <iostream>
#include <algorithm>
#include "containers/circularlist.h"
#include "general/types.h"
#include "containers/array.h"

void DemoCircular() {
    CCircularList<Trait1<T1>> lec;

    std::cout << "Entrada de Datos" << std::endl;
    std::cout << "FORMATO (valor:referencia): ";
    std::cin >> lec;
    
    lec.Insert(20, 200);
    lec.Insert(10, 100);
    lec.Insert(30, 300);
    lec.Insert(5,  50);

    std::cout << "\n1. Orden Ascendente:" << std::endl;
    for (auto it = lec.begin(); it != lec.end(); ++it) 
        std::cout << "[" << *it << "] "; 
    std::cout << std::endl;

    std::cout << "\n2. Orden Descendente:" << std::endl;
    for (T1 i = (T1)lec.getSize() - 1; i >= 0; --i) 
    std::cout << "[" << lec[i] << "] "; 
    std::cout << std::endl;

    std::cout << "\nORDENAMIENTO BIEN" << std::endl;

    std::cout << "\nPrueba Foreach (Multiplicar cada valor por 2):" << std::endl;

    for (auto& val : lec) {
        val *= 2; 
        std::cout << "[" << val << "] ";
    }
    std::cout << std::endl;

    std::cout << "\nPrueba FirstThat (Buscando primero > 30):" << std::endl;
    auto it = std::find_if(lec.begin(), lec.end(), [](T1 v) { return v > 30; });
    
    if (it != lec.end())
        std::cout << "Encontrado: " << *it << std::endl;
    else 
        std::cout << "No encontrado" << std::endl;

    std::cout << "\nPrueba de acceso infinito (operator[]):" << std::endl;
    T1 indices[] = {0, 1, 4, 5, 10};
    for(T1 i : indices) 
        std::cout << "Indice [" << i << "]: " << lec[i] << std::endl;

    std::cout << "\n FINALIZADO" << std::endl;
}