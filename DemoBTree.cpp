#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "containers/BTree.h"
#include "general/types.h"
#include "variadic-util.h"

using namespace std;

using KeyType = char;

//const char * keys="CDAMPIWNBKEHOLJYQZFXVRTSGU";
const char * keys1 = "D1XJ2xTg8zKL9AhijOPQcEowRSp0NbW567BUfCqrs4FdtYZakHIuvGV3eMylmn";
const char * keys2 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
const char * keys3 = "DYZakHIUwxVJ203ejOP9Qc8AdtuEop1XvTRghSNbW567BfiCqrs4FGMyzKLlmn";

const int BTreeSize = 3;

void DemoBTree(){
cout << "---------------------------------------"<<endl; 
cout << "\t\tB-TREE "<<endl; 
cout << "---------------------------------------"<<endl; 

    BTree<BTreeTraitAscending<KeyType>> arbol(3); 
    cout << "Arbol (Orden 3).\n";

    arbol.Insert('A', 10);
    arbol.Insert('B', 20);
    arbol.Insert('C', 30);
    cout << "Tamano de arbol base: " << arbol.size() << "\n";
    arbol.Print(cout);
    cout << "\n";  

    cout << "Insercion"<<endl; 
    for (Size i = 0; keys1[i]; i++) {
        arbol.Insert(keys1[i], i * i);
    }
    
    cout << "Resultado (Altura: " << arbol.height() 
         << " | Tamano: " << arbol.size() << "):\n";

    cout << "Tteradores (Forward & Backward)\n";
    cout << "Primeros 10 elementos (Forward): [ ";
    Size cont = 0;
    for (auto it = arbol.begin(); it != arbol.end() && cont < 10; ++it, ++cont) {
        cout << *it << " ";
    }
    cout << "... ]\n";

    cout << "Ultimos 10 elementos (Backward): [ ";
    cont = 0;
    for (auto it = arbol.rbegin(); it != arbol.rend() && cont < 10; ++it, ++cont) {
        cout << *it << " ";
    }
    cout << "... ]\n\n";
}