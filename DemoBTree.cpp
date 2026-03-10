#include <time.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include "containers/BTree.h"

const char * keys1 = "D1XJ2xTg8zKL9AhijOPQcEowRSp0NbW567BUfCqrs4FdtYZakHIuvGV3eMylmn";

const int BTreeSize = 3;

template <typename keyType, typename ObjIDType>
void PrintKey(tagObjectInfo<keyType, ObjIDType> &info) {
    cout << info.key << " ";
}

template <typename keyType, typename ObjIDType>
bool EsLetraG(tagObjectInfo<keyType, ObjIDType> &info) {
    return info.key == 'G';
}

void DemoBTree(){
    BTree<char> bt(BTreeSize);

    for (int i = 0; keys1[i]; i++)
        bt.Insert(keys1[i], i * i);

    cout << bt;

    bt.Inorden(&PrintKey<char, long>);
    cout << endl;

    bt.Preorden(&PrintKey<char, long>);
    cout << endl;

    bt.Postorden(&PrintKey<char, long>);
    cout << endl;

    bt.ForEach(&PrintKey<char, long>);
    cout << endl;

    auto *found = bt.FirstThat(&EsLetraG<char, long>);
    if (found) cout << found->key << " -> " << found->ObjID << endl;

    for (auto it = bt.begin(); it != bt.end(); ++it)
        cout << (*it).key << " ";
    cout << endl;

    for (auto it = bt.rbegin(); it != bt.rend(); ++it)
        cout << (*it).key << " ";
    cout << endl;

    BTree<char> bt2(std::move(bt));
    cout << "size: " << bt2.size() << endl;
}
