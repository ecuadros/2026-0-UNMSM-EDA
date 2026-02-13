#include <assert.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "containers/queue.h"
#include "containers/lists.h"

using namespace std;

using QueueInt = CQueue< QueueTraits<T1> >;
using QueueString = CQueue< QueueTraits<string> >;

// Test 1: Verificar operaciones básicas push y pop
static void TestBasicPushPop(ostream &log) {
    QueueInt q;
    q.push(10, 1);
    q.push(20, 2);
    q.push(30, 3);

    T1 first = q.pop();
    T1 second = q.pop();
    T1 third = q.pop();

    assert(first == 10);
    assert(second == 20);
    assert(third == 30);

    log << "TestBasicPushPop: PASSED" << endl;
}

// Test 2: Verificar que pop en cola vacía lance excepción
static void TestEmptyQueueException(ostream &log) {
    QueueInt q;
    bool exception_caught = false;
    
    try {
        (void)q.pop();
    } catch (const exception &) {
        exception_caught = true;
    }
    
    assert(exception_caught);
    log << "TestEmptyQueueException: PASSED" << endl;
}

// Test 3: Verificar serialización y deserialización
static void TestSerializationDeserialization(ostream &log) {
    QueueInt q;
    q.push(5, 50);
    q.push(7, 70);
    q.push(9, 90);

    ostringstream output_stream;
    output_stream << q;

    istringstream input_stream(output_stream.str());
    QueueInt q_restored;
    input_stream >> q_restored;

    T1 val1 = q_restored.pop();
    T1 val2 = q_restored.pop();
    T1 val3 = q_restored.pop();

    assert(val1 == 5);
    assert(val2 == 7);
    assert(val3 == 9);

    log << "TestSerializationDeserialization: PASSED" << endl;
}

// Test 4: Verificar cola de strings con push y pop
static void TestStringOperations(ostream &log) {
    QueueString q;
    q.push("first", 1);
    q.push("second", 2);
    q.push("third", 3);

    string str1 = q.pop();
    string str2 = q.pop();
    string str3 = q.pop();

    assert(str1 == "first");
    assert(str2 == "second");
    assert(str3 == "third");

    log << "TestStringOperations: PASSED" << endl;
}

// Test 5: Verificar serialización de strings
static void TestStringStreamOperations(ostream &log) {
    QueueString q;
    q.push("alpha", 10);
    q.push("beta", 20);
    q.push("gamma", 30);

    ostringstream out;
    out << q;

    istringstream in(out.str());
    QueueString q_copy;
    in >> q_copy;

    string str1 = q_copy.pop();
    string str2 = q_copy.pop();
    string str3 = q_copy.pop();

    assert(str1 == "alpha");
    assert(str2 == "beta");
    assert(str3 == "gamma");

    log << "TestStringStreamOperations: PASSED" << endl;
}

// Test 6: Verificar constructores de copia y movimiento, y operadores de asignación
static void TestCopyAndMoveSemantics(ostream &log) {
    bool exception_thrown = false;
    
    // === Prueba de Constructor de Copia ===
    QueueInt original_queue;
    original_queue.push(1, 10);
    original_queue.push(2, 20);
    original_queue.push(3, 30);

    QueueInt copied_queue(original_queue);
    
    // Verificar que la copia funciona correctamente
    assert(copied_queue.pop() == 1);
    assert(copied_queue.pop() == 2);
    assert(copied_queue.pop() == 3);
    
    exception_thrown = false;
    try {
        (void)copied_queue.pop();
    } catch (const exception &) {
        exception_thrown = true;
    }
    assert(exception_thrown);
    
    // Verificar que el original no fue afectado
    assert(original_queue.pop() == 1);
    assert(original_queue.pop() == 2);
    assert(original_queue.pop() == 3);
    
    exception_thrown = false;
    try {
        (void)original_queue.pop();
    } catch (const exception &) {
        exception_thrown = true;
    }
    assert(exception_thrown);
    
    log << "TestCopyAndMoveSemantics: Copy Constructor PASSED" << endl;

    // === Prueba de Operador de Asignación por Copia ===
    QueueInt source_queue;
    source_queue.push(10, 100);
    source_queue.push(20, 200);

    QueueInt destination_queue;
    destination_queue.push(5, 50);
    destination_queue = source_queue;

    assert(destination_queue.pop() == 10);
    assert(destination_queue.pop() == 20);
    
    exception_thrown = false;
    try {
        (void)destination_queue.pop();
    } catch (const exception &) {
        exception_thrown = true;
    }
    assert(exception_thrown);

    // Verificar que el origen sigue intacto
    assert(source_queue.pop() == 10);
    assert(source_queue.pop() == 20);
    
    exception_thrown = false;
    try {
        (void)source_queue.pop();
    } catch (const exception &) {
        exception_thrown = true;
    }
    assert(exception_thrown);

    // Prueba de auto-asignación
    source_queue.push(1, 1);
    source_queue = source_queue;
    assert(source_queue.pop() == 1);
    
    log << "TestCopyAndMoveSemantics: Copy Assignment PASSED" << endl;

    // === Prueba de Constructor de Movimiento ===
    QueueInt move_source;
    move_source.push(100, 1);
    move_source.push(200, 2);
    move_source.push(300, 3);

    QueueInt move_destination(std::move(move_source));
    
    assert(move_destination.pop() == 100);
    assert(move_destination.pop() == 200);
    assert(move_destination.pop() == 300);
    
    exception_thrown = false;
    try {
        (void)move_destination.pop();
    } catch (const exception &) {
        exception_thrown = true;
    }
    assert(exception_thrown);

    // Verificar que el origen quedó vacío
    exception_thrown = false;
    try {
        (void)move_source.pop();
    } catch (const exception &) {
        exception_thrown = true;
    }
    assert(exception_thrown);
    
    log << "TestCopyAndMoveSemantics: Move Constructor PASSED" << endl;

    // === Prueba de Operador de Asignación por Movimiento ===
    QueueInt move_assign_source;
    move_assign_source.push(1000, 1);
    move_assign_source.push(2000, 2);

    QueueInt move_assign_dest;
    move_assign_dest.push(50, 5);
    move_assign_dest = std::move(move_assign_source);

    assert(move_assign_dest.pop() == 1000);
    assert(move_assign_dest.pop() == 2000);
    
    exception_thrown = false;
    try {
        (void)move_assign_dest.pop();
    } catch (const exception &) {
        exception_thrown = true;
    }
    assert(exception_thrown);

    // Verificar que el origen quedó vacío
    exception_thrown = false;
    try {
        (void)move_assign_source.pop();
    } catch (const exception &) {
        exception_thrown = true;
    }
    assert(exception_thrown);
    
    log << "TestCopyAndMoveSemantics: Move Assignment PASSED" << endl;
}

void DemoQueue() {
    ofstream logFile("queue_tests.log");
    
    logFile << "=== Iniciando pruebas de Queue ===" << endl;
    
    TestBasicPushPop(logFile);
    TestEmptyQueueException(logFile);
    TestSerializationDeserialization(logFile);
    TestStringOperations(logFile);
    TestStringStreamOperations(logFile);
    TestCopyAndMoveSemantics(logFile);
    
    logFile << "=== Todas las pruebas completadas exitosamente ===" << endl;
    logFile << "DemoQueue: Done" << endl;
}