#include <iostream>
#include "containers/queue.h"
using namespace std;

template <typename T>
struct SimpleTrait {
    using value_type = T;
};

int main() {
    CQueue<SimpleTrait<int>> queue;
    
    cout << "- TEST QUEUE -" << endl;
    cout << "Pusheando: 10, 20, 30" << endl;
    queue.push(10);
    queue.push(20);
    queue.push(30);
    
    cout << queue << endl;
    
    cout << "Pop: " << queue.pop() << endl;
    cout << queue << endl;
    
    cout << "Front: " << queue.front() << endl;
    cout << "Back: " << queue.back() << endl;
    cout << "Size: " << queue.getSize() << endl;
    
    // Probar constructor copia
    cout << "\n=== Constructor Copia ===" << endl;
    CQueue<SimpleTrait<int>> queue2(queue);
    cout << "Queue original: " << queue;
    cout << "Queue copiada: " << queue2;
    
    return 0;
}