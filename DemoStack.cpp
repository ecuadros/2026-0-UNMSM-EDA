#include <iostream>
#include "containers/stack.h"
using namespace std;

template <typename T>
struct SimpleTrait {
    using value_type = T;
};

int main() {
    CStack<SimpleTrait<int>> stack;
    
    cout << "Pusheando: 10, 20, 30" << endl;
    stack.push(10);
    stack.push(20);
    stack.push(30);
    
    cout << stack << endl;
    
    cout << "Pop: " << stack.pop() << endl;
    cout << stack << endl;
    
    return 0;
}