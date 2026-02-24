#include <iostream>
#include <fstream>
using namespace std;

#include "containers/binarytree.h"
#include "variadic-util.h"

using T1 = int;

template <typename Q>
void Print(Q &elem) { cout << elem << ","; }

bool Mult3(T1 &elem) { return elem % 3 == 0; }

void DemoBinaryTree() {
    CBinaryTree< TreeTraitAscending<T1> > t1;

    t1.Insert(30, 1);
    t1.Insert(10, 2);
    t1.Insert(50, 3);
    t1.Insert(20, 4);
    t1.Insert(40, 5);

    cout << t1;

    t1.Inorder(&Print<T1>);   cout << endl;
    t1.Preorder(&Print<T1>);  cout << endl;
    t1.Postorder(&Print<T1>); cout << endl;

    t1.Foreach(&Suma<T1>, 5);
    cout << t1;

    auto iter = t1.FirstThat(&Mult3);
    if (iter != t1.end())
        cout << "Primer multiplo de 3: " << *iter << endl;

    t1.Remove(50);
    cout << t1;

    ofstream of("binarytree.txt");
    of << t1;
    of.close();

    CBinaryTree< TreeTraitAscending<T1> > t2(t1);
    cout << t2;

    CBinaryTree< TreeTraitAscending<T1> > t3(std::move(t1));
    cout << t3;
}