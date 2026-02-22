#include <iostream>

// Forward declarations
void DemoBinaryTree();
void DemoAVLTree();

int main() {
    std::cout << "\n";
    std::cout << "========================================================\n";
    std::cout << "           PRUEBAS DE ARBOL BINARIO Y AVL             \n";
    std::cout << "========================================================\n\n";

    std::cout << "--- PRUEBAS DE ARBOL BINARIO -------------------------\n";
    try {
        DemoBinaryTree();
        std::cout << "\n[OK] Pruebas de ArbolBinario PASADAS\n\n";
    } catch (const std::exception &e) {
        std::cerr << "[ERROR] Pruebas de ArbolBinario FALLARON: " << e.what() << "\n\n";
        return 1;
    }

    std::cout << "--- PRUEBAS DE ARBOL AVL ----------------------------\n";
    try {
        DemoAVLTree();
        std::cout << "\n[OK] Pruebas de ArbolAVL PASADAS\n\n";
    } catch (const std::exception &e) {
        std::cerr << "[ERROR] Pruebas de ArbolAVL FALLARON: " << e.what() << "\n\n";
        return 1;
    }

    std::cout << "========================================================\n";
    std::cout << "               [OK] TODAS LAS PRUEBAS PASARON           \n";
    std::cout << "========================================================\n\n";
    return 0;
}
