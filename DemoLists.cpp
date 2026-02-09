#include <iostream>
#include <fstream>
#include <iterator>
#include "containers/lists.h"
#include "foreach.h"

using namespace std;

using T1 = int;
using Tf = float;

auto suma = [](auto &val, auto ...args) { val += (args + ...); };
auto par  = [](auto &val)               { return val % 2 == 0; };

void DemoLists(){
    CLinkedList< AscendingTrait<T1> > l1;
    l1.Insert(31, 5);
    l1.Insert(8, 3);
    l1.push_back(10, 3);
    cout << l1;
    cout << "l1[1] = " << l1[0] << endl;
    Foreach(l1, suma, 1);
    auto first_par = FirstThat(l1, par);
    cout << "first_par = " << *first_par << endl;
    cout << "finish #1" << endl << endl;

    CLinkedList< DescendingTrait<Tf> > l2;
    l2.Insert(20.1, 5);
    l2.Insert(11.35, 3);
    l2.push_back(52.5, 3);
    cout << l2;
    cout << "l2[3] = " << l2[2] << endl;
    Foreach(l2, suma, 5+2, 3);
    cout << l2;
    cout << "finish #2" << endl << endl;

    ofstream out("data_l1.txt");
    out << l1;
    out.close();
    cout << endl << "finish #3" << endl << endl;

    CLinkedList<AscendingTrait<int>> l3;
    ifstream in("datalist.txt");
    in >> l3;
    in.close();

    cout << "datalist -> list 3" << endl;
    for (auto it = l3.begin(); it != l3.end(); ++it) { cout << *it << " , "; }
    cout << endl << "finish #4" << endl << endl;

    CLinkedList<AscendingTrait<T1>> l4(l3);
    cout << "Copia:" << endl;
    cout << l4;
    cout << "finish #5 contructor de copia" << endl << endl;

    CLinkedList<AscendingTrait<T1>> l5(std::move(l4));
    cout << "Movida:" << endl;
    cout << l4;
    cout << l5;
    cout << "finish #6 move constructor" << endl << endl;

    auto it_ultimo = l3.begin() + (l3.getSize() - 1);
    if (it_ultimo.m_pNode->GetNext() == l3.begin().m_pNode)
        cout << "el ultimo apunta al primero. es circular." << endl;
    cout << "finish #7 linkedlistcircular" << endl << endl;
}