#include <iostream>
#include "containers/lists.h"

using namespace std;


void PrintDoubleCircular(int &val){
    cout << val << " ";
}

void AddTen(int &val){
    val += 10;
}

void DemoDoubleLinkedListCircular(){

    cout << endl;
    cout << " DEMO: LISTA DOBLE ENLAZADA CIRCULAR " << endl;
    cout << endl << endl;
    
    // 1. Crear lista circular con push_back
    cout << "1. Crear lista circular con push_back:" << endl;
    CDoubleLinkedListCircular<DoubleCircularAscendingTrait<int>> ldc1;
    ldc1.push_back(10, 1);
    ldc1.push_back(20, 2);
    ldc1.push_back(30, 3);
    cout << ldc1 << endl;
    
    // 2. Verificar circularidad
    cout << "2. Verificar si es circular:" << endl;
    cout << "¿Es circular? " << (ldc1.IsCircular() ? "SI" : "NO") << endl;
    cout << "Explicacion: Ultimo.Next = Primero, Primero.Prev = Ultimo" << endl << endl;
    
    // 3. Push front y push back
    cout << "3. Combinar push_front y push_back:" << endl;
    CDoubleLinkedListCircular<DoubleCircularAscendingTrait<int>> ldc2;
    ldc2.push_back(30, 3);
    ldc2.push_back(40, 4);
    ldc2.push_front(20, 2);
    ldc2.push_front(10, 1);
    cout << ldc2 << endl;
    
    // 4. Insert ordenado (Ascendente)
    cout << "4. Insert ordenado (Ascendente):" << endl;
    CDoubleLinkedListCircular<DoubleCircularAscendingTrait<int>> ldc3;
    ldc3.Insert(50, 5);
    ldc3.Insert(30, 3);
    ldc3.Insert(40, 4);
    ldc3.Insert(10, 1);
    ldc3.Insert(20, 2);
    cout << ldc3 << endl;
    
    // 5. Insert ordenado (Descendente)
    cout << "5. Insert ordenado (Descendente):" << endl;
    CDoubleLinkedListCircular<DoubleCircularDescendingTrait<int>> ldc4;
    ldc4.Insert(10, 1);
    ldc4.Insert(50, 5);
    ldc4.Insert(30, 3);
    ldc4.Insert(20, 2);
    ldc4.Insert(40, 4);
    cout << ldc4 << endl;
    
    // 6. Forward Iterator (una vuelta completa hacia adelante)
    cout << "6. Forward Iterator (una vuelta adelante):" << endl;
    cout << "Elementos: ";
    for(auto it = ldc1.begin(); it != ldc1.end(); ++it){
        cout << *it << " ";
    }
    cout << endl << endl;
    
    // 7. Backward Iterator (una vuelta completa hacia atras)
    cout << "7. Backward Iterator (una vuelta atras):" << endl;
    cout << "Elementos en reversa: ";
    for(auto it = ldc1.rbegin(); it != ldc1.rend(); ++it){
        cout << *it << " ";
    }
    cout << endl << endl;
    
    // 8. Operador []
    cout << "8. Acceso con operator[]:" << endl;
    cout << "Elemento [0]: " << ldc1[0] << endl;
    cout << "Elemento [1]: " << ldc1[1] << endl;
    cout << "Elemento [2]: " << ldc1[2] << endl;
    ldc1[1] = 99;
    cout << "Despues de modificar [1] a 99: " << ldc1 << endl;
    
    // 9. Foreach (recorre una vuelta completa)
    cout << "9. Foreach - Imprimir (una vuelta):" << endl;
    cout << "Elementos: ";
    ldc1.Foreach(PrintDoubleCircular);
    cout << endl << endl;
    
    cout << "10. Foreach - Sumar 10 a cada elemento:" << endl;
    ldc1.Foreach(AddTen);
    cout << "Despues de sumar 10: " << ldc1 << endl;
    
    // 11. FirstThat
    cout << "11. FirstThat - Buscar elemento > 100:" << endl;
    int *pFound = ldc1.FirstThat([](int val){ return val > 100; });
    if(pFound)
        cout << "Encontrado: " << *pFound << endl;
    else
        cout << "No encontrado" << endl;
    cout << endl;
    
    cout << "12. FirstThat - Buscar elemento == 20:" << endl;
    pFound = ldc1.FirstThat([](int val){ return val == 20; });
    if(pFound)
        cout << "Encontrado: " << *pFound << endl;
    else
        cout << "No encontrado" << endl;
    cout << endl;
    
    // 13. Constructor Copia
    cout << "13. Constructor Copia:" << endl;
    CDoubleLinkedListCircular<DoubleCircularAscendingTrait<int>> ldc5(ldc3);
    cout << "Lista original: " << ldc3;
    cout << "Lista copiada:  " << ldc5 << endl;
    
    // 14. Move Constructor
    cout << "14. Move Constructor:" << endl;
    CDoubleLinkedListCircular<DoubleCircularAscendingTrait<int>> ldc6(std::move(ldc5));
    cout << "Lista movida: " << ldc6;
    cout << "Lista original (vacia): " << ldc5 << endl;
    
    // 15. getSize
    cout << "15. Tamanio de las listas:" << endl;
    cout << "ldc1.getSize() = " << ldc1.getSize() << endl;
    cout << "ldc3.getSize() = " << ldc3.getSize() << endl;
    cout << "ldc6.getSize() = " << ldc6.getSize() << endl << endl;
    

    cout << endl;
    cout << " FIN DEMO DOUBLELINKEDLIST CIRCULAR" << endl;
    cout  << endl << endl;
}
