#include <iostream>
#include "containers/lists.h"
#include <utility>
#include "containers/binarytree.h"
using namespace std;
void PrintItem(int& val) {
    cout << val << " ";
}

bool IsGreaterThan(int& val, int threshold) {
    return val > threshold;
}
void DemoBinaryTree(){
   cout << "\n========================================\n";
    cout << "   DEMO: CBinaryTree (Ascendente)\n";
    cout << "========================================\n";
    
    CBinaryTree<TreeTraitAscending<int>> bst;
    
    // Inserciones manuales
    int arr[] = {50, 30, 70, 20, 40, 60, 80};
    for(int i = 0; i < 7; ++i) {
        bst.Insert(arr[i]);
    }

    // Prueba de operador >> (cin)
    cout << "Ingrese un numero extra para el BST: ";
    cin >> bst; 

    // Prueba de operador << (cout)
    cout << "Impresion (cout <<): " << bst << "\n\n";

    // Modos de lectura
    cout << "[Recorridos]\n";
    cout << "Preorden:  "; bst.preorden(PrintItem);  cout << "\n";
    cout << "Inorden:   "; bst.inorden(PrintItem);   cout << "\n";
    cout << "Postorden: "; bst.postorden(PrintItem); cout << "\n\n";

    // Iteradores
    cout << "[Iteradores]\n";
    cout << "Forward:   ";
    for (auto it = bst.begin(); it != bst.end(); ++it) cout << *it << " ";
    cout << "\nBackward:  ";
    for (auto it = bst.rbegin(); it != bst.rend(); ++it) cout << *it << " ";

    // Algoritmos variádicos
    cout << "\n\n[Algoritmos]\n";
    cout << "Foreach:   ";
    bst.Foreach(PrintItem);
    
    auto itFound = bst.FirstThat(IsGreaterThan, 65);
    cout << "\nFirstThat (> 65): ";
    if (itFound != bst.end()) cout << *itFound;
    cout << "\n";



}