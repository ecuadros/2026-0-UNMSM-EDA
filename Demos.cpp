#include <iostream>
#include "containers/binarytree.h"
#include "containers/avl.h"

using namespace std;

void DemoTrees() {
    cout << "-----------------------------------------" << endl;
    cout << "\t BINARY TREE" << endl;
    cout << "-----------------------------------------" << endl;

    CBinaryTree<TreeTraitAscending<int>> bst;

    bst.Insert(70); bst.Insert(30); 
    bst.Insert(75); bst.Insert(90);

    cout << "Inorder: " << bst << endl;

    cout << "Preorder (x10): ";
    bst.Preorder([](int& v, ref_type& r, int factor) {
        cout << (v * factor) << " ";
    }, 10);
    cout << endl;

    int target = 75;
    int* found = bst.FirstThat([](int& v, ref_type& r, int t) {
        return v == t;
    }, target);
    
    if(found) cout << "Encontrado FirstThat: " << *found << endl;

    cout << "Iterando con Forward: ";
    for(auto it = bst.begin(); it != bst.end(); ++it) {
        cout << *it << " ";
    }
    cout << endl;

    cout << "\n---------------------------------" << endl;
    cout << "\tAVL (BALANCEADO)" << endl;
    cout << "---------------------------------" << endl;

    CAVL<TreeTraitAscending<int>> avl;
    
    avl.Insert(10, 1); avl.Insert(20, 2); avl.Insert(30, 3);
    
    cout << "AVL: " << avl << endl;
}