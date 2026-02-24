#include <iostream>
#include <string>
#include "containers/btree.h" 

using namespace std;

const char * keys1 = "D1XJ2xTg8zKL9AhijOPQcEowRSp0NbW567BUfCqrs4FdtYZakHIuvGV3eMylmn";
const char * keys2 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
const char * keys3 = "DYZakHIUwxVJ203ejOP9Qc8AdtuEop1XvTRghSNbW567BfiCqrs4FGMyzKLlmn";

const int BTreeSize = 3;

void DemoBTree() {
    cout << "======================================================" << endl;
    cout << "        DEMO B-TREE    " << endl;
    cout << "======================================================" << endl << endl;

    BTree<char> bt(BTreeSize);

    // ---------------------------------------------------------
    // 1. INSERCIÓN
    // ---------------------------------------------------------
    cout << "--- 1. INSERCION DE CLAVES ---" << endl;
    for (int i = 0; keys1[i]; i++) {
        bt.Insert(keys1[i], i * i);
    }
    cout << "\nEstructura del arbol despues de insertar:" << endl;
    bt.Print(cout);
    cout << endl;

    // ---------------------------------------------------------
    // 2. BÚSQUEDA 
    // ---------------------------------------------------------
    cout << "--- 2. BUSQUEDA DE CLAVES ---" << endl;
    for (int i = 0; keys2[i]; i++) {
        long ObjID = bt.Search(keys2[i]);
        if (ObjID != -1) {
            cout << "Encontrado '" << keys2[i] << "' (ID = " << ObjID << ")" << endl;
        } else {
            cout << "No encontrado '" << keys2[i] << "'" << endl;
        }
    }
    cout << endl;

    // ---------------------------------------------------------
    // 3. ELIMINACIÓN 
    // ---------------------------------------------------------
    cout << "--- 3. ELIMINACION DE CLAVES ---" << endl;
    for (int i = 0; keys3[i]; i++) {
        
        if (bt.Remove(keys3[i], -1)) {
            cout << "Clave '" << keys3[i] << "' eliminada con exito." << endl;
        } else {
            cout << "No se pudo eliminar '" << keys3[i] << "' (No encontrada)." << endl;
        }
    }

    cout << "\nEstructura del arbol final despues de la eliminacion:" << endl;
    bt.Print(cout);
    cout.flush();

    cout << "\n======================================================" << endl;
    cout << "                     FIN DE LA DEMO                   " << endl;
    cout << "======================================================" << endl;
}