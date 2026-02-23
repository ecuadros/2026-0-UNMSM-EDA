#include <iostream>
#include <ctime>
#include "containers/BTree.h"

int main() {
    srand(time(NULL));

    BTree<int, long> tree(4);

    std::cout << "Iniciando Stress Test: 100,000 inserciones..." << std::endl;

    for(int i = 0; i < 100000; i++) {
        tree.Insert(rand() % 1000000, i); 
    }

    std::cout << "Inserción completada con éxito." << std::endl;

    long id;
    int llaveABuscar = 500;if (tree.Search(llaveABuscar, id)) {
        std::cout << "Encontrado: llave " << llaveABuscar << " con ID " << id << std::endl;
    } else {
        std::cout << "La llave " << llaveABuscar << " no existe en esta ejecución." << std::endl;
    }

    return 0;
}