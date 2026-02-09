#include <cassert>
#include <fstream>
#include <sstream>
#include "containers/lists.h"
#include "containers/circulardoublelinkedlist.h"

using namespace std;

static ofstream logFile("circulardoublelinkedlist_tests.log");

using AscendingCDLL = CCircularDoubleLinkedList<AscendingTrait<T1>>;
using DescendingCDLL = CCircularDoubleLinkedList<DescendingTrait<T1>>;
using UnorderedCDLL = CCircularDoubleLinkedList<UnorderedTrait<T1>>;

static void testCircularDoubleBasic() {
    logFile << "Prueba basica: circular double push_back y operator[]" << endl;
    AscendingCDLL list;
    list.push_back(30, 3);
    list.push_back(10, 1);
    list.push_back(20, 2);
    assert(list.getSize() == 3);
    assert(list[0] == 10);
    assert(list[1] == 20);
    assert(list[2] == 30);
    logFile << list << endl;
}

static void testCircularDoubleOrderedInsert() {
    logFile << "Prueba: insert ordenado circular double" << endl;
    DescendingCDLL list; // DescendingTrait => orden ascendente
    list.insert(20, 1);
    list.insert(10, 2);
    list.insert(30, 3);
    assert(list.getSize() == 3);
    assert(list[0] == 30);
    assert(list[1] == 20);
    assert(list[2] == 10);
    logFile << list << endl;
}

static void testCircularDoubleUnorderedInsert() {
    logFile << "Prueba: insert por indice circular double no ordenada" << endl;
    UnorderedCDLL list;
    list.insert(10, 1);
    list.insert(30, 3);
    list.insert(20, 2, 1);
    list.insert(5, 4, 0);
    assert(list.getSize() == 4);
    assert(list[0] == 5);
    assert(list[1] == 10);
    assert(list[2] == 20);
    assert(list[3] == 30);
    logFile << list << endl;
}

static void testCircularDoubleIterators() {
    logFile << "Prueba: iteradores circular double (forward)" << endl;
    UnorderedCDLL list;
    list.insert(1, 1);
    list.insert(2, 2);
    list.insert(3, 3);

    T1 sumF = 0;
    size_t countF = 0;
    for (auto it = list.begin(); it != list.end(); ++it) {
        sumF += *it;
        ++countF;
    }
    assert(countF == list.getSize());
    assert(sumF == 6);
}

static void testCircularDoubleIO() {
    logFile << "Prueba: IO circular double" << endl;
    AscendingCDLL list;
    list.push_back(10, 1);
    list.push_back(20, 2);
    list.push_back(30, 3);

    stringstream ss;
    ss << list;

    AscendingCDLL readBack;
    ss >> readBack;
    assert(readBack.getSize() == list.getSize());
    for (size_t i = 0; i < list.getSize(); ++i) {
        assert(readBack[i] == list[i]);
    }
}

static void testCircularDoubleAssignment() {
    logFile << "Prueba: operador= circular double" << endl;
    AscendingCDLL a;
    a.push_back(10, 1);
    a.push_back(20, 2);
    a.push_back(30, 3);

    AscendingCDLL b;
    b.push_back(99, 9);
    b = a;

    assert(b.getSize() == a.getSize());
    for (size_t i = 0; i < a.getSize(); ++i) {
        assert(b[i] == a[i]);
    }

    b = b;
    assert(b.getSize() == a.getSize());
}

static void testCircularDoubleForeachFirstThat() {
    logFile << "Prueba: forEach y firstThat circular double" << endl;
    UnorderedCDLL list;
    list.insert(10, 1);
    list.insert(20, 2);
    list.insert(30, 3);
    list.insert(40, 4);
    list.insert(50, 5);
    list.insert(60, 6);

    auto firstMultipleOfThree = list.firstThat([](const T1 &x) { return x % 3 == 0; });
    assert(firstMultipleOfThree == 30);

    list.forEach([](T1 &x) { x /= 10; });
    assert(list.getSize() == 6);
    assert(list.firstThat([](const T1 &x) { return x % 3 == 0; }) == 3);
}

void DemoCircularDoubleLinkedLists() {
    testCircularDoubleBasic();
    testCircularDoubleOrderedInsert();
    testCircularDoubleUnorderedInsert();
    testCircularDoubleIterators();
    testCircularDoubleIO();
    testCircularDoubleAssignment();
    testCircularDoubleForeachFirstThat();
}

