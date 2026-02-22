#include <iostream>
#include "containers/lists.h"
#include <utility>
#include "containers/binarytree.h"
using namespace std;
namespace TwoFunctions {
    void PrintNode(int& val) {
        cout << val << " ";
    }

    bool IsGreaterThan(int& val, int threshold) {
        return val > threshold;
    }
}
void DemoBinaryTree(){
   using namespace TwoFunctions;

    cout << "=== DEMO BINARY TREE (ASCENDING) ===" << endl;
    
    // 1. Instancia con Trait Ascendente
    CBinaryTree<TreeTraitAscending<int>> bTree;

    // 2. Insert
    bTree.Insert(50);
    bTree.Insert(30);
    bTree.Insert(70);
    bTree.Insert(20);
    bTree.Insert(40);

    // 3. Operadores cin y cout
    cout << "Ingresa un numero para insertar: ";
    cin >> bTree; 
    cout << "Arbol actual: " << bTree << "\n\n";

    // 4. Los 3 Recorridos (Ahora mucho mas limpios)
    cout << "InOrden:   "; bTree.inorden(PrintNode); cout << endl;
    cout << "PreOrden:  "; bTree.preorden(PrintNode); cout << endl;
    cout << "PostOrden: "; bTree.postorden(PrintNode); cout << "\n\n";

    // 5. Foreach y FirstThat
    cout << "Foreach: "; 
    bTree.Foreach(PrintNode); 
    cout << endl;

    // Uso de 'auto' simple para evitar errores de deduccion
    auto pNode = bTree.FirstThat(IsGreaterThan, 60);
    if (pNode != nullptr) {
        cout << "FirstThat (> 60): Encontrado con exito.\n\n";
    }

    // 6. Remove
    bTree.Remove(30);
    cout << "Despues de Remove(30): " << bTree << "\n\n";

    // 7. Copy y Move Constructor
    CBinaryTree<TreeTraitAscending<int>> copyTree = bTree;
    CBinaryTree<TreeTraitAscending<int>> moveTree = std::move(copyTree);
    
    // 8. Iteradores (Forward y Backward) sobre el arbol movido
    cout << "Forward Iterator (Arbol Movido): ";
    for (auto it = moveTree.begin(); it != moveTree.end(); ++it) {
        cout << *it << " ";
    }
    
    cout << "\nBackward Iterator (Arbol Movido): ";
    for (auto it = moveTree.rbegin(); it != moveTree.rend(); ++it) {
        cout << *it << " ";
    }
    cout << "\n";


}