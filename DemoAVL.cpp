#include <iostream>
#include "containers/lists.h"
#include <utility>
#include "containers/binarytree.h"
#include "containers/AVL-Tree.h"
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

    cout << "=== DEMO AVL TREE (DESCENDING) ===" << endl;
    
    // Instancia con Trait Descendente
    CAVLTree<TreeTraitDescending<int>> avlTree;

    // Insertamos secuencialmente. 
    // Un arbol normal colapsaria en una linea recta, pero el AVL rotara.
    avlTree.Insert(10);
    avlTree.Insert(20);
    avlTree.Insert(30); 
    avlTree.Insert(40);
    avlTree.Insert(50); 

    cout << "Ingresa un numero para insertar en el AVL: ";
    cin >> avlTree; 
    cout << "Arbol AVL actual: " << avlTree << "\n\n";

    cout << "InOrden:   "; avlTree.inorden(PrintNode); cout << endl;
    cout << "PreOrden:  "; avlTree.preorden(PrintNode); cout << endl;
    cout << "PostOrden: "; avlTree.postorden(PrintNode); cout << "\n\n";

    cout << "Foreach: "; 
    avlTree.Foreach(PrintNode); 
    cout << endl;

    auto pNode = avlTree.FirstThat(IsExactMatch, 30);
    if (pNode != nullptr) {
        cout << "FirstThat (== 30): Encontrado con exito.\n\n";
    }

    // Al eliminar, el AVL tambien verificara su balanceo
    avlTree.Remove(20);
    cout << "Despues de Remove(20): " << avlTree << "\n\n";

    CAVLTree<TreeTraitDescending<int>> copyAvl = avlTree;
    CAVLTree<TreeTraitDescending<int>> moveAvl = std::move(copyAvl);
    
    cout << "Forward Iterator (AVL Movido): ";
    for (auto it = moveAvl.begin(); it != moveAvl.end(); ++it) {
        cout << *it << " ";
    }
    
    cout << "\nBackward Iterator (AVL Movido): ";
    for (auto it = moveAvl.rbegin(); it != moveAvl.rend(); ++it) {
        cout << *it << " ";
    }
    cout << "\n";
}