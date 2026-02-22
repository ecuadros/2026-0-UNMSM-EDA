#include <iostream>
#include <fstream>
#include <string>
#include <utility>

// Tus archivos de cabecera exactos
#include "containers/binarytree.h"
#include "containers/avltree.h"

using namespace std;
using T1 = int; 

// Utilidad de alto rendimiento para imprimir sin copias
auto imprimir = [](const T1& val) { cout << val << " "; };

void DemoBinarytree() {
    cout << "  DEMO: BinaryTree (Ascendente)\n";
    
    CBinaryTree<TreeTraitAscending<T1>> arbol;
    
    cout << "Insertando: 50, 30, 70, 20, 40, 60, 80...\n";
    arbol.Insert(50); arbol.Insert(30); arbol.Insert(70);
    arbol.Insert(20); arbol.Insert(40); arbol.Insert(60);
    arbol.Insert(80);

    cout << "\nRecorridos \n";
    cout << "PreOrder  : "; arbol.PreOrderVariadic(imprimir); cout << endl;
    cout << "InOrder   : "; arbol.InOrderVariadic(imprimir); cout << endl;
    cout << "PostOrder : "; arbol.PostOrderVariadic(imprimir); cout << endl;

    cout << "\nForeach (Modificacion In-Place)\n";
    cout << "Sumando 10 a todo...\n";
    arbol.ForEach([](T1& val) { val += 10; });
    cout << "Estado    : "; arbol.InOrderVariadic(imprimir); cout << endl;

    cout << "Multiplicando por 2 \n";
    arbol.ForEach([](T1& val) { val *= 2; });
    cout << "Estado    : "; arbol.InOrderVariadic(imprimir); cout << endl;

    cout << "\nFirst that \n";
    int limite = 100;
    cout << "Buscando el primer elemento mayor a " << limite << "\n";
    T1* encontrado = arbol.FirstThat([&limite](const T1& val) { return val > limite; });
    if (encontrado) cout << "-> Encontrado: " << *encontrado << endl;

    cout << "\nEliminacion \n";
    cout << "Borrando raiz actual (120) \n";
    arbol.Remove(120);
    cout << "Estado    : "; arbol.InOrderVariadic(imprimir); cout << endl;

    cout << "\nPersistencia en disco \n";
    string archivo = "arbol_data.txt";
    cout << "Guardando en disco ('" << archivo << "') \n";
    ofstream out(archivo);
    if (out.is_open()) {
        out << arbol; 
        out.close();
    }

    cout << "Reconstruyendo desde disco \n";
    CBinaryTree<TreeTraitAscending<T1>> arbolLeido;
    ifstream in(archivo);
    if (in.is_open()) {
        T1 temp;
        while (in >> temp) { 
            arbolLeido.Insert(std::move(temp)); 
        }
        in.close();
        cout << "-> Arbol recuperado : "; arbolLeido.InOrderVariadic(imprimir); cout << endl;
    }

    cout << "\nSemantica de constructores \n";
    CBinaryTree<TreeTraitAscending<T1>> arbolCopia(arbolLeido);
    cout << "Copia profunda creada. Insertando 999 en la copia \n";
    arbolCopia.Insert(999);
    cout << "Original  : "; arbolLeido.InOrderVariadic(imprimir); cout << endl;
    cout << "Copia     : "; arbolCopia.InOrderVariadic(imprimir); cout << endl;

    cout << "\nAplicando Move Constructor (Transferencia cero-copias) \n";
    CBinaryTree<TreeTraitAscending<T1>> arbolMovido(std::move(arbolCopia));
    cout << "Origen (vaciado) : "; arbolCopia.InOrderVariadic(imprimir); cout << endl;
    cout << "Destino (movido) : "; arbolMovido.InOrderVariadic(imprimir); cout << endl;
}

void DemoAvltree() {
    cout << "  DEMO: CAVLTree \n";

    CBinaryTree<TreeTraitDescending<T1>> arbolNormal;
    CAVLTree<TreeTraitDescending<T1>> arbolAVL;

    cout << "Insertando datos secuenciales: 10, 20, 30, 40, 50 \n";
    for(int i = 10; i <= 50; i += 10) {
        arbolNormal.Insert(i);
        arbolAVL.Insert(i);
    }

    cout << "\nEl CBinaryTree normal se ha degenerado en una lista (O(N)).\n";
    cout << "Recorrido iterador Forward (Descendente):\n";
    for(auto it = arbolNormal.begin(); it != arbolNormal.end(); ++it) {
        cout << *it << " ";
    }
    cout << "\n";

    cout << "\nEl CAVLTree ha detectado el desbalance y ha aplicado rotaciones (O(log N)).\n";
    cout << "Recorrido iterador Forward (Descendente):\n";
    for(auto it = arbolAVL.begin(); it != arbolAVL.end(); ++it) {
        cout << *it << " ";
    }
}

