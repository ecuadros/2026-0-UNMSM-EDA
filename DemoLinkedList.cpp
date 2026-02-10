#include <iostream>
#include "containers/lists.h"

using namespace std;

// Función auxiliar para Foreach
void PrintValue(int &val){
    cout << val << " ";
}

void MultiplyBy2(int &val){
    val *= 2;
}

void DemoLinkedList(){
    
    cout << "   DEMO: LISTA ENLAZADA SIMPLE (LES)" << endl;
    
    // 1. Crear lista y usar Insert (ordenado)

    cout << "1. Crear lista con Insert (ordenado):" << endl;
    CLinkedList<AscendingTrait<T1>> l1;
    l1.Insert(30, 1);
    l1.Insert(10, 1);
    l1.Insert(20, 2);
    l1.Insert(50, 5);
    l1.Insert(40, 4);
    l1.Insert(60, 6);
    cout << l1 << endl;
    
    // 2. Push back
    cout << "2. Agregar al final con push_back:" << endl;
    CLinkedList<AscendingTrait<T1>> l2;
    l2.push_back(10, 1);
    l2.push_back(20, 2);
    l2.push_back(30, 3);
    cout << l2 << endl;
    
    // 3. Constructor Copia
    cout << "3. Constructor Copia:" << endl;
    CLinkedList<AscendingTrait<T1>> l3(l1);
    cout << "Lista original: " << l1;
    cout << "Lista copiada:  " << l3 << endl;
    
    // 4. Operador []
    cout << "4. Acceso con operator[]:" << endl;
    cout << "Elemento en indice [0]: " << l1[0] << endl;
    cout << "Elemento en indice [2]: " << l1[2] << endl;
    
    cout <<  endl;
 

// Recorrer la lista para ver todos los valores
cout << "Recorrer l1 con iteradores: ";
for(auto it = l1.begin(); it != l1.end(); ++it){
    cout << *it << " ";
}
cout << endl;

    
    // 5. Iteradores
    cout << "5. Recorrer con iteradores:" << endl;
    cout << "Elementos: ";
    for(auto it = l1.begin(); it != l1.end(); ++it){
        cout << *it << " ";
    }
    cout << endl << endl;
    
    // 6. Foreach
    cout << "6. Foreach - Imprimir:" << endl;
    cout << "Elementos: ";
    l1.Foreach(PrintValue);
    cout << endl << endl;
    
    cout << "7. Foreach - Multiplicar por 2:" << endl;
    l1.Foreach(MultiplyBy2);
    cout << "Despues de multiplicar: " << l1 << endl;
    
    // 8. FirstThat
    cout << "8. FirstThat - Buscar primer elemento > 100:" << endl;
    int *pFound = l1.FirstThat([](int val){ return val > 100; });
    if(pFound)
        cout << "Encontrado: " << *pFound << endl;
    else
        cout << "No encontrado" << endl;
    cout << endl;
    
    cout << "9. FirstThat - Buscar primer elemento > 50:" << endl;
    pFound = l1.FirstThat([](int val){ return val > 50; });
    if(pFound)
        cout << "Encontrado: " << *pFound << endl;
    else
        cout << "No encontrado" << endl;
    cout << endl;
    
    // 10. Move Constructor
    cout << "10. Move Constructor:" << endl;
    CLinkedList<AscendingTrait<T1>> l4(std::move(l3));
    cout << "Lista movida: " << l4;
    cout << "Lista original (vacia): " << l3 << endl;
    
    // 11. getSize
    cout << "11. Tamanio de la lista:" << endl;
    cout << "l1.getSize() = " << l1.getSize() << endl;
    cout << "l4.getSize() = " << l4.getSize() << endl << endl;
    
   
    cout << " FIN DEMO LINKEDLIST" << endl;

}