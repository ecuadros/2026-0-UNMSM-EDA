// binarytree.h

#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__

#include <iostream>
#include <fstream>
#include <mutex>
#include "../general/types.h"
#include "../util.h"

using namespace std;

template <typename T>
struct TreeTraitAscending {
    using value_type  = T;
    using CompareFunc = std::greater<T>;
};

template <typename T>
struct TreeTraitDescending {
    using value_type  = T;
    using CompareFunc = std::less<T>;
};

// NodeBinaryTree
template <typename Traits>
class NodeBinaryTree {
public:
    using value_type  = typename Traits::value_type;
    using Node        = NodeBinaryTree<Traits>;
    using CompareFunc = typename Traits::CompareFunc;

    value_type  m_data;
    ref_type    m_ref;
    int         m_height    = 1;
    Node       *m_pChild[2] = { nullptr, nullptr };

    NodeBinaryTree() {}

    NodeBinaryTree(value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref) {}

    value_type  GetValue()    const { return m_data; }
    value_type &GetValueRef()       { return m_data; }
    ref_type    GetRef()      const { return m_ref; }
    ref_type   &GetRefRef()         { return m_ref; }
};

// CBinaryTree
template <typename Traits>
class CBinaryTree {
public:
    using value_type  = typename Traits::value_type;
    using Node        = NodeBinaryTree<Traits>;
    using CompareFunc = typename Traits::CompareFunc;

    // Forward Iterator (inorder)
    class ForwardIterator {
    public:
        CBinaryTree *m_pTree;
        long         m_pos;

        ForwardIterator(CBinaryTree *pTree, long pos = 0)
            : m_pTree(pTree), m_pos(pos) {}

        ForwardIterator(ForwardIterator &another)
            : m_pTree(another.m_pTree), m_pos(another.m_pos) {}

        ForwardIterator &operator++() {
            if (m_pos < m_pTree->m_nElements) ++m_pos;
            return *this;
        }

        bool operator!=(const ForwardIterator &another) const {
            return m_pTree != another.m_pTree || m_pos != another.m_pos;
        }

        value_type &operator*() {
            return m_pTree->GetByInorderPos(m_pos);
        }
    };

    // Backward Iterator (inorder inverso)
    class BackwardIterator {
    public:
        CBinaryTree *m_pTree;
        long         m_pos;

        BackwardIterator(CBinaryTree *pTree, long pos = 0)
            : m_pTree(pTree), m_pos(pos) {}

        BackwardIterator(BackwardIterator &another)
            : m_pTree(another.m_pTree), m_pos(another.m_pos) {}

        BackwardIterator &operator++() {
            if (m_pos > -1) --m_pos;
            return *this;
        }

        bool operator!=(const BackwardIterator &another) const {
            return m_pTree != another.m_pTree || m_pos != another.m_pos;
        }

        value_type &operator*() {
            return m_pTree->GetByInorderPos(m_pos);
        }
    };

protected:
    Node        *m_pRoot     = nullptr;
    size_t       m_nElements = 0;
    CompareFunc  comp;
    mutable mutex m_mutex;

public:
    // Constructor por defecto
    CBinaryTree() {}

    // Constructor copia
    CBinaryTree(CBinaryTree &another) {
        lock_guard<mutex> lock(another.m_mutex);
        m_pRoot     = CopySubtree(another.m_pRoot);
        m_nElements = another.m_nElements;
    }

    // Move Constructor
    CBinaryTree(CBinaryTree &&another) {
        lock_guard<mutex> lock(another.m_mutex);
        m_pRoot           = another.m_pRoot;
        m_nElements       = another.m_nElements;
        another.m_pRoot   = nullptr;
        another.m_nElements = 0;
    }

    // Destructor seguro
    virtual ~CBinaryTree() {
        DestroySubtree(m_pRoot);
        m_pRoot = nullptr;
    }

    // Insert / Remove
    void Insert(const value_type &val, ref_type ref = -1) {
        lock_guard<mutex> lock(m_mutex);
        InternalInsert(m_pRoot, val, ref);
        ++m_nElements;
    }

    bool Remove(const value_type &val) {
        lock_guard<mutex> lock(m_mutex);
        bool removed = InternalRemove(m_pRoot, val);
        if (removed) --m_nElements;
        return removed;
    }

    size_t getSize() const { return m_nElements; }

    // Iteradores
    ForwardIterator  begin()  { return ForwardIterator(this, 0); }
    ForwardIterator  end()    { return ForwardIterator(this, m_nElements); }
    BackwardIterator rbegin() { return BackwardIterator(this, m_nElements - 1); }
    BackwardIterator rend()   { return BackwardIterator(this, -1); }

    // Inorder con variadic
    template <typename ObjFunc, typename ...Args>
    void Inorder(ObjFunc of, Args ...args) {
        lock_guard<mutex> lock(m_mutex);
        InternalInorder(m_pRoot, of, args...);
    }

    // Preorder con variadic
    template <typename ObjFunc, typename ...Args>
    void Preorder(ObjFunc of, Args ...args) {
        lock_guard<mutex> lock(m_mutex);
        InternalPreorder(m_pRoot, of, args...);
    }

    // Postorder con variadic
    template <typename ObjFunc, typename ...Args>
    void Postorder(ObjFunc of, Args ...args) {
        lock_guard<mutex> lock(m_mutex);
        InternalPostorder(m_pRoot, of, args...);
    }

    // Foreach con variadic
    template <typename ObjFunc, typename ...Args>
    void Foreach(ObjFunc of, Args ...args) {
        ::Foreach(*this, of, args...);
    }

    // FirstThat con variadic
    template <typename ObjFunc, typename ...Args>
    auto FirstThat(ObjFunc of, Args ...args) {
        return ::FirstThat(*this, of, args...);
    }

    // operator<<
    friend ostream &operator<<(ostream &os, CBinaryTree<Traits> &tree) {
        os << "CBinaryTree: size=" << tree.m_nElements << endl;
        tree.InternalInorder(tree.m_pRoot, [&os](value_type &val) {
            os << val << " ";
        });
        os << endl;
        return os;
    }

    friend istream &operator>>(istream &is, CBinaryTree<Traits> &tree) {
        typename Traits::value_type val;
        ref_type ref;
        while (is >> val >> ref)
            tree.Insert(val, ref);
        return is;
    }

    value_type &GetByInorderPos(long pos) {
        long       idx     = 0;
        Node      *result  = nullptr;
        InorderCollect(m_pRoot, pos, idx, result);
        return result->GetValueRef();
    }

private:

    // Insert interno
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref) {
        if (!rParent) {
            rParent = new Node(val, ref);
            return;
        }
        auto path = comp(val, rParent->GetValue());
        InternalInsert(rParent->m_pChild[path], val, ref);
    }

    // Remove interno
    bool InternalRemove(Node *&rNode, const value_type &val) {
        if (!rNode) return false;
        if (val == rNode->GetValue()) {
            if (!rNode->m_pChild[0] && !rNode->m_pChild[1]) {
                delete rNode;
                rNode = nullptr;
            } else if (!rNode->m_pChild[0]) {
                Node *tmp = rNode;
                rNode = rNode->m_pChild[1];
                delete tmp;
            } else if (!rNode->m_pChild[1]) {
                Node *tmp = rNode;
                rNode = rNode->m_pChild[0];
                delete tmp;
            } else {
                Node *succ = rNode->m_pChild[1];
                while (succ->m_pChild[0])
                    succ = succ->m_pChild[0];
                rNode->m_data = succ->m_data;
                rNode->m_ref  = succ->m_ref;
                InternalRemove(rNode->m_pChild[1], succ->GetValue());
            }
            return true;
        }
        auto path = comp(val, rNode->GetValue());
        return InternalRemove(rNode->m_pChild[path], val);
    }

    Node *CopySubtree(Node *src) {
        if (!src) return nullptr;
        Node *newNode       = new Node(src->m_data, src->m_ref);
        newNode->m_pChild[0] = CopySubtree(src->m_pChild[0]);
        newNode->m_pChild[1] = CopySubtree(src->m_pChild[1]);
        return newNode;
    }

    void DestroySubtree(Node *node) {
        if (!node) return;
        DestroySubtree(node->m_pChild[0]);
        DestroySubtree(node->m_pChild[1]);
        delete node;
    }

    // Recorridos internos con variadic
    template <typename ObjFunc, typename ...Args>
    void InternalInorder(Node *node, ObjFunc of, Args ...args) {
        if (!node) return;
        InternalInorder(node->m_pChild[0], of, args...);
        of(node->GetValueRef(), args...);
        InternalInorder(node->m_pChild[1], of, args...);
    }

    template <typename ObjFunc, typename ...Args>
    void InternalPreorder(Node *node, ObjFunc of, Args ...args) {
        if (!node) return;
        of(node->GetValueRef(), args...);
        InternalPreorder(node->m_pChild[0], of, args...);
        InternalPreorder(node->m_pChild[1], of, args...);
    }

    template <typename ObjFunc, typename ...Args>
    void InternalPostorder(Node *node, ObjFunc of, Args ...args) {
        if (!node) return;
        InternalPostorder(node->m_pChild[0], of, args...);
        InternalPostorder(node->m_pChild[1], of, args...);
        of(node->GetValueRef(), args...);
    }

    void InorderCollect(Node *node, long target, long &current, Node *&result) {
        if (!node) return;
        InorderCollect(node->m_pChild[0], target, current, result);
        if (current == target) result = node;
        ++current;
        InorderCollect(node->m_pChild[1], target, current, result);
    }
};

void DemoBinaryTree();

#endif // __BINARYTREE_H__