/*#include <iostream>
#include "util.h"
#include "pointers.h"
#include "containers/array.h"
#include "containers/lists.h"
#include "containers/BTree.h"

// g++ -std=c++17 -Wall -g -pthread main.cpp -o main

int main() {
    std::cout << "Hello EDA-UNMSM!" << std::endl;
    // DemoUtil();
    // DemoPointers1();
    // DemoArray();
    // DemoLists();
    DemoBTree();
    return 0;
}*/
#include <iostream>
#include "containers/BTree.h"

using namespace std;

/* ========= Funciones auxiliares ========= */

template<typename Key, typename Obj>
void PrintInOrder(const typename BTree<Key, Obj>::ObjectInfo &info, int level, void *extra)
{
    cout << info.key << " ";
}

template<typename Key, typename Obj>
typename BTree<Key, Obj>::ObjectInfo*
FindGreaterThan(typename BTree<Key, Obj>::ObjectInfo &info, int level, void *extra)
{
    int value = *(int*)extra;
    if(info.key > value)
        return &info;
    return nullptr;
}

/* ========= MAIN ========= */

int main()
{
    cout << "===== TEST B-TREE =====\n\n";

    BTree<int, int> tree(3, true);

    cout << "Insertando elementos...\n";

    for(int i = 1; i <= 15; i++)
        tree.Insert(i, i*10);

    cout << "Size: " << tree.size() << endl;
    cout << "Height: " << tree.height() << endl;

    cout << "\nImpresion con operator<<:\n";
    cout << tree << endl;

    cout << "\nRecorrido InOrder:\n";
    tree.InOrder(PrintInOrder<int,int>, nullptr);
    cout << endl;

    cout << "\nRecorrido PreOrder:\n";
    tree.PreOrder(PrintInOrder<int,int>, nullptr);
    cout << endl;

    cout << "\nRecorrido PostOrder:\n";
    tree.PostOrder(PrintInOrder<int,int>, nullptr);
    cout << endl;

    cout << "\nBusqueda:\n";
    cout << "Buscar 7: " << tree.Search(7) << endl;
    cout << "Buscar 100 (no existe): " << tree.Search(100) << endl;

    cout << "\nProbando iterador forward:\n";
    for(auto it = tree.begin(); it != tree.end(); ++it)
    {
        cout << (*it).key << " ";
    }
    cout << endl;

    cout << "\nProbando iterador backward:\n";
    for(auto it = tree.rbegin(); it != tree.rend(); --it)
    {
        cout << (*it).key << " ";
    }
    cout << endl;

    cout << "\nProbando FirstThat (key > 10):\n";
    int limit = 10;
    auto result = tree.FirstThat(FindGreaterThan<int,int>, &limit);
    if(result)
        cout << "Encontrado: " << result->key << endl;
    else
        cout << "No encontrado\n";

    cout << "\nEliminando 5 y 6...\n";
    tree.Remove(5, 50);
    tree.Remove(6, 60);

    cout << "Size despues de eliminar: " << tree.size() << endl;

    cout << "\nArbol final:\n";
    cout << tree << endl;

    cout << "\nRecorrido final con iterador:\n";
    for(auto it = tree.begin(); it != tree.end(); ++it)
        cout << (*it).key << " ";
    cout << endl;

    cout << "\n===== FIN DEL TEST =====\n";

    return 0;
}