#include <iostream>
#include <sstream>
#include <string>
#include "containers/BTree.h"

using namespace std;

const char *keys1 = "D1XJ2xTg8zKL9AhijOPQcEowRSp0NbW567BUfCqrs4FdtYZakHIuvGV3eMylmn";
const int BTreeSize = 3;

void DemoBTree() {
    BTree<char, int> bt(BTreeSize);

    cout << "\n Demo BTree (uso simple) \n";
    int inserted = 0;
    for (int i = 0; keys1[i]; ++i) {
        if (bt.Insert(keys1[i], 100 + i)) inserted++;
    }
    cout << "Insertados: " << inserted
         << " | size=" << bt.size()
         << " | height=" << bt.height()
         << " | order=" << bt.GetOrder() << "\n";

    cout << "\nEstructura interna (Print):\n";
    bt.Print(cout);
    cout << "\n\nSerializacion (operator<<):\n" << bt << "\n";

    cout << "\nRecorrido con iteradores (forward):\n";
    int shown = 0;
    for (auto it = bt.begin(); it != bt.end() && shown < 20; ++it, ++shown)
        cout << it->key << "(" << it->ObjID << ") ";
    cout << "... \n";

    cout << "Recorrido inverso (backward):\n";
    shown = 0;
    for (auto it = bt.rbegin(); it != bt.rend() && shown < 20; ++it, ++shown)
        cout << it->key << "(" << it->ObjID << ") ";
    cout << "... \n";

    cout << "\nForEach por rango de iteradores (10 primeros):\n";
    auto start = bt.begin();
    auto stop = bt.begin();
    int steps = 0;
    while (stop != bt.end() && steps < 10) {
        ++stop;
        ++steps;
    }
    bt.ForEach(start, stop, [](const auto &info) { cout << info.key << " "; });
    cout << "\n";

    auto found = bt.FirstThat(bt.begin(), bt.end(),
                              [](const auto &info, char target) { return info.key == target; }, 'Q');
    if (found) cout << "FirstThat encontro: " << found->key << " -> " << found->ObjID << "\n";

    cout << "\nTraversals:\n";
    cout << "inorder:  ";
    bt.inorderTraversal([](char k) { cout << k << " "; });
    cout << "\npreorder: ";
    bt.preorderTraversal([](char k) { cout << k << " "; });
    cout << "\npostorder:";
    bt.postorderTraversal([](char k) { cout << " " << k; });
    cout << "\n";

    cout << "\nSearch en el arbol grande:\n";
    cout << "Search('A') -> ObjID " << bt.Search('A') << "\n";
    cout << "Search('?') -> ObjID " << bt.Search('?') << " (0 suele indicar no encontrado)\n";
    cout << "size actual=" << bt.size() << ", height actual=" << bt.height() << "\n";

    cout << "\nRemove en arbol pequeno:\n";
    BTree<char, int> tiny(BTreeSize);
    tiny.Insert('A', 1);
    tiny.Insert('B', 2);
    tiny.Insert('C', 3);
    tiny.Insert('D', 4);
    tiny.Insert('E', 5);
    cout << "tiny antes: " << tiny << "\n";
    cout << "remove('C'): " << (tiny.Remove('C', 3) ? "ok" : "fallo") << "\n";
    cout << "remove('C') otra vez: " << (tiny.Remove('C', 3) ? "ok" : "fallo") << "\n";
    cout << "tiny despues: " << tiny << "\n";

    cout << "\nSerializacion y carga (operator>>):\n";
    stringstream ss;
    ss << bt;
    BTree<char, int> copy(BTreeSize);
    ss >> copy;
    cout << "copia size=" << copy.size() << ", height=" << copy.height() << "\n";

    cout << "\nInsert duplicado de 'A': " << (copy.Insert('A', 9999) ? "insertado" : "rechazado") << "\n";
    cout << "DemoBTree finished.\n";
}
