#include <iostream>
#include "containers/lists.h"

using namespace std;
using T1 = int;
namespace {
    bool Mult9(T1 &elem){
      return elem % 9 == 0;
    }

    template <typename Q>
    void Print(Q &elem){    
        cout << elem << ",";     
    }
}
void DemoDoubleListCircular(){
    CDoubleLinkedListCircular<AscendingTrait <T1> > l1;
   
    cout << " DOUBLE LINKED LIST (CIRCULAR) " << endl;
    // 1. LISTA ASCENDENTE 
    cout << "Ingreso en lista Ascendente (Insert )" << endl; 
    l1.Insert(270, 4);
    l1.Insert(20, 3);
    l1.Insert(40, 3);
    l1.Insert(90, 57);
    l1.Insert(10, 6);
    cout << " L1: " << l1;
    cout<< "Ingreso sin importar orden (Push Back)" << endl;
    l1.push_back(17, 8);
    cout << " L1: " << l1;
    cout<<endl; 
    // 2. CONSTRUCTOR COPIA 
    cout << "Constructor Copia (L2 copia de L1)" << endl;
    CDoubleLinkedListCircular<AscendingTrait<T1> > l2 = l1;
    cout << "Modificando L1 " << endl;
    l1.Insert(999, 0); 
    cout << "L1 (Original): " << l1;
    cout << "L2 (Copia):    " << l2;
    cout<<endl;
    // 3. CONSTRUCTOR MOVE 
    cout << "Constructor Move (L3 roba a L2)" << endl;
    CDoubleLinkedListCircular< AscendingTrait<T1> > l3 = std::move(l2);
    cout << "L2 (Vacia): " << l2; 
    cout << "L3 (Nueva): " << l3; 
    cout<<endl;
    // 4. FOREACH Y FIRSTTHAT 
    cout << "Algoritmos Externos (Foreach & FirstThat)" << endl;
    cout << "Foreach: ";
    cout << endl;
    cout << "Forward Iterator "<<endl;
    ::Foreach(l3.begin(), l3.end(), &Print<T1>);
    cout << endl;
    cout << "Foreach: ";
    cout << endl;
    cout << "Backward Iterator "<<endl;
    ::Foreach(l3.rbegin(), l3.rend(), &Print<T1>);
    cout << endl;
    auto it = ::FirstThat(l3.begin(), l3.end(), &Mult9);
    if (it != l3.end()) { 
        cout << "FirstThat (Multiplo 9): " << *it << endl;
    } else {
        cout << "FirstThat: No encontrado." << endl;
    }
    cout<<endl;
    // 5. OPERATOR >> 
    cout << "Operator >> | Escribe un numero y su ref para agregar en el L3 (Insert)  :";
    cin >> l3;
    cout << "L3 Actualizada: " << l3;
    cout<<endl;
    // 6. LISTA DESCENDENTE 
    cout << "Lista Descendente (Nuevo Trait)" << endl;
    CDoubleLinkedListCircular< DescendingTrait<T1> > DSC;
    DSC.Insert(100, 10);
    DSC.Insert(50, 20);  
    DSC.Insert(10, 34); 
    cout << "Descendente: " << DSC;
    cout<<endl;
    // Acceso por índice []
    cout << "Elemento indice [2]: " << DSC[2] << endl;
    cout<<endl;
    
    

}