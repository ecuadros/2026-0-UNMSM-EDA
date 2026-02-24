#include <iostream>
#include "containers/avls.h"

using namespace std;
using T1 = int;

void DemoAVL() {
    cout << "=== DEMO AVL ===" << endl << endl;
    
   
    // Test : Rotación simple izquierda
    cout << "Test: Rotación simple izquierda" << endl;
    CAVL<AscendingTraitAVL<T1>> avl1;
    
    cout << "Insertando: 10, 20, 30" << endl;
    avl1.insert(10);
    avl1.insert(20);
    avl1.insert(30);
    
    cout << avl1;
    avl1.printTabuladoConAlturas();
    cout << endl;
    
    // Test: Rotación simple derecha
    cout << "Test: Rotación simple derecha" << endl;
    CAVL<AscendingTraitAVL<T1>> avl2;
    
    cout << "Insertando: 30, 20, 10" << endl;
    avl2.insert(30);
    avl2.insert(20);
    avl2.insert(10);
    
    cout << avl2;
    avl2.printTabuladoConAlturas();
    cout << endl;
    
    // Test: Rotación doble Izquierda-Derecha
    cout << "Test: Rotación doble Izquierda-Derecha" << endl;
    CAVL<AscendingTraitAVL<T1>> avl3;
    
    cout << "Insertando: 30, 10, 20" << endl;
    avl3.insert(30);
    avl3.insert(10);
    avl3.insert(20);
    
    cout << avl3;
    avl3.printTabuladoConAlturas();
    cout << endl;
    
    // Test: Rotación doble Derecha-Izquierda
    cout << "Test: Rotación doble Derecha-Izquierda" << endl;
    CAVL<AscendingTraitAVL<T1>> avl4;
    
    cout << "Insertando: 10, 30, 20" << endl;
    avl4.insert(10);
    avl4.insert(30);
    avl4.insert(20);
    
    cout << avl4;
    avl4.printTabuladoConAlturas();
    cout << endl;
    
    // Test: Balanceo con múltiples inserciones
    cout << "Test: Balanceo automático" << endl;
    CAVL<AscendingTraitAVL<T1>> avlGrande;
    
    int valores[] = {50, 25, 75, 10, 30, 60, 80, 5, 15, 27, 55, 1};
    cout << "Insertando: ";
    for (int v : valores) {
        cout << v << " ";
        avlGrande.insert(v);
    }
    cout << endl << endl;
    
    cout << avlGrande;
    cout << "Árbol balanceado:" << endl;
    avlGrande.printTabuladoConAlturas();
    cout << endl;
    
    // Test: Remove con rebalanceo
    cout << "Test: Remove con rebalanceo automático" << endl;
    cout << "Antes de remove(25):" << endl;
    avlGrande.printTabuladoConAlturas();
    
    avlGrande.remove(25);
    cout << "Después de remove(25):" << endl;
    cout << avlGrande;
    avlGrande.printTabuladoConAlturas();
    cout << endl;
    
    // Test: Herencia de métodos
    cout << "Test: Métodos heredados de CBinaryTree" << endl;
    
    cout << "Foreach (heredado): ";
    avlGrande.Foreach([](T1 &val) { cout << val << " "; });
    cout << endl;
    
    cout << "Inorden (heredado): ";
    avlGrande.inorden([](T1 &val) { cout << val << " "; });
    cout << endl;
    
    T1 res;
    if (avlGrande.FirstThat(res, [](T1 &val) { return val > 50; })) {
        cout << "FirstThat > 50 (heredado): " << res << endl;
    }
    cout << endl;
    
    // Test: Iteradores heredados
    cout << "Test 10: Iteradores heredados" << endl;
    cout << "Forward: ";
    for (auto it = avlGrande.begin(); it != avlGrande.end(); ++it) {
        cout << *it << " ";
    }
    cout << endl;
    
    cout << "Backward: ";
    for (auto it = avlGrande.rbegin(); it != avlGrande.rend(); ++it) {
        cout << *it << " ";
    }
    cout << endl << endl;
    
    // Test: Constructor copia
    cout << "Test: Constructor copia " << endl;
    CAVL<AscendingTraitAVL<T1>> avlCopy = avlGrande;
    cout << "Copia: " << avlCopy << endl;
    
    // Test: Move constructor
    cout << "Test: Move constructor" << endl;
    CAVL<AscendingTraitAVL<T1>> avlMove = std::move(avlCopy);
    cout << "Movido: " << avlMove;
    cout << "Original: " << avlCopy << endl;

    cout << "=== FIN DEMO AVL ===" << endl << endl;
}