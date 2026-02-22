#include "containers/heap.h"
#include <iostream>

using MaxHeap = CHeap<MaxHeapConfig<int>>;

int main() {
    MaxHeap h;
    h.push(5, 0);
    h.push(2, 0);
    h.push(7, 0);
    h.push(3, 0);

    std::cout << "Heap created:\n" << h << std::endl;

    int r1 = h.pop();
    std::cout << "First pop: " << r1 << " (expected 7)\n" << h << std::endl;

    int r2 = h.pop();
    std::cout << "Second pop: " << r2 << " (expected 5)\n" << h << std::endl;

    int r3 = h.pop();
    std::cout << "Third pop: " << r3 << " (expected 3)\n" << h << std::endl;

    int r4 = h.pop();
    std::cout << "Fourth pop: " << r4 << " (expected 2)\n" << h << std::endl;

    return 0;
}
