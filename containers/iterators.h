#include <stack>

// Punto 1, 2, 5: Forward Iterator
template <typename K, typename O>
class ForwardBTreeIterator {
    using BTPage = CBTreePage<K, O>;
    std::stack<pair<BTPage*, int>> m_Stack;

public:
    ForwardBTreeIterator(BTPage* root) { pushLeft(root); }
    
    void pushLeft(BTPage* page) {
        while (page) {
            m_Stack.push({page, 0});
            page = page->m_SubPages[0];
        }
    }

    auto& operator*() { return m_Stack.top().first->m_Keys[m_Stack.top().second]; }

    ForwardBTreeIterator& operator++() {
        auto& top = m_Stack.top();
        top.second++;
        if (top.second < top.first->m_KeyCount) {
            pushLeft(top.first->m_SubPages[top.second]);
        } else {
            m_Stack.pop();
        }
        return *this;
    }

    bool operator!=(const ForwardBTreeIterator& other) const { return m_Stack.size() != other.m_Stack.size(); }
};

// Punto 3: Backward Iterator
template <typename K, typename O>
class BackwardBTreeIterator {
    using BTPage = CBTreePage<K, O>;
    std::stack<pair<BTPage*, int>> m_Stack;

public:
    BackwardBTreeIterator(BTPage* root) { pushRight(root); }

    void pushRight(BTPage* page) {
        while (page) {
            m_Stack.push({page, page->m_KeyCount - 1});
            page = page->m_SubPages[page->m_KeyCount];
        }
    }

    auto& operator*() { return m_Stack.top().first->m_Keys[m_Stack.top().second]; }

    BackwardBTreeIterator& operator++() {
        auto& top = m_Stack.top();
        if (top.second > 0) {
            top.second--;
            pushRight(top.first->m_SubPages[top.second + 1]);
        } else {
            m_Stack.pop();
        }
        return *this;
    }

    bool operator!=(const BackwardBTreeIterator& other) const { return m_Stack.size() != other.m_Stack.size(); }
};