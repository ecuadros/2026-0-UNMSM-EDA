#include <iostream>
#include <fstream>
#include <iterator>
#include "containers/lists.h"
#include "foreach.h"
#include "containers/traits.h"

using namespace std;

using T1 = int;

void DemoLists(){
    CStack<AscendingTrait<T1>> stack;

    stack.push(10, 1);
    stack.push(20, 2);
    stack.push(30, 3);
    stack.push(40, 4);
    cout << stack << endl;

    cout << "top = " << stack.top() << " // 40" << endl;
    cout << "pop = " << stack.pop() << " // -40" << endl;
    cout << "pop = " << stack.pop() << " // .30" << endl;
    cout << "isEmpty? = " << (stack.isEmpty() ? "true" : "false") << endl;
    cout << stack;
    cout << "finish #1 - push + top and pop\n" << endl;

    stack.push(50, 5);
    stack.push(60, 6);
    cout << stack;
    cout << "finish #2 - push\n" << endl;

    cout << "vaciar stack:" << endl;
    for(;!stack.isEmpty();){ cout << "pop() = " << stack.pop() << endl; }
    // stack.pop(); //vacio
    cout << "isEmpty? = " << (stack.isEmpty() ? "true" : "false") << endl;
    cout << "finish #3 - empty\n" << endl;
}
