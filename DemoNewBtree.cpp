#include <iostream>
#include <string>
#include <thread>
#include "containers/NewBtree.h"

using namespace std;

// Funcion auxiliar requerida para demostrar los recorridos estructurales con Variadic
using InfoType = BNEWTree<char>::ObjectInfo;
void printNodeVariadic(InfoType& info, Size level, string sufijo) {
    // Imprime la clave y el sufijo adicional pasado por el paquete variadic
    cout << info.key << sufijo;
}

void DemoNewBTree() {
    cout << "Demostracion de Estructura NewBTree Iniciada\n\n";

    cout << "Requisito: Constructor\n";
    BNEWTree<char> bt(3);
    
    // Insertando elementos iniciales
    bt.Insert('M', 1);
    bt.Insert('B', 2);
    bt.Insert('Z', 3);
    bt.Insert('A', 4);
    bt.Insert('F', 5);
    bt.Insert('Y', 6);
    cout << "Arbol poblado correctamente con elementos iniciales.\n\n";

    cout << "Requisito: operator <<\n";
    cout << bt << "\n";

    cout << "Requisito: Forward Iterator (begin, end)\n";
    for (auto it = bt.begin(); it != bt.end(); ++it) {
        cout << it->key << " ";
    }
    cout << "\n\n";

    cout << "Requisito: Backward Iterator (rbegin, rend)\n";
    for (auto it = bt.rbegin(); it != bt.rend(); ++it) {
        cout << it->key << " ";
    }
    cout << "\n\n";

    cout << "Requisito: inorden con variadic\n";
    bt.InOrder(printNodeVariadic, string(", "));
    cout << "\n\n";

    cout << "Requisito: preorden con variadic\n";
    bt.PreOrder(printNodeVariadic, string(", "));
    cout << "\n\n";

    cout << "Requisito: postorden con variadic\n";
    bt.PostOrder(printNodeVariadic, string(", "));
    cout << "\n\n";

    cout << "Requisito: FirstThat con variadic e integracion foreach\n";
    // Buscamos un elemento especifico usando una funcion lambda y argumentos variadic
    auto encontrado = bt.FirstThat([](InfoType& info, char objetivo) {
        return info.key == objetivo;
    }, 'Z');
    
    if (encontrado) {
        cout << "Exito: Elemento encontrado: " << encontrado->key << " con ID " << encontrado->ObjID << "\n\n";
    }

    cout << "Requisito: Move Constructor\n";
    // Forzamos el movimiento del arbol 'bt' hacia 'btMove'
    BNEWTree<char> btMove = std::move(bt);
    cout << "Arbol movido exitosamente. Tamano del nuevo arbol: " << btMove.size() << "\n\n";

    cout << "Requisito: Concurrencia (mutex)\n";
    cout << "Lanzando dos hilos para insertar datos simultaneamente...\n";
    
    // Tarea que ejecutaran los hilos
    auto tareaInsercion = [](BNEWTree<char>& arbol, char inicio, int cantidad) {
        for (int i = 0; i < cantidad; ++i) {
            arbol.Insert(inicio + i, 100 + i);
        }
    };

    // Lanzamos dos hilos que atacaran al mismo arbol al mismo tiempo
    std::thread hilo1(tareaInsercion, std::ref(btMove), 'a', 5); // Inserta a,b,c,d,e
    std::thread hilo2(tareaInsercion, std::ref(btMove), '1', 5); // Inserta 1,2,3,4,5

    hilo1.join();
    hilo2.join();
    
    cout << "Hilos finalizados. Insercion segura completada gracias al std::mutex.\n";
    cout << "Nuevo tamano total del arbol: " << btMove.size() << "\n\n";

    cout << "Requisito: operator >> (Generacion automatica de ID)\n";
    cout << "Escribe un solo caracter (letra o numero) y presiona Enter: ";
    cin >> btMove;
    
    cout << "\nArbol final actualizado:\n";
    cout << btMove << "\n";

    cout << "Requisito: Destructor\n";
    cout << "Al finalizar esta funcion, el destructor virtual limpiara la memoria recursivamente.\n";
}
