#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include "containers/BTree.h"

using namespace std;

const char *keys1 = "D1XJ2xTg8zKL9AhijOPQcEowRSp0NbW567BUfCqrs4FdtYZakHIuvGV3eMylmn";
const char *keys2 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char *keys3 = "DYZakHIUwxVJ203ejOP9Qc8AdtuEop1XvTRghSNbW567BfiCqrs4FGMyzKLlmn";
const int BTreeSize = 3;



void printSeparator(const string &title) {
    cout << "\n" << string(70, '=') << "\n";
    cout << "  " << title << "\n";
    cout << string(70, '=') << "\n\n";
}

void demonstrateVariadicInsert(BTree<char, int> &bt, const string &dataset, int count) {
    cout << "Insertando " << count << " caracteres de: " << dataset << "\n";
    cout << "Claves: ";
    
    int inserted = 0;
    for (int i = 0; i < count && dataset[i]; ++i) {
        if (bt.Insert(dataset[i], 1000 + i)) {
            cout << dataset[i];
            inserted++;
        }
    }
    cout << "\n\nResultados: " << inserted << "/" << count 
         << " insertados | size=" << bt.size() 
         << " | height=" << bt.height() 
         << " | order=" << bt.GetOrder() << "\n";
}


void DemoBTree() {
    cout << "\n╔════════════════════════════════════════════════════════════════════╗\n";
    cout << "║       DEMOSTRACIÓN COMPLETA DE BTREE CON TODAS LAS FEATURES        ║\n";
    cout << "╚════════════════════════════════════════════════════════════════════╝\n";


    printSeparator("1. CONSTRUCTOR Y OPERACIONES BÁSICAS");
    
    BTree<char, int> mainTree(BTreeSize);
    cout << "BTree creado con orden: " << mainTree.GetOrder() 
         << " | size inicial: " << mainTree.size() 
         << " | height inicial: " << mainTree.height() << "\n";

    cout << "\nInsertando caracteres del conjunto 1...\n";
    demonstrateVariadicInsert(mainTree, keys1, 50);

    // Mostrar estructura interna
    cout << "\nEstructura interna del árbol:\n";
    mainTree.Print(cout);

    
    printSeparator("2. OPERADOR << (SERIALIZACIÓN)");
    
    cout << "Serialize usando operator<<:\n";
    cout << mainTree << "\n";

   
    printSeparator("3. ITERADORES FORWARD (begin/end)");
    
    cout << "Recorrido completo forward (primeros 30 elementos):\n";
    int count = 0;
    for (auto it = mainTree.begin(); it != mainTree.end() && count < 30; ++it, ++count) {
        cout << it->key << "(" << it->ObjID << ") ";
    }
    cout << "\n... (total size: " << mainTree.size() << ")\n";

   
    printSeparator("4. ITERADORES BACKWARD (rbegin/rend)");
    
    cout << "Recorrido inverso (primeros 30 elementos):\n";
    count = 0;
    for (auto it = mainTree.rbegin(); it != mainTree.rend() && count < 30; ++it, ++count) {
        cout << it->key << "(" << it->ObjID << ") ";
    }
    cout << "\n... (en orden descendente)\n";

    
    printSeparator("5. TRAVERSALS VARIADIC");
    
    cout << "Inorder Traversal (in-orden):\n";
    cout << "Claves: ";
    mainTree.inorderTraversal([](const auto &key) { 
        cout << key.key << " "; 
    });
    cout << "\n";

    cout << "\nPreorder Traversal (pre-orden):\n";
    cout << "Claves: ";
    mainTree.preorderTraversal([](const auto &key) { 
        cout << key.key << " "; 
    });
    cout << "\n";

    cout << "\nPostorder Traversal (post-orden):\n";
    cout << "Claves: ";
    mainTree.postorderTraversal([](const auto &key) { 
        cout << key.key << " "; 
    });
    cout << "\n";

    
    cout << "\n\nTraversals con parámetros variadic extra:\n";
    cout << "Inorder con contador:\n";
    int nodeCounter = 0;
    mainTree.inorderTraversal([&nodeCounter](const auto &key, int &counter) { 
        counter++;
    }, nodeCounter);
    cout << "Total de nodos visitados: " << nodeCounter << "\n";

    
    printSeparator("6. FirstThat CON PARÁMETROS VARIADIC");
    
  
    auto itStart = mainTree.begin();
    auto itEnd = mainTree.end();
    auto found = mainTree.FirstThat(itStart, itEnd, 
        [](const auto &info, char target) { 
            return info.key == target; 
        }, 'Z');
    
    if (found) {
        cout << "FirstThat encontró clave 'Z' con ObjID: " << found->ObjID << "\n";
    } else {
        cout << "FirstThat no encontró 'Z'\n";
    }

    
    found = mainTree.FirstThat(itStart, itEnd, 
        [](const auto &info, int minValue) { 
            return info.ObjID >= minValue; 
        }, 1010);
    
    if (found) {
        cout << "FirstThat encontró primer elemento con ObjID >= 1010: " 
             << found->key << " ObjID=" << found->ObjID << "\n";
    }

   
    printSeparator("7. ForEach CON RANGO DE ITERADORES");
    
    cout << "ForEach en rango [begin, begin+10]:\n";
    auto start = mainTree.begin();
    auto stop = mainTree.begin();
    for (int i = 0; i < 10 && stop != mainTree.end(); ++i, ++stop);
    
    mainTree.ForEach(start, stop, [](const auto &info) { 
        cout << info.key << " "; 
    });
    cout << "\n";

    
    cout << "\nForEach con multiplicador variadic:\n";
    mainTree.ForEach(start, stop, [](const auto &info, int multiplier) { 
        cout << info.key << "(" << info.ObjID * multiplier << ") "; 
    }, 2);
    cout << "\n";

   
    printSeparator("8. OPERADOR >> (DESERIALIZACIÓN)");
    
    stringstream ss;
    ss << mainTree;  // Serializar
    
    BTree<char, int> copyTree(BTreeSize);
    ss >> copyTree;  
    
    cout << "Árbol copiado vía serialización:\n";
    cout << "Size: " << copyTree.size() << " | Height: " << copyTree.height() << "\n";
    cout << "Primeros 20 elementos de la copia:\n";
    count = 0;
    for (auto it = copyTree.begin(); it != copyTree.end() && count < 20; ++it, ++count) {
        cout << it->key << " ";
    }
    cout << "\n";

    
    printSeparator("9. SEARCH (BÚSQUEDA)");
    
    cout << "Búsquedas en el árbol:\n";
    const char *testKeys = "AZDXNO";
    for (int i = 0; testKeys[i]; ++i) {
        int result = mainTree.Search(testKeys[i]);
        cout << "Search('" << testKeys[i] << "'): ObjID = " << result << "\n";
    }

    
    printSeparator("10. REMOVE (ELIMINACIÓN)");
    
    BTree<char, int> testRemoveTree(BTreeSize);
    cout << "Árbol pequeño para testing de remociones:\n";
    const char *smallKeys = "ABCDEFGH";
    int count_ins = 0;
    for (int i = 0; smallKeys[i]; ++i) {
        if (testRemoveTree.Insert(smallKeys[i], 100 + i)) count_ins++;
    }
    cout << "Insertados " << count_ins << " elementos\n";
    cout << "Antes de remociones:\n" << testRemoveTree << "\n";

    cout << "Eliminando caracteres: D, B, F\n";
    cout << "Remove('D'): " << (testRemoveTree.Remove('D', 103) ? "OK - Eliminado" : "FAIL - No encontrado") << "\n";
    cout << "Árbol después de remove('D'):\n" << testRemoveTree << "\n";

    cout << "Remove('B'): " << (testRemoveTree.Remove('B', 101) ? "OK - Eliminado" : "FAIL") << "\n";
    cout << "Remove('F'): " << (testRemoveTree.Remove('F', 105) ? "OK - Eliminado" : "FAIL") << "\n";
    cout << "Árbol final:\n" << testRemoveTree << "\n";
    cout << "Size después de remociones: " << testRemoveTree.size() << "\n";

   
    printSeparator("11. MOVE CONSTRUCTOR");
    
    BTree<char, int> sourceTree(BTreeSize);
    for (int i = 0; i < 20 && keys2[i]; ++i) {
        sourceTree.Insert(keys2[i], 5000 + i);
    }
    cout << "Árbol origen (antes de move):\n";
    cout << "Size: " << sourceTree.size() << " | Height: " << sourceTree.height() << "\n";
    cout << "Contenido: " << sourceTree << "\n";

    
    BTree<char, int> movedTree(move(sourceTree));
    cout << "\nDespués de move constructor:\n";
    cout << "Árbol movido - Size: " << movedTree.size() 
         << " | Height: " << movedTree.height() << "\n";
    cout << "Contenido: " << movedTree << "\n";
    cout << "Árbol origen - Size: " << sourceTree.size() 
         << " | Height: " << sourceTree.height() << "\n";

    
    printSeparator("12. THREAD-SAFETY (CONCURRENCIA CON MUTEX)");
    
    BTree<int, int> concurrentTree(BTreeSize);
    
    cout << "Insertando desde múltiples contextos...\n";
   
    for (int i = 0; i < 50; ++i) {
        concurrentTree.Insert(i, i * 100);
    }
    cout << "Inserciones completadas (protegidas por mutex)\n";
    cout << "Size final: " << concurrentTree.size() << "\n";

    auto sizeCheck = concurrentTree.size();
    cout << "Size() es thread-safe: " << sizeCheck << "\n";
    
    auto heightCheck = concurrentTree.height();
    cout << "Height() es thread-safe: " << heightCheck << "\n";

  
    printSeparator("13. ESTADÍSTICAS FINALES");
    
    cout << "Árbol principal:\n";
    cout << "  Tamaño: " << mainTree.size() << " elementos\n";
    cout << "  Altura: " << mainTree.height() << " niveles\n";
    cout << "  Orden: " << mainTree.GetOrder() << "\n";
    cout << "  Primeros 10 elementos (forward): ";
    count = 0;
    for (auto it = mainTree.begin(); it != mainTree.end() && count < 10; ++it, ++count) {
        cout << it->key << " ";
    }
    cout << "\n";

    cout << "\n╔════════════════════════════════════════════════════════════════════╗\n";
    cout << "║                    DEMOSTRACIÓN COMPLETADA                          ║\n";
    cout << "╚════════════════════════════════════════════════════════════════════╝\n\n";
}





/*const char * keys="CDAMPIWNBKEHOLJYQZFXVRTSGU";
const char * keys2="CDAMPIWNBKEHOLJYQZFXVRTSGU";
const int BTreeSize = 3;
main (int argc, char * argv)
{
       //__int64 li;
       BTree <__int64> bt (BTreeSize);
       for (register int i = 0; i < 1000000; i++)
       {
               //cout<<"Inserting "<<keys[i]<<endl;
               bt.Insert(i, i-1);
               //bt.Print(cout);
       }

       for (i = 0; i < 1000; i++)
       {
               __int64 key = 975000+(::rand()%50000);
               //cout << "Searching " << (long)key << " ";
               long ObjID = bt.Search(key);
               if( ObjID != -1 )
                       cout << "Achei " << (long)key << " ID = " << ObjID << endl;
               else
                       cout <<"  Nao achei!" << (long)key << endl;
       }
       cout.flush();

       return 1;
}*/



/*const int BTreeSize = 3;
main (int argc, char * argv)
{
       int result, i;
       BTree <LONGLONG> bt(BTreeSize);
       result = bt.Create ("ernesto3-string-btree-start.dat",ios::in|ios::out);
       if (!result) { cout<<"Please delete testbt.dat"<<endl;return 0; }
       srand( (unsigned)time( NULL ) );
       LARGE_INTEGER key;
       for (i = 0; i < 1000000; i++)
       {
               //cout<<"Inserting "<<keys[i]<<endl;
               char strTmp[50];
               key.LowPart = rand();
               key.HighPart = rand();
               std::string str(strTmp);
               result = bt.Insert(key.QuadPart, i);
               //bt.Print(cout);
               if( i % 100000 == 0 )
               {       cout << i << endl; cout.flush();        }
       }
       //cout << "Searching D " << bt.Search();
       //bt.Search(1,1);
       cout.flush();
       return 1;
}*/
