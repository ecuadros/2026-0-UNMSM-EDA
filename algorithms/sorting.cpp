#include <iostream>
#include "sorting.h"

using namespace std;

// Bubble  

template <typename T>
void SortContainer(T *vect, ContainerRange n){
    // BurbujaRecursivo(vect, n, &::Mayor<T>); 
    // cout << "Array ordenado Ascendente:\n";
    // PrintArray(vect, n, cout);

    // BurbujaRecursivo(vect, n, &::Menor<T>);
    // cout << "Array ordenado Descendente:\n";
    // PrintArray(vect, n, cout);
    // cout << endl;
}
void DemoBurbuja(){
    // cout << "DemoBurbuja \n";
    // int arr1[] = {5, 2, 8, 15, 1, 9, 4, 7, 3, 6};
    // auto n = sizeof(arr1) / sizeof(arr1[0]);
    // SortContainer(arr1, n);

    // string arr2[] = {"Hola", "que", "tal", "como", "estas", "yo", "bien", "hasta", "luego", "amigo"};
    // n = sizeof(arr2) / sizeof(arr2[0]);
    // SortContainer(arr2, n);
}

// ContainerRange particionar(ContainerElemType* arr, ContainerRange first, ContainerRange last, CompFunc pComp) {
//     auto pivote = arr[last];  // Pivote es el elemento de referencia
//     auto i = (first - 1);

//     for (auto j = first; j <= last - 1; j++) {
//         if (arr[j] == pivote) ++i;
//         if ( (*pComp)(arr[j], pivote) ){
//             ++i; intercambiar(arr[i], arr[j]);
//         }
//     }
//     intercambiar(arr[i + 1], arr[last]);
//     return (i + 1);
// }




void DemoQuickSort() {
    // cout << "DemoQuickSort \n";
    // // ContainerElemType arr[] = {5, 2, 8, 15, 1, 9, 4, 7, 3, 6};
    // string arr[] = {"Hola", "que", "tal", "como", "estas", "yo", "bien", "hasta", "luego", "amigo"};
    // auto n = sizeof(arr) / sizeof(arr[0]);

    // QuickSort(arr, 0, n - 1, &Mayor);
    // cout << "Arreglo ordenado Ascendente: \n";
    // PrintArray(arr, n, cout);

    // QuickSort(arr, 0, n - 1, &Menor);
    // cout << "Arreglo ordenado Descendente: \n";
    // PrintArray(arr, n, cout);

    // cout << endl;
}

// Función para mezclar dos subarreglos ordenados de arr[].

// left es para el índice izquierdo y right es para el índice derecho del
// sub-array de arr a ordenar
// void MergeSort( ContainerElemType* arr, 
//                 ContainerRange const begin, 
//                 ContainerRange const end,
//                 CompFunc pComp) {
//     if (begin >= end)
//         return; // Return recursivamente

//     auto mid = begin + (end - begin) / 2;
//     MergeSort(arr, begin, mid, pComp);
//     MergeSort(arr, mid + 1, end, pComp);
//     Merge(arr, begin, mid, end, pComp);
// }

void DemoMergeSort(){
    // cout << "DemoMergeSort" << endl;
    // // ContainerElemType arr[] = {5, 2, 8, 15, 1, 9, 4, 7, 3, 6};
    // string arr[] = {"Hola", "que", "tal", "como", "estas", "yo", "bien", "hasta", "luego", "amigo"};

    // auto n = sizeof(arr) / sizeof(arr[0]);

    // MergeSort(arr, 0, n-1, &Mayor);
    // cout << "Array ordenado Ascendente:\n";
    // PrintArray(arr, n, cout);

    // MergeSort(arr, 0, n-1, &Menor);
    // cout << "Array ordenado Descendente:\n";
    // PrintArray(arr, n, cout);

    // cout << endl;
}

void DemoSorting(){
    DemoBurbuja();
    // QuickSort añadido
    // DemoQuickSort();
    // DemoMergeSort();
}

template <typename T>
void Print(T &p1, T &p2){
    cout << "p1: " << p1 << " p2: " << p2 << endl;
}

template <>
void Print(char &p1, char &p2){
    cout << "char1: " << p1 << " char2: " << p2 << endl;
}

void DemoTemplates(){
    int    a = 5, b = 8;
    double x = 20, y = 30;
    string str1="CS", str2 = "UNI";
    char   c1 = 'X', c2 = 'Y';

    Print(a, b);
    Print(x, y);
    Print(str1, str2);
    Print(c1, c2);
}

