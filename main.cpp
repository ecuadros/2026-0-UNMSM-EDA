/*#include <iostream>
#include "util.h"
#include "pointers.h"
#include "containers/array.h"
#include "containers/lists.h"

// g++ -std=c++17 -Wall -g -pthread main.cpp -o main

int main() {
    std::cout << "Hello EDA-UNMSM!" << std::endl;
    // DemoUtil();
    // DemoPointers1();
    // DemoArray();
    DemoLists();
    return 0;
}*/
#include <iostream>
#include "containers/linkedlist.h"
#include "containers/queue.h"
using namespace std;

int main(){
    CQueue<int> q;

    cout<< "Insertando elementos... " << endl;

    q.push(10);
    q.push(20);
    q.push(30);

    cout << "Frente de la cola: " << q.front() << endl;

    cout << "Eliminando elementos... " << endl;
    q.pop();

    cout << "Frente de la cola luego del pop: " << q.front() << endl;

    cout << "\nProbando constructor copia... " << endl;
    CQueue<int> q2(q); // Constructor copia



    cout << "Frente de la cola copia: " << q2.front() << endl;

    cout << "\nProbando constructor move... " << endl;
    CQueue<int> q3(std::move(q2)); // Constructor move

    cout << "Frente de la cola movida: " << q3.front() << endl;

    cout << "\nComparando colas... " << endl;
    if (q3 > q) {
        cout << "q3 tiene mas elementos que q" << endl;
    } else{
        cout << "q3 no tiene mas elementos que q" << endl;
    }

    return 0;
}