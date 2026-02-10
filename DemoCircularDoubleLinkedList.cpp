#include <iostream>
#include "containers/circulardoublelinkedlist.h"
#include "list_traits.h"

using namespace std;

void DemoCircularDoubleLinkedList() {
    cout << "\n--- Demo CCircularDoubleLinkedList ---" << endl;
    
    CCircularDoubleLinkedList<AscendingTrait<int>> cdlist;
    
    cdlist.push_back(5, 1);
    cdlist.push_back(10, 2);
    cdlist.push_back(15, 3);
    cdlist.push_back(20, 4);
    
    cout << "Lista circular doble:" << endl;
    cout << cdlist;
    
    cout << "Tamaño: " << cdlist.getSize() << endl;
    cout << "Acceso circular cdlist[0]: " << cdlist[0] << endl;
    cout << "Acceso circular cdlist[5]: " << cdlist[5] << endl;
    cout << "Acceso circular cdlist[8]: " << cdlist[8] << endl;
    
    CCircularDoubleLinkedList<AscendingTrait<int>> sorted;
    sorted.Insert(50, 1);
    sorted.Insert(10, 2);
    sorted.Insert(30, 3);
    sorted.Insert(20, 4);
    
    cout << "Insert ordenado:" << endl;
    cout << sorted;
    
    int sum = 0;
    cdlist.Foreach([&sum](int val) { sum += val; });
    cout << "Suma (Foreach): " << sum << endl;
    
    cout << "Iterador forward: ";
    auto it = cdlist.begin();
    for(size_t i = 0; i < cdlist.getSize(); ++i) {
        cout << *it << " ";
        ++it;
    }
    cout << endl;
}
