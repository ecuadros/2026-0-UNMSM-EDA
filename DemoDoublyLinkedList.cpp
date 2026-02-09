//
// Created by aroon on 2/9/26.
//
#include <cassert>
#include <fstream>
#include <sstream>
#include "containers/lists.h"
#include "containers/doublelinkedlist.h"

using namespace std;

static ofstream logFile("doublelinkedlist_tests.log");

using AscendingDLL = CDoubleLinkedList<AscendingTrait<T1>>;
using DescendingDLL = CDoubleLinkedList<DescendingTrait<T1>>;
using UnorderedDLL = CDoubleLinkedList<UnorderedTrait<T1>>;

void testDoubleBasic() {
    logFile << "Prueba basica: double push_back y operator[]" << endl;
    AscendingDLL list;
    list.push_back(30, 3);
    logFile << "insertando 30 exitosamente" << endl;
    logFile << list << endl;
    list.push_back(10, 1);
    logFile << "insertando 10 exitosamente" << endl;
    logFile << list << endl;
    list.push_back(20, 2);
    logFile << "insertando 20 exitosamente" << endl;
    logFile << list << endl;
    assert(list.getSize() == 3);
    assert(list[0] == 10);
    assert(list[1] == 20);
    assert(list[2] == 30);
}

void testDoubleOrderedInsert() {
    logFile << "Prueba: insert ordenado double" << endl;
    DescendingDLL list; // DescendingTrait => orden ascendente
    list.Insert(20, 1);
    list.Insert(10, 2);
    list.Insert(30, 3);
    assert(list.getSize() == 3);
    assert(list[0] == 30);
    assert(list[1] == 20);
    assert(list[2] == 10);
    logFile << list << endl;
}

void testDoubleUnorderedInsert() {
    logFile << "Prueba: insert por indice double no ordenada" << endl;
    UnorderedDLL list;
    list.Insert(10, 1);
    list.Insert(30, 3);
    list.Insert(20, 2, 1);
    list.Insert(5, 4, 0);
    assert(list.getSize() == 4);
    assert(list[0] == 5);
    assert(list[1] == 10);
    assert(list[2] == 20);
    assert(list[3] == 30);
    logFile << list << endl;
}

void testDoubleIterators() {
    logFile << "Prueba: iteradores double (forward/backward)" << endl;
    UnorderedDLL list;
    list.Insert(1, 1);
    list.Insert(2, 2);
    list.Insert(3, 3);
    logFile << list << endl;
    T1 sumF = 0;
    size_t countF = 0;

    auto inicio= list.begin();
    auto final = list.end();
    for (auto it = list.begin(); it != list.end(); ++it) {
        logFile << "iterador forward: " << sumF << endl;
        sumF += *it;
        ++countF;
    }
    assert(countF == list.getSize());
    assert(sumF == 6);
    T1 sumB = 0;
    size_t countB = 0;

    auto inicio2 = list.rbegin();
    auto final2 = list.rend();
    for (auto it = list.rbegin(); it != list.rend(); ++it) {
        logFile << "iterador backward: " << sumB << endl;
        sumB += *it;
        ++countB;
    }
    assert(countB == list.getSize());
    assert(sumB == 6);
}

static void testDoubleIO() {
    logFile << "Prueba: IO double" << endl;
    AscendingDLL list;
    list.push_back(10, 1);
    list.push_back(20, 2);
    list.push_back(30, 3);

    stringstream ss;
    ss << list;

    AscendingDLL readBack;
    ss >> readBack;
    assert(readBack.getSize() == list.getSize());
    for (size_t i = 0; i < list.getSize(); ++i) {
        assert(readBack[i] == list[i]);
    }
}

static void testDoubleAssignment() {
    logFile << "Prueba: operador= double" << endl;
    AscendingDLL a;
    a.push_back(10, 1);
    a.push_back(20, 2);
    a.push_back(30, 3);

    AscendingDLL b;
    b.push_back(99, 9);
    b = a;

    assert(b.getSize() == a.getSize());
    for (size_t i = 0; i < a.getSize(); ++i) {
        assert(b[i] == a[i]);
    }

    b = b;
    assert(b.getSize() == a.getSize());
}

static void testDoubleForeachFirstThat() {
    logFile << "Prueba: forEach y firstThat double" << endl;
    UnorderedDLL list;
    list.Insert(10, 1);
    list.Insert(20, 2);
    list.Insert(30, 3);
    list.Insert(40, 4);
    list.Insert(50, 5);
    list.Insert(60, 6);

    const auto firstMultipleOfThree = list.firstThat(
        [](const T1 &x) { return x % 3 == 0; }
    );
    assert(firstMultipleOfThree == 30);

    list.forEach([](T1 &x) { x /= 10; });
    assert(list.getSize() == 6);
    assert(list.firstThat(
        [](const T1 &x) { return x % 3 == 0; }) == 3
        );
}

void DemoDoubleLinkedLists() {
    testDoubleBasic();
    testDoubleOrderedInsert();
    testDoubleUnorderedInsert();
    testDoubleIterators();
    testDoubleIO();
    testDoubleAssignment();
    testDoubleForeachFirstThat();
}
