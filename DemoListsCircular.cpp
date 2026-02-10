#include <iostream>
#include "containers/lists.h"
#include <utility>

using namespace std;
using T1 = int;

// --- NAMESPACE ANONIMO (Funciones privadas para este archivo) ---
namespace {
    bool Mult9(T1 &elem){
      return elem % 9 == 0;
    }

    template <typename Q>
    void Print(Q &elem){    
        cout << elem << ",";     
    }
}

void DemoListsCircular(){
    cout << " LINKED LIST (CIRCULAR) " << endl;
    // 1. LISTA DESCENDENTE 
    cout << "Lista Descendente (Insert )" << endl;
    CLinkedListCircular< DescendingTrait<T1> > l1;  
    l1.Insert(20, 4);
    l1.Insert(20, 23);
    l1.Insert(40, 3);
    l1.Insert(70, 57);
    cout << "Estado L1: " << l1; 
    cout<< "Ingreso sin orden (Pushback)" <<endl;
    l1.push_back(10, 6);
    l1.push_back(17, 8);
    cout << "Estado L1: " << l1; 
    // 2. CONSTRUCTOR COPIA 
    cout << "Constructor Copia (L2 copia de L1)" << endl;
    CLinkedListCircular< DescendingTrait<T1> > l2 = l1;
    cout << "Modificando L1 " << endl;
    l1.Insert(999, 0); 
    cout << "L1 (Original): " << l1;
    cout << "L2 (Copia):    " << l2;
    // 3. CONSTRUCTOR MOVE 
    cout << "Constructor Move (L3 roba a L2)" << endl;
    CLinkedListCircular< DescendingTrait<T1> > l3 = std::move(l2);
    cout << "L2 (Vacia): " << l2; 
    cout << "L3 (Nueva): " << l3; 
    // 4. FOREACH Y FIRSTHAT 
    cout << "Algoritmos Externos (Foreach & FirstThat)" << endl;
    cout <<"Foreach : ";
    cout <<"Forward Iterator : ";
    ::Foreach(l3.begin(), l3.end(), &Print<T1>);
    cout << endl;
    auto it = ::FirstThat(l3.begin(), l3.end(), &Mult9);
    if (it != l3.end()) { 
        cout << "FirstThat (Multiplo 9): " << *it << endl;
    } else {
        cout << "FirstThat: No encontrado." << endl;
    }
    // 5. OPERATOR >> 
    cout << "Operator >> (Escribe un numero y su ref): ";
    cin >> l3;
    cout << "L3 Actualizada: " << l3;
    // 6. LISTA ASCENDENTE (Cambio de Trait)
    cout << "Lista Ascendente (Nuevo Trait)" << endl;
    CLinkedListCircular< AscendingTrait<T1> > ASC;
    ASC.Insert(100, 18);
    ASC.Insert(50, 20);  
    ASC.Insert(10, 30); 
    cout << "L_Ascendente: " << ASC;
    // Acceso por índice []
    cout << "Elemento indice [1]: " << ASC[1] << endl;
    cout<<endl;
}