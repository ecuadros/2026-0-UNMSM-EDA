#include <iostream>
#include "containers/doublelinkedlist.h"

using namespace std;

int main(){
    using Traits = DoubleAscendingTrait<int>;
    CCircularDoubleLinkedList<Traits> list;

    list.push_back(10, 100);
    list.push_back(5, 200);
    list.push_back(20, 300);

    cout << list << endl;

    cout << "Forward iteration:" << endl;
    for(auto it = list.begin(); it != list.end(); ++it){
        cout << *it << " ";
    }
    cout << endl;

    cout << "Backward iteration:" << endl;
    for(auto it = list.rbegin(); it != list.rend(); ++it){
        cout << *it << " ";
    }
    cout << endl;

    return 0;
}
