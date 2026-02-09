#include <iostream>
#include "containers/lists.h"
#include <utility>
using namespace std;
void ImprimirNum(int &n) { cout << "[" << n << "] "; }
bool EsPar(int &n)       { return n % 2 == 0; }
bool Mult9(T1 &elem){
  return elem%9 == 0;
}
template <typename Q>
    void Print(Q &elem){    cout << elem << ",";     }
void DemoListsCircular(){
    CLinkedListCircular< AscendingTrait<int> > listOriginal;
    listOriginal.push_back(10, 1);
    listOriginal.push_back(20, 2);
    listOriginal.push_back(30, 3);
    cout << "1. Lista Original creada: " << listOriginal;

    // --- PRUEBA 1: COPY CONSTRUCTOR ---
    cout << "\n[TEST COPY] Creando 'listCopy' desde 'listOriginal'..." << endl;
    CLinkedListCircular< AscendingTrait<int> > listCopy = listOriginal; 
    
    cout << "   -> Copia:    " << listCopy;

    cout << "   * Modificamos 'listOriginal' agregando 999..." << endl;
    listOriginal.push_back(999, 0);

    cout << "   -> Original: " << listOriginal;
    cout << "   -> Copia:    " << listCopy;
    
    if(listCopy.getSize() != listOriginal.getSize()) {
        cout << "   [OK] La copia es INDEPENDIENTE (Deep Copy exitosa)." << endl;
    } else {
        cout << "   [ERROR] La copia cambio con la original (Shallow Copy)." << endl;
    }

    // --- PRUEBA 2: MOVE CONSTRUCTOR ---
    cout << "\n[TEST MOVE] Moviendo recursos de 'listCopy' a 'listMoved'..." << endl;
    // Usamos std::move para forzar el Move Constructor
    CLinkedListCircular< AscendingTrait<int> > listMoved = std::move(listCopy);

    cout << "   -> Lista Nueva (Moved): " << listMoved;
    cout << "   -> Lista Vieja (Source): " << listCopy; // Debería estar vacía

    if (listCopy.getSize() == 0 && listMoved.getSize() > 0) {
        cout << "   [OK] El robo de recursos fue exitoso (Move Constructor)." << endl;
    } else {
        cout << "   [ERROR] El movimiento fallo." << endl;
    }
    cout << "=======================================\n" << endl;

    cout << "\n=== TEST INTERNO FOREACH / FIRSTTHAT ===" << endl;
    
    // 1. Crear lista
    CLinkedListCircular< AscendingTrait<int> > milista;
    milista.push_back(10, 1);
    milista.push_back(35, 2);
    milista.push_back(40, 3); // Este es par

    // 2. Prueba FOREACH Interno
    // Nota: Llamamos a milista.Foreach, no al ::Foreach global
    cout << "Lista: ";
    milista.Foreach(&ImprimirNum); 
    cout << endl;

    // 3. Prueba FIRSTTHAT Interno
    // Buscamos el primer número par (debería ser 10 o 40 según orden)
    auto it = milista.FirstThat(&EsPar);
    
    if (it != milista.end()) {
        cout << "FirstThat encontro un PAR: " << *it << endl;
    } else {
        cout << "FirstThat no encontro nada." << endl;
    }
}
