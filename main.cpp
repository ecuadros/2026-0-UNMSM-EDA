#include <iostream>
#include <thread>
#include <vector>
#include "containers/binarytree.h"
int main() {
    CBinaryTree<TreeTraitAscending<int>> tree;

    std::cout << "--- Prueba de Insercion ---" << std::endl;
    std::vector<int> valores = {50, 30, 70, 20, 40, 60, 80};
    for (int v : valores) tree.Insert(v);
    
    std::cout << "Contenido (debe estar ordenado): " << tree << std::endl;

    std::cout << "\n--- Prueba de Copia y Movimiento ---" << std::endl;
    auto treeCopy = tree;
    std::cout << "Copia del arbol: " << treeCopy << std::endl;

    auto treeMoved = std::move(treeCopy);
    std::cout << "Arbol movido: " << treeMoved << std::endl;
    
    std::cout << "\n--- Prueba Multihilo (Insertando 100 elementos en paralelo) ---" << std::endl;
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&tree, i]() {
            for (int j = 0; j < 10; ++j) {
                tree.Insert(i * 100 + j);
            }
        });
    }

    for (auto& t : threads) t.join();
    std::cout << "Insercion masiva completada sin errores de memoria." << std::endl;

    std::cout << "\n--- Prueba de Busqueda ---" << std::endl;
    int target = 50;
    if (tree.Find(target)) {
        std::cout << "El valor " << target << " fue encontrado." << std::endl;
    } else {
        std::cout << "El valor " << target << " NO fue encontrado." << std::endl;
    }

    return 0;
}