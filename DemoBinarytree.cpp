#include <iostream>
#include <sstream>
#include <string>
#include "containers/binarytree.h"

using namespace std;

static void PrintIntBT(int &x, const string &sep){ cout << x << sep; }
static bool GreaterThanBT(int &x, int v){ return x > v; }

template <typename Tree>
static void BuildStandardTree(Tree &bt){
    bt.Insert(70, 70);
    bt.Insert(30, 30);
    bt.Insert(80, 80);
    bt.Insert(10, 10);
    bt.Insert(40, 40);
    bt.Insert(75, 75);
    bt.Insert(90, 90);
}

void DemoBinaryTree(){
    using BT = CBinaryTree<TreeTraitAscending<int>>;

    cout << "\n================ DEMO BINARY TREE ================\n";

    // [1] Insert / Size / Empty
    cout << "\n[1] Insert / Size / Empty\n";
    BT bt;
    cout << "empty inicial: " << (bt.empty() ? "SI" : "NO") << endl;

    BuildStandardTree(bt);

    cout << "inorden: ";
    bt.inorden(PrintIntBT, string(" "));
    cout << "\nsize: " << bt.size() << endl;

    // [2] Remove
    cout << "\n[2] Remove\n";
    cout << "Remove(10): " << (bt.Remove(10) ? "OK" : "NO") << endl;
    cout << "Remove(30): " << (bt.Remove(30) ? "OK" : "NO") << endl;

    cout << "inorden despues remove: ";
    bt.inorden(PrintIntBT, string(" "));
    cout << "\nsize: " << bt.size() << endl;

    // Reconstruimos para continuar con el árbol estándar
    bt.clear();
    BuildStandardTree(bt);

    // [3] Recorridos variadic
    cout << "\n[3] Recorridos (variadic)\n";
    cout << "IN : ";   bt.inorden(PrintIntBT, string(" "));   cout << endl;
    cout << "PRE: ";   bt.preorden(PrintIntBT, string(" "));  cout << endl;
    cout << "POST: ";  bt.postorden(PrintIntBT, string(" ")); cout << endl;

    // [4] Iteradores forward / backward
    cout << "\n[4] Iteradores (begin/end, rbegin/rend)\n";
    cout << "Forward : ";
    for(auto it = bt.begin(); it != bt.end(); ++it) cout << *it << " ";
    cout << endl;

    cout << "Backward: ";
    for(auto it = bt.rbegin(); it != bt.rend(); ++it) cout << *it << " ";
    cout << endl;

    // [5] Foreach / FirstThat
    cout << "\n[5] Foreach / FirstThat\n";
    cout << "Foreach : ";
    bt.Foreach(PrintIntBT, string(" "));
    cout << endl;

    auto it = bt.FirstThat(GreaterThanBT, 50);
    cout << "FirstThat(>50): ";
    if(it != bt.end()) cout << *it << endl;
    else cout << "No encontrado\n";

    // [6] operator<< / operator>>
    cout << "\n[6] operator<< / operator>>\n";
    stringstream ss;
    ss << bt;

    BT btFromSS;
    ss >> btFromSS;

    cout << "Leido de stringstream (inorden): ";
    btFromSS.inorden(PrintIntBT, string(" "));
    cout << endl;

    // [7] Copy Constructor
    cout << "\n[7] Copy Constructor\n";
    BT btCopyCtor(bt);

    cout << "btCopyCtor (inorden): ";
    for(auto itc = btCopyCtor.begin(); itc != btCopyCtor.end(); ++itc)
        cout << *itc << " ";
    cout << endl;

    // [8] Move Constructor + clear
    cout << "\n[8] Move Constructor + clear()\n";
    BT btMove(std::move(btFromSS));

    cout << "btFromSS.size (despues move): " << btFromSS.size() << endl;
    cout << "btMove.size: " << btMove.size() << endl;

    btMove.clear();
    cout << "btMove.size (despues clear): " << btMove.size()
         << " | empty: " << (btMove.empty() ? "SI" : "NO") << endl;

    // [9] Info de destructor seguro (visible en código)
    cout << "\n[9] Destructor seguro\n";
    cout << "Implementado en la clase (~CBinaryTree -> clear()).\n";

    cout << "\n================ FIN DEMO BINARY TREE ================\n";
}