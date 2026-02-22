#include "containers/binarytree.h"

#include <cassert>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <iomanip>

using std::vector;

static std::ofstream logFile("binarytree_tests.log");

static void printVector(const vector<int> &v, const std::string &label) {
    std::cout << "  " << label << ": [ ";
    for (size_t i = 0; i < v.size(); ++i) {
        std::cout << v[i];
        if (i < v.size() - 1) std::cout << ", ";
    }
    std::cout << " ]\n";
}

void TestBasicInsertion() {
    std::cout << "  [1/8] Prueba_InserccionBasica\n";
    using Traits = TreeTraitAscending<T1>;
    CBinaryTree<Traits> tree;

    std::cout << "        Insertando: 50, 30, 70, 20, 40, 60, 80\n";
    tree.Insert(50, 1);
    tree.Insert(30, 2);
    tree.Insert(70, 3);
    tree.Insert(20, 4);
    tree.Insert(40, 5);
    tree.Insert(60, 6);
    tree.Insert(80, 7);

    vector<T1> inorder;
    tree.inorderTraversal([&inorder](T1 &v) { inorder.push_back(v); });
    
    vector<T1> expected = {20, 30, 40, 50, 60, 70, 80};
    assert((inorder == expected));
    printVector(inorder, "Resultado Inorden");
    std::cout << "        [OK] PASADA\n\n";

    logFile << "TestBasicInsertion: PASADA\n";
}

void TestForwardBackwardIterators() {
    std::cout << "  [2/8] Prueba_IteradoresAdelante_Atras\n";
    using Traits = TreeTraitAscending<T1>;
    CBinaryTree<Traits> tree;

    std::cout << "        Insertando: 5, 3, 7, 2, 4, 6, 8\n";
    for (T1 v : {5, 3, 7, 2, 4, 6, 8}) tree.Insert(v, v);

    vector<T1> forward;
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        forward.push_back(*it);
    }
    vector<T1> expectedFwd = {2, 3, 4, 5, 6, 7, 8};
    assert((forward == expectedFwd));
    printVector(forward, "Iteracion Adelante");

    vector<T1> backward;
    for (auto it = tree.rbegin(); it != tree.rend(); ++it) {
        backward.push_back(*it);
    }
    vector<T1> expectedBwd = {8, 7, 6, 5, 4, 3, 2};
    assert((backward == expectedBwd));
    printVector(backward, "Iteracion Atras");
    std::cout << "        [OK] PASADA\n\n";

    logFile << "TestForwardBackwardIterators: PASADA\n";
}

void TestAllTraversals() {
    std::cout << "  [3/8] Prueba_TodosLosRecorridos\n";
    using Traits = TreeTraitAscending<T1>;
    CBinaryTree<Traits> tree;

    std::cout << "        Insertando: 5, 3, 7, 2, 4, 6, 8\n";
    for (T1 v : {5, 3, 7, 2, 4, 6, 8}) tree.Insert(v, v);

    vector<T1> preorder;
    tree.preorderTraversal([&preorder](T1 &v) { preorder.push_back(v); });
    vector<T1> expectedPre = {5, 3, 2, 4, 7, 6, 8};
    assert((preorder == expectedPre));
    printVector(preorder, "Preorden (Raiz>Izq>Der)");

    vector<T1> inorder;
    tree.inorderTraversal([&inorder](T1 &v) { inorder.push_back(v); });
    vector<T1> expectedIn = {2, 3, 4, 5, 6, 7, 8};
    assert((inorder == expectedIn));
    printVector(inorder, "Inorden (Izq>Raiz>Der)");

    vector<T1> postorder;
    tree.postorderTraversal([&postorder](T1 &v) { postorder.push_back(v); });
    vector<T1> expectedPost = {2, 4, 3, 6, 8, 7, 5};
    assert((postorder == expectedPost));
    printVector(postorder, "Postorden (Izq>Der>Raiz)");
    std::cout << "        [OK] PASADA\n\n";

    logFile << "TestAllTraversals: PASADA\n";
}

void TestRemovalCases() {
    std::cout << "  [4/8] Prueba_EliminacionDeCasos\n";
    using Traits = TreeTraitAscending<T1>;
    CBinaryTree<Traits> tree;

    std::cout << "        Insertando: 50, 30, 70, 20, 40, 60, 80, 10, 25, 35\n";
    for (T1 v : {50, 30, 70, 20, 40, 60, 80, 10, 25, 35}) {
        tree.Insert(v, v);
    }

    std::cout << "        Eliminando nodo hoja (10)\n";
    T1 leafValue = 10;
    assert(tree.remove(leafValue) == 10);

    std::cout << "        Eliminando nodo con un hijo (20)\n";
    T1 oneChildValue = 20;
    assert(tree.remove(oneChildValue) == 20);

    std::cout << "        Eliminando nodo con dos hijos (30)\n";
    T1 twoChildrenValue = 30;
    assert(tree.remove(twoChildrenValue) == 30);

    vector<T1> remaining;
    tree.inorderTraversal([&remaining](T1 &v) { remaining.push_back(v); });
    vector<T1> expectedRemain = {25, 35, 40, 50, 60, 70, 80};
    assert((remaining == expectedRemain));
    printVector(remaining, "Restantes tras eliminaciones");
    std::cout << "        [OK] PASADA\n\n";

    logFile << "TestRemovalCases: PASADA\n";
}

void TestCopyConstructor() {
    std::cout << "  [5/8] Prueba_ConstructorCopia\n";
    using Traits = TreeTraitAscending<T1>;
    CBinaryTree<Traits> original;

    std::cout << "        Creando arbol original: 5, 3, 7, 2, 4, 6, 8\n";
    for (T1 v : {5, 3, 7, 2, 4, 6, 8}) original.Insert(v, v);

    std::cout << "        Creando copia del arbol\n";
    CBinaryTree<Traits> copied(original);

    vector<T1> originalData, copiedData;
    original.inorderTraversal([&originalData](T1 &v) { originalData.push_back(v); });
    copied.inorderTraversal([&copiedData](T1 &v) { copiedData.push_back(v); });

    assert(originalData == copiedData);
    printVector(copiedData, "Contenido del arbol copiado");
    std::cout << "        [OK] PASADA\n\n";

    logFile << "TestCopyConstructor: PASADA\n";
}

void TestMoveConstructor() {
    std::cout << "  [6/8] Prueba_ConstructorMovimiento\n";
    using Traits = TreeTraitAscending<T1>;
    CBinaryTree<Traits> original;

    std::cout << "        Creando arbol original: 5, 3, 7, 2, 4, 6, 8\n";
    for (T1 v : {5, 3, 7, 2, 4, 6, 8}) original.Insert(v, v);

    std::cout << "        Moviendo arbol a nuevo objeto\n";
    CBinaryTree<Traits> moved(std::move(original));

    vector<T1> movedData;
    moved.inorderTraversal([&movedData](T1 &v) { movedData.push_back(v); });
    vector<T1> expectedMoved = {2, 3, 4, 5, 6, 7, 8};
    assert((movedData == expectedMoved));
    printVector(movedData, "Contenido del arbol movido");
    std::cout << "        [OK] PASADA\n\n";

    logFile << "TestMoveConstructor: PASADA\n";
}

void TestSerializationDeserialization() {
    std::cout << "  [7/8] Prueba_Serializacion_Deserializacion\n";
    using Traits = TreeTraitAscending<T1>;
    CBinaryTree<Traits> tree;

    std::cout << "        Creando arbol: 5, 3, 7, 2, 4, 6, 8\n";
    for (T1 v : {5, 3, 7, 2, 4, 6, 8}) tree.Insert(v, v);

    std::cout << "        Serializando arbol a string\n";
    std::ostringstream oss;
    oss << tree;
    std::string serialized = oss.str();
    std::cout << "        Longitud serializada: " << serialized.length() << " caracteres\n";

    std::cout << "        Deserializando desde string\n";
    std::istringstream iss(serialized);
    CBinaryTree<Traits> restored;
    iss >> restored;

    vector<T1> original, restoredData;
    tree.inorderTraversal([&original](T1 &v) { original.push_back(v); });
    restored.inorderTraversal([&restoredData](T1 &v) { restoredData.push_back(v); });

    assert(original == restoredData);
    printVector(restoredData, "Contenido del arbol restaurado");
    std::cout << "        [OK] PASADA\n\n";

    logFile << "TestSerializationDeserialization: PASADA\n";
}

void TestForeachAndFirstThat() {
    std::cout << "  [8/8] Prueba_Foreach_PrimerQue\n";
    using Traits = TreeTraitAscending<T1>;
    CBinaryTree<Traits> tree;

    std::cout << "        Insertando: 5, 3, 7, 2, 4\n";
    for (T1 v : {5, 3, 7, 2, 4}) tree.Insert(v, v);

    std::cout << "        Ejecutando Foreach para contar elementos\n";
    int count = 0;
    auto counter = [&count](T1 &) { count++; };
    tree.Foreach(counter);
    std::cout << "        Cantidad de elementos: " << count << "\n";
    assert(count == 5);

    std::cout << "        Buscando elemento igual a 4 usando PrimerQue\n";
    auto isFour = [](T1 &v) { return v == 4; };
    T1 found = tree.FirstThat(isFour);
    std::cout << "        Valor encontrado: " << found << "\n";
    assert(found == 4);
    std::cout << "        [OK] PASADA\n\n";

    logFile << "TestForeachAndFirstThat: PASADA\n";
}

void DemoBinaryTree() {
    std::cout << "\n";
    TestBasicInsertion();
    TestForwardBackwardIterators();
    TestAllTraversals();
    TestRemovalCases();
    TestCopyConstructor();
    TestMoveConstructor();
    TestSerializationDeserialization();
    TestForeachAndFirstThat();

    logFile << "\n=== Todas las Pruebas de ArbolBinario PASADAS ===\n";
    logFile.flush();
}
