#include <iostream>
#include "containers/binarytree.h"
using namespace std;

void DemoBinaryTree() {
    CBinaryTree<TreeTraitAscending<int>> tree;

    tree << 50 << 30 << 70 << 20 << 40 << 60 << 80;

    // Inorden
    cout << "Inorden: ";
    tree.Inorden([](int val){ cout << val << " "; });
    cout << endl;

    // Preorden
    cout << "Preorden: ";
    tree.Preorden([](int val){ cout << val << " "; });
    cout << endl;

    // Postorden
    cout << "Postorden: ";
    tree.Postorden([](int val){ cout << val << " "; });
    cout << endl;

    // Forward iterator
    cout << "Forward iterator: ";
    for (auto it = tree.begin(); it != tree.end(); ++it)
        cout << *it << " ";
    cout << endl;

    // Backward iterator
    cout << "Backward iterator: ";
    for (auto it = tree.rbegin(); it != tree.rend(); ++it)
        cout << *it << " ";
    cout << endl;

    // operator
    cout << "operator<<: " << tree << endl;

    // FirstThat (primer multiplo de 3)
    auto found = tree.FirstThat([](int val){ return val % 3 == 0; });
    if (found) cout << "Primer multiplo de 3: " << *found << endl;

    // operator>> extrae el minimo
    int val;
    tree >> val;
    cout << "Extraido (minimo, debe ser 20): " << val << endl;
    cout << "Arbol tras extraccion: " << tree << endl;

    // Copy constructor
    CBinaryTree<TreeTraitAscending<int>> treeCopy(tree);
    cout << "Copia: " << treeCopy << endl;

    // Move constructor
    CBinaryTree<TreeTraitAscending<int>> treeMoved(std::move(tree));
    cout << "Movido: " << treeMoved << endl;
}