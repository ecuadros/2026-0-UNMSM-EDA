#include <iostream>
#include <thread>
#include <vector>
#include <functional> 
#include "containers/linkedlist.h" 

using namespace std;

using T1 = int;

void ThreadTask(CLinkedList<AscendingTrait<T1>>& list, T1 start, T1 count) {
    for (auto i = 0; i < count; ++i) {
        list.Insert(start + i, i);
    }
}

void DemoListsV2() {
    CLinkedList<AscendingTrait<T1>> list;

    thread t1(ThreadTask, ref(list), 100, 5);
    thread t2(ThreadTask, ref(list), 10, 5);
    
    t1.join();
    t2.join();

    cout << list << endl;

    cout << "Forward Iterator: ";
    for (auto it = list.begin(); it != list.end(); ++it) {
        cout << *it << " -> ";
    }
    cout << "NULL" << endl;

    if (list.getSize() > 5) {
        cout << "\nElemento en la posicion 5: " << list[5] << endl;
    }

    cout << "\nConstructor Copia" << endl;
    CLinkedList<AscendingTrait<T1>> copiedList = list;
    cout << copiedList << endl;

    cout << "Foreach  " << endl;
    list.Foreach([](T1& val) {
        cout << "[" << val * 2 << "] ";
    });
    cout << endl;
}

