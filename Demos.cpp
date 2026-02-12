#include <iostream>
#include <fstream>
#include "containers/lists.h"
#include "containers/stack.h"
using namespace std;

void DemoStack(){
    CStack<Trait1<T1>> pila1;
    
    cout << "Insertando elementos (Push): 10, 20, 30 " << endl;
    pila1.push(10);
    pila1.push(20);
    pila1.push(30);

    cout << "Pila actual: " << pila1 << endl;

    cout << "El tope es: " << pila1.top() << endl;

    pila1.pop();
    cout << " Hicimos pop(). Nuevo tope: " << pila1.top() << endl;
    cout << " Size actual: " << pila1.size() << endl;

    cout << endl << "Prueba de Lectura desde Archivo " << endl;

    ofstream archivoDatos("datos_pila.txt");
    archivoDatos << "100 200 500"; 
    archivoDatos.close();

    CStack<Trait1<T1>> pilaDesdeArchivo;
    ifstream archivoEntrada("datos_pila.txt");
    if (archivoEntrada.is_open()) {
        archivoEntrada >> pilaDesdeArchivo; 
        archivoEntrada.close();
        cout << " Cargando 'datos_pila.txt'. Resultado:" << endl;
        cout << pilaDesdeArchivo << endl;
    }

    cout << " Prueba de Escritura en Archivo " << endl;
    ofstream archivoReporte("reporte_pila.txt");
    if (archivoReporte.is_open()) {
        archivoReporte << pila1; 
        archivoReporte.close();
        cout << "    Pila 1 guardada en 'reporte_pila.txt'." << endl;
    }

    cout << endl << " Prueba Constructor Copia " << endl;
    CStack<Trait1<T1>> pilaCopia(pila1);

    cout << "    Modificamos la copia (Push 999)  " << endl;
    pilaCopia.push(999);

    cout << "    Original (Intacta) -> Top = " << pila1.top() << " (Size: " << pila1.size() << ")" << endl;
    cout << "    Copia (Modificada) -> Top = " << pilaCopia.top() << " (Size: " << pilaCopia.size() << ")" << endl;

    cout << endl << " Prueba Move Constructor..." << endl;
    cout << "    Moviendo recursos de 'Original' a 'PilaMovida'..." << endl;

    CStack<Trait1<T1>> pilaMovida(move(pila1));

    cout << "    PilaMovida Top: " << pilaMovida.top() << endl; 
    
    if (pila1.empty()) {
        cout << "    Original quedo vacia " << endl;
    } else {
        cout << "    ERROR, Original no quedo vacia " << endl;
    }

}

