#include <iostream>
#include "containers/binarytree.h"
#include "containers/avl.h"

using namespace std;

void DemoTrees(){

    cout << " DEMO CBinaryTree (BST)\n";

    CBinaryTree<TreeTraitAscending<int>> tree;

    // INSERTAR ELEMENTOS
    tree.Insert(50, 1);
    tree.Insert(30, 2);
    tree.Insert(70, 3);
    tree.Insert(20, 4);
    tree.Insert(40, 5);
    tree.Insert(60, 6);
    tree.Insert(80, 7);

    cout << "Operator<<: \n";
    cout << tree << endl << endl;

    // RECORRIDOS
    cout << "InOrder: ";
    tree.InOrder(PrintInt);
    cout << endl;

    cout << "PreOrder: ";
    tree.PreOrder(PrintInt);
    cout << endl;

    cout << "PostOrder: ";
    tree.PostOrder(PrintInt);
    cout << endl << endl;

    // FOREACH (equivalente a InOrder)
    cout << "Foreach (InOrder): ";
    tree.Foreach(PrintInt);
    cout << endl << endl;

    // FIRST THAT
    auto par = tree.FirstThat(EsPar);
    if(par != tree.end()){
        cout << "Primer número par encontrado: " << *par << endl;
    }

    cout << endl;

    // ITERADORES FORWARD
    cout << "Iteración FORWARD (ascendente): ";
    for(auto it = tree.begin(); it != tree.end(); ++it){
        cout << *it << " ";
    }
    cout << endl;

    // ITERADORES BACKWARD
    cout << "Iteración BACKWARD (descendente): ";
    for(auto it = tree.rbegin(); it != tree.rend(); ++it){
        cout << *it << " ";
    }
    cout << endl << endl;

    // REMOVE
    cout << "Eliminando 70...\n";
    tree.Remove(70);
    cout << "Árbol luego de eliminar 70:\n";
    cout << tree << endl << endl;

    // Operator>>
    cout << "Ingrese un nuevo árbol: \n";
    cin >> tree;

    cout << "Árbol ingresado:\n";
    cout << tree << endl;

    
    // AVL
    

    cout << " DEMO CBinaryTreeAVL (balanceado)\n";

    CBinaryTreeAVL<TreeTraitAscending<int>> avl;

    // INSERTAR MUCHOS ELEMENTOS (para forzar rotaciones)
    int valores[] = {50,40,30,20,10,90,70,80,100,60};

    for(int i = 0; i < 10; i++){
        avl.Insert(valores[i], i+1);
    }

    cout << "AVL balanceado (operator<<):\n";
    cout << avl << endl << endl;

    // RECORRIDOS
    cout << "InOrder AVL: ";
    avl.InOrder(PrintInt);
    cout << endl;

    cout << "PreOrder AVL: ";
    avl.PreOrder(PrintInt);
    cout << endl;

    cout << "PostOrder AVL: ";
    avl.PostOrder(PrintInt);
    cout << endl << endl;

    // ITERADORES
    cout << "Forward AVL: ";
    for(auto it = avl.begin(); it != avl.end(); ++it){
        cout << *it << " ";
    }
    cout << endl;

    cout << "Backward AVL: ";
    for(auto it = avl.rbegin(); it != avl.rend(); ++it){
        cout << *it << " ";
    }
    cout << endl << endl;

    // FIRST THAT
    auto parAVL = avl.FirstThat(EsPar);
    if(parAVL != avl.end()){
        cout << "Primer par en AVL: " << *parAVL << endl;
    }

    cout << endl;

    // REMOVE EN AVL
    cout << "Eliminando 50 (raíz AVL)...\n";
    avl.Remove(50);

    cout << "AVL luego de eliminar 50:\n";
    cout << avl << endl << endl;

    cout << "Recorrido InOrder final AVL: ";
    avl.InOrder(PrintInt);
    cout << endl;


}

// Función auxiliar para imprimir
void PrintInt(int &x){
    cout << x << " ";
}

// Función para FirstThat
bool EsPar(int &x){
    return x % 2 == 0;
}
