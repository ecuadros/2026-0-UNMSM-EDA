#include <iostream>
#include <sstream>
#include "containers/BStarTrees.h"

using namespace std;
using T1    = int;
using Trait = BStarAscendingTrait<T1>;
using OI    = BStarObjectInfo<T1, ref_type>;


// Funciones variadic para la demo


// Imprime clave y ObjID de un nodo
void PrintInfo(OI &info) {
    cout << "(" << info.key << ":" << info.ObjID << ") ";
}

// Incrementa el ObjID 

void AddToObjID(OI &info, long cantidad) {
    info.ObjID += cantidad;
}


bool EsMultiploDe(OI &info, T1 divisor) {
    return info.key % divisor == 0;
}



void DemoBStarTree() {
    cout << "=== DEMO B estrella   ===" << endl << endl;


    cout << "Test 1: Creacion e Insercion " << endl;
    CBStarTree<Trait> tree;
    tree.Insert(50, 1);
    tree.Insert(30, 2);
    tree.Insert(70, 3);
    tree.Insert(20, 4);
    tree.Insert(40, 5);
    tree.Insert(60, 6);
    tree.Insert(80, 7);
    tree.Insert(10, 8);
    tree.Insert(25, 9);
    tree.Insert(35, 10);
    cout << tree << endl;


    cout << "Test 2: Busqueda" << endl;
    ref_type found = tree.Search(40);
    cout << "Search(40): ObjID = " << found;
    if (found != -1) {
        cout << " [fue encontrado]";
    } else {
        cout << " [NO fue encontrado]";
    }
    cout << endl;

    found = tree.Search(99);
    cout << "Search(99): ObjID = " << found;
    if (found != -1) {
        cout << " [fue encontrado]";
    } else {
        cout << " [NO fue encontrado]";
    }
    cout << endl << endl;

    cout << "Test 3: Inorden con variadic " << endl;
    cout << "Inorden: ";
    tree.inorden(PrintInfo);
    cout << endl << endl;


    cout << "Test 4: Preorden con variadic " << endl;
    cout << "Preorden: ";
    tree.preorden(PrintInfo);
    cout << endl << endl;


    cout << "Test 5: Postorden con variadic " << endl;
    cout << "Postorden: ";
    tree.postorden(PrintInfo);
    cout << endl << endl;


    cout << "Test 6: Foreach variadic sumar(+100) a cada nodo" << endl;
    cout << "Antes:   ";
    tree.inorden(PrintInfo);
    cout << endl;
    tree.Foreach(AddToObjID, (long)100);
    cout << "Despues: ";
    tree.inorden(PrintInfo);
    cout << endl << endl;


    cout << "Test 7: Forward Iterator " << endl;
    cout << "::Foreach inorden: ";
    ::Foreach(tree.begin(), tree.end(), PrintInfo);
    cout << endl << endl;


    cout << "Test 8: Backward Iterator " << endl;
    cout << "Inorden inverso: ";
    ::Foreach(tree.rbegin(), tree.rend(), PrintInfo);
    cout << endl << endl;


    cout << "Test 9: firstThat multiplo de 7" << endl;
    OI resultado;
    bool hayResultado = tree.firstThat(resultado, EsMultiploDe, (T1)7);
    if (hayResultado) {
        cout << "Primer multiplo de 7: clave=" << resultado.key
             << " ObjID=" << resultado.ObjID << endl;
    } else {
        cout << "No se encontro multiplo de 7." << endl;
    }
    cout << endl;


    cout << "Test 10: ::FirstThat" << endl;
    auto it = ::FirstThat(tree.begin(), tree.end(),
        [](OI &info) { return info.key > 60; });
    if (it != tree.end()) {
        cout << "Primer elemento con clave > 60: clave=" << (*it).key << endl;
    } else {
        cout << "No encontrado." << endl;
    }
    cout << endl;


    cout << "Test 11: Remove (eliminar clave 35)" << endl;
    bool removed = tree.Remove(35);
    if (removed) {
        cout << "Eliminado correctamente." << endl;
    } else {
        cout << "No encontrado." << endl;
    }
    cout << tree << endl;

    cout << "Test 12: size, height, isEmpty" << endl;
    cout << "el size() es    = " << tree.size()   << endl;
    cout << "el height()  = " << tree.height() << endl;
    cout << "¿isEmpty()? = ";
    if (tree.isEmpty()) {
        cout << "Si";
    } else {
        cout << "No";
    }
    cout << endl << endl;


    cout << "Test 13: Insertar clave duplicada (clave 50 ya existe)" << endl;
    bool dup = tree.Insert(50);
    if (dup) {
        cout << "Insert(50): Insertado" << endl;
    } else {
        cout << "Insert(50): Duplicado rechazado" << endl;
    }
    cout << endl;


    cout << "Test 14: operator >> " << endl;
    {
        istringstream ss("100 99");
        ss >> tree;
        cout << "Insertado via >>: clave=100, ObjID=99" << endl;
        cout << tree << endl;
    }


    cout << "Test 15: Persistencia" << endl;
    tree.SaveToFile("bstartree_data.txt");
    cout << "Guardado en bstartree_data.txt" << endl;

    CBStarTree<Trait> tree2;
    tree2.LoadFromFile("bstartree_data.txt");
    cout << "Arbol cargado desde archivo:" << endl;
    cout << tree2 << endl;


    cout << "Test 16: Move Constructor (std::exchange)" << endl;
    CBStarTree<Trait> tree3 = std::move(tree2);
    cout << "Arbol movido (tree3): size=" << tree3.size() << endl;
    cout << "Origen tras move (tree2): isEmpty = ";
    if (tree2.isEmpty()) {
        cout << "Si";
    } else {
        cout << "No";
    }
    cout << endl << endl;


    cout << "Test 17: BStarDescendingTrait" << endl;
    CBStarTree< BStarDescendingTrait<T1> > treeDesc;
    treeDesc.Insert(50, 1);
    treeDesc.Insert(30, 2);
    treeDesc.Insert(70, 3);
    treeDesc.Insert(20, 4);
    treeDesc.Insert(80, 5);
    cout << "Inorden (DescendingTrait): ";
    using OID = BStarObjectInfo<T1, ref_type>;
    treeDesc.inorden([](OID &i) { cout << "(" << i.key << ":" << i.ObjID << ") "; });
    cout << endl << endl;



    cout << "Test 18: Arbol vacio" << endl;
    CBStarTree<Trait> treeEmpty;
    ref_type r = treeEmpty.Search(42);
    cout << "Search(42) en arbol vacio: " << r;
    if (r == -1) {
        cout << " [Correcto: no encontrado]";
    }
    cout << endl << endl;

    cout << "=== FIN DEMO arbol b estrella ===" << endl << endl;
}
