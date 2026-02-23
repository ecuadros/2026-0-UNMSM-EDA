#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include "containers/heap.h"

using namespace std;

void DemoHeap() {
    cout << "\n==================== DEMO HEAP ====================\n";

    cout << "\n[1] Construccion de MinHeap<int>\n";
    CHeap<MinHeapTrait<T1>> heapMinA(3);

    heapMinA.Push(18);
    heapMinA.Push(5);
    heapMinA.Push(11);
    heapMinA.Push(2);
    heapMinA.Push(9);

    cout << "heapMinA:\n" << heapMinA << endl;
    cout << "Top de heapMinA: " << heapMinA.top() << endl;

    cout << "\n[2] Pop() sobre MinHeap\n";
    cout << "Pop #1 -> " << heapMinA.Pop() << endl;
    cout << "Pop #2 -> " << heapMinA.Pop() << endl;
    cout << "heapMinA despues de 2 Pop():\n" << heapMinA << endl;
    cout << "Nuevo top: " << heapMinA.top() << endl;

    cout << "\n[3] Construccion de MaxHeap<int>\n";
    CHeap<MaxHeapTrait<T1>> heapMaxA(2);

    heapMaxA.Push(14);
    heapMaxA.Push(30);
    heapMaxA.Push(7);
    heapMaxA.Push(26);
    heapMaxA.Push(19);

    cout << "heapMaxA:\n" << heapMaxA << endl;
    cout << "Top de heapMaxA: " << heapMaxA.top() << endl;

    cout << "\n[4] Copy constructor (MaxHeap)\n";
    CHeap<MaxHeapTrait<T1>> heapMaxB(heapMaxA);
    cout << "heapMaxB (copia de heapMaxA):\n" << heapMaxB << endl;

    cout << "\n[5] Copy assignment (MinHeap)\n";
    CHeap<MinHeapTrait<T1>> heapMinB(1);
    heapMinB.Push(100);
    heapMinB.Push(50);

    cout << "heapMinB antes de asignar:\n" << heapMinB << endl;

    heapMinB = heapMinA;

    cout << "heapMinB despues de copiar heapMinA:\n" << heapMinB << endl;

    cout << "\n[6] Move constructor (MinHeap)\n";
    CHeap<MinHeapTrait<T1>> heapMinC(std::move(heapMinB));
    cout << "heapMinC (movido desde heapMinB):\n" << heapMinC << endl;
    cout << "heapMinB despues del move:\n" << heapMinB << endl;

    cout << "\n[7] Move assignment (MaxHeap)\n";
    CHeap<MaxHeapTrait<T1>> heapMaxC;
    heapMaxC = std::move(heapMaxB);

    cout << "heapMaxC (movido desde heapMaxB):\n" << heapMaxC << endl;
    cout << "heapMaxB despues del move:\n" << heapMaxB << endl;
    cout << "heapMaxB empty()? " << (heapMaxB.empty() ? "SI" : "NO") << endl;

    cout << "\n[8] Lectura desde archivo con operator>>\n";
    ifstream fin("Heap.txt");
    if (!fin.is_open()) {
        ofstream fout("Heap.txt");
        fout << "8 21 4 17 33 9 1 28 12";
        fout.close();
        fin.open("Heap.txt");
    }

    CHeap<MinHeapTrait<T1>> heapArchivoMin;
    fin >> heapArchivoMin;
    fin.close();

    cout << "heapArchivoMin (desde Heap.txt):\n" << heapArchivoMin << endl;
    cout << "Top heapArchivoMin: " << heapArchivoMin.top() << endl;

    fin.open("Heap.txt");
    CHeap<MaxHeapTrait<T1>> heapArchivoMax;
    fin >> heapArchivoMax;
    fin.close();

    cout << "heapArchivoMax (desde Heap.txt):\n" << heapArchivoMax << endl;
    cout << "Top heapArchivoMax: " << heapArchivoMax.top() << endl;

    cout << "\n[9] Heap<string> (MinHeap)\n";
    CHeap<MinHeapTrait<string>> heapStr(4);

    heapStr.Push("lima");
    heapStr.Push("arequipa");
    heapStr.Push("cusco");
    heapStr.Push("piura");
    heapStr.Push("tacna");

    cout << "heapStr:\n" << heapStr << endl;
    cout << "Top heapStr: " << heapStr.top() << endl;
    cout << "Pop heapStr -> " << heapStr.Pop() << endl;
    cout << "heapStr despues de Pop():\n" << heapStr << endl;

}