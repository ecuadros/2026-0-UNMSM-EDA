#include <iostream>
#include "containers/queue.h"

using namespace std;

using namespace std;
using T1 = int;

void DemoCircularQueue(){

    // Crear cola con capacidad fija
    CircularQueue< QueueTrait<T1, std::less<T1>> > q1(5);

    // Push() 

    q1.push(1);
    q1.push(21);
    q1.push(-5);
    q1.push(45);

    cout << q1 << endl << endl;

    // Front()

    cout << "Elemento en el front: "
         << q1.front() << endl << endl;

    // Pop()

    q1.pop();
    cout << "Tras pop():  ";
    cout << q1 << endl;

    cout << "Nuevo front: "
         << q1.front() << endl << endl;

    // getSize(), empty(), full()

    cout << "Size = " << q1.getSize() << endl;
    cout << "¿Está vacía? " << (q1.empty() ? "Sí" : "No") << endl;
    cout << "¿Está llena? " << (q1.full() ? "Sí" : "No") << endl << endl;

    // Copy constructor

    CircularQueue< QueueTrait<T1, std::less<T1>> > q2 = q1;

    cout << "q1:  " << q1 << endl;
    cout << "q2 (copia):  " << q2 << endl;

    q2.pop();

    cout << "q1 (intacto):  " << q1 << endl;
    cout << "q2 (modificado):  " << q2 << endl << endl;

    // Move constructor

    CircularQueue< QueueTrait<T1, std::less<T1>> > q3 = std::move(q1);

    cout << "q1 tras move:  " << q1 << endl;
    cout << "q3 (recursos de q1):  " << q3 << endl << endl;

    // Clear()

    q3.clear();

    cout << "q3 tras clear:  " << q3 << endl;

    cout << "¿Está vacía? " << (q3.empty() ? "Sí" : "No") << endl << endl;

    // Operator >>

    CircularQueue< QueueTrait<T1, std::less<T1>> > q5(5);

    cin >> q5;

    cout << "Cola ingresada:  ";
    cout << q5 << endl << endl;
}


