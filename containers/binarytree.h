#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__

#include <iostream>
#include <mutex>
#include <stdexcept>
#include <utility>  
#include <algorithm> 
#include "../general/types.h"
#include "../compareFunc.h"


// TRAITS PARA BINARY TREE

template <typename T>
struct AscendingTrait {
    using value_type = T;
    
    
    static int obtenerIndice(const T &valor, const T &nodo) {
        if (Menor(valor, nodo)) {
            return 0;  // Izquierda
        } else {
            return 1;  // Derecha
        }
    }
};

template <typename T>
struct DescendingTrait {
    using value_type = T;
    
    
    static int obtenerIndice(const T &valor, const T &nodo) {
        if (Mayor(valor, nodo)) {
            return 0;  
        } else {
            return 1;  
        }
    }
};


// FORWARD 

template <typename Traits>
class CBinaryTree;

template <typename Traits>
std::ostream& operator<<(std::ostream& os, CBinaryTree<Traits>& tree);

template <typename Traits>
std::istream& operator>>(std::istream& is, CBinaryTree<Traits>& tree);




template <typename Traits>
class NodeBinaryTree {
    using value_type = typename Traits::value_type;
    using Node = NodeBinaryTree<Traits>;
    
    template <typename T>
    friend class CBinaryTree;
    
    template <typename T>
    friend class BinaryTreeForwardIterator;
    
    template <typename T>
    friend class BinaryTreeBackwardIterator;
    
    template <typename T>
    friend std::ostream& operator<<(std::ostream& os, CBinaryTree<T>& tree);
    
public:
    value_type m_data;
    ref_type m_ref;
    Node *m_pChild[2];  
    Node *m_pParent;
    int m_height;  

public:
    NodeBinaryTree(value_type value, ref_type ref = -1) 
        : m_data(value), m_ref(ref), m_pParent(nullptr), m_height(1) {
        m_pChild[0] = nullptr;
        m_pChild[1] = nullptr;
    }
    
    value_type& GetDataRef() { return m_data; }
    const value_type& GetData() const { return m_data; }
    ref_type GetRef() const { return m_ref; }
};


// FORWARD ITERATOR 

template <typename Traits>
class BinaryTreeForwardIterator {
    using Node = NodeBinaryTree<Traits>;
    using value_type = typename Traits::value_type;
    
private:
    Node *m_pCurrent;
    
    Node* encontrarMinimo(Node *node) {
        while (node && node->m_pChild[0]) {
            node = node->m_pChild[0];
        }
        return node;
    }

public:
    BinaryTreeForwardIterator(Node *root) {
        m_pCurrent = encontrarMinimo(root);
    }
    
    BinaryTreeForwardIterator() : m_pCurrent(nullptr) {}
    
    bool operator!=(const BinaryTreeForwardIterator &other) const {
        return m_pCurrent != other.m_pCurrent;
    }
    
    BinaryTreeForwardIterator& operator++() {
        if (!m_pCurrent) {
            return *this;
        }
        
        if (m_pCurrent->m_pChild[1]) {
            m_pCurrent = encontrarMinimo(m_pCurrent->m_pChild[1]);
        } else {
            Node *parent = m_pCurrent->m_pParent;
            while (parent && m_pCurrent == parent->m_pChild[1]) {
                m_pCurrent = parent;
                parent = parent->m_pParent;
            }
            m_pCurrent = parent;
        }
        
        return *this;
    }
    
    value_type& operator*() {
        return m_pCurrent->m_data;
    }
};


// BACKWARD ITERATOR 

template <typename Traits>
class BinaryTreeBackwardIterator {
    using Node = NodeBinaryTree<Traits>;
    using value_type = typename Traits::value_type;
    
private:
    Node *m_pCurrent;
    
    Node* encontrarMaximo(Node *node) {
        while (node && node->m_pChild[1]) {
            node = node->m_pChild[1];
        }
        return node;
    }

public:
    BinaryTreeBackwardIterator(Node *root) {
        m_pCurrent = encontrarMaximo(root);
    }
    
    BinaryTreeBackwardIterator() : m_pCurrent(nullptr) {}
    
    bool operator!=(const BinaryTreeBackwardIterator &other) const {
        return m_pCurrent != other.m_pCurrent;
    }
    
    BinaryTreeBackwardIterator& operator++() {
        if (!m_pCurrent) {
            return *this;
        }
        
        if (m_pCurrent->m_pChild[0]) {
            m_pCurrent = encontrarMaximo(m_pCurrent->m_pChild[0]);
        } else {
            Node *parent = m_pCurrent->m_pParent;
            while (parent && m_pCurrent == parent->m_pChild[0]) {
                m_pCurrent = parent;
                parent = parent->m_pParent;
            }
            m_pCurrent = parent;
        }
        
        return *this;
    }
    
    value_type& operator*() {
        return m_pCurrent->m_data;
    }
};


// CLASE BINARY TREE BASE (VIRTUAL PARA AVL)

template <typename Traits>
class CBinaryTree {
public:
    using value_type = typename Traits::value_type;
    using Node = NodeBinaryTree<Traits>;
    using ForwardIterator = BinaryTreeForwardIterator<Traits>;
    using BackwardIterator = BinaryTreeBackwardIterator<Traits>;
    
protected:
    Node *m_pRoot;
    size_t m_nElements;
    mutable std::mutex m_mutex;
    
  
    
    int altura(Node *nodo) const {
        if (!nodo) {
            return 0;
        }
        return nodo->m_height;
    }
    

    void actualizarAltura(Node *nodo) {
        if (!nodo) {
            return;
        }
        
        int alturaIzq = altura(nodo->m_pChild[0]);
        int alturaDer = altura(nodo->m_pChild[1]);
        
        if (alturaIzq > alturaDer) {
            nodo->m_height = 1 + alturaIzq;
        } else {
            nodo->m_height = 1 + alturaDer;
        }
    }
    

    Node* copiarArbol(Node *other, Node *parent = nullptr) {
        if (!other) {
            return nullptr;
        }
        
        Node *nuevoNodo = new Node(other->m_data, other->m_ref);
        nuevoNodo->m_pParent = parent;
        nuevoNodo->m_height = other->m_height;
        
        nuevoNodo->m_pChild[0] = copiarArbol(other->m_pChild[0], nuevoNodo);
        nuevoNodo->m_pChild[1] = copiarArbol(other->m_pChild[1], nuevoNodo);
        
        return nuevoNodo;
    }
    

    void eliminarArbol(Node *nodo) {
        if (nodo) {
            eliminarArbol(nodo->m_pChild[0]);
            eliminarArbol(nodo->m_pChild[1]);
            delete nodo;
        }
    }
    
    
    // Insert VIRTUAL 
    
    virtual Node* insertarNodo(Node *nodo, const value_type &valor, ref_type ref, Node *parent) {
        if (!nodo) {
            ++m_nElements;
            Node *nuevo = new Node(valor, ref);
            nuevo->m_pParent = parent;
            return nuevo;
        }
        
        int indice = Traits::obtenerIndice(valor, nodo->m_data);
        nodo->m_pChild[indice] = insertarNodo(nodo->m_pChild[indice], valor, ref, nodo);
        
        actualizarAltura(nodo);
        return nodo;
    }
    
    
    // Remove VIRTUAL 
    
    virtual Node* eliminarNodo(Node *nodo, const value_type &valor) {
        if (!nodo) {
            return nullptr;
        }
        
        if (Equal(valor, nodo->m_data)) {
            // Caso 1: Nodo hoja
            if (!nodo->m_pChild[0] && !nodo->m_pChild[1]) {
                delete nodo;
                --m_nElements;
                return nullptr;
            }
            
            // Caso 2: Un hijo
            if (!nodo->m_pChild[0]) {
                Node *hijo = nodo->m_pChild[1];
                hijo->m_pParent = nodo->m_pParent;
                delete nodo;
                --m_nElements;
                return hijo;
            }
            
            if (!nodo->m_pChild[1]) {
                Node *hijo = nodo->m_pChild[0];
                hijo->m_pParent = nodo->m_pParent;
                delete nodo;
                --m_nElements;
                return hijo;
            }
            
            // Caso 3: Dos hijos 
            Node *sucesor = nodo->m_pChild[1];
            while (sucesor->m_pChild[0]) {
                sucesor = sucesor->m_pChild[0];
            }
            
            nodo->m_data = sucesor->m_data;
            nodo->m_ref = sucesor->m_ref;
            nodo->m_pChild[1] = eliminarNodo(nodo->m_pChild[1], sucesor->m_data);
        } else {
            int indice = Traits::obtenerIndice(valor, nodo->m_data);
            nodo->m_pChild[indice] = eliminarNodo(nodo->m_pChild[indice], valor);
        }
        
        if (nodo) {
            actualizarAltura(nodo);
        }
        return nodo;
    }
    
    
    // Recorridos con variadic
    
    template <typename Func, typename... Args>
    void internoInorden(Node *nodo, Func fn, Args&&... args) {
        if (nodo) {
            internoInorden(nodo->m_pChild[0], fn, std::forward<Args>(args)...);
            fn(nodo->GetDataRef(), std::forward<Args>(args)...);
            internoInorden(nodo->m_pChild[1], fn, std::forward<Args>(args)...);
        }
    }
    
    template <typename Func, typename... Args>
    void internoPreorden(Node *nodo, Func fn, Args&&... args) {
        if (nodo) {
            fn(nodo->GetDataRef(), std::forward<Args>(args)...);
            internoPreorden(nodo->m_pChild[0], fn, std::forward<Args>(args)...);
            internoPreorden(nodo->m_pChild[1], fn, std::forward<Args>(args)...);
        }
    }
    
    template <typename Func, typename... Args>
    void internoPostorden(Node *nodo, Func fn, Args&&... args) {
        if (nodo) {
            internoPostorden(nodo->m_pChild[0], fn, std::forward<Args>(args)...);
            internoPostorden(nodo->m_pChild[1], fn, std::forward<Args>(args)...);
            fn(nodo->GetDataRef(), std::forward<Args>(args)...);
        }
    }
    

    // FirstThat 

    template <typename Func, typename... Args>
    bool internoFirstThat(Node *nodo, value_type &resultado, Func fn, Args&&... args) {
        if (!nodo) {
            return false;
        }
        
        if (internoFirstThat(nodo->m_pChild[0], resultado, fn, std::forward<Args>(args)...)) {
            return true;
        }
        
        if (fn(nodo->GetDataRef(), std::forward<Args>(args)...)) {
            resultado = nodo->m_data;
            return true;
        }
        
        if (internoFirstThat(nodo->m_pChild[1], resultado, fn, std::forward<Args>(args)...)) {
            return true;
        }
        
        return false;
    }
    

    // Impresión con tabulacion

    void imprimirTabulado(Node *nodo, int nivel, std::ostream &os) {
        if (nodo) {
            imprimirTabulado(nodo->m_pChild[1], nivel + 1, os);
            
            for (int i = 0; i < nivel; ++i) {
                os << "\t";
            }
            os << nodo->m_data << std::endl;
            
            imprimirTabulado(nodo->m_pChild[0], nivel + 1, os);
        }
    }

public:
    // Constructor 
    CBinaryTree() : m_pRoot(nullptr), m_nElements(0) {}
    
    
    // Constructor Copia
    
    CBinaryTree(const CBinaryTree &other) {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        m_pRoot = copiarArbol(other.m_pRoot);
        m_nElements = other.m_nElements;
    }
    
    
    // Move Constructor
    
    CBinaryTree(CBinaryTree &&other) noexcept {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        m_pRoot = std::exchange(other.m_pRoot, nullptr);
        m_nElements = std::exchange(other.m_nElements, 0);
    }
    
    
    // Destructor VIRTUAL 
    
    virtual ~CBinaryTree() {
        std::lock_guard<std::mutex> lock(m_mutex);
        eliminarArbol(m_pRoot);
        m_pRoot = nullptr;
        m_nElements = 0;
    }
    
    
    // Insert 
    
    virtual void insert(const value_type &valor, ref_type ref = -1) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_pRoot = insertarNodo(m_pRoot, valor, ref, nullptr);
    }
    
    
    // Remove 
    
    virtual bool remove(const value_type &valor) {
        std::lock_guard<std::mutex> lock(m_mutex);
        size_t antes = m_nElements;
        m_pRoot = eliminarNodo(m_pRoot, valor);
        return m_nElements < antes;
    }
    
    
    // Iteradores
    
    ForwardIterator begin() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return ForwardIterator(m_pRoot);
    }
    
    ForwardIterator end() {
        return ForwardIterator();
    }
    
    BackwardIterator rbegin() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return BackwardIterator(m_pRoot);
    }
    
    BackwardIterator rend() {
        return BackwardIterator();
    }
    
    
    // Recorridos 
    
    template <typename Func, typename... Args>
    void inorden(Func fn, Args&&... args) {
        std::lock_guard<std::mutex> lock(m_mutex);
        internoInorden(m_pRoot, fn, std::forward<Args>(args)...);
    }
    
    template <typename Func, typename... Args>
    void preorden(Func fn, Args&&... args) {
        std::lock_guard<std::mutex> lock(m_mutex);
        internoPreorden(m_pRoot, fn, std::forward<Args>(args)...);
    }
    
    template <typename Func, typename... Args>
    void postorden(Func fn, Args&&... args) {
        std::lock_guard<std::mutex> lock(m_mutex);
        internoPostorden(m_pRoot, fn, std::forward<Args>(args)...);
    }
    
   
    // Foreach 
    
    template <typename Func, typename... Args>
    void Foreach(Func fn, Args&&... args) {
        for (auto it = begin(); it != end(); ++it) {
            fn(*it, std::forward<Args>(args)...);
        }
    }
    
    
    // FirstThat ENCAPSULADO
    
    template <typename Func, typename... Args>
    bool FirstThat(value_type &resultado, Func fn, Args&&... args) {
        std::lock_guard<std::mutex> lock(m_mutex);
        return internoFirstThat(m_pRoot, resultado, fn, std::forward<Args>(args)...);
    }
    
    
    // Impresión Tabulada
    
    void printTabulado(std::ostream &os = std::cout) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_pRoot) {
            imprimirTabulado(m_pRoot, 0, os);
        } else {
            os << "(árbol vacío)" << std::endl;
        }
    }
    

    bool isEmpty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_pRoot == nullptr;
    }
    
    size_t getSize() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_nElements;
    }
    
    int getAltura() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return altura(m_pRoot);
    }
    

    friend std::ostream& operator<< <Traits>(std::ostream& os, CBinaryTree<Traits>& tree);
    friend std::istream& operator>> <Traits>(std::istream& is, CBinaryTree<Traits>& tree);
};


// OPERADORES

template <typename Traits>
std::ostream& operator<<(std::ostream& os, CBinaryTree<Traits>& tree) {
    std::lock_guard<std::mutex> lock(tree.m_mutex);
    
    os << "CBinaryTree: size = " << tree.m_nElements << std::endl;
    os << "Inorden: [";
    
    bool primero = true;
    tree.internoInorden(tree.m_pRoot, [&](typename Traits::value_type &val) {
        if (!primero) {
            os << ", ";
        }
        os << val;
        primero = false;
    });
    
    os << "]" << std::endl;
    return os;
}

template <typename Traits>
std::istream& operator>>(std::istream& is, CBinaryTree<Traits>& tree) {
    typename Traits::value_type valor;
    if (is >> valor) {
        tree.insert(valor);
    }
    return is;
}

#endif // __BINARYTREE_H__