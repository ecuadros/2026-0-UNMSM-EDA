#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include "containers/lists.h"

using namespace std;

using AscendingLL = CLinkedList<AscendingTrait<T1>>;
using DescendingLL = CLinkedList<DescendingTrait<T1>>;

void testInsert(AscendingLL& original) {
    cout << "Prueba de lista enlazada" << endl;
    cout << "insertar 3 elementos: 20, 30, 10 en ese orden" << endl;
    cout << "Lista enlazada ascendente" << endl;
    original.Insert(20, 5);
    original.Insert(30, 3);
    original.Insert(10, 13);
    cout << original << endl;
    cout << "imprimir valores en indices 2 y 3 (error en este ultimo)" << endl;
    cout << original[2] << endl;
    try {
        cout << original[3] << endl;
    } catch (const exception& e) {
        cout << e.what() << endl;
    }
    cout << "insertar valores 10 y 50" << endl;
    original.Insert(10, 10);
    original.Insert(50, 1);
    cout << original << endl;
    cout << "si el tamaño es 5 salio bien" << endl;
}

void testCopyConstructor(AscendingLL& original) {
    cout << endl << "Prueba de copia de lista enlazada" << endl;
    cout << "Lista original: " << original << endl;
    AscendingLL copy(original);
    cout << "Lista copia: " << copy << endl;
    cout << "añadiendo 120 y 40 a la lista copia" << endl;
    T1 bar = 120;
    copy.push_back(bar, 1);
    T1 bar2 = 40;
    copy.push_back(bar2, 2);
    cout << "Lista copia modificada: " << copy << endl;
    cout << "Lista original: " << original << endl;
}

void testMoveConstructor() {
    cout << "Prueba de movimiento de lista enlazada" << endl;
    // Crear lista temporal
    CLinkedList<AscendingTrait<T1>> temp;
    temp.Insert(42, 42);
    temp.Insert(24, 24);
    cout << "Lista temporal antes del movimiento: " << temp << endl;

    // Mover la lista temporal (usando el constructor de movimiento)
    CLinkedList<AscendingTrait<T1>> moved(std::move(temp));
    cout << "Lista movida: " << moved << endl;
    cout << "Lista temporal después del movimiento (debería estar vacía): " << temp << endl;
    cout << endl;
}

void testInsertUpdatesLast() {
    cout << "Prueba de actualizar m_pLast al insertar al final" << endl;
    CLinkedList<AscendingTrait<T1>> list;

    list.push_back(10, 1);
    list.push_back(20, 2);
    list.Insert(30, 3); // fuerza InternalInsert a llegar al final

    assert(list.getSize() == 3);
    assert(list.m_pLast != nullptr);
    assert(list.m_pLast->GetValue() == 30);
    cout << "OK: m_pLast apunta al ultimo nodo" << endl;
}

void testIterators() {
    cout << "Prueba de iteradores forward" << endl;
    CLinkedList<AscendingTrait<T1>> list;
    list.push_back(10, 1);
    list.push_back(20, 2);
    list.push_back(30, 3);

    size_t count = 0;
    T1 sum = 0;
    for (auto it = list.begin(); it != list.end(); ++it) {
        sum += *it;
        ++count;
    }
    assert(count == list.getSize());
    assert(sum == 60);

    auto it = list.begin();
    T1 first = *it;
    auto it2 = it++;
    assert(first == 10);
    assert(*it2 == 10);
    assert(*it == 20);

    cout << "OK: iteradores avanzan y dereferencian correctamente" << endl;
}

void testQuotedStringIO() {
    cout << "Prueba de IO con strings y std::quoted" << endl;

    using StringList = CLinkedList<AscendingTrait<string>>;
    StringList list;
    list.push_back(string("a: b) c"), 7);

    stringstream ss;
    ss << list;

    StringList readBack;
    ss >> readBack;

    assert(readBack.getSize() == 1);
    assert(readBack.m_pRoot != nullptr);
    assert(readBack.m_pRoot->GetValue() == "a: b) c");
    assert(readBack.m_pRoot->GetRef() == 7);

    cout << "OK: strings con ':' y ')' se leen/escriben bien" << endl;

    cout << "Strings con escaped quotes" << endl;
    list.clear();
    readBack.clear();

    list.Insert("1 string \"", 1);
    list.Insert("2 string \'", 2);
    list.Insert("\"3 string \"", 3);
    list.Insert("\'4 string \'", 4);

    cout << "La lista de strings con escaped quotes: " << list << endl;

    ss.str("");
    ss << list;
    ss >> readBack;

    assert(readBack.getSize() == 4);
    cout << readBack[0] << endl;
    assert(readBack[0] == "\"3 string \"");
    assert(readBack[1] == "\'4 string \'");
    assert(readBack[2] == "1 string \"");
    assert(readBack[3] == "2 string \'");

    cout << "OK: strings con escaped quotes se leen/escriben bien" << endl;
}

void testWriteRead(string outFilePath = "linkedlistsample.log") {
    cout << "Prueba de lectura y escritura" << endl;
    CLinkedList<AscendingTrait<T1>> list;

    list.push_back(10, 1);
    list.push_back(20, 2);
    list.push_back(30, 3);
    list.Insert(40, 4);
    list.Insert(5, 5);

    cout << "Lista original: " << list << endl;
    cout << "archivo de escritura: " << outFilePath << endl;
    ofstream outFile(outFilePath);
    if (outFile.is_open()) {
        outFile << list;
        outFile.close();
    } else {
        cout << "Error al abrir archivo de escritura" << endl;
    }

    CLinkedList<AscendingTrait<T1>> list2;
    ifstream inFile(outFilePath);
    if (inFile.is_open()) {
        inFile >> list2;
        inFile.close();
        cout << "Lista cargada desde archivo: " << list2 << endl;
        // Verificar que la lista se haya cargado correctamente
        assert(list2.getSize() == list.getSize());
        for (size_t i = 0; i < list.getSize(); ++i) {
            assert(list[i] == list2[i]);
        }
        cout << "OK: La lista se guardó y recuperó correctamente" << endl;
    } else {
        cout << "Error al abrir el archivo para lectura." << endl;
    }
}

void DemoLists(){
    CLinkedList< AscendingTrait<T1> > l1;

    testInsert(l1);
    testCopyConstructor(l1);
    testMoveConstructor();
    testInsertUpdatesLast();
    testIterators();
    testQuotedStringIO();
    testWriteRead();
}
