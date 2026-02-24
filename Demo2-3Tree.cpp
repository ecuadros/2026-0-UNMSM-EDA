#include <iostream>
#include <string>
#include <utility>
#include "containers/2-3Tree.h" 

using namespace std;


using TTInfoType = TwoThreeTree<char>::ObjectInfo;

void printNodeVariadicTT(TTInfoType& info, Size level, string sufijo) {
    cout << info.key << sufijo;
}

bool findNodeVariadicTT(TTInfoType& info, char target) {
    return info.key == target;
}

void DemoTwoThreeTree() {
    cout << " 2-3 TREE "<<endl;

    cout << "Probando  inserciones : \n";
    TwoThreeTree<char> bt(3);

    bt.Insert('M', 1);
    bt.Insert('A', 2);
    bt.Insert('Z', 3);
    bt.Insert('E', 4);
    bt.Insert('X', 5);
    bt.Insert('P', 6);
    bt.Insert('L', 7);
    bt.Insert('O', 8);

    cout << "Arbol actual (Operator <<):\n";
    cout << bt << "\n";

    cout << "Forward Iterator:\n";
    for (auto it = bt.begin(); it != bt.end(); ++it) {
        cout << it->key << " ";
    }
    cout << "\n\n";

    cout << "Backward Iterator:\n";
    for (auto it = bt.rbegin(); it != bt.rend(); ++it) {
        cout << it->key << " ";
    }
    cout << "\n\n";

    cout << "Inorden variadic: ";
    bt.InOrder(printNodeVariadicTT, string(", "));
    cout << "\n";

    cout << "Preorden variadic: ";
    bt.PreOrder(printNodeVariadicTT, string(", "));
    cout << "\n";

    cout << "Postorden variadic: ";
    bt.PostOrder(printNodeVariadicTT, string(", "));
    cout << "\n\n";

    cout << "FirstThat variadic (buscando 'X'):\n";
    auto encontrado = bt.FirstThat(findNodeVariadicTT, 'X');
    if (encontrado) {
        cout << "Elemento encontrado con ID: " << encontrado->ObjID << "\n\n";
    } else {
        cout << "Elemento no encontrado.\n\n";
    }
    cout << "Copy Constructor (copiando bt a btCopy)...\n";
    TwoThreeTree<char> btCopy = bt;
    cout << "Tamano del arbol original (bt): " << bt.size() << "\n";
    cout << "Tamano del arbol copiado (btCopy): " << btCopy.size() << "\n\n";

    cout << "Move Constructor (moviendo bt a btMove)...\n";
    TwoThreeTree<char> btMove = std::move(bt);
    cout << "Tamano del nuevo arbol (btMove): " << btMove.size() << "\n";
    cout << "Tamano del arbol original (bt): " << bt.size() << " (quedo vacio tras el move)\n\n";

    cout << "Operator >> (Escribe un caracter y presiona Enter):\n";
    cin >> btMove;
    cout << "\nArbol despues de la entrada:\n";
    cout << btMove << "\n";

    
}