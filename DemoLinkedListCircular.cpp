#include <iostream>
#include "containers/lists.h"

using namespace std;


void PrintValueCircular(int &val){
    cout << val << " ";
}

void DemoLinkedListCircular(){
    cout << endl;
    cout << "  DEMO: LISTA ENLAZADA CIRCULAR (LEC)" << endl;
    cout  << endl << endl;
    
    // 1. Crear lista circular
    cout << "1. Crear lista circular con push_back:" << endl;
    CLinkedListCircular<AscendingTraitCircular<T1>> lc1;
    lc1.push_back(10, 1);
    lc1.push_back(20, 2);
    lc1.push_back(30, 3);
    cout << lc1 << endl;
    
    // 2. Verificar circularidad
    cout << "2. Verificar si es circular:" << endl;
    cout << "¿Es circular? " << (lc1.IsCircular() ? "SI" : "NO") << endl << endl;
    
    // 3. Insert ordenado
    cout << "3. Insert ordenado en lista circular:" << endl;
    CLinkedListCircular<AscendingTraitCircular<T1>> lc2;
    lc2.Insert(50, 5);
    lc2.Insert(30, 3);
    lc2.Insert(40, 4);
    lc2.Insert(10, 1);
    lc2.Insert(20, 2);
    cout << lc2 << endl;
    
    // 4. Constructor Copia
    cout << "4. Constructor Copia:" << endl;
    CLinkedListCircular<AscendingTraitCircular<T1>> lc3(lc1);
    cout << "Lista original: " << lc1;
    cout << "Lista copiada:  " << lc3 << endl;
    
    // 5. Operador []
    cout << "5. Acceso con operator[]:" << endl;
    cout << "Elemento en indice [0]: " << lc1[0] << endl;
    cout << "Elemento en indice [1]: " << lc1[1] << endl;
    cout << "Elemento en indice [2]: " << lc1[2] << endl << endl;
   
    // 6. Iteradores (recorre una vuelta completa)
    cout << "6. Recorrer con iteradores (una vuelta):" << endl;
    cout << "Elementos: ";
    for(auto it = lc1.begin(); it != lc1.end(); ++it){
        cout << *it << " ";
    }
    cout << endl << endl;
    
    // 7. Foreach (recorre una vuelta completa)
    cout << "7. Foreach - Imprimir (una vuelta):" << endl;
    cout << "Elementos: ";
    lc1.Foreach(PrintValueCircular);
    cout << endl << endl;
    
    // 8. FirstThat
    cout << "8. FirstThat - Buscar primer elemento > 50:" << endl;
    int *pFound = lc1.FirstThat([](int val){ return val > 50; });
    if(pFound)
        cout << "Encontrado: " << *pFound << endl;
    else
        cout << "No encontrado" << endl;
    cout << endl;
    
    cout << "9. FirstThat - Buscar primer elemento == 10:" << endl;
    pFound = lc1.FirstThat([](int val){ return val == 10; });
    if(pFound)
        cout << "Encontrado: " << *pFound << endl;
    else
        cout << "No encontrado" << endl;
    cout << endl;
    
    // 10. Move Constructor
    cout << "10. Move Constructor:" << endl;
    CLinkedListCircular<AscendingTraitCircular<T1>> lc4(std::move(lc3));
    cout << "Lista movida: " << lc4;
    cout << "Lista original (vacia): " << lc3 << endl;
    
    // 11. getSize
    cout << "11. Tamanio de la lista circular:" << endl;
    cout << "lc1.getSize() = " << lc1.getSize() << endl;
    cout << "lc2.getSize() = " << lc2.getSize() << endl << endl;
    
    cout << endl;
    cout << "   FIN DEMO LINKEDLIST CIRCULAR" << endl;
    cout << endl << endl;
}
