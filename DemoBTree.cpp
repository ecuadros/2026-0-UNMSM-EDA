//#include <iostream.h>
//#include <time.h>
//#include <stdlib.h>
//#include <string>
//#include <iostream>
//#include "containers/BTree.h"

//const char * keys="CDAMPIWNBKEHOLJYQZFXVRTSGU";
//const char * keys1 = "D1XJ2xTg8zKL9AhijOPQcEowRSp0NbW567BUfCqrs4FdtYZakHIuvGV3eMylmn";
//const char * keys2 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
//const char * keys3 = "DYZakHIUwxVJ203ejOP9Qc8AdtuEop1XvTRghSNbW567BfiCqrs4FGMyzKLlmn";

//const int BTreeSize = 3;
//void DemoBTree(){
       //int i;
       //BTree <char> bt (BTreeSize);
       //for (i = 0; keys1[i]; i++)
       //{
               //cout<<"Inserting "<<keys1[i]<<endl;
               //bt.Insert(keys1[i], i*i);
               //bt.Print(cout);
       //}
       //bt.Print(cout);
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
//}









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

#include <iostream>
#include <fstream>
using namespace std;

#include "containers/BTree.h"
#include "variadic-util.h"

using T1 = int;

template <typename Q>
void Print(Q &elem){ cout << elem << ","; }

bool EsMayor20(tagObjectInfo<int, long> &info, int, void *){ return info.key > 20; }

void DemoBTree() {
    BTree<T1> bt;

    bt.Insert(30, 1);
    bt.Insert(10, 2);
    bt.Insert(50, 3);
    bt.Insert(20, 4);
    bt.Insert(40, 5);
    bt.Insert(60, 6);
    bt.Insert(15, 7);

    cout << bt << endl;

    // forward iterator
    ::ForEach(bt.begin(), bt.end(), &Print<T1>); cout << endl;

    // backward iterator
    ::ForEach(bt.rbegin(), bt.rend(), &Print<T1>); cout << endl;

    // inorder variadic
    bt.Inorder([](tagObjectInfo<T1, long> &info, void *) {
        cout << info.key << ",";
    }, nullptr); cout << endl;

    // preorder variadic
    bt.Preorder([](tagObjectInfo<T1, long> &info, void *) {
        cout << info.key << ",";
    }, nullptr); cout << endl;

    // postorder variadic
    bt.Postorder([](tagObjectInfo<T1, long> &info, void *) {
        cout << info.key << ",";
    }, nullptr); cout << endl;

    // search
    auto id = bt.Search(20);
    cout << "Search(20): id=" << id << endl;

    // operator>> (persistencia)
    ofstream of("btree.txt");
    of << bt;
    of.close();

    // remove
    bt.Remove(20, 4);
    cout << bt << endl;

    // move constructor
    BTree<T1> bt2(std::move(bt));
    cout << bt2 << endl;
}