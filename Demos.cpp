#include <iostream>
#include <string>
#include "containers/binarytree.h"
#include "containers/traits.h"
#include "general/types.h"

using namespace std;

// Funcion Helper para probar los recorridos Variadic
void ImprimirVariadic(T1 valor, string prefijo, string sufijo) {
    cout << prefijo << valor << sufijo;
}

void Demos() {
    //datos de prueba
    T1 datos[]      = {45, 12, 89, 33, 5, 76, 21, 99, 1, 64, 70, 85, 14, 2, 150, 35, 92, 17, 86, 33, 19};
    auto numDatos   = sizeof(datos) / sizeof(datos[0]);

    //AVL Ascending (menor a mayor)
    cout << "*** AVL Tree : TreeTraitAscending ***" << endl;
    
    CBinaryTree<TreeTraitAscending<T1>> ascTree; 
    
    cout << "1. push datos (Insert) ..." << endl;
    for (size_t i=0; i<numDatos; ++i) { ascTree.Insert(datos[i], i); }

    cout << "AscTree : \n" << ascTree << "\n";
    cout << "SizeTree: " << ascTree.size() << endl;

    cout << "\n2. Forward Iterator (begin a end) ..." << endl;
    cout << " -> ";
    for (auto it = ascTree.begin(); it != ascTree.end(); ++it) {
        cout << *it << " ";
    }
    cout << "\n";

    cout << "\n3. Backward Iterator (rbegin a rend) ..." << endl;
    cout << " -> ";
    for (auto it = ascTree.rbegin(); it != ascTree.rend(); ++it) {
        cout << *it << " ";
    }
    cout << "\n";

    cout << "\n4. Recorridos Variadic ..." << endl;
    cout << " -> InOrder  : ";
    ascTree.InOrder(ImprimirVariadic, "[", "] ");
    cout << "\n -> PreOrder : ";
    ascTree.PreOrder(ImprimirVariadic, "", ", ");
    cout << "\n -> PostOrder: ";
    ascTree.PostOrder(ImprimirVariadic, "<", "> ");
    cout << "\n";

    T1 valExtracc = 33;
    cout << "\n5. extraer: " << valExtracc << "..." << endl;
    ascTree.remove(valExtracc); // Borrará la primera coincidencia
    
    cout << "nuevo arbol: \n" << ascTree << endl;
    cout << "SizeTree: " << ascTree.size() << endl;
    cout << "\n";

    //AVL Descending (mayor a menor)
    cout << "*** AVL Tree : TreeTraitDescending ***" << endl;
    
    CBinaryTree<TreeTraitDescending<T1>> descTree; 
    
    cout << "1. push datos (Insert) ..." << endl;
    for (size_t i=0; i<numDatos; ++i) { descTree.Insert(datos[i], i); }

    cout << "DescTree : \n" << descTree << "\n";
    cout << "SizeTree: " << descTree.size() << endl;

    valExtracc = 12;
    cout << "\n2. extraer: " << valExtracc << "..." << endl;
    descTree.remove(valExtracc);
    
    cout << "nuevo arbol: \n" << descTree << endl;
    cout << "SizeTree: " << descTree.size() << endl;
    cout << "\n";
}

