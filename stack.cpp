#include "stack.h"
#include <iostream>
#include <string>

using namespace std;

// Definir un trait para int
struct IntTraits {
    using T = int;
};

// Definir un trait para string
struct StringTraits {
    using T = std::string;
};

int main() {
    // Prueba con int
    CStack<IntTraits> stackInt;
    stackInt.push(10, 1);
    stackInt.push(20, 2);
    stackInt.push(30, 3);
    cout << "Stack de int: " << stackInt << endl;
    cout << "Pop: " << stackInt.pop() << endl;
    cout << "Stack de int tras pop: " << stackInt << endl;

    // Prueba de constructor copia
    CStack<IntTraits> stackIntCopy(stackInt);
    cout << "Stack copia: " << stackIntCopy << endl;

    // Prueba de move constructor
    CStack<IntTraits> stackIntMove(std::move(stackIntCopy));
    cout << "Stack movido: " << stackIntMove << endl;

    // Prueba con string
    CStack<StringTraits> stackStr;
    stackStr.push("uno", 1);
    stackStr.push("dos", 2);
    stackStr.push("tres", 3);
    cout << "Stack de string: " << stackStr << endl;
    cout << "Pop: " << stackStr.pop() << endl;
    cout << "Stack de string tras pop: " << stackStr << endl;

    // Prueba de operador >>
    cout << "Ingresa un stack en formato:         ";
    CStack<IntTraits> stackInput;
    cin >> stackInput;
    cout << "Stack ingresado: " << stackInput << endl;

    // Validación y demostración 
    if (!cin || stackInput.empty()) {
        cout << "Error: El formato ingresado es incorrecto o el stack está vacío." << endl;
    } else {
        cout << "Probando comportamiento LIFO (pop): ";
        try {
            while (!stackInput.empty()) {
                cout << stackInput.pop();
                if (!stackInput.empty()) cout << ", ";
            }
            cout << endl;
        } catch (const std::exception& e) {
            cout << "\nError al hacer pop: " << e.what() << endl;
        }
    }

    return 0;
}
