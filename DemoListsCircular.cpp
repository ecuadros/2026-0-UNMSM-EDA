#include <iostream>
#include "containers/lists.h"
#include <utility>
using namespace std;
bool Mult9(T1 &elem){
  return elem%9 == 0;
}
template <typename Q>
    void Print(Q &elem){    cout << elem << ",";     }
void DemoListCircular(){
    cout << "=======================================" << endl;
    cout << "       DEMO LISTA CIRCULAR             " << endl;
    cout << "=======================================" << endl;

    // 1. Usamos la clase Circular
    CLinkedListCircular< AscendingTrait<T1> > l1;

    // 2. Insertamos datos (Mismas operaciones que en la lineal)
    l1.push_back(10, 6);
    l1.Insert(20, 4);
    l1.Insert(20, 3);
    l1.Insert(40, 3);
    l1.Insert(90, 57);

    // 3. Imprimimos usando el operator<< de la clase
    cout << "Estado Inicial: " << l1 << endl;

    // 4. Probamos el Foreach EXTERNO
    // GRACIAS al iterador que hicimos, esto NO se cuelga, da 1 sola vuelta.
    cout << "Foreach Externo: ";
    ::Foreach(l1.begin(), l1.end(), &Print<T1>); 
    cout << endl;

    // 5. Probamos FirstThat
    // Buscará el 40 (que es múltiplo de 8)
    auto iter = l1.FirstThat( &Mult9 ); 
    if( iter != l1.end() )
    {   
        cout << "El primer multiplo de 8 es: " << *iter << endl; 
    }
    else 
    {
        cout << "No se encontro multiplo de 8." << endl;
    }

    // 6. Probamos el Operador [] (Modificación)
    // Vamos a cambiar el elemento en el índice 3 (el 40) por un 50
    cout << "Modificando indice 3 (valor 40) a 50..." << endl;
    l1[3] = 50;

    // 7. Verificamos el cambio final
    cout << "Lista Final:     ";
    ::Foreach(l1.begin(), l1.end(), &Print<T1>);
    cout << endl << endl;
}