#ifndef __SORTING_H__
#define __SORTING_H__
#include "../util.h"
#include "../compareFunc.h"

// void BurbujaClasico(ContainerElemType* arr,
                    // ContainerRange n, CompFunc pComp);


// Bubble
template <typename T, typename CompFunc>
void BurbujaRecursivo(T& arr, 
                      ContainerRange n,
                      CompFunc pComp,
                      ContainerRange start = 0) {
    auto end = start;
    if (n <= 1)
        return;
    for (auto j = 0; j < n - 1; ++j)
        if ( pComp(arr[++end], arr[start]) )
            intercambiar(arr[start], arr[end]);
    BurbujaRecursivo(arr, n - 1, pComp, start + 1);
}

void DemoBurbuja();

// ContainerRange  particionar(ContainerElemType* arr, ContainerRange first, ContainerRange last, CompFunc pComp);
// void QuickSort  (ContainerElemType* arr, ContainerRange first, ContainerRange last, CompFunc pComp);
// void DemoQuickSort();

// void Merge(ContainerElemType* arr, const ContainerRange left, const ContainerRange mid, const ContainerRange right, CompFunc pComp); 
// void MergeSort(ContainerElemType* arr, const ContainerRange begin, const ContainerRange end, CompFunc pComp);
// void DemoMergeSort();

void DemoSorting();
void DemoTemplates();

#endif // __SORTING_H__