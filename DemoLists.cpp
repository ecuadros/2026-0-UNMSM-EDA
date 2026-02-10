#include <iostream>
#include "containers/doublelinkedlist.h"

using namespace std;

void DemoDoubleLists() {
    cout << "--- TEST LDE (SIMPLE DOBLE) ---" << endl;
    CDoubleLinkedList<AscendingTrait<int>, false> lde;
    lde.Insert(15, 0); lde.Insert(5, 0); lde.Insert(10, 0);
    cout << "Forward: " << lde << endl;

    cout << "Backward: ";
    for(auto it = lde.rbegin(); it != lde.rend(); ++it) cout << *it << " ";
    cout << endl;

    cout << "--- TEST LDEC (CIRCULAR DOBLE) ---" << endl;
    CDoubleLinkedList<AscendingTrait<int>, true> ldec;
    ldec.Insert(100, 0); ldec.Insert(200, 0); ldec.Insert(150, 0);
    // Requisito: Primer y último conectados (se maneja en Insert)
    cout << ldec << endl;

    cout << "--- TEST FIRSTTHAT ---" << endl;
    int* res = ldec.FirstThat([](int n){ return n > 120; });
    if(res) cout << "Encontrado > 120: " << *res << endl;
}