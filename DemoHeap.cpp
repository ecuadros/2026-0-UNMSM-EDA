#include <iostream>
#include <utility>
#include "containers/heap.h"

using namespace std;
using T1 = int;

void DemoHeap(){

    cout << "PROBANDO MAX HEAP\n";

    CHeap< MaxHeapTrait<T1> > h1;

    cout << "\nInsertando elementos...\n";
    T1 vec[] = {20, 5, 30, 10, 40, 15, 60, 25, 35};

    for(T1 x : vec){
        cout << "push(" << x << ")\n";
        h1.push(x);
        cout << " -> " << h1 << endl;
    }

    cout << "\nTop actual(máximo): " << h1.top() << endl;

    cout << "\nEliminando 3 elementos con pop():\n";
    for(int i=0;i<3;i++){
        cout << "pop()\n";
        h1.pop();
        cout << " -> " << h1 << endl;
    }

    cout << "PROBANDO MIN HEAP\n";

    CHeap< MinHeapTrait<T1> > h2;

    T1 vec2[] = {50, 20, 80, 35, 15, 60, 10, 90, 5};

    for(T1 x : vec2){
        cout << "push(" << x << ")\n";
        h2.push(x);
        cout << " -> " << h2 << endl;
    }

    cout << "\nTop actual (mínimo): " << h2.top() << endl;


    cout << "\n COPY CONSTRUCTOR\n";

    CHeap< MaxHeapTrait<T1> > h3;

    h3.push(100);
    h3.push(50);
    h3.push(75);

    cout << "Heap original h3: " << h3 << endl;

    CHeap< MaxHeapTrait<T1> > h4 = h3;

    cout << "Heap copia h4:    " << h4 << endl;

    cout << "\nModificando h3 (pop):\n";
    h3.pop();

    cout << "h3: " << h3 << endl;
    cout << "h4 (no cambia): " << h4 << endl;


    cout << "\n MOVE CONSTRUCTOR\n";

    CHeap< MaxHeapTrait<T1> > h5;

    h5.push(11);
    h5.push(87);
    h5.push(78);

    cout << "h5 antes del move: " << h5 << endl;

    CHeap< MaxHeapTrait<T1> > h6 = std::move(h5);

    cout << "h5 (movido, vacío): " << h5 << endl;
    cout << "h6 (recibe datos): " << h6 << endl;

    //  OPERATOR >>
    cout << "\n INGRESO MANUAL\n";

    CHeap< MaxHeapTrait<T1> > h8;

    cin >> h8;

    cout << "Heap ingresado: " << h8 << endl;
    cout << "Top: " << h8.top() << endl;

}
