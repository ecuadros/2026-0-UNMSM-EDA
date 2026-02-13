#include <iostream>
#include <stdexcept>
using namespace std;
#include "containers/stack.h"

using T1 = int;

void DemoStack() {
    cout << "\nDemoStack" << endl;
    cout << "------------------------" << endl;

    //Crear stack y hacer push de varios elementos
    CStack< StackTrait<T1> > s1;
    s1.push(10, 0);
    s1.push(20, 1);
    s1.push(30, 2);
    s1.push(40, 3);
    s1.push(50, 4);
    cout << "Stack s1 luego de 5 push:" << endl;
    cout << s1;

    //Pop: debe retornar el último elemento insertado (LIFO)
    T1 val = s1.pop();
    cout << "pop() retornó: " << val << endl;
    cout << "Stack s1 luego del pop:" << endl;
    cout << s1;

    //operator>> : insertar desde cin (simulado con stringstream)
    //s1 puede recibir elementos desde un istream
    //Ejemplo comentado para no bloquear ejecución:
    //cin >> s1;

    //Constructor copia
    CStack< StackTrait<T1> > s2(s1);
    cout << "Stack s2 (copia de s1):" << endl;
    cout << s2;

    //Verificar independencia: pop en s2 no afecta s1
    s2.pop();
    cout << "s2 luego de pop (s1 no debe cambiar):" << endl;
    cout << "s1: " << s1;
    cout << "s2: " << s2;

    //Move Constructor
    CStack< StackTrait<T1> > s3(move(s1));
    cout << "Stack s3 (move de s1):" << endl;
    cout << s3;
    cout << "s1 luego del move (debe estar vacío), size = " << s1.getSize() << endl;

    //Vaciar el stack con pop hasta que esté vacío
    cout << "Vaciando s3 con pop:" << endl;
    while (s3.getSize() > 0)
        cout << "  pop() = " << s3.pop() << endl;
    cout << "s3 vacío, size = " << s3.getSize() << endl;

    //Pop en stack vacío debe lanzar excepción
    cout << "Intentando pop() en stack vacío:" << endl;
    try {
        s3.pop();
    } catch (const runtime_error &e) {
        cout << "Excepción capturada: " << e.what() << endl;
    }

    //Stack con strings
    CStack< StackTrait<string> > ss;
    ss.push("UNMSM", 0);
    ss.push("EDA",   1);
    ss.push("CS",    2);
    cout << "\nStack de strings:" << endl;
    cout << ss;
    cout << "pop() = " << ss.pop() << endl;
    cout << ss;

    cout << "---------------------------" << endl;
    cout << "Fin DemoStack \n" << endl;
}