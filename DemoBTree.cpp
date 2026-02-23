#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "containers/BTree.h"
#include "containers/BTreeTraits.h"
#include "general/types.h"
#include "variadic-util.h"

using namespace std;

using KeyType = char;

//const char * keys="CDAMPIWNBKEHOLJYQZFXVRTSGU";
const KeyType* keys1 = "D1XJ2xTg8zKL9AhijOPQcEowRSp0NbW567BUfCqrs4FdtYZakHIuvGV3eMylmn";
const KeyType* keys2 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
const KeyType* keys3 = "DYZakHIUwxVJ203ejOP9Qc8AdtuEop1XvTRghSNbW567BfiCqrs4FGMyzKLlmn";

void DemoBTree(){
    cout << "[1] TEST DE CONSTRUCTORES Y GESTION DE MEMORIA\n";
    BTree<BTreeTraitAscending<KeyType>> arbol(3); 
    cout << "-> Arbol base inicializado (Orden 3).\n";

    arbol.Insert('A', 10);
    arbol.Insert('B', 20);
    arbol.Insert('C', 30);
    cout << "-> Se insertaron 3 elementos iniciales. Tamano de arbol base: " << arbol.size() << "\n";
    arbol.Print(cout);
    cout << "\n"; 

    BTree<BTreeTraitAscending<KeyType>> arbolCopia(arbol);
    cout << "-> Constructor de Copia ejecutado. Tamano de arbolCopia: " << arbolCopia.size() << "\n";

    BTree<BTreeTraitAscending<KeyType>> arbolMovido(std::move(arbolCopia));
    cout << "-> Move Constructor ejecutado. Tamano de arbolMovido: " << arbolMovido.size() << "\n";
    cout << "-> Tamano de arbolCopia tras ser movido: " << arbolCopia.size() << "\n\n";

    BTree<BTreeTraitAscending<KeyType>> arbolVisual(3); 
    const KeyType* visualKeys = "MFTKXPRZL";
    for (Size i = 0; visualKeys[i]; i++) {
        arbolVisual.Insert(visualKeys[i], i);
    }
    cout << "Estructura horizontal (Raiz a la izquierda, hijos a la derecha):\n";
    arbolVisual.Print(cout);
    cout << "\n";

    cout << "[2] TEST DE INSERCION MASIVA \n";
    for (Size i = 0; keys1[i]; i++) {
        arbol.Insert(keys1[i], i * i);
    }
    
    cout << "Estructura del arbol resultante (Altura: " << arbol.height() 
         << " | Tamano: " << arbol.size() << "):\n";
    cout << "(Arbol impreso en memoria correctamente)\n\n";

    cout << "[3] TEST DE BUSQUEDA (Usando keys2 y SearchInPage)\n";
    Size encontrados = 0, no_encontrados = 0;
    
    for (Size i = 0; keys2[i]; i++) {
        ref_type refResult = arbol.Search(keys2[i]);
        if (refResult != -1) encontrados++;
        else no_encontrados++;
    }
    cout << "-> De keys2: Se encontraron " << encontrados << " claves.\n";
    cout << "-> De keys2: No se encontraron " << no_encontrados << " claves.\n\n";

    cout << "[4] TEST DE FOREACH Y FIRST THAT\n";
    auto buscarVocal = [](auto& info, Size level) {
        KeyType c = info.key;
        return (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
    };

    auto pEncontrado = arbol.FirstThat(buscarVocal);

    if (pEncontrado) {
        cout << "-> (FirstThat): La primera vocal minuscula hallada en el arbol es: '" << pEncontrado->key << "'\n";
    }

    KeyType target = 'Z';
    cout << "\nEjecutando ForEach variadico (Buscando la letra '" << target << "'):\n";
    
    auto imprimirNivel = [](auto& info, Size level, KeyType objetivo) {
        if (info.key == objetivo) {
            cout << "-> [ForEach] El caracter '" << info.key << "' se ubica en el nivel de profundidad: " << level << endl;
        }
    };

    arbol.Foreach(imprimirNivel, target);

    cout << "[5] TEST DE ITERADORES (Forward & Backward)\n";
    cout << "Los primeros 10 elementos (Forward): [ ";
    Size cont = 0;
    for (auto it = arbol.begin(); it != arbol.end() && cont < 10; ++it, ++cont) {
        cout << *it << " ";
    }
    cout << "... ]\n";

    cout << "Los ultimos 10 elementos (Backward): [ ";
    cont = 0;
    for (auto it = arbol.rbegin(); it != arbol.rend() && cont < 10; ++it, ++cont) {
        cout << *it << " ";
    }
    cout << "... ]\n\n";

    cout << "[6] TEST DE ELIMINACION Y FILE I/O\n";
    string filename = "btree_test_data.txt";
    
    ofstream fileOut(filename);
    if (fileOut.is_open()) {
        fileOut << arbol; 
        fileOut.close();
        cout << "-> Arbol exportado correctamente a '" << filename << "'\n";
    }

    cout << "\nEliminando algunas claves usando keys3...\n";
    for (Size i = 0; i < 5; i++) { 
        if(arbol.Remove(keys3[i], -1)) {
            cout << "Clave '" << keys3[i] << "' eliminada con exito.\n";
        }
    }

    cout << "\nVaciando por completo el arbol original (Clear)...\n";
    arbol.Clear();
    cout << "Tamano actual: " << arbol.size() << "\n";

    cout << "\nRecuperando arbol intacto desde el archivo de texto...\n";
    ifstream fileIn(filename);
    if (fileIn.is_open()) {
        fileIn >> arbol; 
        fileIn.close();
        cout << "-> Arbol recuperado. Nuevo tamano: " << arbol.size() << "\n";
        cout << "\nImprimiendo arbol recuperado (primeros niveles visibles):\n";
        arbol.Print(cout);
    }
}