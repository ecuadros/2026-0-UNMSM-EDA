#include <iostream>
#include <sstream>
#include "containers/linkedlist.h"

using namespace std;

void DemoLists() {
    cout << "--- TEST LE SIMPLE ---" << endl;
    CLinkedList<AscendingTrait<int>, false> le;
    le.Insert(30, 0);
    le.Insert(10, 0);
    le.Insert(20, 0);
    cout << le << endl;

    cout << "--- TEST OPERATOR >> (LECTURA) ---" << endl;
    CLinkedList<AscendingTrait<int>, false> leEntrada;
    stringstream ss("50 40 60");
    ss >> leEntrada; // Uso del operador >>
    cout << "Datos cargados desde flujo: " << leEntrada << endl;

    cout << "--- TEST LEC CIRCULAR ---" << endl;
    CLinkedList<DescendingTrait<int>, true> lec;
    lec.Insert(100, 0);
    lec.Insert(50, 0);
    lec.Insert(80, 0);
    cout << lec << endl;

    cout << "--- TEST FOREACH ---" << endl;
    le.Foreach([](int &n) { n *= 2; });
    cout << "LE x2: " << le << endl;

    cout << "--- TEST COPIA Y CONCURRENCIA ---" << endl;
    auto copia = lec;
    cout << "Copia de LEC: " << copia << endl;
}#include <iostream>
#include <sstream>
#include "containers/linkedlist.h"

using namespace std;

void DemoLists() {
    cout << "--- TEST LE SIMPLE ---" << endl;
    CLinkedList<AscendingTrait<int>, false> le;
    le.Insert(30, 0);
    le.Insert(10, 0);
    le.Insert(20, 0);
    cout << le << endl;

    cout << "--- TEST OPERATOR >> (LECTURA) ---" << endl;
    CLinkedList<AscendingTrait<int>, false> leEntrada;
    stringstream ss("50 40 60");
    ss >> leEntrada; // Uso del operador >>
    cout << "Datos cargados desde flujo: " << leEntrada << endl;

    cout << "--- TEST LEC CIRCULAR ---" << endl;
    CLinkedList<DescendingTrait<int>, true> lec;
    lec.Insert(100, 0);
    lec.Insert(50, 0);
    lec.Insert(80, 0);
    cout << lec << endl;

    cout << "--- TEST FOREACH ---" << endl;
    le.Foreach([](int &n) { n *= 2; });
    cout << "LE x2: " << le << endl;

    cout << "--- TEST COPIA Y CONCURRENCIA ---" << endl;
    auto copia = lec;
    cout << "Copia de LEC: " << copia << endl;
}