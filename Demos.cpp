#include <iostream>
#include "containers/binarytree.h"
#include "containers/avltree.h"

using namespace std;

void DemoBinaryTree() {
    
    cout << "\n CBinaryTree Ascendente " << endl;

    CBinaryTree<TreeTraitAscending<T1>> ascTree;
    ascTree.Insert(50, 0);
    ascTree.Insert(30, 1);
    ascTree.Insert(70, 2);
    ascTree.Insert(20, 3);
    ascTree.Insert(40, 4);
    ascTree.Insert(60, 5);
    ascTree.Insert(80, 6);

    cout << "\nArbol tras Insert (50, 30, 70, 20, 40, 60, 80):" << endl;
    cout << ascTree << endl;

    cout << "Recorrido preorden:" << endl;
    ascTree.preorden([](auto &val) { cout << val << " "; });
    cout << endl;

    cout << "Recorrido postorden:" << endl;
    ascTree.postorden([](auto &val) { cout << val << " "; });
    cout << endl;

    cout << "\nEliminando 30" << endl;
    ascTree.Remove(30);
    cout << ascTree << endl;

    // Copy constructor
    CBinaryTree<TreeTraitAscending<T1>> ascCopy(ascTree);
    cout << "Copy constructor:" << endl;
    cout << ascCopy << endl;

    // Move constructor
    CBinaryTree<TreeTraitAscending<T1>> ascMoved(move(ascTree));
    cout << "Move constructor (tamaño del arbol original: " << ascTree.getSize() << "):" << endl;
    cout << "Nuevo arbol: " << endl;
    cout << ascMoved << endl;

    // Foreach
    cout << "Foreach (valores * 2):" << endl;
    ascMoved.Foreach([](auto &val) { cout << val * 2 << " "; });
    cout << endl;

    // FirstThat
    auto it = ascMoved.FirstThat([](auto &val) { return val > 50; });
    if (it != ascMoved.end())
        cout << "FirstThat (primer valor > 50): " << *it << endl;

    // Iterador forward
    cout << "\nIterador forward:" << endl;
    for (auto it = ascMoved.begin(); it != ascMoved.end(); ++it)
        cout << *it << " ";
    cout << endl;

    // Iterador backward
    cout << "Iterador backward:" << endl;
    for (auto it = ascMoved.rbegin(); it != ascMoved.rend(); ++it)
        cout << *it << " ";
    cout << endl;

    // cin
    cout << "\nIngresa un par (valor ref) para insertar en el arbol:" << endl;
    cin >> ascMoved;
    cout << ascMoved << endl;

    // ── Descending ──
    cout << "\n Demo CBinaryTree Descendente " << endl;

    CBinaryTree<TreeTraitDescending<T1>> descTree;
    descTree.Insert(50, 0);
    descTree.Insert(30, 1);
    descTree.Insert(70, 2);
    descTree.Insert(20, 3);
    descTree.Insert(40, 4);

    cout << "\nArbol tras Insert (50, 30, 70, 20, 40):" << endl;
    cout << descTree << endl;

    cout << "Eliminando 50" << endl;
    descTree.Remove(50);
    cout << descTree << endl;

    CBinaryTree<TreeTraitDescending<T1>> descCopy(descTree);
    cout << "Copy constructor:" << endl;
    cout << descCopy << endl;

    CBinaryTree<TreeTraitDescending<T1>> descMoved(move(descTree));
    cout << "Move constructor (tamaño del arbol original: " << descTree.getSize() << "):" << endl;
    cout << "Nuevo arbol: " << endl;
    cout << descMoved << endl;
    cout << "\nIngresa un par (valor ref) para insertar en el arbol:" << endl;
    cin >> descMoved;
    cout << descMoved << endl;
}

void DemoAvlTree() {
    // ── Ascending ──
    cout << "\n Demo CAvlTree Ascending " << endl;

    CAvlTree<AVLTraitAscending<T1>> ascAvl;
    ascAvl.Insert(10, 0);
    ascAvl.Insert(20, 1);
    ascAvl.Insert(30, 2);  // provoca rotación izquierda
    ascAvl.Insert(40, 3);
    ascAvl.Insert(50, 4);  // provoca rotación izquierda
    ascAvl.Insert(25, 5);  // provoca rotación derecha-izquierda

    cout << "\nArbol tras Insert (10, 20, 30, 40, 50, 25):" << endl;
    cout << ascAvl << endl;

    cout << "Recorrido preorden:" << endl;
    ascAvl.preorden([](auto &val) { cout << val << " "; });
    cout << endl;

    cout << "Recorrido postorden:" << endl;
    ascAvl.postorden([](auto &val) { cout << val << " "; });
    cout << endl;

    cout << "\nEliminando 30" << endl;
    ascAvl.Remove(30);
    cout << ascAvl << endl;

    // Copy constructor
    CAvlTree<AVLTraitAscending<T1>> ascCopy(ascAvl);
    cout << "Copy constructor:" << endl;
    cout << ascCopy << endl;

    // Move constructor
    CAvlTree<AVLTraitAscending<T1>> ascMoved(move(ascAvl));
    cout << "Move constructor (tamaño del arbol original: " << ascAvl.getSize() << "):" << endl;
    cout << "Nuevo arbol: " << endl;
    cout << ascMoved << endl;

    // Foreach
    cout << "Foreach (valores * 2):" << endl;
    ascMoved.Foreach([](auto &val) { cout << val * 2 << " "; });
    cout << endl;

    // FirstThat
    auto it = ascMoved.FirstThat([](auto &val) { return val > 25; });
    if (it != ascMoved.end())
        cout << "FirstThat (primer valor > 25): " << *it << endl;

    // Iterador forward
    cout << "\nIterador forward:" << endl;
    for (auto it = ascMoved.begin(); it != ascMoved.end(); ++it)
        cout << *it << " ";
    cout << endl;

    // Iterador backward
    cout << "Iterador backward:" << endl;
    for (auto it = ascMoved.rbegin(); it != ascMoved.rend(); ++it)
        cout << *it << " ";
    cout << endl;

    // cin
    cout << "\nIngresa un par (valor ref) para insertar en el arbol:" << endl;
    cin >> ascMoved;
    cout << ascMoved << endl;

    // ── Descending ──
    cout << "\n Demo CAvlTree Descending " << endl;

    CAvlTree<AVLTraitDescending<T1>> descAvl;
    descAvl.Insert(50, 0);
    descAvl.Insert(40, 1);
    descAvl.Insert(30, 2);  // provoca rotación derecha
    descAvl.Insert(20, 3);
    descAvl.Insert(10, 4);  // provoca rotación derecha
    descAvl.Insert(35, 5);  // provoca rotación izquierda-derecha

    cout << "\nArbol tras Insert (50, 40, 30, 20, 10, 35):" << endl;
    cout << descAvl << endl;

    cout << "\nEliminando 40" << endl;
    descAvl.Remove(40);
    cout << descAvl << endl;

    CAvlTree<AVLTraitDescending<T1>> descCopy(descAvl);
    cout << "Copy constructor:" << endl;
    cout << descCopy << endl;

    CAvlTree<AVLTraitDescending<T1>> descMoved(move(descAvl));
    cout << "Move constructor (tamaño del arbol original: " << descAvl.getSize() << "):" << endl;
    cout << descMoved << endl;
}

