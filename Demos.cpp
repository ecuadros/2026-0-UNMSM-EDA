#include <iostream>
#include "containers/lists.h"

using namespace std;

template <typename T>
bool Mayor40(const T& val) {
    return val > 40;
}
void DemoLinkedList(){
    CLinkedList< AscendingTrait<T1> > l1;

    l1.Insert(20, 1);
    l1.Insert(30, 2);
    l1.Insert(10, 3);
    l1.Insert(40, 4);
    //resultado [ 10,20,30,40 ] ; ordenada
    cout << "original" << endl;
    cout << l1 << endl;

    //probando copia
    cout<<"Lista Copia"<<endl;
    CLinkedList<AscendingTrait<T1> > l2 = l1;
    cout << l2 << endl;

    //reverso de la lista
    cout << "Reverso" << endl;
    l1.PrintReverse();
try {
    //acceso por indice
    cout<<"Pruebaaaa por acceso de indice:  "<<l1[2]<<endl;
    cout << "Modificando indice 2 a 999" << endl;
    l1[2] = 999;
    cout<<"nueva lista"<<l1;
}   catch(exception &e) {
    cout <<"ERROR: "<< e.what() << endl;
}
    //porbando pushback
    cout<<"PushBack "<<endl;
    l1.push_back(50,5);
    cout<<l1<<endl;
    //fistThat
    cout<<"FirstThat(mayores a 40)"<<endl;
    auto iter = FirstThat(l1, Mayor40<T1>);
    if( iter != l1.end() )
    {   cout << "encontrado: "<<*iter<<endl; }
    else {
        cout<<"no encontrado"<<endl;

    }
    //Constructor copia
    cout<<"Constructor copia"<<endl;
    CLinkedList<AscendingTrait<T1> > l3(l1);
    cout<<"Lista Original: "<<l1<<endl;
    cout<<"Lista Copia: "<<l3<<endl;
    cout<<"Borrar original"<<endl;
    l1.clear();
    cout<<"Lista Original(vacia): "<<l1<<endl;
    cout<<"Lista Copia (intacta): "<<l3<<endl;

    //Move Constructor
    cout<<"Move Constructor"<<endl;
    CLinkedList<AscendingTrait<T1> > l4(std::move(l3));
    cout<<"Lista movida: "<<l4<<endl;
    cout<<"Lista Copia(ahora vacia): "<<l3<<endl;

}

void DemoLinkedListCircular(){
    CCircularList<AscendingTrait<T1> > l1;
    cout<<" "<<endl;
    cout<<"Lista Circular"<<endl;
    l1.Insert(20, 1);
    l1.Insert(30, 2);
    l1.Insert(10, 3);
    l1.Insert(40, 4);
    cout<<l1<<endl;
    l1.push_back(50,5);
    cout<<" "<<endl;
    cout<<"Compobacion de push_back"<<endl;
    cout<<l1<<endl;
    cout<<"verifiacion si realmente estan conectados tanto el ultimo como el primero "<<endl;
    auto vuelta = l1.begin();
    for(size_t i = 0; i < 11; ++i){
        cout<<*vuelta<<" -> ";
        ++vuelta;
    }
    cout<<endl;

}
void DemoLists(){
    DemoLinkedList();
    DemoLinkedListCircular();
}
