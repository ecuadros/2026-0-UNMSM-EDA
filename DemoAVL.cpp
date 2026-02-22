#include <iostream>
#include "containers/lists.h"
#include <utility>
#include "containers/AVL-Tree.h"
using namespace std;
void PrintItem(int& val) {
    cout << val << " ";
}

bool IsGreaterThan(int& val, int threshold) {
    return val > threshold;
}
void DemoAVL(){
   cout << "\n========================================\n";
    cout << "   DEMO: CAVLTree (Descendente)\n";
    cout << "========================================\n";
    
    CAVLTree<TreeTraitDescending<int>> avl;
    
    // Inserción secuencial (En un BST normal crearía una rama infinita, 
    // pero el AVL usará sus rotaciones para balancearse automáticamente).
    int arr[] = {10, 20, 30, 40, 50, 60, 70};
    for(int i = 0; i < 7; ++i) {
        avl.Insert(arr[i]);
    }

    // Prueba de operador >> (cin)
    cout << "Ingrese un numero extra para el AVL: ";
    cin >> avl; 

    // Prueba de operador << (cout)
    cout << "Impresion (cout <<): " << avl << "\n\n";

    // Modos de lectura
    cout << "[Recorridos]\n";
    cout << "Preorden:  "; avl.preorden(PrintItem);  cout << "\n";
    cout << "Inorden:   "; avl.inorden(PrintItem);   cout << "\n";
    cout << "Postorden: "; avl.postorden(PrintItem); cout << "\n\n";

    // Iteradores
    cout << "[Iteradores]\n";
    cout << "Forward:   ";
    for (auto it = avl.begin(); it != avl.end(); ++it) cout << *it << " ";
    cout << "\nBackward:  ";
    for (auto it = avl.rbegin(); it != avl.rend(); ++it) cout << *it << " ";

    // Algoritmos variádicos
    cout << "\n\n[Algoritmos]\n";
    cout << "Foreach:   ";
    avl.Foreach(PrintItem);
    
    auto itFound = avl.FirstThat(IsGreaterThan, 45);
    cout << "\nFirstThat (> 45): ";
    if (itFound != avl.end()) cout << *itFound;
    cout << "\n\n";


    
}