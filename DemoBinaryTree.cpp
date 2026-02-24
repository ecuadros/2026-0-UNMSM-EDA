#include <iostream>
#include "containers/binarytree.h"

using namespace std;

// Funciones auxiliares
void PrintValue(int &val) {
    cout << val << " ";
}

void MultiplyBy2(int &val) {
    val *= 2;
}

void DemoBinaryTree() {
    cout << "========================================" << endl;
    cout << "        DEMO: BINARY TREE " << endl;
    cout << "========================================" << endl << endl;
    
    // 1. Crear árbol y hacer Insert
    cout << "1. Crear Binary Tree (Ascending) y hacer Insert:" << endl;
    CBinaryTree<TreeTraitAscending<int>> tree1;
    tree1.Insert(50, 5);
    tree1.Insert(30, 3);
    tree1.Insert(70, 7);
    tree1.Insert(20, 2);
    tree1.Insert(40, 4);
    tree1.Insert(60, 6);
    tree1.Insert(80, 8);
    cout << tree1 << endl;
    
    // 2. Forward Iterator (Inorden)
    cout << "2. Forward Iterator (Inorden - ascendente):" << endl;
    cout << "Elementos: ";
    for (auto it = tree1.begin(); it != tree1.end(); ++it) {
        cout << *it << " ";
    }
    cout << endl << endl;
    
    // 3. Backward Iterator (Inorden inverso)
    cout << "3. Backward Iterator (Inorden inverso - descendente):" << endl;
    cout << "Elementos: ";
    for (auto it = tree1.rbegin(); it != tree1.rend(); ++it) {
        cout << *it << " ";
    }
    cout << endl << endl;
    
    // 4. Recorrido Inorden
    cout << "4. Recorrido Inorden:" << endl;
    cout << "Elementos: ";
    tree1.Inorden(PrintValue);
    cout << endl << endl;
    
    // 5. Recorrido Preorden
    cout << "5. Recorrido Preorden:" << endl;
    cout << "Elementos: ";
    tree1.Preorden(PrintValue);
    cout << endl << endl;
    
    // 6. Recorrido Posorden
    cout << "6. Recorrido Posorden:" << endl;
    cout << "Elementos: ";
    tree1.Posorden(PrintValue);
    cout << endl << endl;
    
    // 7. Foreach (usa Inorden)
    cout << "7. Foreach - Multiplicar por 2:" << endl;
    tree1.Foreach(MultiplyBy2);
    cout << "Despues de multiplicar: " << tree1 << endl;
    
    // 8. FirstThat
    cout << "8. FirstThat - Buscar elemento > 100:" << endl;
    int *pFound = tree1.FirstThat([](int val){ return val > 100; });
    if (pFound)
        cout << "Encontrado: " << *pFound << endl;
    else
        cout << "No encontrado" << endl;
    cout << endl;
    
    cout << "9. FirstThat - Buscar elemento == 60:" << endl;
    pFound = tree1.FirstThat([](int val){ return val == 60; });
    if (pFound)
        cout << "Encontrado: " << *pFound << endl;
    else
        cout << "No encontrado" << endl;
    cout << endl;
    
    // 10. Remove
    cout << "10. Remove - Eliminar elementos:" << endl;
    CBinaryTree<TreeTraitAscending<int>> tree2;
    tree2.Insert(50);
    tree2.Insert(30);
    tree2.Insert(70);
    tree2.Insert(20);
    tree2.Insert(40);
    tree2.Insert(60);
    tree2.Insert(80);
    
    cout << "Arbol original: " << tree2;
    tree2.Remove(20);
    cout << "Despues de Remove(20): " << tree2;
    tree2.Remove(30);
    cout << "Despues de Remove(30): " << tree2;
    tree2.Remove(50);
    cout << "Despues de Remove(50): " << tree2 << endl;
    
    // 11. Constructor Copia
    cout << "11. Constructor Copia:" << endl;
    CBinaryTree<TreeTraitAscending<int>> tree3;
    tree3.Insert(100);
    tree3.Insert(50);
    tree3.Insert(150);
    
    CBinaryTree<TreeTraitAscending<int>> treeCopy(tree3);
    cout << "Arbol original: " << tree3;
    cout << "Arbol copiado:  " << treeCopy << endl;
    
    // 12. Move Constructor
    cout << "12. Move Constructor:" << endl;
    CBinaryTree<TreeTraitAscending<int>> tree4;
    tree4.Insert(10);
    tree4.Insert(5);
    tree4.Insert(15);
    
    CBinaryTree<TreeTraitAscending<int>> treeMoved(std::move(tree4));
    cout << "Arbol movido: " << treeMoved;
    cout << "Arbol original (vacio): " << tree4 << endl;
    
    // 13. Operator < y >
    cout << "13. Comparacion de arboles (operator < y >):" << endl;
    CBinaryTree<TreeTraitAscending<int>> t1;
    t1.Insert(1);
    t1.Insert(2);
    
    CBinaryTree<TreeTraitAscending<int>> t2;
    t2.Insert(1);
    t2.Insert(2);
    t2.Insert(3);
    t2.Insert(4);
    
    cout << "t1 size: " << t1.getSize() << endl;
    cout << "t2 size: " << t2.getSize() << endl;
    cout << "t1 < t2: " << (t1 < t2 ? "true" : "false") << endl;
    cout << "t1 > t2: " << (t1 > t2 ? "true" : "false") << endl << endl;
    
    // 14. Árbol Descendente
    cout << "14. Binary Tree (Descending):" << endl;
    CBinaryTree<TreeTraitDescending<int>> treeDesc;
    treeDesc.Insert(50);
    treeDesc.Insert(30);
    treeDesc.Insert(70);
    treeDesc.Insert(20);
    treeDesc.Insert(40);
    
    cout << "Arbol descendente (Inorden): ";
    treeDesc.Inorden(PrintValue);
    cout << endl << endl;
    
    // 15. Visualización del árbol
    cout << "15. Estructura del arbol:" << endl;
    cout << "Insertando: 50, 30, 70, 20, 40, 60, 80" << endl << endl;
    cout << "Estructura:" << endl;
    cout << "        50" << endl;
    cout << "       /  \\" << endl;
    cout << "      30   70" << endl;
    cout << "     / \\   / \\" << endl;
    cout << "    20 40 60 80" << endl << endl;
    
    cout << "Inorden (Izq-Raiz-Der):  20 30 40 50 60 70 80" << endl;
    cout << "Preorden (Raiz-Izq-Der): 50 30 20 40 70 60 80" << endl;
    cout << "Posorden (Izq-Der-Raiz): 20 40 30 60 80 70 50" << endl << endl;
    
    // 16. getSize e isEmpty
    cout << "16. getSize e isEmpty:" << endl;
    cout << "tree1.getSize() = " << tree1.getSize() << endl;
    cout << "tree1.isEmpty() = " << (tree1.isEmpty() ? "true" : "false") << endl << endl;
    
    // 17. Operator >> (Ingreso de datos por consola)
    cout << "17. Operator >> (Ingreso por consola):" << endl;
    CBinaryTree<TreeTraitAscending<int>> treeInput;
    cout << "Ingrese 3 numeros enteros separados por espacio y presione Enter (Ej: 15 8 22): ";
    for (int i = 0; i < 3; ++i) {
        cin >> treeInput; 
    }
    cout << "\nArbol creado con operator>>: \n" << treeInput << endl;

    // 18. Clear
    cout << "18. Limpiar arbol con Clear():" << endl;
    tree1.Clear();
    cout << "Despues de Clear(): " << tree1;
    cout << "Esta vacio? " << (tree1.isEmpty() ? "SI" : "NO") << endl << endl;
    
    cout << "========================================" << endl;
    cout << "         FIN DEMO BINARY TREE" << endl;
    cout << "========================================" << endl << endl;
}
