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

    cout << " DEMO BINARY TREE " << endl;
    cout << " ASCENDING  BTree " << endl;
    // 1. Acending y Descending
    CBinaryTree<TreeTraitAscending<int>> bTree;

    // 2. Insert
    bTree.Insert(50);
    bTree.Insert(30);
    bTree.Insert(70);
    bTree.Insert(20);
    bTree.Insert(40);
    cout<<bTree;
    cout<<endl;
     CBinaryTree<TreeTraitDescending<int>> cTree;
    cout << "DESCENDING CTree " <<endl;
    cTree.Insert(50);
    cTree.Insert(30);
    cTree.Insert(70);
    cTree.Insert(20);
    cTree.Insert(40);
    cout<<cTree;
    cout<<endl;
    // 3. Operadores cin y cout
    cout << "Ingresa un numero para insertar en BTree: ";
    cin >> bTree; 
    cout << "Arbol BTree: " << bTree << "\n\n";
    
    cout<<"Los tres recorridos : "<<endl;
    // 4. Los 3 Recorridos (Ahora mucho mas limpios)
    cout << "InOrden:   "; bTree.inorden(PrintNode); cout << endl;
    cout << "PreOrden:  "; bTree.preorden(PrintNode); cout << endl;
    cout << "PostOrden: "; bTree.postorden(PrintNode); cout << "\n\n";

    // 5. Foreach y FirstThat
    cout << "Foreach: "; 
    bTree.Foreach(PrintNode); 
    cout << endl;
   
    auto pNode = bTree.FirstThat(IsGreaterThan, 60);
    if (pNode != nullptr) {
        cout << "FirstThat (> 60): Encontrado con exito.\n\n";
    }

    // 6. Remove
    bTree.Remove(30);
    cout << "Despues de Remove(30): " << bTree << "\n\n";

    // 7. Copy y Move Constructor
    cout<<"COPY CONSTRUCTOR :"<<endl;
    CBinaryTree<TreeTraitAscending<int>> copyTree = bTree;
    cout<<"Original(BTree) :"<<bTree<<endl;
    cout<<"Copia (CopyTree)   :"<<copyTree<<endl;
    cout<<endl;
    cout<<"MOVE CONSTRUCTOR :"<<endl;
    CBinaryTree<TreeTraitAscending<int>> moveTree = std::move(copyTree);
    cout<<"Original (CopyTree) :"<<copyTree<<endl;
    cout<<"Copia    (MoveTree) :"<<moveTree<<endl;
    cout<<endl;
    // 8. Iteradores (Forward y Backward) 
    cout << "Forward Iterator : ";
    for (auto it = moveTree.begin(); it != moveTree.end(); ++it) {
        cout << *it << " ";
    }
    
    cout << "\nBackward Iterator : ";
    for (auto it = moveTree.rbegin(); it != moveTree.rend(); ++it) {
        cout << *it << " ";
    }
    cout << "\n";


}