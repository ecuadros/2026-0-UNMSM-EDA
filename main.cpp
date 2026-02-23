/*#include <iostream>
#include "util.h"
#include "pointers.h"
#include "containers/array.h"
#include "containers/lists.h"

// g++ -std=c++17 -Wall -g -pthread main.cpp -o main

int main() {
    std::cout << "Hello EDA-UNMSM!" << std::endl;
    // DemoUtil();
    // DemoPointers1();
    // DemoArray();
    DemoLists();
    return 0;
}*/

#include <iostream>
#include "containers/binarytree.h"

using namespace std;

int main(){

    // Crear árbol ascendente (BST normal)
    CBinaryTree<TreeTraitAscending<int>> tree;

    cout << "Insertando elementos..." << endl;

    tree.Insert(50, 1);
    tree.Insert(30, 2);
    tree.Insert(70, 3);
    tree.Insert(20, 4);
    tree.Insert(40, 5);
    tree.Insert(60, 6);
    tree.Insert(80, 7);

    cout << "\nRecorrido InOrder: ";
    tree.InOrder([](int v){ cout << v << " "; });

    cout << "\nRecorrido PreOrder: ";
    tree.PreOrder([](int v){ cout << v << " "; });

    cout << "\nRecorrido PostOrder: ";
    tree.PostOrder([](int v){ cout << v << " "; });

    // Probar FirstThat
    auto ptr = tree.FirstThat([](int v){ return v > 55; });
    if(ptr)
        cout << "\n\nPrimer valor mayor que 55: " << *ptr << endl;

    // Probar iteradores
    cout << "\nIterador forward: ";
    for(auto it = tree.begin(); it != tree.end(); ++it){
        cout << *it << " ";
    }

    cout << "\nIterador reverse: ";
    for(auto it = tree.rbegin(); it != tree.rend(); ++it){
        cout << *it << " ";
    }

    cout << "\nAntes de eliminar:\n";
    tree.InOrder([](int v){ cout << v << " "; });

    tree.Remove(70);

    cout << "\nDespues de eliminar:\n";
    tree.InOrder([](int v){ cout << v << " "; });
    
    // Probar Remove
    cout << "\n\nEliminando 70..." << endl;
    tree.Remove(70);

    cout << "InOrder despues de eliminar: ";
    tree.InOrder([](int v){ cout << v << " "; });

    // Probar constructor copia
    cout << "\n\nProbando constructor copia..." << endl;
    CBinaryTree<TreeTraitAscending<int>> treeCopy(tree);

    cout << "Arbol copia InOrder: ";
    treeCopy.InOrder([](int v){ cout << v << " "; });

    // Probar move constructor
    cout << "\n\nProbando move constructor..." << endl;
    CBinaryTree<TreeTraitAscending<int>> treeMove(std::move(treeCopy));

    cout << "Arbol movido InOrder: ";
    treeMove.InOrder([](int v){ cout << v << " "; });

    // Probar operadores
    cout << "\n\nProbando operadores..." << endl;

    CBinaryTree<TreeTraitAscending<int>> tree2;
    tree2.Insert(50, 1);

    if(treeMove << tree2)
        cout << "Las raices son iguales (operator<<)" << endl;

    if(treeMove >> tree2)
        cout << "Raiz del primero es mayor (operator>>)" << endl;
    else
        cout << "Raiz del primero NO es mayor" << endl;

    cout << "\nPrograma finalizado correctamente." << endl;

    return 0;
}