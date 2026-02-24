#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "containers/binarytree.h"
#include "containers/binarytreeAVL.h"

using namespace std;

static void PrintIntAVL(int &x, const string &sep){ cout << x << sep; }
static bool GreaterThanAVL(int &x, int v){ return x > v; }

void DemoAVL(){
    using BST = CBinaryTree<TreeTraitAscending<int>>;
    using AVL = CBinaryTreeAVL<TreeTraitAscending<int>>;

    cout << "\n================ DEMO AVL =================\n";

    cout << "\n[1] Rotaciones AVL (LL / LR / RR / RL)\n";

    AVL ll; ll.Insert(30,30); ll.Insert(20,20); ll.Insert(10,10);
    cout << "LL root: " << (ll.GetRoot() ? ll.GetRoot()->GetValue() : -1) << " | inorden: ";
    ll.inorden(PrintIntAVL, string(" ")); cout << endl;

    AVL lr; lr.Insert(30,30); lr.Insert(10,10); lr.Insert(20,20);
    cout << "LR root: " << (lr.GetRoot() ? lr.GetRoot()->GetValue() : -1) << " | inorden: ";
    lr.inorden(PrintIntAVL, string(" ")); cout << endl;

    AVL rr; rr.Insert(10,10); rr.Insert(20,20); rr.Insert(30,30);
    cout << "RR root: " << (rr.GetRoot() ? rr.GetRoot()->GetValue() : -1) << " | inorden: ";
    rr.inorden(PrintIntAVL, string(" ")); cout << endl;

    AVL rl; rl.Insert(10,10); rl.Insert(30,30); rl.Insert(20,20);
    cout << "RL root: " << (rl.GetRoot() ? rl.GetRoot()->GetValue() : -1) << " | inorden: ";
    rl.inorden(PrintIntAVL, string(" ")); cout << endl;

    cout << "\n[2] Insert / Remove / Size / Empty\n";
    AVL avl;
    cout << "empty inicial: " << (avl.empty() ? "SI" : "NO") << endl;

    for(int x : {50,20,70,10,30,60,80,25,35}) avl.Insert(x, x);

    cout << "root: " << (avl.GetRoot() ? avl.GetRoot()->GetValue() : -1) << endl;
    cout << "inorden: ";
    avl.inorden(PrintIntAVL, string(" "));
    cout << "\nsize: " << avl.size() << endl;

    cout << "Remove(25): " << (avl.Remove(25) ? "OK" : "NO") << endl;
    cout << "Remove(20): " << (avl.Remove(20) ? "OK" : "NO") << endl;
    cout << "inorden despues remove: ";
    avl.inorden(PrintIntAVL, string(" "));
    cout << "\nsize: " << avl.size() << endl;

    cout << "\n[3] Recorridos (variadic)\n";
    cout << "IN : ";   avl.inorden(PrintIntAVL, string(" "));   cout << endl;
    cout << "PRE: ";   avl.preorden(PrintIntAVL, string(" "));  cout << endl;
    cout << "POST: ";  avl.postorden(PrintIntAVL, string(" ")); cout << endl;

    cout << "\n[4] Iteradores / Foreach / FirstThat\n";

    cout << "Forward : ";
    for(auto it = avl.begin(); it != avl.end(); ++it) cout << *it << " ";
    cout << endl;

    cout << "Backward: ";
    for(auto it = avl.rbegin(); it != avl.rend(); ++it) cout << *it << " ";
    cout << endl;

    cout << "Foreach : ";
    avl.Foreach(PrintIntAVL, string(" "));
    cout << endl;

    auto it = avl.FirstThat(GreaterThanAVL, 55);
    cout << "FirstThat(>55): ";
    if(it != avl.end()) cout << *it << endl;
    else cout << "No encontrado\n";

    cout << "\n[5] Constructor AVL desde BST\n";
    BST bst;
    for(int x : {40,10,60,5,20,50,70}) bst.Insert(x, x);

    AVL avlFromBST(bst);
    cout << "AVL desde BST (inorden): ";
    avlFromBST.inorden(PrintIntAVL, string(" "));
    cout << endl;

    cout << "\n[6] Copy / Move\n";

    AVL copyCtor(avl);
    AVL copyAssign; copyAssign = avl;
    AVL moveCtor(std::move(copyCtor));
    AVL moveAssign; moveAssign = std::move(copyAssign);

    cout << "moveCtor (inorden): ";
    moveCtor.inorden(PrintIntAVL, string(" "));
    cout << endl;

    cout << "moveAssign (inorden): ";
    moveAssign.inorden(PrintIntAVL, string(" "));
    cout << endl;

    cout << "\n[7] operator<< / operator>>\n";

    stringstream ss;
    ss << moveAssign;

    AVL fromSS;
    ss >> fromSS;

    cout << "Leido de stringstream (inorden): ";
    fromSS.inorden(PrintIntAVL, string(" "));
    cout << endl;

    ofstream fout("BinaryTreeAVL.txt");
    fout << fromSS;
    fout.close();

    AVL fromFile;
    ifstream fin("BinaryTreeAVL.txt");
    fin >> fromFile;
    fin.close();

    cout << "Leido de archivo (inorden): ";
    fromFile.inorden(PrintIntAVL, string(" "));
    cout << endl;

    cout << "\n[8] clear()\n";
    fromFile.clear();
    cout << "size: " << fromFile.size() << " | empty: " << (fromFile.empty() ? "SI" : "NO") << endl;

    cout << "\n================ FIN DEMO AVL ================\n";
}