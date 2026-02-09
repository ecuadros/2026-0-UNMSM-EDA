#include <iostream>
#include "containers/lists.h"
#include <utility>
using namespace std;
using T1 = int;
bool Mult8(T1 &elem){
  return elem%8 == 0;
}
template <typename Q>
    void Print(Q &elem){    cout << elem << ",";     }
void DemoLists(){
    CLinkedList< AscendingTrait<T1> > l1;
    l1.push_back(10, 6);
    l1.Insert(20, 4);
    l1.Insert(20, 3);
    l1.Insert(40, 3);
    l1.Insert(70,57);
    cout << l1 << endl;
    ::Foreach(l1.begin(),  l1.end(),  &Print<T1>);
    cout<<endl;
    auto iter = l1.FirstThat( &Mult8 ); 
    if( iter != l1.end() )
    {   cout << "El primer multiplo de 8 es: " << *iter << endl; }
    l1[3]=50;
    ::Foreach(l1.begin(),  l1.end(),  &Print<T1>);

}