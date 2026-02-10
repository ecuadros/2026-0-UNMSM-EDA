#include <iostream>
#include <thread>
#include <vector>
#include <functional>
#include "containers/doublelinkedlist.h" 

using namespace std;

using T1 = int;

void ThreadTask(CDoubleLinkedList<AscendingTrait<T1>>& list, T1 start, T1 count) {
    for (auto i = 0; i < count; ++i) {
        list.Insert(start + i, i);
    }
}

void DemoListsV3() {

    CDoubleLinkedList<AscendingTrait<T1>> list;

    thread t1(ThreadTask, ref(list), 50, 5);  
    thread t2(ThreadTask, ref(list), 10, 5);  
    
    t1.join();
    t2.join();

    cout << "\nAscendingTrait \n" << list << endl;

    cout << "\nForward Iterator " << endl;
    for (auto it = list.begin(); it != list.end(); ++it) {
        cout << *it << " -> ";
    }
    cout << "NULL" << endl;

    cout << "\nBackward Iterator " << endl;
    for (auto it = list.rbegin(); it != list.rend(); ++it) {
        cout << *it << " <- ";
    }
    cout << "ROOT" << endl;

    cout << "\nForeach " << endl;
    list.Foreach([](T1& val) {
        cout << "[" << val * 10 << "] ";
    });
    cout << endl;

    CDoubleLinkedList<AscendingTrait<T1>> listCopy = list;
    
    if (listCopy.getElements() == list.getElements()) {
        cout << "Constructor de copia size = " << listCopy.getElements() << endl;
    }

    if (list.getRoot() != nullptr && list.getLast() != nullptr) {
        cout << "\nVerificando Integridad LDEC:" << endl;
        cout << "Root->Prev es Last? " << (list.getRoot()->GetPrev() == list.getLast() ? "SI" : "NO") << endl;
        cout << "Last->Next es Root? " << (list.getLast()->GetNext() == list.getRoot() ? "SI" : "NO") << endl;
    }

}
