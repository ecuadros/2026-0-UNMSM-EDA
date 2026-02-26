#include <time.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include "containers/BTree.h"
using namespace std;

const char * keys1 = "D1XJ2xTg8zKL9AhijOPQcEowRSp0NbW567BUfCqrs4FdtYZakHIuvGV3eMylmn";

const int BTreeSize = 3;
void DemoBTree(){
    int i;
    BTree<char> bt(BTreeSize);
    for (i = 0; keys1[i]; i++)
        bt.Insert(keys1[i], i*i);

    bt.Print(cout);

    // Forward iterator
    cout << "\nForward iterator: ";
    for (auto it = bt.begin(); it != bt.end(); ++it)
        cout << (*it).key << " ";
    cout << endl;

    // Backward iterator
    cout << "Backward iterator: ";
    for (auto it = bt.rbegin(); it != bt.rend(); ++it)
        cout << (*it).key << " ";
    cout << endl;

    // ForEach variadic
    cout << "ForEach variadic: ";
    bt.ForEachVariadic([](auto& obj) {
        cout << obj.key << " ";
    });
    cout << endl;

    // FirstThat variadic
    auto found = bt.FirstThatVariadic([](auto& obj) {
        return obj.key == 'A';
    });
    if (found) cout << "FirstThat - encontrado: " << found->key << endl;

    // Move constructor
    BTree<char> bt2(std::move(bt));
    cout << "Move constructor - bt2 forward: ";
    for (auto it = bt2.begin(); it != bt2.end(); ++it)
        cout << (*it).key << " ";
    cout << endl;
}