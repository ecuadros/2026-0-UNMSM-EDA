//#include <iostream.h>
#include <time.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include "containers/BTree.h"

//const char * keys="CDAMPIWNBKEHOLJYQZFXVRTSGU";
const char * keys1 = "D1XJ2xTg8zKL9AhijOPQcEowRSp0NbW567BUfCqrs4FdtYZakHIuvGV3eMylmn";
const char * keys2 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
const char * keys3 = "DYZakHIUwxVJ203ejOP9Qc8AdtuEop1XvTRghSNbW567BfiCqrs4FGMyzKLlmn";

const int BTreeSize = 3;

void DemoBTree() {
    using T1 = int;

    cout << "\n===== BTree Ascendente =====" << endl;
    BTree<TreeTraitAscending<T1>> ascTree;
    ascTree.Insert(50, 0);
    ascTree.Insert(30, 1);
    ascTree.Insert(70, 2);
    ascTree.Insert(20, 3);
    ascTree.Insert(40, 4);
    ascTree.Insert(60, 5);
    ascTree.Insert(80, 6);
    cout << "\nArbol tras Insert (50, 30, 70, 20, 40, 60, 80):" << endl;
    cout << ascTree << endl;

    cout << "Recorrido preorden:" << endl;
    ascTree.preorden([](auto &val) { cout << val.key << " "; });
    cout << endl;

    cout << "Recorrido postorden:" << endl;
    ascTree.postorden([](auto &val) { cout << val.key << " "; });
    cout << endl;

    cout << "\nEliminando 30:" << endl;
    ascTree.Remove(30, 1);
    cout << ascTree << endl;

    cout << "Buscando 70: ObjID = " << ascTree.Search(70) << endl;

    BTree<TreeTraitAscending<T1>> ascCopy(ascTree);
    cout << "\nCopy constructor:" << endl;
    cout << ascCopy << endl;

    BTree<TreeTraitAscending<T1>> ascMoved(move(ascTree));
    cout << "Move constructor (tamaño original: " << ascTree.size() << "):" << endl;
    cout << "Nuevo arbol:" << endl;
    cout << ascMoved << endl;

    auto it = ascMoved.FirstThat([](auto &val) { return val.key > 50; });
    if (it != ascMoved.end())
        cout << "FirstThat (primer valor > 50): (" << it->key << " : " << it->ObjID << ")" << endl;

    cout << "\nIterador forward:" << endl;
    for (auto it = ascMoved.begin(); it != ascMoved.end(); ++it)
        cout << "(" << it->key << " : " << it->ObjID << ") ";
    cout << endl;

    cout << "Iterador backward:" << endl;
    for (auto it = ascMoved.rbegin(); it != ascMoved.rend(); ++it)
        cout << "(" << it->key << " : " << it->ObjID << ") ";
    cout << endl;

    cout << "\nAltura: " << ascMoved.height() << endl;
    cout << "Orden:  " << ascMoved.GetOrder() << endl;
    cout << "Tamaño: " << ascMoved.size()     << endl;

    cout << "\n===== BTree Descendente =====" << endl;
    BTree<TreeTraitDescending<T1>> descTree;
    descTree.Insert(50, 0);
    descTree.Insert(30, 1);
    descTree.Insert(70, 2);
    descTree.Insert(20, 3);
    descTree.Insert(40, 4);
    cout << "\nArbol tras Insert (50, 30, 70, 20, 40):" << endl;
    cout << descTree << endl;

    cout << "Eliminando 50:" << endl;
    descTree.Remove(50, 0);
    cout << descTree << endl;

    BTree<TreeTraitDescending<T1>> descCopy(descTree);
    cout << "Copy constructor:" << endl;
    cout << descCopy << endl;

    BTree<TreeTraitDescending<T1>> descMoved(move(descTree));
    cout << "Move constructor (tamaño original: " << descTree.size() << "):" << endl;
    cout << "Nuevo arbol:" << endl;
    cout << descMoved << endl;

    cout << "\nIterador forward:" << endl;
    for (auto it = descMoved.begin(); it != descMoved.end(); ++it)
        cout << "(" << it->key << " : " << it->ObjID << ") ";
    cout << endl;

    cout << "Iterador backward:" << endl;
    for (auto it = descMoved.rbegin(); it != descMoved.rend(); ++it)
        cout << "(" << it->key << " : " << it->ObjID << ") ";
    cout << endl;
}


// void DemoBTree(){
//        int i;
//        BTree <char> bt (BTreeSize);
//        for (i = 0; keys1[i]; i++)
//        {
//                //cout<<"Inserting "<<keys1[i]<<endl;
//                bt.Insert(keys1[i], i*i);
//                //bt.Print(cout);
//        }
//        bt.Print(cout);
//        for (i = 0; keys2[i]; i++)
//        {
//                cout << "Searching " << keys2[i] << " ";
//                long ObjID = bt.Search(keys2[i]);
//                if( ObjID != -1 )
//                        cout << "Found " << keys2[i] << " ID = " << ObjID << endl;
//                else
//                        cout <<"Not found!" << keys2[i] << endl;
//        }

//        cout.flush();
//        for (i = 0; keys3[i]; i++)
//        {
//                cout << "Removing " << keys3[i] << " ";
//                if( bt.Remove(keys3[i], -1) )
//                        cout << keys3[i] << " removed !" << endl;
//                else
//                        cout <<"Not found!" << keys3[i] << endl;
//                bt.Print(cout);
//        }
//        bt.Print(cout);
//        cout.flush();










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
