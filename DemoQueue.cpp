#include <iostream>
#include "containers/lists.h"

using namespace std;

void DemoQueue(){
    cout << "--- Demo Queue (FIFO) ---" << endl;

    // push
    CQueue< QueueAscendingTrait<int> > q1;
    q1.push(10, 1);
    q1.push(20, 2);
    q1.push(30, 3);
    q1.push(40, 4);
    q1.push(50, 5);
    cout << "Despues de push: " << q1 << endl;

    // pop
    cout << "Pop: " << q1.pop() << endl;
    cout << "Pop: " << q1.pop() << endl;
    cout << "Despues de pop: " << q1 << endl;

    // constructor copia
    CQueue< QueueAscendingTrait<int> > q2(q1);
    cout << "Copia (antes de modificar): " << q2 << endl;
    q2.push(99, 9);
    cout << "Copia (con elemento extra): " << q2 << endl;
    cout << "Original (debe estar igual que antes): " << q1 << endl;

    // move constructor
    CQueue< QueueAscendingTrait<int> > q3(move(q2));
    cout << "Movida (debe tener el contenido de la copia): " << q3 << endl;
    cout << "Original tras move (debe estar vacia): " << q2 << endl;
}
