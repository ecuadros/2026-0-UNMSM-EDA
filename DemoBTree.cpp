#include <iostream>
#include <utility>
#include "containers/BTree.h"

using namespace std;

const char *keys1 = "D1XJ2xTg8zKL9AhijOPQcEowRSp0NbW567BUfCqrs4FdtYZakHIuvGV3eMylmn";
const int BTreeSize = 3;

// ---------------------------------------------------
// Helpers para recorridos variadicos (tu API actual)
// ---------------------------------------------------
void PrintTraversalLimited(BTree<char>::ObjectInfo &info, int level,
                           const char* tag, int &count, int limit)
{
    if (count < limit) {
        cout << tag << " [lvl=" << level << "] "
             << info.key << " -> " << info.ObjID << "\n";
    }
    count++;
}

bool FirstGreaterThan(BTree<char>::ObjectInfo &info, int /*level*/, char lim)
{
    return info.key > lim;
}

// ---------------------------------------------------
// Demo principal
// ---------------------------------------------------
void DemoBTree()
{
    cout << "\n===== DEMO BTREE =====\n";

    // [1] Constructor + insercion
    BTree<char> bt(BTreeSize);
    int inserted = 0;
    for (int i = 0; keys1[i]; ++i) {
        if (bt.Insert(keys1[i], i * i)) inserted++;
    }

    cout << "[1] Arbol creado\n";
    cout << "Insertados: " << inserted
         << " | size=" << bt.size()
         << " | height=" << bt.height()
         << " | order=" << bt.GetOrder() << "\n";

    // [2] Iteradores (solo primeros 12)
    cout << "\n[2] Forward iterator (12 primeros)\n";
    int shown = 0;
    for (BTree<char>::iterator it = bt.begin(); it != bt.end() && shown < 12; ++it, ++shown) {
        cout << it->key << "(" << it->ObjID << ") ";
    }
    cout << "...\n";

    cout << "[3] Backward iterator (12 primeros)\n";
    shown = 0;
    for (BTree<char>::reverse_iterator it = bt.rbegin(); it != bt.rend() && shown < 12; ++it, ++shown) {
        cout << it->key << "(" << it->ObjID << ") ";
    }
    cout << "...\n";

    // [4] Search (solo unas pruebas)
    cout << "\n[4] Search\n";
    cout << "Search('A') -> " << bt.Search('A') << "\n";
    cout << "Search('m') -> " << bt.Search('m') << "\n";
    cout << "Search('?') -> " << bt.Search('?') << " (0 = no encontrado)\n";

    // [5] Traversals variadicos (limitados)
    cout << "\n[5] InOrder variadico (12 primeros)\n";
    int cIn = 0;
    bt.InOrder(PrintTraversalLimited, "IN", cIn, 12);
    cout << "Total visitados InOrder: " << cIn << "\n";

    cout << "\n[6] PreOrder variadico (12 primeros)\n";
    int cPre = 0;
    bt.PreOrder(PrintTraversalLimited, "PRE", cPre, 12);
    cout << "Total visitados PreOrder: " << cPre << "\n";

    cout << "\n[7] PostOrder variadico (12 primeros)\n";
    int cPost = 0;
    bt.PostOrder(PrintTraversalLimited, "POST", cPost, 12);
    cout << "Total visitados PostOrder: " << cPost << "\n";

    // [8] FirstThatV
    cout << "\n[8] FirstThatV\n";
    BTree<char>::ObjectInfo* p = bt.FirstThatV(FirstGreaterThan, 'm');
    if (p)
        cout << "Primero con key > 'm': " << p->key << " -> " << p->ObjID << "\n";
    else
        cout << "No encontrado\n";

    // [9] InsertMany + move constructor (pequeno)
    cout << "\n[9] InsertMany + Move Constructor\n";
    BTree<char> manyTree(BTreeSize);
    manyTree.InsertMany(
        std::pair<char,long>('M', 100),
        std::pair<char,long>('C', 200),
        std::pair<char,long>('X', 300),
        std::pair<char,long>('A', 400),
        std::pair<char,long>('Q', 500)
    );

    cout << "manyTree:\n";
    for (BTree<char>::iterator it = manyTree.begin(); it != manyTree.end(); ++it)
        cout << it->key << " -> " << it->ObjID << "\n";

    BTree<char> moved(std::move(manyTree));
    cout << "moved (despues del move):\n";
    for (BTree<char>::iterator it = moved.begin(); it != moved.end(); ++it)
        cout << it->key << " -> " << it->ObjID << "\n";

    // [10] operator< y operator>
    cout << "\n[10] operator< y operator>\n";
    BTree<char> a(BTreeSize), b(BTreeSize);

    a.InsertMany(
        std::pair<char,long>('A', 1),
        std::pair<char,long>('B', 2),
        std::pair<char,long>('C', 3)
    );
    b.InsertMany(
        std::pair<char,long>('A', 1),
        std::pair<char,long>('B', 2),
        std::pair<char,long>('D', 4)
    );

    cout << "A < B ? " << (a < b) << "\n";
    cout << "A > B ? " << (a > b) << "\n";
    cout << "B > A ? " << (b > a) << "\n";

    // [11] Remove en arbol pequeno (con Print para que no sea gigante)
    cout << "\n[11] Remove (arbol pequeno)\n";
    BTree<char> tiny(BTreeSize);
    tiny.Insert('A', 1);
    tiny.Insert('B', 2);
    tiny.Insert('C', 3);
    tiny.Insert('D', 4);
    tiny.Insert('E', 5);

    cout << "tiny antes (Print):\n";
    tiny.Print(cout);
    cout << "\n";

    cout << "Remove('C', 3): " << (tiny.Remove('C', 3) ? "ok" : "fallo") << "\n";
    cout << "tiny despues (Print):\n";
    tiny.Print(cout);
    cout << "\n";

    // [12] Destructor por scope
    cout << "\n[12] Destructor (por scope)\n";
    {
        BTree<char> local(BTreeSize);
        local.Insert('L', 11);
        local.Insert('O', 22);
        local.Insert('K', 33);
        cout << "Dentro del scope: local creado.\n";
    }
    cout << "Fuera del scope: destructor ejecutado.\n";

    cout << "\n===== FIN DEMO BTREE =====\n";
}