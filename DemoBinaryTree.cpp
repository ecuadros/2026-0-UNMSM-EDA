#include <iostream>
#include "containers/binarytrees.h"

using namespace std;
using T1 = int;


template <typename T>
void Print(T &val) {
    cout << val << " ";
}

template <typename T>
void Add(T &val, T amount) {
    val += amount;
}

template <typename T>
bool IsMultiple(T &val, T divisor) {
    return val % divisor == 0;
}

void DemoBinaryTree() {
    cout << "=== DEMO BINARY TREE REFACTORIZADO ===" << endl << endl;
    
    // Test: Árbol Ascendente
    cout << "Test: Árbol Ascendente " << endl;
    CBinaryTree<AscendingTrait<T1>> treeAsc;
    treeAsc.insert(50);
    treeAsc.insert(30);
    treeAsc.insert(70);
    treeAsc.insert(20);
    treeAsc.insert(40);
    treeAsc.insert(60);
    treeAsc.insert(80);
    cout << treeAsc << endl;
    
    // Test: Árbol Descendente
    cout << "Test: Árbol Descendente" << endl;
    CBinaryTree<DescendingTrait<T1>> treeDesc;
    treeDesc.insert(50);
    treeDesc.insert(30);
    treeDesc.insert(70);
    treeDesc.insert(20);
    treeDesc.insert(40);
    treeDesc.insert(60);
    treeDesc.insert(80);
    cout << treeDesc << endl;
    
    // Test: Impresión con tabulación
    cout << "Test: Impresión Tabulada (Ascendente)" << endl;
    treeAsc.printTabulado();
    cout << endl;
    
    // Test: Foreach 
    cout << "Test: Foreach" << endl;
    cout << "Resultado: ";
    treeAsc.Foreach(Print<T1>);
    cout << endl << endl;
    
    // Test: FirstThat 
    cout << "Test: FirstThat (múltiplo de 7)" << endl;
    T1 resultado;
    if (treeAsc.FirstThat(resultado, IsMultiple<T1>, 7)) {
        cout << "Encontrado: " << resultado << endl;
    } else {
        cout << "No encontrado" << endl;
    }
    cout << endl;
    
    // Test: Recorridos variadic 
    cout << "Test: Recorridos variadic " << endl;
    cout << "Inorden: ";
    treeAsc.inorden(Print<T1>);
    cout << endl;
    
    cout << "Preorden: ";
    treeAsc.preorden(Print<T1>);
    cout << endl;
    
    cout << "Postorden: ";
    treeAsc.postorden(Print<T1>);
    cout << endl << endl;
    
    // Test: Iteradores 
    cout << "Test: Iteradores " << endl;
    cout << "Forward: ";
    for (auto it = treeAsc.begin(); it != treeAsc.end(); ++it) {
        cout << *it << " ";
    }
    cout << endl;
    
    cout << "Backward: ";
    for (auto it = treeAsc.rbegin(); it != treeAsc.rend(); ++it) {
        cout << *it << " ";
    }
    cout << endl << endl;
    
  
    // Test: Constructor copia
    cout << "Test: Constructor copia" << endl;
    CBinaryTree<AscendingTrait<T1>> treeCopy = treeAsc;
    cout << "Copia: " << treeCopy << endl;
    
    // Test: Move constructor
    cout << "Test 12: Move constructor" << endl;
    CBinaryTree<AscendingTrait<T1>> treeMove = std::move(treeCopy);
    cout << "Movido: " << treeMove;
    cout << "Original: " << treeCopy << endl;
    
    // Test: Remove VIRTUAL
    cout << "Test: Remove " << endl;
    cout << "Antes:" << endl;
    treeAsc.printTabulado();
    treeAsc.remove(30);
    cout << "Después de remove(30):" << endl;
    treeAsc.printTabulado();
    cout << endl;
    
    // Test 14: Comparación visual
    cout << "Test 14: Comparación Ascendente vs Descendente" << endl;
    CBinaryTree<AscendingTrait<T1>> asc;
    CBinaryTree<DescendingTrait<T1>> desc;
    
    int valores[] = {50, 30, 70, 20, 40};
    for (int v : valores) {
        asc.insert(v);
        desc.insert(v);
    }
    
    cout << "\nAscendente:" << endl;
    asc.printTabulado();
    
    cout << "\nDescendente:" << endl;
    desc.printTabulado();
    
    cout << "=== FIN DEMO BINARY TREE ===" << endl << endl;
}