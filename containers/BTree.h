#include <mutex>

template <typename keyType, typename ObjIDType>
class BTree {
    CBTreePage<keyType, ObjIDType>* m_pRoot;
    mutable std::mutex m_mtx; // Punto 10: Concurrencia

public:
    BTree(int order = 31) : m_pRoot(new CBTreePage<keyType, ObjIDType>(order)) {}

    // Escritura Segura
    bt_ErrorCode Insert(const keyType& key, const ObjIDType id) {
        lock_guard<mutex> lock(m_mtx);
        return m_pRoot->Insert(key, id);
    }

    // Recorridos Variádicos
    template <typename... Funcs>
    void InOrder(Funcs&&... fns) {
        lock_guard<mutex> lock(m_mtx);
        m_pRoot->InOrder(0, std::forward<Funcs>(fns)...);
    }

    // Punto 1, 2, 3: Accesores de Iteradores
    auto begin()  { return ForwardBTreeIterator<keyType, ObjIDType>(m_pRoot); }
    auto end()    { return ForwardBTreeIterator<keyType, ObjIDType>(nullptr); }
    auto rbegin() { return BackwardBTreeIterator<keyType, ObjIDType>(m_pRoot); }
    auto rend()   { return BackwardBTreeIterator<keyType, ObjIDType>(nullptr); }

    // Punto 4: Sobrecarga de Operador
    friend ostream& operator<<(ostream& os, BTree& tree) {
        tree.InOrder([&os](auto& info, int level) {
            os << string(level * 2, ' ') << info.key << endl;
        });
        return os;
    }
};