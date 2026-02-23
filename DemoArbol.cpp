#include<iostream>
#include"containers/binarytree.h"
#include"general/types.h"
using namespace std;

void imprimirDato(T1& v) {
    cout << v << " ";
}

void demoarbol() {
    CBinaryTree<TreeTraitAscending<T1>> tree;

    tree >> 50 >> 30 >> 70 >> 20 >> 40 >> 60 >> 80;
    cout << "Arbol completo: " << tree <<   endl;
    cout << "Preorden: ";
    tree.preorder(imprimirDato);
    cout << "\nPostorden: ";
    tree.postorder(imprimirDato);
    cout <<   endl;

    T1* encontrado = tree.FirstThat([](T1 v, T1 target) { return v > target; }, 65);
    if(encontrado)   cout << "Primero mayor a 65: " << *encontrado <<   endl;
    cout << "Iterador Forward: ";
    for(auto it = tree.begin(); it != tree.end(); ++it) {
          cout << *it << " ";
    }
      cout << "\nBorrando el 30...";
    tree.Remove(30);
      cout << "\nArbol tras borrar: " << tree <<   endl;
    {
        CBinaryTree<TreeTraitAscending<T1>> treeCopia = tree;
          cout << "Copia del arbol: " << treeCopia <<   endl;
    } 

    T1 valorUsuario;
    cout << "\nIngrese un valor para buscar si existe uno mayor en el arbol: ";
    cin >> valorUsuario;
    T1* buscado = tree.FirstThat([](T1 v, T1 target) { 
        return v > target; 
    }, valorUsuario);
    
    if (buscado) 
        cout << "El primer valor encontrado mayor a " << valorUsuario << " es: " << *buscado << endl;
    else 
        cout << "No se encontro ningun valor mayor a " << valorUsuario << " en el arbol." << endl;

}