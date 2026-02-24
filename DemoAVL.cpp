#include <iostream>
#include <utility>
#include "containers/binarytree.h"
#include "containers/Avl.h" 

using namespace std;

namespace TreeFunctions {
    void PrintNode(int& val) {
        cout << val << " ";
    }

    bool IsGreaterThan(int& val, int threshold) {
        return val > threshold;
    }

    bool IsExactMatch(int& val, int target) {
        return val == target;
    }
}

void DemoAVL(){
    using namespace TreeFunctions;

    cout << "============================================" << endl;
    cout << "   DEMO AVL TREE      " << endl;
    cout << "============================================" << endl;
    
    CAVL<TreeTraitDescending<int>> avlTree;

    // 1. Armamos el árbol inicial
    cout << "\n Construyendo arbol inicial: Insertando 10, 20, 30, 40, 50...\n";
    avlTree.Insert(10);
    avlTree.Insert(20);
    avlTree.Insert(30); 
    avlTree.Insert(40);
    avlTree.Insert(50); 

    
    cout << "\n--- ESTRUCTURA DEL ARBOL INICIAL ---\n";
    avlTree.PrintTree(); 
    cout << "------------------------------------\n";

    // 2. Inserción interactiva
    cout << "\n Ingresa un nuevo numero para INSERTAR en el AVL: ";
    cin >> avlTree; 
    
    cout << "\n--- ESTRUCTURA DESPUES DE INSERTAR ---\n";
    avlTree.PrintTree();
    cout << "\n" << avlTree << "\n"; 

    // 3. Borrado interactivo
    int numToRemove;
    cout << " Ingresa el numero que deseas ELIMINAR del AVL: ";
    cin >> numToRemove;
    
    cout << "\n  Eliminando el nodo " << numToRemove << "...\n";
    avlTree.Remove(numToRemove);
    
    cout << "\n--- ESTRUCTURA DESPUES DE ELIMINAR ---\n";
    avlTree.PrintTree();
    cout << "\n" << avlTree << "\n\n";

    // 4. Pruebas de recorridos 
    cout << "--- PRUEBAS DE ITERADORES Y FUNCIONES AVANZADAS ---\n";
    cout << "Inorden:   "; avlTree.Inorden(PrintNode); cout << endl;
    cout << "Preorden:  "; avlTree.Preorden(PrintNode); cout << endl;
    cout << "Posorden:  "; avlTree.Posorden(PrintNode); cout << "\n";

    cout << "Foreach:   "; 
    avlTree.Foreach(PrintNode); 
    cout << endl;

    auto pNode = avlTree.FirstThat(IsExactMatch, 30);
    if (pNode != nullptr) {
        cout << "FirstThat: Nodo con valor 30 encontrado con exito.\n";
    }

    cout << "\n  Evaluando Copy Constructor y Move Constructor...\n";
    CAVL<TreeTraitDescending<int>> copyAvl = avlTree;
    CAVL<TreeTraitDescending<int>> moveAvl = std::move(copyAvl);
    
    cout << "Forward Iterator (AVL Movido):  ";
    for (auto it = moveAvl.begin(); it != moveAvl.end(); ++it) {
        cout << *it << " ";
    }
    
    cout << "\nBackward Iterator (AVL Movido): ";
    for (auto it = moveAvl.rbegin(); it != moveAvl.rend(); ++it) {
        cout << *it << " ";
    }
    cout << "\n\n Demo AVL finalizado exitosamente.\n";
}