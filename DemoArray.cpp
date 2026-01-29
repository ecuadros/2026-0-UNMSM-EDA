#include <assert.h>
#include <iostream>
#include <fstream>
using namespace std;
#include "containers/array.h"
#include "compareFunc.h"
#include "variadic-util.h"

using T1 = int;

void DemoArray() {
    const int N = 5;
    //  /using 
    CArray< Trait1<T1> > arr1(N);
    for (auto i = 0; i < N; ++i)
      arr1.push_back(i * 10);
    cout << "First part finished" << endl;
    arr1[3] = 43;
    arr1.push_back(80);
    arr1.push_back(75);

    // test for [] operator with out of bounds index
    arr1[9] = 10;
    arr1[12] = 15;

    cout << "Ready to print" << endl;
    cout << arr1 << endl;

    ofstream of("array.txt");
    of << arr1;
    of.close();
    cout << "DemoArray finished." << endl;

    arr1.sort(&Mayor);
    cout << arr1 << endl;

    arr1.sort(&Menor);
    cout << arr1 << endl;

    arr1.Foreach(&Suma<T1>, 4);
    cout << arr1 << endl;

    arr1.Foreach(&Mult<T1>, 3);
    cout << arr1 << endl;

    cout << "Antes del Foreach externo, getSize()=" << arr1.getSize() << endl;
    Foreach(arr1, &IncBasico<T1>);
    cout << arr1 << endl;
    cout << "Terminó" << endl;

    using T2 = string;
    CArray< Trait1<T2> > arr2(N);
    arr2[2] = "UNMSM";
    arr2[4] = "FC";
    cout << arr2 <<endl;
    arr2.Foreach(&Suma<T2>, "Enero");
    cout << arr2 <<endl;

    int x = Test(5, 3, 2);
    cout << x << endl;
    auto r = Test(1, 2, 3.4, 4, 5, 6, 7);
    cout << r << endl;

    auto str = Test(string("Hola"), string("FC"), string("UNMSM") );
    cout << str << endl;
}

