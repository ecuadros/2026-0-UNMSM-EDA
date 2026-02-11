#include "containers/queue.h"
#include <iostream>
#include <string>

using namespace std;

void DemoQueue() {
    T1 cantidadMax;
    cout << "--- SIMULADOR DE COLAS (FIFO) ---" << endl;
    cout << "Cuantos registros deseas ingresar en total?: ";
    cin >> cantidadMax;

    Queue<QueueTraits<Registro>> cola(cantidadMax);
    cout << "\nIniciando carga de " << cantidadMax << " registros..." << endl;
    
    for (T1 i = 0; i < cantidadMax; ++i) {
        Registro nuevo;
        cout << "\n[REGISTRO " << (i + 1) << "/" << cantidadMax << "]" << endl;
        cout << "   > Referencia: ";
        cin >> nuevo.id;
        cout << "   > VALOR : ";
        cin >> nuevo.url;
        cola.enqueue(nuevo);
        cout << "[OK] Encolado correctamente." << endl;
    }

    cout << "ESTADO ACTUAL DE LA COLA: " << cola << endl;

    if (!cola.isEmpty()) {
        cout << "\n[PROCESANDO]" << endl;
        Registro sacado;
        cola.dequeue(sacado); 
        cout << "Elemento procesado y fuera de cola:" << endl;
        cout << "   ID: " << sacado.id << " | URL: " << sacado.url << endl;
    }
    cout << "\n[FINAL] Cola resultante: " << cola << endl;
}