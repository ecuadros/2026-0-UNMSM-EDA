#include <iostream>
#include "containers/binarytree.h"
#include "containers/avl.h"
#include "variadic-util.h"

using namespace std;

template <typename T>
void Print(T& val) {
    cout << val << " ";
}

void DemoBinaryTree() {
    // BinaryTree básico
    CBinaryTree<TreeTraitAscending<int>> bt;
    bt.Insert(50);
    bt.Insert(30);
    bt.Insert(70);
    bt.Insert(20);
    bt.Insert(40);
    
    cout << "BinaryTree: " << endl;
    cout << bt << endl;
    
    cout << "Inorden: ";
    bt.inorden(&Print<int>);
    cout << endl;
    
    cout << "Preorden: ";
    bt.preorden(&Print<int>);
    cout << endl;
    
    // AVL con rotaciones
    CAVLTree<TreeTraitAscending<int>> avl;
    avl.Insert(10);
    avl.Insert(20);
    avl.Insert(30);  // Provoca rotación
    avl.Insert(40);
    avl.Insert(50);  // Provoca rotación
    
    cout << "\nAVL Tree (auto-balanceado): " << endl;
    cout << avl << endl;
}
