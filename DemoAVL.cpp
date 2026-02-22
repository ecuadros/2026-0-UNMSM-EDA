#include <iostream>
#include "containers/avl.h"
#include "variadic-util.h"

using namespace std;

// funciones variadic
void ImprimirAVL(int &n){
    cout << n << " ";
}

bool EsMayorQueAVL(int &n, int limite){
    return n > limite;
}

void DemoAVL(){
    cout << endl;
    cout << "========================================" << endl;
    cout << "===== Demo AVL (Auto-balanceado) =======" << endl;
    cout << "========================================" << endl;

    // --- Caso LL: insercion secuencial descendente ---
    cout << endl << "--- Caso LL (rotacion derecha) ---" << endl;
    CAVL< AscendingBTreeTrait<int> > avlLL;
    avlLL.Insert(30, 1);
    avlLL.Insert(20, 2);
    avlLL.Insert(10, 3);  // aqui se produce la rotacion LL
    cout << avlLL;

    // --- Caso RR: insercion secuencial ascendente ---
    cout << endl << "--- Caso RR (rotacion izquierda) ---" << endl;
    CAVL< AscendingBTreeTrait<int> > avlRR;
    avlRR.Insert(10, 1);
    avlRR.Insert(20, 2);
    avlRR.Insert(30, 3);  // aqui se produce la rotacion RR
    cout << avlRR;

    // --- Caso LR: zig-zag izquierda-derecha ---
    cout << endl << "--- Caso LR (rotacion izq-der) ---" << endl;
    CAVL< AscendingBTreeTrait<int> > avlLR;
    avlLR.Insert(30, 1);
    avlLR.Insert(10, 2);
    avlLR.Insert(20, 3);  // aqui se produce la rotacion LR
    cout << avlLR;

    // --- Caso RL: zig-zag derecha-izquierda ---
    cout << endl << "--- Caso RL (rotacion der-izq) ---" << endl;
    CAVL< AscendingBTreeTrait<int> > avlRL;
    avlRL.Insert(10, 1);
    avlRL.Insert(30, 2);
    avlRL.Insert(20, 3);  // aqui se produce la rotacion RL
    cout << avlRL;

    // --- AVL completo con mas elementos ---
    cout << endl << "--- AVL completo ---" << endl;
    CAVL< AscendingBTreeTrait<int> > avl;
    avl.Insert(50, 1);
    avl.Insert(30, 2);
    avl.Insert(70, 3);
    avl.Insert(20, 4);
    avl.Insert(40, 5);
    avl.Insert(60, 6);
    avl.Insert(80, 7);
    avl.Insert(10, 8);
    avl.Insert(25, 9);
    avl.Insert(35, 10);
    avl.Insert(45, 11);
    cout << avl;

    // Inorder con variadic
    cout << endl << "Inorder:   ";
    avl.Inorder(ImprimirAVL);
    cout << endl;

    // Preorder con variadic
    cout << "Preorder:  ";
    avl.Preorder(ImprimirAVL);
    cout << endl;

    // Postorder con variadic
    cout << "Postorder: ";
    avl.Postorder(ImprimirAVL);
    cout << endl;

    // Forward Iterator (inorder)
    cout << "Forward:   ";
    for( auto it = avl.begin(); it != avl.end(); ++it )
        cout << *it << " ";
    cout << endl;

    // Backward Iterator (reverse inorder)
    cout << "Backward:  ";
    for( auto it = avl.rbegin(); it != avl.rend(); ++it )
        cout << *it << " ";
    cout << endl;

    // Remove con rebalanceo
    cout << endl << "--- Remove(20) con rebalanceo ---" << endl;
    avl.Remove(20);
    cout << avl;

    cout << endl << "--- Remove(70) con rebalanceo ---" << endl;
    avl.Remove(70);
    cout << avl;

    // Constructor Copia
    cout << endl << "--- Constructor Copia ---" << endl;
    CAVL< AscendingBTreeTrait<int> > copia(avl);
    cout << "Copia: " << copia;

    // Move Constructor
    cout << endl << "--- Move Constructor ---" << endl;
    CAVL< AscendingBTreeTrait<int> > moved(std::move(copia));
    cout << "Moved: " << moved;
    cout << "Copia (vacio): size = " << copia.getSize() << endl;
}
