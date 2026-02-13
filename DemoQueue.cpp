#include <iostream>
#include "containers/queue.h"

using namespace std;

void DemoQueue() {
    // Queue básico
    CQueue<int> q1;
    q1.push(10);
    q1.push(20);
    q1.push(30);
    cout << q1 << endl;
    
    cout << "Pop: " << q1.pop() << endl;
    cout << q1 << endl;

    // Constructor copia
    CQueue<int> q2(q1);
    cout << q2 << endl;
}
