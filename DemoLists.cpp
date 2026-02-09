#include <iostream>
#include "containers/lists.h"
#include "foreach.h"
#include <utility>
#include <sstream>

using namespace std;

void DemoLists(){
    CCircularLinkedList< AscendingTrait<T1> > l1;
    l1.Insert(20, 5);
    l1.Insert(30, 3);
    l1.Insert(10, 7);
    l1.Insert(25, 9);

    cout << l1 << endl;

    cout << "Foreach values: ";
    Foreach(l1, [](int &x){ cout << x << " "; });
    cout << endl;
    std::istringstream in("20 5 30 3 10 7");
    CCircularLinkedList< AscendingTrait<T1> > l2;
    in >> l2;
    cout << "l2 (leida con >>): " << l2 << endl;
}