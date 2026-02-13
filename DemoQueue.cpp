#include <iostream>
#include <stdexcept>
using namespace std;
#include "containers/queue.h"

using T1 = int;

void DemoQueue() {
    cout << "\nDemoQueue" << endl;
    cout << "------------------------" << endl;

    //Crear queue y hacer push de varios elementos
    CQueue< QueueTrait<T1> > q1;
    q1.push(10, 0);
    q1.push(20, 1);
    q1.push(30, 2);
    q1.push(40, 3);
    q1.push(50, 4);
    cout << "Queue q1 luego de 5 push:" << endl;
    cout << q1;

    //Pop: debe retornar el primer elemento insertado (FIFO)
    T1 val = q1.pop();
    cout << "pop() retornó: " << val << endl;
    cout << "Queue q1 luego del pop:" << endl;
    cout << q1;

    //operator>> : insertar desde istream
    //Ejemplo comentado para no bloquear ejecución:
    //cin >> q1;

    //Constructor copia
    CQueue< QueueTrait<T1> > q2(q1);
    cout << "Queue q2 (copia de q1):" << endl;
    cout << q2;

    //Verificar independencia: pop en q2 no afecta q1
    q2.pop();
    cout << "q2 luego de pop (q1 no debe cambiar):" << endl;
    cout << "q1: " << q1;
    cout << "q2: " << q2;

    //Move Constructor
    CQueue< QueueTrait<T1> > q3(move(q1));
    cout << "Queue q3 (move de q1):" << endl;
    cout << q3;
    cout << "q1 luego del move (debe estar vacío), size = " << q1.getSize() << endl;

    //Vaciar la queue con pop hasta que esté vacía
    cout << "Vaciando q3 con pop:" << endl;
    while (q3.getSize() > 0)
        cout << "  pop() = " << q3.pop() << endl;
    cout << "q3 vacía, size = " << q3.getSize() << endl;

    //Pop en queue vacía debe lanzar excepción
    cout << "Intentando pop() en queue vacía:" << endl;
    try {
        q3.pop();
    } catch (const runtime_error &e) {
        cout << "Excepción capturada: " << e.what() << endl;
    }

    //Queue con strings
    CQueue< QueueTrait<string> > qs;
    qs.push("UNMSM", 0);
    qs.push("EDA",   1);
    qs.push("CS",    2);
    cout << "\nQueue de strings:" << endl;
    cout << qs;
    cout << "pop() = " << qs.pop() << endl;
    cout << qs;

    cout << "--------------------------" << endl;
    cout << "Fin DemoQueue\n" << endl;
}