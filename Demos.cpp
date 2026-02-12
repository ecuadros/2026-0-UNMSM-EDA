#include <iostream>
#include "containers/stack.h"


using namespace std;
using T1 = int;

void DemoStack(){

    // Push()
 
    CStack< StackTrait<T1, std::less<T1>> > s1;

    s1.push(1);
    s1.push(-1);
    s1.push(5);
    s1.push(8);
    s1.push(78);

    cout << s1 << endl << endl;

    // Top()

    cout << "Elemento en el top: " << s1.top() << endl << endl;

    // Pop()

    s1.pop();
    cout << "Tras pop():  ";
    cout << s1 << endl;
    cout << "Nuevo top: " << s1.top() << endl << endl;

    // getSize() y empty()
    
    cout << "Size = " << s1.getSize() << endl;
    cout << "¿Está vacío? " << (s1.empty() ? "Sí" : "No") << endl << endl;

    // Copy constructor 

    CStack< StackTrait<T1, std::less<T1>> > s2 = s1;

    cout << "s1:  " << s1 << endl;
    cout << "s2 (copia):  " << s2 << endl;

    s2.pop();

    cout << "s1 (intacto):  " << s1 << endl;
    cout << "s2 (modificado):  " << s2 << endl << endl;

    // Move constructor
    
    CStack< StackTrait<T1, std::less<T1>> > s3 = std::move(s1);

    cout << "s1 tras move:  " << s1 << endl;
    cout << "s3 (recursos de s1):  " << s3 << endl << endl;


    // Clear()
  
    s3.clear();

    cout << "s3 tras clear:  " << s3 << endl;
    cout << "¿Está vacío? " << (s3.empty() ? "Sí" : "No") << endl << endl;

    // Operator>>
   
    CStack< StackTrait<T1, std::less<T1>> > s5;

    cin >> s5;

    cout << "Stack ingresado:  ";
    cout << s5 << endl << endl;

}

