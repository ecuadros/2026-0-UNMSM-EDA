#include <cassert>
#include <fstream>
#include <sstream>
#include <thread>
#include <vector>
#include "containers/lists.h"

using namespace std;

static ofstream logFile("circularlinkedlist_tests.log");

using AscendingCLL = CCircularLinkedList<AscendingTrait<T1>>;
using DescendingCLL = CCircularLinkedList<DescendingTrait<T1>>;
using UnorderedCLL = CCircularLinkedList<UnorderedTrait<T1>>;

static void testCircularBasic() {
    logFile << "Prueba basica: circular push_back y operator[]" << endl;
    logFile << "Lista circular ascendente" << endl;
    AscendingCLL list;
    list.push_back(30, 3);
    list.push_back(10, 1);
    list.push_back(20, 2);
    logFile << list << endl;
    assert(list.getSize() == 3);
    assert(list[0] == 10);
    assert(list[1] == 20);
    assert(list[2] == 30);
    logFile << list << endl;
}

static void testCircularOrderedInsert() {
    logFile << "Prueba: insert ordenado circular" << endl;
    logFile << "Lista circular descendente" << endl;
    DescendingCLL list; // DescendingTrait => orden ascendente
    list.insert(20, 1);
    list.insert(10, 2);
    list.insert(30, 3);
    assert(list.getSize() == 3);
    assert(list[0] == 30);
    assert(list[1] == 20);
    assert(list[2] == 10);
    logFile << list << endl;
}

static void testCircularUnorderedInsert() {
    logFile << "Prueba: insert por indice en circular no ordenada" << endl;
    logFile << "Lista circular no ordenada" << endl;
    UnorderedCLL list;
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

static void testCircularIterators() {
    logFile << "Prueba: iteradores circulares" << endl;
    UnorderedCLL list;
    list.insert(1, 1);
    list.insert(2, 2);
    list.insert(3, 3);
    logFile << list << endl;
    // primero se prueba el iterador forward
    T1 sumF = 0;
    size_t countF = 0;
    for (auto it = list.begin(); it != list.end(); ++it) {
        logFile << "iterador forward: " << *it << endl;
        sumF += *it;
        ++countF;
    }
    assert(countF == list.getSize());
    assert(sumF == 6);
}

static void testCircularIO() {
    logFile << "Prueba: IO circular" << endl;
    AscendingCLL list;
    list.push_back(10, 1);
    list.push_back(20, 2);
    list.push_back(30, 3);

    // se escribe a un string stream
    stringstream ss;
    ss << list;
    // se lee de regreso
    AscendingCLL readBack;
    ss >> readBack;
    assert(readBack.getSize() == list.getSize());
    for (size_t i = 0; i < list.getSize(); ++i) {
        assert(readBack[i] == list[i]);
    }
}

static void testCircularAssignment() {
    logFile << "Prueba: operador= circular" << endl;
    AscendingCLL a;
    a.push_back(10, 1);
    a.push_back(20, 2);
    a.push_back(30, 3);

    AscendingCLL b;
    b.push_back(99, 9);
    b = a;

    // se itera y se espera que todos los items sean iguales
    assert(b.getSize() == a.getSize());
    for (size_t i = 0; i < a.getSize(); ++i) {
        assert(b[i] == a[i]);
    }

    // en la linked list regular se comprobo que aqui se ahorra el copiar
    b = b;
    assert(b.getSize() == a.getSize());
}

void testCircularQuotedStringIO() {
    logFile << "Prueba de IO con strings y std::quoted" << endl;

    using StringList = CCircularLinkedList<AscendingTrait<string>>;
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

    list.insert("1 string \"", 1);
    list.insert("2 string \'", 2);
    list.insert("\"3 string \"", 3);
    list.insert("\'4 string \'", 4);

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
}

void testCircularConcurrency() {
    logFile << "Prueba: concurrencia circular (push_back + iterador)" << endl;
    UnorderedCLL list;
    const Size threads = 4;
    const Size perThread = 50;

    // lista con los workers
    vector<thread> workers;
    workers.reserve(threads + 1);

    for (Size t = 0; t < threads; ++t) {
        // añade 4 workers
        workers.emplace_back([t, &list]() {
            // cada worker añade items en un distinto rango
            T1 base = t * perThread;
            for (Size i = 0; i < perThread; ++i) {
                list.push_back(base + i, base + i);
            }
        });
    }
    // añade un worker iterador
    workers.emplace_back([&list]() {
        logFile << "Lista circular: iterador " << endl;
        try {
            for (auto it = list.begin(); it != list.end(); ++it)
                logFile << "(" << *it << ")";
            logFile << endl;
        } catch (const exception &e) {
            logFile << "Iterador lanzo excepcion: " << e.what() << endl;
        }
    });

    for (auto &th : workers) th.join();

    assert(list.getSize() == threads * perThread);
    logFile << "OK: size == " << list.getSize() << endl;
}

void testCircularForeachFirstThat() {
    logFile << "Prueba: forEach y firstThat en circular" << endl;
    UnorderedCLL list;
    list.insert(10, 1);
    list.insert(20, 2);
    list.insert(30, 3);
    list.insert(40, 4);
    list.insert(50, 5);
    list.insert(60, 6);

    auto firstMultipleOfThree = list.firstThat([](const T1 &x) { return x % 3 == 0; });
    logFile << "primer multiplo de 3: " << firstMultipleOfThree << endl;
    assert(firstMultipleOfThree == 30);

    list.forEach([](T1 &x) { x /= 10; });
    assert(list.getSize() == 6);
    assert(list.firstThat([](const T1 &x) { return x % 3 == 0; }) == 3);
    logFile << list << endl;
    logFile << "OK: forEach y firstThat circular" << endl;
}

void DemoCircularLinkedLists() {
    testCircularBasic();
    testCircularOrderedInsert();
    testCircularUnorderedInsert();
    testCircularIterators();
    testCircularIO();
    testCircularAssignment();
    testCircularQuotedStringIO();
    testCircularConcurrency();
    testCircularForeachFirstThat();
}
