//
// Created by aroon on 2/18/26.
//

#include "containers/heap.h"
#include <cassert>
#include <sstream>

using MaxHeap = CHeap<AscendingHeapTrait<T1>>;
using MinHeap = CHeap<DescendingHeapTrait<T1>>;

void TestMaxHeapOrder() {
    std::cout << "TestMaxHeapOrder\n";
    MaxHeap h;
    h.push(3, 0);
    h.push(1, 0);
    h.push(4, 0);
    h.push(2, 0);

    assert(h.pop() == 4);
    assert(h.pop() == 3);
    assert(h.pop() == 2);
    assert(h.pop() == 1);
}

void TestMinHeapOrder() {
    std::cout << "TestMinHeapOrder\n";
    MinHeap h;
    h.push(3, 0);
    h.push(1, 0);
    h.push(4, 0);
    h.push(2, 0);

    assert(h.pop() == 1);
    assert(h.pop() == 2);
    assert(h.pop() == 3);
    assert(h.pop() == 4);
}

void TestDuplicates() {
    std::cout << "TestDuplicates\n";
    MinHeap h;
    h.push(2, 0);
    h.push(2, 0);
    h.push(1, 0);
    h.push(1, 0);

    assert(h.pop() == 1);
    assert(h.pop() == 1);
    assert(h.pop() == 2);
    assert(h.pop() == 2);
}

void TestTryPopEmpty() {
    std::cout << "TestTryPopEmpty\n";
    MaxHeap h;
    T1 out = h.pop();
    assert(out == 0);
}

void TestStreamOperators() {
    std::cout << "TestStreamOperators\n";
    MaxHeap h;
    h.push(5, 10);
    h.push(1, 11);
    h.push(3, 12);

    std::stringstream ss;
    ss << h;

    MaxHeap h2;
    ss >> h2;

    assert(h2.pop() == 5);
    assert(h2.pop() == 3);
    assert(h2.pop() == 1);
}

void TestStringStreamOperators() {
    std::cout << "TestStringStreamOperators\n";
    CHeap<AscendingHeapTrait<std::string>> h;
    h.push("hola", 1);
    h.push("\'\"wa\"\'\"", 2);
    h.push("albion online es un mmorpg no lineal", 3);

    std::stringstream ss;
    ss << h;

    CHeap<AscendingHeapTrait<std::string>> h2;
    ss >> h2;

    assert(h2.pop() == "hola");
    assert(h2.pop() == "albion online es un mmorpg no lineal");
    assert(h2.pop() == "\'\"wa\"\'\"");
}


void DemoHeap() {
    TestMaxHeapOrder();
    TestMinHeapOrder();
    TestDuplicates();
    TestTryPopEmpty();
    TestStreamOperators();
    TestStringStreamOperators();
}
