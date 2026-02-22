//
// Created by aroon on 2/18/26.
//

#include "containers/binarytree.h"

#include <cassert>
#include <fstream>
#include <sstream>
#include <vector>

using std::vector;

std::ofstream logFile("binarytree_tests.log");

void TestInsertAndIterators() {
    using Traits = TreeTraitAscending<T1>;
    CBinaryTree<Traits> tree;

    tree.Insert(5, 1);
    tree.Insert(3, 2);
    tree.Insert(7, 3);
    tree.Insert(2, 4);
    tree.Insert(4, 5);

    vector<T1> fwd;
    for (auto it = tree.begin(); it != tree.end(); ++it) fwd.push_back(*it);
    assert((fwd == vector<T1>({2,3,4,5,7})));

    vector<T1> bwd;
    for (auto it = tree.rbegin(); it != tree.rend(); ++it) bwd.push_back(*it);
    assert((bwd == vector<T1>({7,5,4,3,2})));

    logFile << "TestInsertAndIterators: OK\n";
}

void TestTraversals() {
    using Traits = TreeTraitAscending<T1>;
    CBinaryTree<Traits> tree;

    tree.Insert(5, 1);
    tree.Insert(3, 2);
    tree.Insert(7, 3);
    tree.Insert(2, 4);
    tree.Insert(4, 5);

    vector<T1> pre;
    tree.preorderTraversal([&pre](T1 &v){ pre.push_back(v); });
    assert((pre == vector<T1>({5,3,2,4,7})));

    vector<T1> in;
    tree.inorderTraversal([&in](T1 &v){ in.push_back(v); });
    assert((in == vector<T1>({2,3,4,5,7})));

    vector<T1> post;
    tree.postorderTraversal([&post](T1 &v){ post.push_back(v); });
    assert((post == vector<T1>({2,4,3,7,5})));

    logFile << "TestTraversals: OK\n";
}

void TestRemove() {
    using Traits = TreeTraitAscending<T1>;
    CBinaryTree<Traits> tree;

    for (T1 v : {5,3,7,2,4,6,8,1}) tree.Insert(v, v);

    T1 leaf = 1;
    assert(tree.remove(leaf) == 1);

    T1 one_child = 2;
    assert(tree.remove(one_child) == 2);

    T1 two_children = 7;
    assert(tree.remove(two_children) == 7);

    vector<T1> in;
    tree.inorderTraversal([&in](T1 &v){ in.push_back(v); });
    assert((in == vector<T1>({3,4,5,6,8})));

    logFile << "TestRemove: OK\n";
}

void TestCopyMoveAndIO() {
    using Traits = TreeTraitAscending<T1>;
    CBinaryTree<Traits> tree;
    for (T1 v : {5,3,7,2,4,6,8}) tree.Insert(v, v);

    CBinaryTree<Traits> copy(tree);
    vector<T1> in_copy;
    copy.inorderTraversal([&in_copy](T1 &v){ in_copy.push_back(v); });
    assert((in_copy == vector<T1>({2,3,4,5,6,7,8})));

    CBinaryTree<Traits> moved(std::move(tree));
    vector<T1> in_moved;
    moved.inorderTraversal([&in_moved](T1 &v){ in_moved.push_back(v); });
    assert((in_moved == vector<T1>({2,3,4,5,6,7,8})));

    std::ostringstream oss;
    oss << moved;
    std::istringstream iss(oss.str());
    CBinaryTree<Traits> restored;
    iss >> restored;

    vector<T1> in_restored;
    restored.inorderTraversal([&in_restored](T1 &v){ in_restored.push_back(v); });
    assert((in_restored == in_moved));

    logFile << "TestCopyMoveAndIO: OK\n";
}

void TestForeachFirstThat() {
    using Traits = TreeTraitAscending<T1>;
    CBinaryTree<Traits> tree;
    for (T1 v : {5,3,7}) tree.Insert(v, v);

    int count = 0;
    auto count_fn = [&count](T1 &){ count++; };
    tree.Foreach(count_fn);
    assert(count == 3);

    auto is_seven = [](T1 &v){ return v == 7; };
    auto found = tree.FirstThat(is_seven);
    assert(found == 7);

    logFile << "TestForeachFirstThat: OK\n";
}

void DemoBinaryTree() {
    TestInsertAndIterators();
    TestTraversals();
    TestRemove();
    TestCopyMoveAndIO();
    TestForeachFirstThat();
}
