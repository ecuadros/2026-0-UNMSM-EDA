#include "containers/avl.h"

#include <cassert>
#include <fstream>
#include <vector>
#include <iostream>
#include <iomanip>

using std::vector;

using Traits = TreeTraitAscending<T1>;
using AVL = CAVLTree<Traits>;

static std::ofstream logFile("avltree_tests.log");

static void printVector(const vector<int> &v, const std::string &label) {
    std::cout << "  " << label << ": [ ";
    for (size_t i = 0; i < v.size(); ++i) {
        std::cout << v[i];
        if (i < v.size() - 1) std::cout << ", ";
    }
    std::cout << " ]\n";
}

void TestAVLInsertionBalance() {
    std::cout << "  [1/6] Prueba_BalanceInserccion\n";
    AVL tree;

    std::cout << "        Insertando caso LL: 30, 20, 10\n";
    for (T1 v : {30, 20, 10}) {
        tree.Insert(v, v);
        AVL::validateTreeBalance(tree);
    }
    std::cout << "        Arbol rebalanceado y validado\n";

    std::cout << "        Insertando caso LR: 25, 28, 27\n";
    for (T1 v : {25, 28, 27}) {
        tree.Insert(v, v);
        AVL::validateTreeBalance(tree);
    }
    std::cout << "        Arbol rebalanceado y validado\n";

    std::cout << "        Insertando mas valores: 5, 4, 3, 2, 1\n";
    for (T1 v : {5, 4, 3, 2, 1}) {
        tree.Insert(v, v);
        AVL::validateTreeBalance(tree);
    }
    std::cout << "        Arbol rebalanceado y validado\n";
    std::cout << "        [OK] PASADA\n\n";

    logFile << "TestAVLInsertionBalance: PASADA\n";
}

void TestAVLRemovalBalance() {
    std::cout << "  [2/6] Prueba_BalanceEliminacion\n";
    AVL tree;

    std::cout << "        Construyendo arbol AVL completo\n";
    std::cout << "        Insertando: 50, 30, 70, 20, 40, 60, 80, 10, 25, 35, 45, 55, 65, 75, 85\n";
    for (T1 v : {50, 30, 70, 20, 40, 60, 80, 10, 25, 35, 45, 55, 65, 75, 85}) {
        tree.Insert(v, v);
    }
    AVL::validateTreeBalance(tree);
    std::cout << "        Arbol validado\n";

    std::cout << "        Eliminando nodos: 10, 25, 30, 70, 50\n";
    for (T1 v : {10, 25, 30, 70, 50}) {
        tree.remove(v);
        AVL::validateTreeBalance(tree);
        std::cout << "        Eliminado " << v << ", arbol rebalanceado y validado\n";
    }
    std::cout << "        [OK] PASADA\n\n";

    logFile << "TestAVLRemovalBalance: PASADA\n";
}

void TestAVLInorderTraversal() {
    std::cout << "  [3/6] Prueba_RecorridoInorden\n";
    AVL tree;

    std::cout << "        Insertando en orden aleatorio: 50, 25, 75, 10, 30, 60, 80, 5, 15, 27, 55, 65, 77, 90\n";
    for (T1 v : {50, 25, 75, 10, 30, 60, 80, 5, 15, 27, 55, 65, 77, 90}) {
        tree.Insert(v, v);
    }

    vector<T1> inorder;
    tree.inorderTraversal([&inorder](T1 &v) { inorder.push_back(v); });

    std::cout << "        Verificando que el recorrido inorden esta ordenado\n";
    for (size_t i = 1; i < inorder.size(); ++i) {
        assert(inorder[i - 1] < inorder[i]);
    }
    printVector(inorder, "Recorrido inorden ordenado");
    std::cout << "        [OK] PASADA\n\n";

    logFile << "TestAVLInorderTraversal: PASADA\n";
}

void TestAVLDuplicateInsertions() {
    std::cout << "  [4/6] Prueba_InserccionesDuplicadas\n";
    AVL tree;

    std::cout << "        Insertando: 50, 30, 70, 20, 40\n";
    for (T1 v : {50, 30, 70, 20, 40}) {
        tree.Insert(v, v);
    }

    std::cout << "        Insertando duplicados: 50, 30\n";
    tree.Insert(50, 100);
    tree.Insert(30, 200);

    AVL::validateTreeBalance(tree);
    std::cout << "        Arbol validado con duplicados\n";

    vector<T1> inorder;
    tree.inorderTraversal([&inorder](T1 &v) { inorder.push_back(v); });
    printVector(inorder, "Contenido final del arbol");
    std::cout << "        [OK] PASADA\n\n";

    logFile << "TestAVLDuplicateInsertions: PASADA\n";
}

void TestAVLLargeDataset() {
    std::cout << "  [5/6] Prueba_ConjuntoDatosGrande\n";
    AVL tree;

    std::cout << "        Insertando 100 valores (1-100)\n";
    for (T1 v = 1; v <= 100; ++v) {
        tree.Insert(v, v);
    }

    AVL::validateTreeBalance(tree);
    std::cout << "        Arbol validado tras 100 inserciones\n";

    std::cout << "        Eliminando 50 valores (1-50)\n";
    for (T1 v = 1; v <= 50; ++v) {
        tree.remove(v);
        if (v % 10 == 0) {
            AVL::validateTreeBalance(tree);
            std::cout << "        Validado en eliminacion " << v << "\n";
        }
    }

    AVL::validateTreeBalance(tree);
    std::cout << "        Validacion final pasada\n";
    std::cout << "        [OK] PASADA\n\n";

    logFile << "TestAVLLargeDataset: PASADA\n";
}

void TestAVLComplexScenario() {
    std::cout << "  [6/6] Prueba_EscenarioComplejo\n";
    AVL tree;

    std::cout << "        Construyendo estructura compleja\n";
    std::cout << "        Insertando: 100, 50, 150, 25, 75, 125, 175, ...\n";
    vector<T1> insertOrder = {100, 50, 150, 25, 75, 125, 175, 12, 37, 62, 87, 112, 137, 162, 187};
    for (T1 v : insertOrder) {
        tree.Insert(v, v);
    }

    AVL::validateTreeBalance(tree);
    std::cout << "        Arbol validado\n";

    std::cout << "        Eliminando: 100, 50, 150, 25, 75\n";
    vector<T1> removeOrder = {100, 50, 150, 25, 75};
    for (T1 v : removeOrder) {
        tree.remove(v);
        AVL::validateTreeBalance(tree);
        std::cout << "        Eliminado " << v << ", validado\n";
    }

    vector<T1> remaining;
    tree.inorderTraversal([&remaining](T1 &v) { remaining.push_back(v); });

    std::cout << "        Verificando que los valores restantes estan ordenados\n";
    for (size_t i = 1; i < remaining.size(); ++i) {
        assert(remaining[i - 1] < remaining[i]);
    }
    printVector(remaining, "Valores restantes");
    std::cout << "        [OK] PASADA\n\n";

    logFile << "TestAVLComplexScenario: PASADA\n";
}

void DemoAVLTree() {
    std::cout << "\n";
    TestAVLInsertionBalance();
    TestAVLRemovalBalance();
    TestAVLInorderTraversal();
    TestAVLDuplicateInsertions();
    TestAVLLargeDataset();
    TestAVLComplexScenario();

    logFile << "\n=== Todas las Pruebas de ArbolAVL PASADAS ===\n";
    logFile.flush();
}
