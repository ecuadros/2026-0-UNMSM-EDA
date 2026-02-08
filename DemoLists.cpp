#include <iostream>
#include "containers/lists.h"

using namespace std;
using T1 = int;
template <typename T>
void  Mult7(T &elem){
  if(elem%7 == 0)
  { cout<< elem <<" "  ;} 
}
template <typename Q>
    void Print(Q &elem){    cout << elem << ",";     }
void DemoLists(){
    CLinkedList< AscendingTrait<T1> > l1;
    l1.push_back(10, 6);
    l1.Insert(20, 4);
    l1.Insert(20, 3);
    l1.Insert(30, 3);
    l1.Insert(70,57);
    cout << l1 << endl;
    ::Foreach(l1.begin(),  l1.end(),  &Print<T1>);
    cout<<endl;
    ::Foreach(l1.begin(),l1.end(),  &Mult7<T1>);

}