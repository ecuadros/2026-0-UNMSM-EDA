#include "Stack.h"
#include <iostream>
#include <string>

using namespace std;

void DemoStack() {
    T1 cantidadMax;
    cout << "Cuantos registros deseas ingresar como maximo? ";
    cin >> cantidadMax;

    Stack<StackTraits<Registro>> historial(cantidadMax); 
    string opcion;

    cout << "Usted definira manualmente la referencia (ID) y el valor (URL)." << endl;

    while (historial.getSize() < (T1)cantidadMax) {
        cout << "\n[REGISTRO #" << (historial.getSize() + 1) << "]" << endl;
        cout << "Desea agregar un registro? (escriba 'si' para continuar o 'salir'): ";
        cin >> opcion;

        if (opcion == "salir") break;
        Registro nuevo;
        cout << "   > Ingrese Referencia Numerica (ID): ";
        cin >> nuevo.id;
        cout << "   > Ingrese Valor (URL): ";
        cin >> nuevo.url;
        historial.push(nuevo);
        
        cout << "\n Registrado: " << nuevo.url << " [Ref: " << nuevo.id << "]" << endl;
        cout << "Pila actual: " << historial << endl;
    }

    if (historial.getSize() >= (T1)cantidadMax) {
        cout << "\n!! Se ha alcanzado el limite de registros definido !!" << endl;
    }
    
    cout << "HISTORIAL COMPLETO INGRESADO: " << historial << endl;
    if (historial.getSize() > 0) {
        cout << "\n[BOTON ATRAS] Ejecutando Pop para recuperar ultimo dato..." << endl;
        
        Registro sacado;
        historial.pop(sacado); 
        
        cout << "Se elimino de la pila el registro:" << endl;
        cout << "      > Referencia (ID): " << sacado.id << endl;
        cout << "      > Valor (URL)    : " << sacado.url << endl;
    } else {
        cout << "\nNo hay datos para eliminar." << endl;
    }

    cout << "\nFINALIZADO: " << historial << endl;
}