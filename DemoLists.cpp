#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include <thread>
#include <vector>
#include "containers/lists.h"

using namespace std;

using AscendingLL = CLinkedList<AscendingTrait<T1>>;
using DescendingLL = CLinkedList<DescendingTrait<T1>>;


static ofstream logFile("linkedlist_tests.log");
// if (logFile.is_open()) return logFile;

void testInsert(AscendingLL& original) {
    logFile << "=== Prueba de inserciones basicas ===" << endl;
    logFile << "insertar 3 elementos: 20, 30, 10 en ese orden" << endl;
    logFile << "Lista enlazada ascendente" << endl;
    original.Insert(20, 5);
    original.Insert(30, 3);
    original.Insert(10, 13);
    assert(original.getSize() == 3);
    assert(original[0] == 10);
    assert(original[1] == 20);
    assert(original[2] == 30);
    logFile << original << endl;
    logFile << "imprimir valores en indices 2 y 3 (error en este ultimo)" << endl;
    logFile << original[2] << endl;
    bool threw = false;
    try {
        logFile << original[3] << endl;
    } catch (const exception& e) {
        threw = true;
        logFile << e.what() << endl;
    }
    assert(threw);
    logFile << "insertar valores 10 y 50" << endl;
    original.Insert(10, 10);
    original.Insert(50, 1);
    assert(original.getSize() == 5);
    assert(original[0] == 10);
    assert(original[1] == 10);
    assert(original[2] == 20);
    assert(original[3] == 30);
    assert(original[4] == 50);
    logFile << "Lista enlazada final:" << endl;
    logFile << original << endl;
    logFile << "OK: inserciones basicas funcionan y respetan orden\n" << endl;
}

void testCopyConstructor(AscendingLL& original) {
    logFile << "=== Prueba de copia de lista enlazada ===" << endl;
    logFile << "Lista original: " << original << endl;
    AscendingLL copy(original);
    assert(copy.getSize() == original.getSize());
    for (size_t i = 0; i < original.getSize(); ++i) {
        assert(copy[i] == original[i]);
    }
    logFile << "Lista copia: " << copy << endl;
    logFile << "añadiendo 120 y 40 a la lista copia" << endl;
    T1 bar = 120;
    copy.push_back(bar, 1);
    T1 bar2 = 40;
    copy.push_back(bar2, 2);
    assert(copy.getSize() == original.getSize() + 2);
    logFile << "Lista copia modificada: " << copy << endl;
    logFile << "Lista original: " << original << endl;
    assert(original.getSize() + 2 == copy.getSize());
    logFile << "OK: copia de lista enlazada funciona correctamente\n" << endl;
}

void testMoveConstructor() {
    logFile << "=== Prueba de movimiento de lista enlazada ===" << endl;
    // Crear lista temporal
    CLinkedList<AscendingTrait<T1>> temp;
    temp.Insert(42, 42);
    temp.Insert(24, 24);
    logFile << "Lista temporal antes del movimiento: " << temp << endl;

    // Mover la lista temporal (usando el constructor de movimiento)
    CLinkedList<AscendingTrait<T1>> moved(std::move(temp));
    assert(moved.getSize() == 2);
    assert(temp.getSize() == 0);
    logFile << "Lista movida: " << moved << endl;
    logFile << "Lista temporal después del movimiento (debería estar vacía): " << temp << endl;
    logFile << "OK: movimiento de lista enlazada funciona correctamente\n" << endl;
}

void testInsertUpdatesLast() {
    logFile << "=== Prueba de actualizar m_pLast al insertar al final ===" << endl;
    CLinkedList<AscendingTrait<T1>> list;

    list.push_back(10, 1);
    list.push_back(20, 2);
    list.Insert(30, 3); // fuerza InternalInsert a llegar al final

    assert(list.getSize() == 3);
    assert(list.m_pLast != nullptr);
    assert(list.m_pLast->GetValue() == 30);
    logFile << "OK: insert actualiza m_pLast al ultimo nodo correctamente\n" << endl;
}

void testIterators() {
    logFile << "=== Prueba de iteradores forward ===" << endl;
    CLinkedList<AscendingTrait<T1>> list;
    list.push_back(10, 1);
    list.push_back(20, 2);
    list.push_back(30, 3);

    size_t count = 0;
    T1 sum = 0;
    logFile << "sumar los elementos de la lista: " << list << endl;
    for (auto it = list.begin(); it != list.end(); ++it) {
        logFile << "sumar " << *it << endl;
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

    logFile << "OK: iteradores avanzan y dereferencian correctamente\n" << endl;
}

void testQuotedStringIO() {
    logFile << "=== Prueba de IO con strings y std::quoted ===" << endl;

    using StringList = CLinkedList<AscendingTrait<string>>;
    StringList list;
    list.push_back(string("a: b) c"), 7);

    logFile << "lista de string: " << list << endl;
    stringstream ss;
    ss << list;

    StringList readBack;
    ss >> readBack;
    logFile << "lista leida: " << readBack << endl;
    assert(readBack.getSize() == 1);
    assert(readBack.m_pRoot != nullptr);
    assert(readBack.m_pRoot->GetValue() == "a: b) c");
    assert(readBack.m_pRoot->GetRef() == 7);

    logFile << "OK: strings con ':' y ')' se leen/escriben bien" << endl;

    logFile << "Strings con escaped quotes" << endl;
    list.clear();
    readBack.clear();

    list.Insert("1 string \"", 1);
    list.Insert("2 string \'", 2);
    list.Insert("\"3 string \"", 3);
    list.Insert("\'4 string \'", 4);

    logFile << "La lista con escaped quotes: " << list << endl;
    ss.str("");
    ss << list;
    ss >> readBack;
    logFile << "Lista leida: " << readBack << endl;
    assert(readBack.getSize() == 4);
    logFile << readBack[0] << endl;
    assert(readBack[0] == "\"3 string \"");
    assert(readBack[1] == "\'4 string \'");
    assert(readBack[2] == "1 string \"");
    assert(readBack[3] == "2 string \'");

    logFile << "OK: strings con escaped quotes se leen/escriben bien\n" << endl;
}

void testWriteRead(string outFilePath = "linkedlist_write.log") {
    logFile << "=== Prueba de lectura y escritura ===" << endl;
    CLinkedList<AscendingTrait<T1>> list;

    list.push_back(10, 1);
    list.push_back(20, 2);
    list.push_back(30, 3);
    list.Insert(40, 4);
    list.Insert(5, 5);

    logFile << "Lista original: " << list << endl;
    logFile << "archivo de escritura: " << outFilePath << endl;
    ofstream outFile(outFilePath);
    if (outFile.is_open()) {
        outFile << list;
        outFile.close();
    } else logFile << "Error al abrir archivo de escritura" << endl;

    CLinkedList<AscendingTrait<T1>> list2;
    ifstream inFile(outFilePath);
    if (inFile.is_open()) {
        inFile >> list2;
        inFile.close();
        logFile << "Lista cargada desde archivo: " << list2 << endl;
        // Verificar que la lista se haya cargado correctamente
        assert(list2.getSize() == list.getSize());
        for (size_t i = 0; i < list.getSize(); ++i) {
            assert(list[i] == list2[i]);
        }
        logFile << "OK: La lista se guardó y recuperó correctamente\n" << endl;
    } else logFile << "Error al abrir el archivo para lectura.\n" << endl;
}

void testLLConcurrency() {
    logFile << "=== Prueba de concurrencia: push_back, iteraciones, lectura y escritura ===" << endl;
    AscendingLL list;
    AscendingLL temp;
    const Size threads = 6;
    const Size perThread = 50;

    vector<thread> workers;
    workers.reserve(threads);
    stringstream ss;

    workers.emplace_back([&]() {
        logFile << "LinkedList: << list en el estado en que este" << endl;
        ss << list;
        logFile << "LinkedList: >> list en el estado en que este" << endl;
        ss >> temp;
    });
    for (int t = 0; t < threads - 2; ++t) {
        workers.emplace_back([t, &list]() {
            T1 base = t * perThread;
            for (Size i = 0; i < perThread; ++i) {
                list.push_back(base + i, base + i);
            }
        });
    }
    workers.emplace_back([&]() {
        logFile << "LinkedList: iterador " << endl;
        for (auto it = list.begin(); it != list.end(); ++it)
            try {
                logFile << "(" << *it << ")";
            } catch (const exception& e) {
                logFile << e.what();
                break;
            }
        logFile << endl;
    });

    for (auto &th : workers) th.join();

    // intentamos leer la lista a ver en que estado esta
    logFile << "LinkedList: list: " << list << endl;
    logFile << "LinkedList: temp: " << temp << endl;

    assert(list.getSize() == static_cast<size_t>((threads - 2) * perThread));
    logFile << "tamaño final: " << list.getSize() << endl;
    logFile << "OK: concurrencia funciona correctamente\n" << endl;
}

void testAssignmentOperator() {
    logFile << "=== Prueba de operador de asignacion ===" << endl;
    CLinkedList<AscendingTrait<T1>> a;
    a.push_back(10, 1);
    a.push_back(20, 2);
    a.push_back(30, 3);

    CLinkedList<AscendingTrait<T1>> b;
    b.push_back(99, 9);
    b = a;

    assert(b.getSize() == a.getSize());
    for (size_t i = 0; i < a.getSize(); ++i) {
        assert(a[i] == b[i]);
    }

    b = b; // self-assign should be no-op
    assert(b.getSize() == a.getSize());
    for (size_t i = 0; i < a.getSize(); ++i) {
        assert(a[i] == b[i]);
    }
    logFile << "OK: operador= copia y self-assign\n" << endl;
}

void testAllListOrders() {
    logFile << "=== Prueba: listas ordenadas y no ordenadas ===" << endl;

    // AscendingTrait va en orden ascendente
    CLinkedList<AscendingTrait<T1>> asc;
    asc.Insert(20, 1);
    asc.Insert(10, 2);
    asc.Insert(30, 3);
    logFile << "lista ordenada ascendente" << endl;
    logFile << asc << endl;
    assert(asc.getSize() == 3);
    assert(asc[0] == 10);
    assert(asc[1] == 20);
    assert(asc[2] == 30);

    // DescendingTrait usa std::less => orden ascendente
    CLinkedList<DescendingTrait<T1>> desc;
    desc.Insert(20, 1);
    desc.Insert(10, 2);
    desc.Insert(30, 3);
    logFile << "lista ordenada descendente" << endl;
    logFile << desc << endl;
    assert(desc.getSize() == 3);
    assert(desc[0] == 30);
    assert(desc[1] == 20);
    assert(desc[2] == 10);

    // Unordered: por defecto
    CLinkedList<UnorderedTrait<T1>> unord;
    unord.Insert(10, 1);
    unord.Insert(30, 3);
    unord.Insert(20, 2, 1); // insert en medio
    unord.Insert(5, 4, 0);  // insert al inicio
    unord.Insert(50, 5, -1);  // testeo de la chistosada
    logFile << "lista no ordenada" << endl;
    logFile << unord << endl;
    assert(unord.getSize() == 5);
    assert(unord[0] == 5);
    assert(unord[1] == 10);
    assert(unord[2] == 20);
    assert(unord[3] == 30);
    assert(unord[4] == 50);

    logFile << "OK: orden asc/desc y no ordenada\n" << endl;
}

void testForeachFirstthat() {
    logFile << "=== Prueba de FirstThat y forEach ===" << endl;
    logFile << "Prueba de firstThat" << endl;
    CLinkedList<AscendingTrait<T1>> list;
    list.push_back(10, 1);
    list.push_back(20, 2);
    list.push_back(30, 3);
    list.push_back(40, 4);
    list.push_back(50, 5);
    list.push_back(60, 6);
    auto firstMultipleOfThree = list.firstThat([](const T1 &x) { return x % 3 == 0; });
    logFile << "primer multiplo de 3: " << firstMultipleOfThree  << endl;
    assert(firstMultipleOfThree == 30);
    assert(list.getSize() == 6);
    logFile << "OK: FirstThat funciona correctamente" << endl;
    logFile << "Prueba de forEach" << endl;
    list.push_back(70, 7);
    list.push_back(80, 8);
    list.push_back(90, 9);
    list.push_back(100, 10);
    list.push_back(110, 11);
    list.push_back(120, 12);

    logFile << "dividir a todos entre 10" << endl;
    list.forEach([](T1& x) { x /= 10; });
    assert(list.getSize() == 12);
    assert(list.firstThat([](const T1 &x) { return x % 3 == 0; }) == 3);
    logFile << list << endl;

    logFile << "OK: forEach funciona correctamente\n" << endl;
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
    testAssignmentOperator();
    testLLConcurrency();
    testForeachFirstthat();
    testAllListOrders();
    logFile.close();
}

