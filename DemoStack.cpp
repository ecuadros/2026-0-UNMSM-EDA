#include <iostream>
#include "containers/lists.h"

using namespace std;

void DemoStack(){
    cout << "--- Demo Stack (LIFO) ---" << endl;

    // push
    CStack< StackAscendingTrait<int> > s1;
    s1.push(10, 1);
    s1.push(20, 2);
    s1.push(30, 3);
    s1.push(40, 4);
    s1.push(50, 5);
    cout << "Despues de push: " << s1 << endl;

    // pop
    cout << "Pop: " << s1.pop() << endl;
    cout << "Pop: " << s1.pop() << endl;
    cout << "Despues de pop: " << s1 << endl;

    // constructor copia
    CStack< StackAscendingTrait<int> > s2(s1);
    cout << "Copia (antes de modificar): " << s2 << endl;
    s2.push(99, 9);
    cout << "Copia (con elemento extra): " << s2 << endl;
    cout << "Original (debe estar igual que antes): " << s1 << endl;

    // move constructor
    CStack< StackAscendingTrait<int> > s3(move(s2));
    cout << "Movida (debe tener el contenido de la copia): " << s3 << endl;
    cout << "Original tras move (debe estar vacia): " << s2 << endl;
}
