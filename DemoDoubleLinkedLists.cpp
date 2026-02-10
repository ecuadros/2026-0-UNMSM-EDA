#include <iostream>
#include "containers/lists.h"
#include "containers/doublelinkedlist.h"
using namespace std;

using T1=int;


void PrintDouble(int &val){
    cout << val << " ";
}

void MultiplyBy2(int &val){
    val *= 2;
}

void DemoDoubleLinkedList(){

    cout  << endl;
    cout << "  DEMO: LISTA DOBLE ENLAZADA " << endl;
    cout << endl << endl;
    
    // 1. Crear lista con Insert ordenado (Ascending)
    cout << "1. Crear lista con Insert (Ascendente):" << endl;
    CDoubleLinkedList<DoubleAscendingTrait<int>> ld1;
    ld1.Insert(30, 3);
    ld1.Insert(10, 1);
    ld1.Insert(20, 2);
    ld1.Insert(50, 5);
    ld1.Insert(40, 4);
    cout << ld1 << endl;
    
    // 2. Push back y push front
    cout << "2. Push back y push front:" << endl;
    CDoubleLinkedList<DoubleAscendingTrait<int>> ld2;
    ld2.push_back(20, 2);
    ld2.push_back(30, 3);
    ld2.push_front(10, 1);  
    ld2.push_front(5, 0);   
    cout << ld2 << endl;
    
    // 3. Lista Descendente
    cout << "3. Lista Descendente con Insert:" << endl;
    CDoubleLinkedList<DoubleDescendingTrait<int>> ld3;
    ld3.Insert(10, 1);
    ld3.Insert(30, 3);
    ld3.Insert(20, 2);
    ld3.Insert(40, 4);
    cout << ld3 << endl;
    
    // 4. Constructor Copia
    cout << "4. Constructor Copia:" << endl;
    CDoubleLinkedList<DoubleAscendingTrait<int>> ld4(ld1);
    cout << "Lista original: " << ld1;
    cout << "Lista copiada:  " << ld4 << endl;
    
    // 5. Forward Iterator (adelante)
    cout << "5. Forward Iterator (begin -> end):" << endl;
    cout << "Elementos: ";
    for(auto it = ld1.begin(); it != ld1.end(); ++it){
        cout << *it << " ";
    }
    cout << endl << endl;
    
    // 6. Backward Iterator (atras) - 
    cout << "6. Backward Iterator (rbegin -> rend):" << endl;
    cout << "Elementos en reversa: ";
    for(auto it = ld1.rbegin(); it != ld1.rend(); ++it){
        cout << *it << " ";
    }
    cout << endl << endl;
    
    // 7. Operador []
    cout << "7. Acceso con operator[]:" << endl;
    cout << "Elemento [0]: " << ld1[0] << endl;
    cout << "Elemento [2]: " << ld1[2] << endl;
    cout << "Elemento [4]: " << ld1[4] << endl;
    ld1[2] = 99;
    cout << "Despues de modificar [2] a 99: " << ld1 << endl;
    
    // 8. Foreach
    cout << "8. Foreach - Imprimir elementos:" << endl;
    cout << "Elementos: ";
    ld1.Foreach(PrintDouble);
    cout << endl << endl;
    
    cout << "9. Foreach - Multiplicar por 2:" << endl;
    ld1.Foreach(MultiplyBy2);
    cout << "Despues de multiplicar: " << ld1 << endl;
    
    // 10. FirstThat
    cout << "10. FirstThat - Buscar elemento > 100:" << endl;
    int *pFound = ld1.FirstThat([](int val){ return val > 100; });
    if(pFound)
        cout << "Encontrado: " << *pFound << endl;
    else
        cout << "No encontrado" << endl;
    cout << endl;
    
    cout << "11. FirstThat - Buscar elemento == 20:" << endl;
    pFound = ld1.FirstThat([](int val){ return val == 20; });
    if(pFound)
        cout << "Encontrado: " << *pFound << endl;
    else
        cout << "No encontrado" << endl;
    cout << endl;
    
    // 12. Move Constructor
    cout << "12. Move Constructor:" << endl;
    CDoubleLinkedList<DoubleAscendingTrait<T1>> ld5(std::move(ld4));
    cout << "Lista movida: " << ld5;
    cout << "Lista original (vacia): " << ld4 << endl;
    
    // 13. getSize
    cout << "13. Tamanio de las listas:" << endl;
    cout << "ld1.getSize() = " << ld1.getSize() << endl;
    cout << "ld2.getSize() = " << ld2.getSize() << endl;
    cout << "ld5.getSize() = " << ld5.getSize() << endl << endl;
    
 
    
    cout << endl;
    cout << "    FIN DEMO DOUBLELINKEDLIST   " << endl;
    cout <<  endl << endl;
}
