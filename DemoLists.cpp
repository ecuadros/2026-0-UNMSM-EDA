#include <iostream>
#include <cassert>
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

    Size count = 0;
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

void DemoLists(){
    CLinkedList< AscendingTrait<T1> > l1;

    testInsert(l1);
    testCopyConstructor(l1);
    testMoveConstructor();
    testInsertUpdatesLast();
    testIterators();
}
