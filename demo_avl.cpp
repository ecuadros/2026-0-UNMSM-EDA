#include <iostream>
#include <string>
#include <vector>
#include "containers/avl.h"
#include "containers/binarytree.h" 
#include "general/types.h"

// 1. Esta función debe llamarse igual que la que pasas al preorder/postorder
void printWithPrefix(T1& val, std::string prefix) {
    std::cout << prefix << val << " ";
}

void demo_avl() {
    // --- 1. Inicializacion y Operador >> ---
    CBinaryTreeAVL<TreeTraitAscending<T1>> avl;
    
    std::cout << "--- Insertando elementos con operador >> ---" << std::endl;
    avl >> 50 >> 30 >> 70 >> 20 >> 40 >> 60 >> 80;
    
    // --- 2. Operador << ---
    std::cout << "Arbol completo (operator<<): " << avl << std::endl;

    // --- 3. Recorridos Variadicos ---
    // Usamos el nombre 'printWithPrefix' definido arriba
    std::cout << "\n--- Recorridos Variadicos ---" << std::endl;
    std::cout << "Preorder con prefijo: ";
    avl.preorder(printWithPrefix, "ID:"); 
    
    std::cout << "\nPostorder con prefijo: ";
    avl.postorder(printWithPrefix, "Node:");
    std::cout << std::endl;

    // --- 4. FirstThat Variadico ---
    T1 limit = 55;
    T1* found = avl.FirstThat([](T1 val, T1 threshold) {
        return val > threshold;
    }, limit);

    if (found) {
        std::cout << "\nFirstThat (> " << limit << "): " << *found << std::endl;
    }

    // --- 5. Forward Iterators ---
    std::cout << "\n--- Iteradores Forward (Inorden) ---" << std::endl;
    for (auto it = avl.begin(); it != avl.end(); ++it) {
        std::cout << "[" << *it << "] ";
    }

    // --- 6. Backward Iterators ---
    std::cout << "\n\n--- Iteradores Backward (Reversa) ---" << std::endl;
    for (auto it = avl.rbegin(); it != avl.rend(); ++it) {
        std::cout << "[" << *it << "] ";
    }

    // --- 7. Semantica de Movimiento ---
    std::cout << "\n\n--- Move Constructor ---" << std::endl;
    CBinaryTreeAVL<TreeTraitAscending<T1>> nuevoArbol = std::move(avl);
    std::cout << "Nuevo arbol (despues de move): " << nuevoArbol << std::endl;

    // --- 8. Remocion (Remove) ---
    std::cout << "\n--- Borrado de nodos (30 y 70) ---" << std::endl;
    nuevoArbol.Remove(30);
    nuevoArbol.Remove(70);
    std::cout << "Resultado final: " << nuevoArbol << std::endl;
}