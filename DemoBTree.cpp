#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <iomanip>
#include "containers/BTree.h"
#include "containers/BTraits.h"
#include "containers/BTreePage.h"
#include "general/types.h"

using namespace std;
void ImprimirConIteradores(BTree<string, long>& arbol) {
    cout << "\nRecorriendo arbol con iteradores (Inorden):" << endl;
    for (auto it = arbol.begin(); it != arbol.end(); ++it) {
        cout << "  Clave: " << setw(8) << left << it->key 
             << " | ID: " << it->ObjID << endl;
    }
}

void DemoBTree() {
    cout << "DEMO B-TREE" << endl;

    BTree<FileKey, long> indiceArchivos(3); 
    indiceArchivos.Insert(FileKey("documento.pdf", 0), 1001);
    indiceArchivos.Insert(FileKey("imagen.png", 512), 1002);
    
    cout << "\n1. Test de BTreeTraits: " << endl;
    indiceArchivos.Print(cout); 
    BTree<string, long> arbolSeguro(3, true);
    cout << "\n2. Test de Seguridad de Hilos: " << endl;
    
    auto tareaInsercion = [&](string prefijo) {
        for( T1  i = 0; i < 3; ++i) {
            string clave = prefijo + to_string(i);
            arbolSeguro.Insert(clave, i * 10);
        }
    };

    thread t1(tareaInsercion, "UserA_");
    thread t2(tareaInsercion, "UserB_");
    
    t1.join();
    t2.join();
    cout << "  Inserciones concurrentes finalizadas. Tamaño: " << arbolSeguro.size() << endl;
    cout << "\n3. Test de Rebalanceo (Orden 3):" << endl;
    const char* keys = "ABCDEFGHIJKL";
    BTree<char,  T1 > arbolBalanceado(3);
    for( T1  i = 0; keys[i]; ++i) {
        arbolBalanceado.Insert(keys[i], i);
    }
    arbolBalanceado.Print(cout);
    ImprimirConIteradores(arbolSeguro);
    cout << "\n4. Test de Eliminacion (Merge/Redistribute):" << endl;
    cout << "Eliminando clave 'F'...";
    arbolBalanceado.Remove('F', 5);
    arbolBalanceado.Print(cout);

    cout << "\n=== DEMO FINALIZADA ===" << endl;
}
