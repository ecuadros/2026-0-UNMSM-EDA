#include <iostream>
#include <fstream>
#include <iterator>
#include "containers/lists.h"
#include "foreach.h"
#include "containers/traits.h"

using namespace std;

using T1 = int;

void DemoLists(){
    CQueue<AscendingTrait<T1>> queue;
    
    queue.enqueue(10, 1);
    queue.enqueue(20, 2);
    queue.enqueue(30, 3);
    queue.enqueue(40, 4);
    cout << queue;

    cout << "front =    " << queue.front()   << " //  10" << endl;
    cout << "dequeue =  " << queue.dequeue() << " // -10" << endl;
    cout << "dequeue =  " << queue.dequeue() << " // -20" << endl;
    cout << queue;
    
    cout << "isEmpty? = " << (queue.isEmpty() ? "true" : "false") << endl;
    cout << "size     = " << queue.size() << endl;

    cout << "finish #1 - queue + dequeue\n" << endl;
    
    queue.enqueue(50, 5);
    queue.enqueue(60, 6);
    cout << queue;
    
    cout << "\nvaciando queue:" << endl;
    while(!queue.isEmpty()){ cout << "dequeue() = " << queue.dequeue() << endl; }
    cout << "isEmpty? = " << (queue.isEmpty() ? "true" : "false") << endl;
    cout << "finish #2 - empty queue\n" << endl;
    
    CHeap<AscendingTrait<T1>> heap;
    heap.insert(10, 1);
    heap.insert(20, 2);
    heap.insert(30, 3);
    cout << heap;
    
    cout << "first        = " << heap.first()        << endl;
    cout << "extractFirst = " << heap.extractFirst() << endl;
    cout << heap;
    cout << "finish #3 - push + extractheap\n" << endl;
}

