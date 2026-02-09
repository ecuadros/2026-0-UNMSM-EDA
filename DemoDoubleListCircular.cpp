#include <iostream>
#include "containers/lists.h"

using namespace std;
void DemoDoubleListCircular(){
    CDoubleLinkedListCircular<AscendingTrait <T1> > l1;

    l1.Insert(20, 5);
    l1.Insert(30, 3);
    l1.Insert(40, 6);
    l1.Insert(50, 8);
    l1.Insert(90, 9);
    cout << l1 << endl;
}