#include <assert.h>
#include <iostream>
#include <fstream>
#include <utility>      // std::move
#include <stdexcept>    // std::out_of_range
using namespace std;

#include "containers/circulardoublelinkedlist.h"
#include "variadic-util.h"
#include "foreach.h"

using T1 = int;

namespace {
// -----------------------------
// helpers de impresión / tests
// -----------------------------
template <typename Q>
void PrintList(Q &elem){
    cout << elem << ",";
}

// Predicados para FirstThat
static bool Mult7_CDC(T1 &x){
    return x % 7 == 0;
}
static bool GreaterThan_CDC(T1 &x, T1 limit){
    return x > limit;
}
static bool DivisibleBy_CDC(T1 &x, T1 d){
    return d != 0 && (x % d == 0);
}

// Para Foreach (miembro)
static void AddConst_CDC(T1 &x, T1 k){
    x += k;
}
} // namespace

void DemoCircularDoubleLinkedLists(){
    cout << "\n=== DemoCCircularDoubleLinkedList started ===\n";

    // ------------------------------------------------------------
    // [A] Base: Insert + push_back + operator<< + archivo (<<)
    // ------------------------------------------------------------
    cout << "\n[A] Base: Insert + push_back + operator<< + archivo\n";
    CCircularDoubleLinkedList< AscendingTrait<T1> > a;
    a.Insert(20,5);
    a.Insert(30,3);
    a.push_back(40,4); // push_back engancha al final (independiente del orden)
    cout << "Ready to print a:\n" << a << endl;

    ofstream of("cirdouble_list_base.txt");
    of << a;
    of.close();
    cout << "cirdouble_list_base.txt written\n";

    // ------------------------------------------------------------
    // [B] Destructor: prueba por scope
    // ------------------------------------------------------------
    cout << "\n[B] Destructor: prueba por scope\n";
    cout << "Antes del scope\n";
    {
        CCircularDoubleLinkedList< AscendingTrait<T1> > tmp;
        tmp.Insert(20,5);
        tmp.Insert(30,3);
        tmp.push_back(40,4);
        cout << "Dentro del scope: " << tmp << endl;
    }
    cout << "Despues del scope (si llegas aqui, destructor OK)\n";

    // ------------------------------------------------------------
    // [C] clear(): vaciar y no fallar en double clear
    // ------------------------------------------------------------
    cout << "\n[C] clear(): debe vaciar y no fallar doble\n";
    CCircularDoubleLinkedList< AscendingTrait<T1> > c;
    c.Insert(10,1);
    c.Insert(20,2);
    cout << "Antes clear: " << c;
    c.clear();
    cout << "Despues clear: " << c;
    c.clear(); // double clear: no debe fallar
    cout << "Despues doble clear: " << c;

    // ------------------------------------------------------------
    // [D] Copy constructor: deep copy
    // ------------------------------------------------------------
    cout << "\n[D] Copy constructor: b(a) debe ser deep copy\n";
    CCircularDoubleLinkedList< AscendingTrait<T1> > b(a);
    cout << "Original a: " << a;
    cout << "Copy b(a):  " << b;
    a.push_back(99,9);
    cout << "After change a: " << a;
    cout << "b should be same: " << b;

    // ------------------------------------------------------------
    // [E] Copy assignment: operator=(const&)
    // ------------------------------------------------------------
    cout << "\n[E] Test operator= (assign)\n";
    CCircularDoubleLinkedList< AscendingTrait<T1> > e1;
    e1.Insert(1,1); e1.Insert(2,2);
    cout << "Before assign:\n";
    cout << "a: " << a;
    cout << "e1:" << e1;
    e1 = a;
    cout << "After e1 = a:\n";
    cout << "a: " << a;
    cout << "e1:" << e1;
    a.push_back(77,7);
    cout << "After change a:\n";
    cout << "a: " << a;
    cout << "e1 should be same: " << e1;

    cout << "Self-assignment test (a = a)\n";
    a = a;
    cout << "a: " << a;

    // ------------------------------------------------------------
    // [F] Move constructor + Move assignment
    // ------------------------------------------------------------
    cout << "\n[F] Test Move Constructor + Move Assignment\n";
    CCircularDoubleLinkedList< AscendingTrait<T1> > msrc;
    msrc.Insert(10,1); msrc.Insert(20,2); msrc.Insert(30,3);
    cout << "Before move, msrc: " << msrc;

    CCircularDoubleLinkedList< AscendingTrait<T1> > mdst(std::move(msrc));
    cout << "After move ctor, mdst: " << mdst;
    cout << "After move ctor, msrc should be empty: " << msrc;

    CCircularDoubleLinkedList< AscendingTrait<T1> > m2;
    m2.Insert(1,1); m2.Insert(2,2);
    cout << "Before move assign, m2: " << m2;
    m2 = std::move(mdst);
    cout << "After move assign, m2: " << m2;
    cout << "After move assign, mdst should be empty: " << mdst;

    // ------------------------------------------------------------
    // [G] operator[] con excepción
    // ------------------------------------------------------------
    cout << "\n[G] Test operator[]\n";
    CCircularDoubleLinkedList< AscendingTrait<T1> > idx;
    idx.Insert(10,1);
    idx.Insert(20,2);
    idx.Insert(30,3);
    cout << "Before: " << idx;
    cout << "idx[0] = " << idx[0] << endl;
    cout << "idx[1] = " << idx[1] << endl;
    cout << "idx[2] = " << idx[2] << endl;

    idx[1] = 99;
    cout << "After idx[1]=99: " << idx;

    try{
        cout << idx[99] << endl;
    }catch(const out_of_range &ex){
        cout << "Caught out_of_range OK: " << ex.what() << endl;
    }

    // ------------------------------------------------------------
    // [H] Traits (Ascending / Descending) usando Insert()
    // ------------------------------------------------------------
    cout << "\n[H] Test Traits (Ascending/Descending)\n";
    CCircularDoubleLinkedList< AscendingTrait<T1> > asc;
    asc.Insert(20,1);
    asc.Insert(10,2);
    asc.Insert(30,3);
    cout << "Ascending (expected 10,20,30): " << asc;

    CCircularDoubleLinkedList< DescendingTrait<T1> > desc;
    desc.Insert(20,1);
    desc.Insert(10,2);
    desc.Insert(30,3);
    cout << "Descending (expected 30,20,10): " << desc;

    // ------------------------------------------------------------
    // [I] Forward iterator: begin/end + ::Foreach + range-for
    // ------------------------------------------------------------
    cout << "\n[I] Test Forward Iterator (begin/end)\n";
    CCircularDoubleLinkedList< AscendingTrait<T1> > it;
    it.Insert(20,1);
    it.Insert(10,2);
    it.Insert(30,3);

    cout << "List: " << it;
    cout << "Foreach(begin,end): ";
    ::Foreach(it.begin(), it.end(), &PrintList<T1>);
    cout << endl;

    cout << "Range-for: ";
    for(auto &x : it) cout << x << ",";
    cout << endl;

    // ------------------------------------------------------------
    // [I2] Backward iterator: rbegin/rend (doble + circular)
    // ------------------------------------------------------------
    cout << "\n[I2] Test Backward Iterator (rbegin/rend)\n";
    cout << "Foreach(rbegin,rend): ";
    ::Foreach(it.rbegin(), it.rend(), &PrintList<T1>);
    cout << endl;

    // ------------------------------------------------------------
    // [J] FirstThat (variadic, como profe)
    // ------------------------------------------------------------
    cout << "\n[J] Test FirstThat (variadic, como profe)\n";
    CCircularDoubleLinkedList< AscendingTrait<T1> > ft;
    ft.Insert(10,2);
    ft.Insert(14,4);
    ft.Insert(20,1);
    ft.Insert(30,3);
    cout << "List: " << ft;

    auto i1 = ft.FirstThat(&Mult7_CDC);
    if(i1 != ft.end()) cout << "First mult of 7: " << *i1 << endl;

    auto i2 = ft.FirstThat(&GreaterThan_CDC, 25);
    if(i2 != ft.end()) cout << "First > 25: " << *i2 << endl;

    auto i3 = ft.FirstThat(&DivisibleBy_CDC, 5);
    if(i3 != ft.end()) cout << "First divisible by 5: " << *i3 << endl;

    auto i4 = ft.FirstThat([](T1 &x, T1 target){ return x == target; }, 10);
    if(i4 != ft.end()) cout << "Found 10 with lambda: " << *i4 << endl;

    // ------------------------------------------------------------
    // [K] Foreach (miembro) - modifica elementos
    // ------------------------------------------------------------
    cout << "\n[K] Foreach (member)\n";
    CCircularDoubleLinkedList< AscendingTrait<T1> > fx;
    fx.Insert(10,1);
    fx.Insert(20,2);
    fx.Insert(30,3);
    cout << "Before Foreach(AddConst,+5): " << fx;
    fx.Foreach(&AddConst_CDC, 5);
    cout << "After  Foreach(AddConst,+5): " << fx;

    // ------------------------------------------------------------
    // [L] operator>> (leer) + move assignment “commit”
    // ------------------------------------------------------------
    cout << "\n[L] operator>> (read from file)\n";
    CCircularDoubleLinkedList< AscendingTrait<T1> > read;
    ifstream inf("cirdouble_list_base.txt");
    if(!inf){
        cout << "No se pudo abrir cirdouble_list_base.txt\n";
    }else{
        inf >> read;
        cout << "Read cirdouble_list_base.txt -> read: " << read;
    }

    cout << "\n=== DemoCCircularDoubleLinkedList finished ===\n";
}
