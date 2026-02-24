#include <iostream>
#include <thread>
#include <sstream>
#include "containers/BTree.h"

using T = int;
using Tree = Tree< BTreeTraitAscending<T> >; 

void DemoBTree(){

    std::cout << "DEMO BTREE\n";

    Tree tree;

    // INSERT
    std::cout << "Insertando elementos...\n";

    for(int i=1;i<=10;i++){
        tree.Insert(i, i*100);
    }

    std::cout << "Tree = " << tree << "\n\n";


    // INORDER
    std::cout << "InOrder:\n";

    tree.InOrder([](auto &info, Size level){
        std::cout << "[L" << level << "] "
                  << info.key << " -> " << info.ObjID << "\n";
    });

    std::cout << "\n";

    
    // PREORDER
    std::cout << "PreOrder:\n";

    tree.PreOrder([](auto &info, Size level){
        std::cout << "[L" << level << "] "
                  << info.key << "\n";
    });

    std::cout << "\n";

    // POSTORDER
    std::cout << "PostOrder:\n";

    tree.PostOrder([](auto &info, Size level){
        std::cout << "[L" << level << "] "
                  << info.key << "\n";
    });

    std::cout << "\n";

    // ITERADOR FORWARD
    std::cout << "Forward Iterator:\n";

    for(auto it = tree.begin(); it != tree.end(); ++it){
        std::cout << it->key << ":" << it->ObjID << " ";
    }

    std::cout << "\n\n";

    // ITERADOR BACKWARD
    std::cout << "Backward Iterator:\n";

    for(auto it = tree.rbegin(); it != tree.rend(); ++it){
        std::cout << it->key << ":" << it->ObjID << " ";
    }

    std::cout << "\n\n";

    // FIRST THAT
    std::cout << "FirstThat (key > 5):\n";

    auto ptr = tree.FirstThat([](auto &info, Size){
        return info.key > 5;
    });

    if(ptr)
        std::cout << "Encontrado: "
                  << ptr->key << ":" << ptr->ObjID << "\n\n";

    // COPY CONSTRUCTOR
    std::cout << "Copy Constructor:\n";

    Tree copyTree = tree;

    std::cout << "CopyTree = " << copyTree << "\n\n";

    
    // MOVE CONSTRUCTOR
    std::cout << "Move Constructor:\n";

    Tree moveTree = std::move(copyTree);

    std::cout << "MoveTree = " << moveTree << "\n\n";

}