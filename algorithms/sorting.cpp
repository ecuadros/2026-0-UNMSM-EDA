#include <iostream>
#include <algorithm> // Para std::swap
#include "sorting.h"
#include "containers/array.h"

using namespace std;

// 1. Burbuja Genérica (Sirve para cualquier tipo T)
template <typename T>
void BurbujaMejorada(T* arr, ContainerRange n, bool (*pComp)(const T&, const T&)) {
    for (auto i = 0; i < n - 1; ++i) {
        for (auto j = i + 1; j < n; ++j) {
            if ((*pComp)(arr[j], arr[i])) { // Cambia según la lógica del comparador
                std::swap(arr[i], arr[j]);
            }
        }
    }
}

// 2. Partición para QuickSort (DEBE ESTAR DESCOMENTADA)
ContainerRange particionar(ContainerElemType* arr, ContainerRange first, ContainerRange last, CompFunc pComp) {
    auto pivote = arr[last]; 
    auto i = (first - 1);

    for (auto j = first; j <= last - 1; j++) {
        // Si el elemento actual es "menor" que el pivote según el comparador
        if ((*pComp)(arr[j], pivote)) {
            i++; 
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[last]);
    return (i + 1);
}

// 3. QuickSort (Corregido)
void QuickSort(ContainerElemType* arr, ContainerRange first, ContainerRange last, CompFunc pComp) {
    if (first < last) {
        auto pivot = particionar(arr, first, last, pComp);
        if (pivot > 0) QuickSort(arr, first, pivot - 1, pComp);
        QuickSort(arr, pivot + 1, last, pComp);
    }
}
