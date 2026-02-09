#include <iostream>
#include "containers/lists.h"
#include "foreach.h"
#include <utility>
#include <sstream>

using namespace std;

void DemoLists(){
    CLinkedList< AscendingTrait<T1> > l1;
    l1.Insert(20, 5);
    l1.Insert(30, 3);

    CLinkedList< AscendingTrait<T1> > l2 = std::move(l1);

    cout << "l2:\n" << l2 << endl;
    cout << "l1 (moved-from):\n" << l1 << endl;

    cout << "Foreach values: ";
    Foreach(l2, [](int &x){cout << x << " ";});
    cout << endl;

    std::istringstream in("20 5 30 3 10 7");
    CLinkedList< AscendingTrait<T1> > l3;
    in >> l3;
    cout << "l3 (leida con >>):\n" << l3 << endl;

    cout << "l3[0]=" << l3[0] << " l3[1]=" << l3[1] << " l3[2]=" << l3[2] << endl;
    l3[1] = 999;
    cout << "l3 despues de l3[1]=999:\n" << l3 << endl;
}