// --- Añadir estos métodos públicos en CBTreePage ---

// Punto 6, 7, 8: Recorridos Variádicos (InOrder, PreOrder, PostOrder)
template <typename... Funcs>
void InOrder(int level, Funcs&&... fns) {
    for (int i = 0; i < m_KeyCount; i++) {
        if (m_SubPages[i]) m_SubPages[i]->InOrder(level + 1, std::forward<Funcs>(fns)...);
        (fns(m_Keys[i], level), ...); // Fold Expression (C++17)
    }
    if (m_SubPages[m_KeyCount]) m_SubPages[m_KeyCount]->InOrder(level + 1, std::forward<Funcs>(fns)...);
}

template <typename... Funcs>
void PreOrder(int level, Funcs&&... fns) {
    for (int i = 0; i < m_KeyCount; i++) {
        (fns(m_Keys[i], level), ...);
        if (m_SubPages[i]) m_SubPages[i]->PreOrder(level + 1, std::forward<Funcs>(fns)...);
    }
    if (m_SubPages[m_KeyCount]) m_SubPages[m_KeyCount]->PreOrder(level + 1, std::forward<Funcs>(fns)...);
}

// Punto 9: FirstThat Variádico
template <typename P, typename... Funcs>
ObjectInfo* FirstThat(P&& pred, int level, Funcs&&... fns) {
    for (int i = 0; i < m_KeyCount; i++) {
        if (m_SubPages[i]) {
            auto* found = m_SubPages[i]->FirstThat(pred, level + 1, std::forward<Funcs>(fns)...);
            if (found) return found;
        }
        if (pred(m_Keys[i], level)) {
            (fns(m_Keys[i], level), ...);
            return &m_Keys[i];
        }
    }
    if (m_SubPages[m_KeyCount]) 
        return m_SubPages[m_KeyCount]->FirstThat(pred, level + 1, std::forward<Funcs>(fns)...);
    return nullptr;
}