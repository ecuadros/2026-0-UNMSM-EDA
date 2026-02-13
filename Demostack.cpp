#include <iostream>
#include "containers/stack.h"

using namespace std;

void DemoStack() {
    // Stack básico
    CStack<int> s1;
    s1.push(10);
    s1.push(20);
    s1.push(30);
    cout << s1 << endl;
    
    cout << "Pop: " << s1.pop() << endl;
    cout << s1 << endl;

    // Constructor copia
    CStack<int> s2(s1);
    cout << s2 << endl;
}
