#include <iostream>
#include "containers/queue.h"

using namespace std;

void DemoQueue(){
    
    CQueue<Trait1<T1>> cola1;

    cout << " Insertando elementos (Push): 10, 20, 30..." << endl;
    cola1.push(10);
    cola1.push(20);
    cola1.push(30);

    cout << " Cola actual: " << cola1 << endl;

    cout << " El frente es: " << cola1.front() << endl;
    cout <<  " El final es: " << cola1.back() << endl;

    cola1.pop();
    cout << " Hicimos pop(). Nuevo frente: " << cola1.front() << endl;
    cout << " Size actual: " << cola1.size() << endl;

    cout << endl << " Prueba de Lectura desde Archivo " << endl;

    ofstream archivoDatos("datos_cola.txt");
    archivoDatos << "100 200 500"; 
    archivoDatos.close();

    CQueue<Trait1<T1>> colaArchivo;
    ifstream archivoEntrada("datos_cola.txt");
    if (archivoEntrada.is_open()) {
        archivoEntrada >> colaArchivo; 
        archivoEntrada.close();
        cout << " Exito, cargando. Resultado:" << endl;
        cout << colaArchivo;
    }

    cout << endl << " Prueba Constructor Copia " << endl;
    CQueue<Trait1<T1>> colaCopia(cola1);

    cout << " Modificamos la copia (Push 999)..." << endl;
    colaCopia.push(999);

    cout << " Original (Back): " << cola1.back() << " (Size: " << cola1.size() << ")" << endl;
    cout << " Copia (Back): " << colaCopia.back() << " (Size: " << colaCopia.size() << ")" << endl;

    cout << endl << " Prueba Move Constructor  " << endl;
    CQueue<Trait1<T1>> colaMovida(move(cola1));

    cout << " ColaMovida Front: " << colaMovida.front() << endl;
    cout << " Original vacia? " << (cola1.empty() ? "SI" : "NO") << endl;

    CQueue<Trait1<T1>> colaUsuario;
    cout << endl << " Ingrese numeros para la cola (Termine con una letra): ";

    cin >> colaUsuario;

    cin.clear(); 
    string basura; getline(cin, basura);

    cout << " Cola leida del teclado: " << colaUsuario << endl;

    ofstream archivoUser("cola_usuario.txt");
    if (archivoUser.is_open()) {
        archivoUser << colaUsuario;
        archivoUser.close();
        cout << " Guardado exitosamente en 'cola_usuario.txt'" << endl;
    }

}

