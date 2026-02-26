#include "containers/Tree.h"

void DemoBinaryTree(){
    Titulo("Prueba de Binary Tree");
    CBinaryTree< TreeTraitAscending<int> > arbol;

    Escribir("1. Insertando valores: 50, 30, 70, 20, 40, 60, 80");
    arbol.Insert(50);
    arbol.Insert(30);
    arbol.Insert(70);
    arbol.Insert(20);
    arbol.Insert(40);
    arbol.Insert(60);
    arbol.Insert(80);

    Escribir(" Operator << (Imprime en Inorden automaticamente)");
    cout << arbol << endl;

    Escribir("Iteradores (Forward y Backward)");
    cout << "Forward (begin a end): [ ";
    for(auto it = arbol.begin(); it != arbol.end(); ++it) {
        cout << *it << " ";
    }
    cout << "]" << endl;

    cout << "Backward (rbegin a rend): [ ";
    for(auto it = arbol.rbegin(); it != arbol.rend(); ++it) {
        cout << *it << " ";
    }
    cout << "]" << endl;

    //  RECORRIDOS CON VARIADIC
    Escribir("Recorridos usando Variadic Templates y Lambdas");
    cout << "Preorden  : [ ";
    arbol.Preorden([](int& val){ cout << val << " "; });
    cout << "]" << endl;

    cout << "Postorden : [ ";
    arbol.Postorden([](int& val){ cout << val << " "; });
    cout << "]" << endl;

    // FIRSTTHAT CON VARIADIC
    Escribir(" FirstThat Variadic (Buscar el primer numero > 65)");
    int limite = 65;
    // Le pasamos la funcion lambda y el argumento extra 'limite'
    int* encontrado = arbol.FirstThat([](int& val, int lim) {
        return val > lim;
    }, limite);

    if (encontrado) cout << "-> Encontrado: " << *encontrado << endl;
    else cout << "-> No encontrado." << endl;

    // REMOVE
    Escribir("Remove (Eliminando el 30, que tiene 2 hijos)");
    arbol.Remove(30);
    cout << "Despues de borrar el 30: " << endl;
    cout << arbol << endl;

    //CONSTRUCTORES COPIA Y MOVE
    Escribir("Constructores Copia y Move");

    // Copia
    CBinaryTree< TreeTraitAscending<int> > copia(arbol);
    copia.Insert(999);
    cout << "Original : " << arbol;
    cout << "Copia (+999) : " << copia;

    // Move
    CBinaryTree< TreeTraitAscending<int> > movido(std::move(copia));
    cout << "Movido (recibe datos de la copia) : " << movido;
    cout << "Copia (ahora debe estar vacia) : " << copia;
    cout << endl;

    // READ
    Escribir("Prueba de Lectura (>>)");
    cout << "Ingresa un valor para el arbol: " << endl;
    cin >> arbol;
    cout << "Arbol actualizado: " << arbol << endl;
}
void DemoAVL(){
    Titulo("Prueba de AVL Tree");
    CAVLTree< TreeTraitAscending<int> > AVL;
    Escribir("Insertando valores: 30, 20, 10");
    AVL.Insert(30);
    AVL.Insert(20);
    AVL.Insert(10);
    cout<<AVL<<endl;
    Escribir("Insertando valores: 40, 50 para forzar rotaciones a la rama derecha");
    AVL.Insert(40);
    AVL.Insert(50);
    cout<<AVL<<endl;
    Escribir("Insertando valores: 25 para una rotacion doble");
    AVL.Insert(25);
    cout<<AVL<<endl;
    //eliminacion
    Escribir("Eliminando valores: 30");
    AVL.Remove(30);
    cout<<AVL<<endl;
    Escribir("Eliminando valores: 20");
    AVL.Remove(20);
    cout<<AVL<<endl;
    Escribir("Eliminando valores: 50");
    AVL.Remove(50);
    cout<<AVL<<endl;
    Escribir("Eliminando valores: 40");
    AVL.Remove(40);
    cout<<AVL<<endl;
    Escribir("Eliminando valores: 25");
    AVL.Remove(25);
    cout<<AVL<<endl;
    Escribir("Eliminando valores: 10");
    AVL.Remove(10);
    cout<<AVL<<endl;

}
void Demo23(){
    Titulo("Prueba de Arbol 2-3");
    Arbol23< TreeTraitAscending<char> > arbol23;
    Escribir("Insertando valores: 0-6");
    arbol23.Insert('0');
    arbol23.Insert('1');
    arbol23.Insert('2');
    arbol23.Insert('3');
    arbol23.Insert('4');
    arbol23.Insert('5');
    arbol23.Insert('6');
    cout << arbol23 << endl;
    Escribir("Vemos el mega split al agregar el 7");
    arbol23.Insert('7');
    cout << arbol23 << endl;
    Escribir("Vemos el split al agregar el 8 y 9");
    arbol23.Insert('8');
    arbol23.Insert('9');
    cout << arbol23 << endl;
    Escribir("Vemos el mega split al agregar A, B, C, D");
    arbol23.Insert('A');
    arbol23.Insert('B');
    arbol23.Insert('C');
    arbol23.Insert('D');
    cout<<  arbol23 << endl;
    arbol23.Insert('E');
    arbol23.Insert('F');
    arbol23.Insert('G');
    cout<<  arbol23 << endl;
    arbol23.Insert('H');
    arbol23.Insert('I');
    arbol23.Insert('J');
    cout<<  arbol23 << endl;
    arbol23.Insert('K');
    arbol23.Insert('L');
    arbol23.Insert('M');
    cout<<  arbol23 << endl;
    arbol23.Insert('N');
    arbol23.Insert('O');
    arbol23.Insert('P');
    cout<<  arbol23 << endl;
    arbol23.Insert('Q');
    arbol23.Insert('R');
    arbol23.Insert('S');
    cout<<  arbol23 << endl;
    arbol23.Insert('T');
    arbol23.Insert('U');
    arbol23.Insert('V');
    cout<<  arbol23 << endl;
    arbol23.Insert('W');
    arbol23.Insert('X');
    arbol23.Insert('Y');
    cout<<  arbol23 << endl;
    arbol23.Insert('Z');
    cout<<  arbol23 << endl;

}

void DemoTree(){
        DemoBinaryTree();
        DemoAVL();
        Demo23();
}