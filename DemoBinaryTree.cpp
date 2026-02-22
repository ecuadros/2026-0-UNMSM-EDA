#include <iostream>
#include "containers/binarytree.h"
#include "variadic-util.h"

using namespace std;

// funciones variadic
void Imprimir(int &n){
    cout << n << " ";
}

bool EsMayorQue(int &n, int limite){
    return n > limite;
}

void DemoBinaryTree(){
    cout << "===== BST Ascending =====" << endl;
    CBTree< AscendingBTreeTrait<int> > bst;

    bst.Insert(50, 1);
    bst.Insert(30, 2);
    bst.Insert(70, 3);
    bst.Insert(20, 4);
    bst.Insert(40, 5);
    bst.Insert(60, 6);
    bst.Insert(80, 7);
    cout << bst;

    // Inorder con variadic
    cout << "Inorder:   ";
    bst.Inorder(Imprimir);
    cout << endl;

    // Preorder con variadic
    cout << "Preorder:  ";
    bst.Preorder(Imprimir);
    cout << endl;

    // Postorder con variadic
    cout << "Postorder: ";
    bst.Postorder(Imprimir);
    cout << endl;

    // Forward Iterator (inorder)
    cout << "Forward:   ";
    for( auto it = bst.begin(); it != bst.end(); ++it )
        cout << *it << " ";
    cout << endl;

    // Backward Iterator (reverse inorder)
    cout << "Backward:  ";
    for( auto it = bst.rbegin(); it != bst.rend(); ++it )
        cout << *it << " ";
    cout << endl;

    // foreach
    cout << endl << "===== Foreach(Suma, 100) =====" << endl;
    bst.Foreach(Suma<int>, 100);
    cout << "Inorder:   ";
    bst.Inorder(Imprimir);
    cout << endl;

    // firstthat
    cout << endl << "===== FirstThat(>150) =====" << endl;
    auto it = bst.FirstThat(EsMayorQue, 150);
    if( it != bst.end() )
        cout << "Encontrado: " << *it << endl;

    // Remove
    cout << endl << "===== Remove(150) =====" << endl;
    bst.Remove(150);
    cout << bst;

    // Constructor Copia
    cout << "===== Constructor Copia =====" << endl;
    CBTree< AscendingBTreeTrait<int> > copia(bst);
    cout << "Copia: " << copia;

    // Move Constructor
    cout << "===== Move Constructor =====" << endl;
    CBTree< AscendingBTreeTrait<int> > moved(std::move(copia));
    cout << "Moved: " << moved;
    cout << "Copia (vacio): size = " << copia.getSize() << endl;
}
