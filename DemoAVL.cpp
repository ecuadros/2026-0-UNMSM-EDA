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

    cout << " DEMO AVL TREE (ASCENDING) " << endl;
    
    cout << "ASCENDING AVL TREE " <<endl; 
    CAVLTree<TreeTraitAscending<int>> avlTree;
    avlTree.Insert(10);
    avlTree.Insert(20);
    avlTree.Insert(30); 
    avlTree.Insert(40);
    avlTree.Insert(50); 
    cout<<avlTree;
    CAVLTree<TreeTraitDescending<int>> DescavlTree;
    DescavlTree.Insert(10);
    DescavlTree.Insert(20);
    DescavlTree.Insert(30); 
    DescavlTree.Insert(40);
    DescavlTree.Insert(50); 
    cout<<DescavlTree;
    cout<<endl;
    cout << "Ingresa un numero para insertar en el AVL TREE: ";
    cin >> avlTree; 
    cout << "Arbol AVL actual: " << avlTree << "\n\n";

    cout << "InOrden:   "; avlTree.inorden(PrintNode); cout << endl;
    cout << "PreOrden:  "; avlTree.preorden(PrintNode); cout << endl;
    cout << "PostOrden: "; avlTree.postorden(PrintNode); cout << "\n\n";

    cout << "Foreach: "; 
    avlTree.Foreach(PrintNode); 
    cout << endl;

    auto pNode1 = avlTree.FirstThat(IsExactMatch, 30);
    if (pNode1 != nullptr) {
        cout << "FirstThat (== 30): Encontrado con exito.\n\n";
    }
   
    auto pNode2 = avlTree.FirstThat(IsGreaterThan, 20);
    if (pNode2 != nullptr) {
        cout << "FirstThat (> 20): Encontrado con exito.\n\n";
    }
    avlTree.Remove(20);
    cout << "Despues de Remove(20): " << avlTree << "\n\n";
    cout<<"COPY CONSTRUCTOR :"<<endl;
    CAVLTree<TreeTraitAscending<int>> copyAvl = avlTree;
    cout<<"Original(AVL Tree) :"<<avlTree<<endl;
    cout<<"Copia (Copy AVL)   :"<<copyAvl<<endl;
    cout<<endl;
    cout<<"MOVE CONSTRUCTOR : "<<endl;
    CAVLTree<TreeTraitAscending<int>> moveAvl = std::move(copyAvl);
    cout<<"Original(Copy AVL) :"<<copyAvl<<endl;
    cout<<"Copia (Move AVL)   :"<<moveAvl<<endl;
    cout<<endl;
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