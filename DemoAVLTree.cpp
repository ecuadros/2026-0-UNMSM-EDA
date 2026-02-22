#include "containers/avl.h"
#include <fstream>
#include <vector>

using std::vector;

using Traits = TreeTraitAscending<T1>;
using AVL = CAVLTree<Traits>;

static std::ofstream logFile("avltree_tests.log");

void AssertBalanced(AVL &tree) {
    CAVLTree<Traits>::AssertBalanced(tree);
}

void TestInsertBalance() {
    AVL tree;

    for (T1 v : {30, 20, 10}) {
        tree.Insert(v, v);
        AssertBalanced(tree);
    }

    for (T1 v : {25, 28, 27}) {
        tree.Insert(v, v);
        AssertBalanced(tree);
    }

    for (T1 v : {5, 4, 3, 2, 1}) {
        tree.Insert(v, v);
        AssertBalanced(tree);
    }

    logFile << "TestInsertBalance: OK\n";
}

void TestRemoveBalance() {
    AVL tree;

    for (T1 v : {50, 30, 70, 20, 40, 60, 80, 10, 25, 35, 45}) {
        tree.Insert(v, v);
    }
    AssertBalanced(tree);

    for (T1 v : {10, 25, 30, 70, 50}) {
        tree.remove(v);
        AssertBalanced(tree);
    }

    logFile << "TestRemoveBalance: OK\n";
}

void DemoAVLTree() {
    TestInsertBalance();
    TestRemoveBalance();
}
