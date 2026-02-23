
#ifndef CBSTARTREEPAGE_H
#define CBSTARTREEPAGE_H

#include <vector>
#include <iostream>
#include <stdexcept>
#include <mutex>
#include <cassert>
#include "../general/types.h"
#include "../util.h"
#include "../foreach.h"

using namespace std;


enum bt_ErrorCode {
    bt_ok,
    bt_overflow,
    bt_underflow,
    bt_duplicate,
    bt_nofound,
    bt_rootmerged
};


// traits para el b estrella 


template <typename _T, typename _Func>
struct BStarTreeTrait {
    using T    = _T;
    using Func = _Func;
};

template <typename _T>
struct BStarAscendingTrait : public BStarTreeTrait<_T, std::greater<_T>> {};

template <typename _T>
struct BStarDescendingTrait : public BStarTreeTrait<_T, std::less<_T>> {};



template <typename keyType, typename ObjIDType>
struct BStarObjectInfo {
    keyType   key;
    ObjIDType ObjID;
    long      UseCounter = 0;

    BStarObjectInfo() {}
    BStarObjectInfo(const keyType &_key, ObjIDType _ObjID)
        : key(_key), ObjID(_ObjID), UseCounter(0) {}

    operator keyType () const { return key; }
    long GetUseCounter() const { return UseCounter; }
};



template <typename Container, typename ObjType>
int bstar_binary_search(Container &container, int first, int last, const ObjType &object) {
    if (first >= last) return first;
    while (first < last) {
        int mid = (first + last) / 2;
        if (object == (ObjType)container[mid]) return mid;
        if (object >  (ObjType)container[mid]) first = mid + 1;
        else                                   last  = mid;
    }
    if (object <= (ObjType)container[first]) return first;
    return last;
}

template <typename Container, typename ObjType>
void bstar_insert_at(Container &container, const ObjType &object, int pos) {
    int size = (int)container.size();
    for (int i = size - 2; i >= pos; i--)
        container[i + 1] = container[i];
    container[pos] = object;
}

template <typename Container>
void bstar_remove(Container &container, int pos) {
    int size = (int)container.size();
    for (int i = pos + 1; i < size; i++)
        container[i - 1] = container[i];
}



template <typename Traits> class CBStarTree;
template <typename Traits> class CBStarTreePage;

// ahora forward 

template <typename Traits>
class BStarForwardIterator {
    using Page       = CBStarTreePage<Traits>;
    using ObjectInfo = BStarObjectInfo<typename Traits::T, ref_type>;

    struct Cursor {
        Page *page  = nullptr;
        int   index = 0;
    };

    vector<Cursor> m_stack;
    ObjectInfo    *m_pCurrent = nullptr;

    void pushLeftmost(Page *p) {
        while (p) {
            m_stack.push_back({p, 0});
            if (!p->m_SubPages.empty() && p->m_SubPages[0]) {
                p = p->m_SubPages[0];
            } else {
                break;
            }
        }
    }

public:
    BStarForwardIterator() = default;

    explicit BStarForwardIterator(Page *root) {
        if (root) {
            pushLeftmost(root);
        }
        advance();
    }

    void advance() {
        m_pCurrent = nullptr;
        while (!m_stack.empty()) {
            Cursor &cur = m_stack.back();
            if (cur.index < cur.page->m_KeyCount) {
                m_pCurrent = &cur.page->m_Keys[cur.index];
                int nextIdx = cur.index + 1;
                cur.index++;
                if (cur.page->m_SubPages.size() > (size_t)nextIdx &&
                    cur.page->m_SubPages[nextIdx]) {
                    pushLeftmost(cur.page->m_SubPages[nextIdx]);
                }
                return;
            }
            m_stack.pop_back();
        }
    }

    bool operator!=(const BStarForwardIterator &other) const {
        return m_pCurrent != other.m_pCurrent;
    }

    BStarForwardIterator &operator++() {
        advance();
        return *this;
    }

    ObjectInfo &operator*() {
        if (!m_pCurrent) {
            throw std::out_of_range("BStarForwardIterator: dereferencia invalida");
        }
        return *m_pCurrent;
    }
};


// ahora backward 


template <typename Traits>
class BStarBackwardIterator {
    using Page       = CBStarTreePage<Traits>;
    using ObjectInfo = BStarObjectInfo<typename Traits::T, ref_type>;

    struct Cursor {
        Page *page  = nullptr;
        int   index = 0;
    };

    vector<Cursor> m_stack;
    ObjectInfo    *m_pCurrent = nullptr;

    void pushRightmost(Page *p) {
        while (p) {
            m_stack.push_back({p, p->m_KeyCount - 1});
            if (!p->m_SubPages.empty() && p->m_SubPages[p->m_KeyCount]) {
                p = p->m_SubPages[p->m_KeyCount];
            } else {
                break;
            }
        }
    }

public:
    BStarBackwardIterator() = default;

    explicit BStarBackwardIterator(Page *root) {
        if (root) {
            pushRightmost(root);
        }
        advance();
    }

    void advance() {
        m_pCurrent = nullptr;
        while (!m_stack.empty()) {
            Cursor &cur = m_stack.back();
            if (cur.index >= 0 && cur.index < cur.page->m_KeyCount) {
                m_pCurrent = &cur.page->m_Keys[cur.index];
                int prevIdx = cur.index;
                cur.index--;
                if (cur.page->m_SubPages.size() > (size_t)prevIdx &&
                    cur.page->m_SubPages[prevIdx]) {
                    pushRightmost(cur.page->m_SubPages[prevIdx]);
                }
                return;
            }
            m_stack.pop_back();
        }
    }

    bool operator!=(const BStarBackwardIterator &other) const {
        return m_pCurrent != other.m_pCurrent;
    }

    BStarBackwardIterator &operator++() {
        advance();
        return *this;
    }

    ObjectInfo &operator*() {
        if (!m_pCurrent) {
            throw std::out_of_range("BStarBackwardIterator: dereferencia invalida");
        }
        return *m_pCurrent;
    }
};


// clase página del b estrella


template <typename Traits>
class CBStarTreePage {
    friend class CBStarTree<Traits>;
    friend class BStarForwardIterator<Traits>;
    friend class BStarBackwardIterator<Traits>;

    using value_type  = typename Traits::T;
    using CompareFunc = typename Traits::Func;
    using BTPage      = CBStarTreePage<Traits>;
    using ObjectInfo  = BStarObjectInfo<value_type, ref_type>;

public:
    using forward_iterator  = BStarForwardIterator<Traits>;
    using backward_iterator = BStarBackwardIterator<Traits>;

    using lpfnForEach2   = void (*)(ObjectInfo &, int, void *);
    using lpfnForEach3   = void (*)(ObjectInfo &, int, void *, void *);
    using lpfnFirstThat2 = ObjectInfo *(*)(ObjectInfo &, int, void *);
    using lpfnFirstThat3 = ObjectInfo *(*)(ObjectInfo &, int, void *, void *);

protected:
    int              m_MinKeys          = 0;
    int              m_MaxKeys          = 0;
    int              m_MaxKeysForChilds = 0;
    bool             m_Unique           = true;
    vector<ObjectInfo> m_Keys;
    vector<BTPage *>   m_SubPages;
    int              m_KeyCount         = 0;


    // Abstracción de índices, para que se vea mejor

    bool IsLeaf()    const { return m_SubPages.empty() || m_SubPages[0] == nullptr; }
    bool IsRoot()    const { return m_MaxKeysForChilds != m_MaxKeys; }
    bool Overflow()  const { return m_KeyCount > m_MaxKeys; }
    bool Underflow() const { return m_KeyCount < MinNumberOfKeys(); }
    bool IsFull()    const { return m_KeyCount >= m_MaxKeys; }

    int MinNumberOfKeys()  const { return (int)(2 * m_MaxKeys / 3.0); }
    int GetFreeCells()     const { return m_MaxKeys - m_KeyCount; }
    int &NumberOfKeys()          { return m_KeyCount; }
    int  GetNumberOfKeys() const { return m_KeyCount; }

    int GetFreeCellsOnLeft(int pos) const {
        if (pos > 0 && m_SubPages[pos - 1]) {
            return m_SubPages[pos - 1]->GetFreeCells();
        }
        return 0;
    }

    int GetFreeCellsOnRight(int pos) const {
        if (pos < m_KeyCount && m_SubPages[pos + 1]) {
            return m_SubPages[pos + 1]->GetFreeCells();
        }
        return 0;
    }

    void SetMaxKeysForChilds(int order) { m_MaxKeysForChilds = order; }


    void Create() {
        Reset();
        m_Keys.resize(m_MaxKeys + 1);
        m_SubPages.resize(m_MaxKeys + 2, nullptr);
        m_KeyCount = 0;
        m_MinKeys  = (int)(2 * m_MaxKeys / 3.0);
    }

    void Reset() {
        for (int i = 0; i < m_KeyCount; i++) {
            delete m_SubPages[i];
        }
        clear();
    }

    void clear() { m_KeyCount = 0; }

    void Destroy() { Reset(); delete this; }

    // Redistribución de claves

    void RedistributeR2L(int pos) {
        BTPage *pSource = m_SubPages[pos];
        BTPage *pTarget = m_SubPages[pos - 1];
        while (pSource->GetNumberOfKeys() > pSource->MinNumberOfKeys() &&
               pTarget->GetNumberOfKeys() < pSource->GetNumberOfKeys()) {
            bstar_insert_at(pTarget->m_Keys,     m_Keys[pos - 1], pTarget->NumberOfKeys()++);
            bstar_insert_at(pTarget->m_SubPages, pSource->m_SubPages[0], pTarget->NumberOfKeys());
            m_Keys[pos - 1] = pSource->m_Keys[0];
            bstar_remove(pSource->m_Keys,     0);
            bstar_remove(pSource->m_SubPages, 0);
            pSource->NumberOfKeys()--;
        }
    }

    void RedistributeL2R(int pos) {
        BTPage *pSource = m_SubPages[pos];
        BTPage *pTarget = m_SubPages[pos + 1];
        while (pSource->GetNumberOfKeys() > pSource->MinNumberOfKeys() &&
               pTarget->GetNumberOfKeys() < pSource->GetNumberOfKeys()) {
            bstar_insert_at(pTarget->m_Keys,     m_Keys[pos], 0);
            bstar_insert_at(pTarget->m_SubPages, pSource->m_SubPages[pSource->NumberOfKeys()], 0);
            pTarget->NumberOfKeys()++;
            m_Keys[pos] = pSource->m_Keys[pSource->NumberOfKeys() - 1];
            pSource->NumberOfKeys()--;
        }
    }

    bool Redistribute1(int &pos) {
        if (m_SubPages[pos]->Underflow()) {
            int nkol = 0;
            if (pos > 0) {
                nkol = m_SubPages[pos - 1]->NumberOfKeys();
            }
            int nkor = 0;
            if (pos < NumberOfKeys()) {
                nkor = m_SubPages[pos + 1]->NumberOfKeys();
            }
            if (nkol > nkor) {
                if (m_SubPages[pos - 1]->NumberOfKeys() > m_SubPages[pos - 1]->MinNumberOfKeys()) {
                    RedistributeL2R(pos - 1);
                } else {
                    if (pos == NumberOfKeys()) {
                        --pos;
                        return false;
                    }
                    return false;
                }
            } else {
                if (m_SubPages[pos + 1]->NumberOfKeys() > m_SubPages[pos + 1]->MinNumberOfKeys()) {
                    RedistributeR2L(pos + 1);
                } else {
                    if (pos == 0) {
                        ++pos;
                        return false;
                    }
                    return false;
                }
            }
        } else {
            int fcol = GetFreeCellsOnLeft(pos);
            int fcor = GetFreeCellsOnRight(pos);
            if (!fcol && !fcor && m_SubPages[pos]->IsFull()) {
                return false;
            }
            if (fcol > fcor) {
                RedistributeR2L(pos);
            } else {
                RedistributeL2R(pos);
            }
        }
        return true;
    }

    bool Redistribute2(int pos) {
        assert(pos > 0 && pos < NumberOfKeys());
        if (m_SubPages[pos - 1]->Underflow()) {
            RedistributeR2L(pos + 1);
            RedistributeR2L(pos);
            if (m_SubPages[pos - 1]->Underflow()) {
                return false;
            }
        } else if (m_SubPages[pos + 1]->Underflow()) {
            RedistributeL2R(pos - 1);
            RedistributeL2R(pos);
            if (m_SubPages[pos + 1]->Underflow()) {
                return false;
            }
        } else {
            RedistributeL2R(pos - 1);
            RedistributeR2L(pos + 1);
            if (m_SubPages[pos]->Underflow()) {
                return false;
            }
        }
        return true;
    }

    bool TreatUnderflow(int &pos) {
        if (Redistribute1(pos)) {
            return true;
        }
        return Redistribute2(pos);
    }

    void MovePage(BTPage *pChildPage,
                  vector<ObjectInfo> &tmpKeys,
                  vector<BTPage *>   &tmpSubPages) {
        int n = pChildPage->GetNumberOfKeys();
        for (int i = 0; i < n; i++) {
            tmpKeys.push_back(pChildPage->m_Keys[i]);
            tmpSubPages.push_back(pChildPage->m_SubPages[i]);
        }
        tmpSubPages.push_back(pChildPage->m_SubPages[n]);
        pChildPage->clear();
    }

    void SplitPageInto3(vector<ObjectInfo> &tmpKeys,
                        vector<BTPage *>   &tmpSubPages,
                        BTPage *&pChild1,
                        BTPage *&pChild2,
                        BTPage *&pChild3,
                        ObjectInfo &oi1,
                        ObjectInfo &oi2) {
        assert(tmpKeys.size() >= 8);
        assert(tmpSubPages.size() >= 9);

        if (!pChild1) {
            pChild1 = new BTPage(m_MaxKeysForChilds, m_Unique);
        }
        pChild1->clear();
        int nKeys = ((int)tmpKeys.size() - 2) / 3;
        int i = 0;
        for (; i < nKeys; i++) {
            pChild1->m_Keys[i]     = tmpKeys[i];
            pChild1->m_SubPages[i] = tmpSubPages[i];
            pChild1->NumberOfKeys()++;
        }
        pChild1->m_SubPages[i] = tmpSubPages[i];
        oi1 = tmpKeys[i++];

        if (!pChild2) {
            pChild2 = new BTPage(m_MaxKeysForChilds, m_Unique);
        }
        pChild2->clear();
        nKeys += ((int)tmpKeys.size() - 2) / 3 + 1;
        int j = 0;
        for (; i < nKeys; i++, j++) {
            pChild2->m_Keys[j]     = tmpKeys[i];
            pChild2->m_SubPages[j] = tmpSubPages[i];
            pChild2->NumberOfKeys()++;
        }
        pChild2->m_SubPages[j] = tmpSubPages[i];
        oi2 = tmpKeys[i++];

        if (!pChild3) {
            pChild3 = new BTPage(m_MaxKeysForChilds, m_Unique);
        }
        pChild3->clear();
        int nTotal = (int)tmpKeys.size();
        for (j = 0; i < nTotal; i++, j++) {
            pChild3->m_Keys[j]     = tmpKeys[i];
            pChild3->m_SubPages[j] = tmpSubPages[i];
            pChild3->NumberOfKeys()++;
        }
        pChild3->m_SubPages[j] = tmpSubPages[i];
    }

    bool SplitRoot() {
        BTPage    *pChild1 = nullptr;
        BTPage    *pChild2 = nullptr;
        BTPage    *pChild3 = nullptr;
        ObjectInfo oi1, oi2;
        SplitPageInto3(m_Keys, m_SubPages, pChild1, pChild2, pChild3, oi1, oi2);
        clear();
        m_Keys[0]     = oi1;
        m_SubPages[0] = pChild1;
        NumberOfKeys()++;
        m_Keys[1]     = oi2;
        m_SubPages[1] = pChild2;
        NumberOfKeys()++;
        m_SubPages[2] = pChild3;
        return true;
    }

    void SplitChild(int pos) {
        BTPage *pChild1 = nullptr;
        BTPage *pChild2 = nullptr;
        if (pos > 0 && m_SubPages[pos - 1]->IsFull()) {
            pChild1 = m_SubPages[pos - 1];
            pChild2 = m_SubPages[pos--];
        }
        if (pos < GetNumberOfKeys() && m_SubPages[pos + 1]->IsFull()) {
            pChild1 = m_SubPages[pos];
            pChild2 = m_SubPages[pos + 1];
        }
        vector<ObjectInfo> tmpKeys;
        vector<BTPage *>   tmpSubPages;
        MovePage(pChild1, tmpKeys, tmpSubPages);
        tmpKeys.push_back(m_Keys[pos]);
        MovePage(pChild2, tmpKeys, tmpSubPages);

        BTPage    *pChild3 = nullptr;
        ObjectInfo oi1, oi2;
        SplitPageInto3(tmpKeys, tmpSubPages, pChild1, pChild2, pChild3, oi1, oi2);

        m_Keys[pos]     = oi1;
        m_SubPages[pos] = pChild1;
        bstar_insert_at(m_Keys,     oi2,     pos + 1);
        bstar_insert_at(m_SubPages, pChild2, pos + 1);
        NumberOfKeys()++;
        m_SubPages[pos + 2] = pChild3;
    }

    bt_ErrorCode Merge(int pos) {
        vector<ObjectInfo> tmpKeys;
        vector<BTPage *>   tmpSubPages;
        BTPage *pChild1 = m_SubPages[pos - 1];
        BTPage *pChild2 = m_SubPages[pos];
        BTPage *pChild3 = m_SubPages[pos + 1];
        MovePage(pChild1, tmpKeys, tmpSubPages);
        tmpKeys.push_back(m_Keys[pos - 1]);
        MovePage(pChild2, tmpKeys, tmpSubPages);
        tmpKeys.push_back(m_Keys[pos]);
        MovePage(pChild3, tmpKeys, tmpSubPages);
        pChild3->Destroy();

        int nKeys = pChild1->GetFreeCells();
        int i = 0;
        for (; i < nKeys; i++) {
            pChild1->m_Keys[i]     = tmpKeys[i];
            pChild1->m_SubPages[i] = tmpSubPages[i];
            pChild1->NumberOfKeys()++;
        }
        pChild1->m_SubPages[i] = tmpSubPages[i];
        m_Keys[pos - 1]        = tmpKeys[i];
        m_SubPages[pos - 1]    = pChild1;
        bstar_remove(m_Keys,     pos);
        bstar_remove(m_SubPages, pos);
        NumberOfKeys()--;

        nKeys = pChild2->GetFreeCells();
        int j = ++i;
        for (i = 0; i < nKeys; i++, j++) {
            pChild2->m_Keys[i]     = tmpKeys[j];
            pChild2->m_SubPages[i] = tmpSubPages[j];
            pChild2->NumberOfKeys()++;
        }
        pChild2->m_SubPages[i] = tmpSubPages[j];
        m_SubPages[pos]        = pChild2;

        if (Underflow()) {
            return bt_underflow;
        }
        return bt_ok;
    }

    bt_ErrorCode MergeRoot() {
        int pos = 1;
        BTPage *pChild1 = m_SubPages[pos - 1];
        BTPage *pChild2 = m_SubPages[pos];
        BTPage *pChild3 = m_SubPages[pos + 1];
        int nKeys = pChild1->NumberOfKeys() + pChild2->NumberOfKeys()
                  + pChild3->NumberOfKeys() + 2;
        vector<ObjectInfo> tmpKeys;
        vector<BTPage *>   tmpSubPages;
        MovePage(pChild1, tmpKeys, tmpSubPages);
        tmpKeys.push_back(m_Keys[pos - 1]);
        MovePage(pChild2, tmpKeys, tmpSubPages);
        tmpKeys.push_back(m_Keys[pos]);
        MovePage(pChild3, tmpKeys, tmpSubPages);
        clear();
        for (int i = 0; i < nKeys; i++) {
            m_Keys[i]     = tmpKeys[i];
            m_SubPages[i] = tmpSubPages[i];
            NumberOfKeys()++;
        }
        m_SubPages[nKeys] = tmpSubPages[nKeys];
        pChild1->Destroy();
        pChild2->Destroy();
        pChild3->Destroy();
        return bt_rootmerged;
    }

    ObjectInfo &GetFirstObjectInfo() {
        if (m_SubPages[0]) {
            return m_SubPages[0]->GetFirstObjectInfo();
        }
        return m_Keys[0];
    }

 
    // Recorridos internos con variadic
  
    template <typename Func, typename... Args>
    void internoInorden(BTPage *p, Func fn, Args&&... args) {
        if (!p) { return; }
        for (int i = 0; i < p->m_KeyCount; i++) {
            internoInorden(p->m_SubPages[i], fn, std::forward<Args>(args)...);
            fn(p->m_Keys[i], std::forward<Args>(args)...);
        }
        internoInorden(p->m_SubPages[p->m_KeyCount], fn, std::forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    void internoPreorden(BTPage *p, Func fn, Args&&... args) {
        if (!p) { return; }
        for (int i = 0; i < p->m_KeyCount; i++) {
            fn(p->m_Keys[i], std::forward<Args>(args)...);
            internoPreorden(p->m_SubPages[i], fn, std::forward<Args>(args)...);
        }
        internoPreorden(p->m_SubPages[p->m_KeyCount], fn, std::forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    void internoPostorden(BTPage *p, Func fn, Args&&... args) {
        if (!p) { return; }
        for (int i = 0; i < p->m_KeyCount; i++) {
            internoPostorden(p->m_SubPages[i], fn, std::forward<Args>(args)...);
        }
        internoPostorden(p->m_SubPages[p->m_KeyCount], fn, std::forward<Args>(args)...);
        for (int i = 0; i < p->m_KeyCount; i++) {
            fn(p->m_Keys[i], std::forward<Args>(args)...);
        }
    }

    template <typename Func, typename... Args>
    bool internoPrimerQue(BTPage *p, ObjectInfo &resultado, Func fn, Args&&... args) {
        if (!p) { return false; }
        for (int i = 0; i < p->m_KeyCount; i++) {
            if (internoPrimerQue(p->m_SubPages[i], resultado, fn, std::forward<Args>(args)...)) {
                return true;
            }
            if (fn(p->m_Keys[i], std::forward<Args>(args)...)) {
                resultado = p->m_Keys[i];
                return true;
            }
        }
        return internoPrimerQue(p->m_SubPages[p->m_KeyCount], resultado, fn, std::forward<Args>(args)...);
    }

public:

    // constructor y destrcutor

    CBStarTreePage(int maxKeys, bool unique = true)
        : m_MaxKeys(maxKeys), m_Unique(unique), m_KeyCount(0) {
        Create();
        SetMaxKeysForChilds(m_MaxKeys);
    }

    virtual ~CBStarTreePage() { Reset(); }


    // operacion del arbol estrella

    bt_ErrorCode Insert(const value_type &key, const ref_type ObjID) {
        int pos = bstar_binary_search(m_Keys, 0, m_KeyCount, key);
        if (pos < m_KeyCount && (value_type)m_Keys[pos] == key && m_Unique) {
            return bt_duplicate;
        }
        if (IsLeaf()) {
            bstar_insert_at(m_Keys, ObjectInfo(key, ObjID), pos);
            m_KeyCount++;
            if (Overflow()) {
                return bt_overflow;
            }
            return bt_ok;
        }
        bt_ErrorCode error = m_SubPages[pos]->Insert(key, ObjID);
        if (error == bt_overflow) {
            if (!Redistribute1(pos)) {
                SplitChild(pos);
            }
            if (Overflow()) {
                return bt_overflow;
            }
            return bt_ok;
        }
        if (Overflow()) {
            return bt_overflow;
        }
        return bt_ok;
    }

    bt_ErrorCode Remove(const value_type &key, const ref_type ObjID) {
        int pos = bstar_binary_search(m_Keys, 0, m_KeyCount, key);
        bt_ErrorCode error = bt_ok;
        if (pos < NumberOfKeys() && key == m_Keys[pos].key) {
            if (!m_SubPages[pos + 1]) {
                bstar_remove(m_Keys, pos);
                NumberOfKeys()--;
                if (Underflow()) {
                    return bt_underflow;
                }
                return bt_ok;
            }
            ObjectInfo &rFirst = m_SubPages[pos + 1]->GetFirstObjectInfo();
            swap(m_Keys[pos], rFirst);
            error = m_SubPages[++pos]->Remove(key, ObjID);
        } else if (pos == NumberOfKeys()) {
            if (!m_SubPages[pos]) {
                return bt_nofound;
            }
            error = m_SubPages[pos]->Remove(key, ObjID);
        } else if (key <= m_Keys[pos].key) {
            if (m_SubPages[pos]) {
                error = m_SubPages[pos]->Remove(key, ObjID);
            } else {
                return bt_nofound;
            }
        }
        if (error == bt_underflow) {
            if (TreatUnderflow(pos)) {
                return bt_ok;
            }
            if (IsRoot() && NumberOfKeys() == 2) {
                return MergeRoot();
            }
            return Merge(pos);
        }
        if (error == bt_nofound) {
            return bt_nofound;
        }
        return bt_ok;
    }

    bool Search(const value_type &key, ref_type &ObjID) {
        int pos = bstar_binary_search(m_Keys, 0, m_KeyCount, key);
        if (pos >= m_KeyCount) {
            if (m_SubPages[pos]) {
                return m_SubPages[pos]->Search(key, ObjID);
            }
            return false;
        }
        if (key == m_Keys[pos].key) {
            ObjID = m_Keys[pos].ObjID;
            m_Keys[pos].UseCounter++;
            return true;
        }
        if (key < m_Keys[pos].key && m_SubPages[pos]) {
            return m_SubPages[pos]->Search(key, ObjID);
        }
        return false;
    }


    // imprimir tabulado

    void Print(ostream &os, int level = 0) const {
        if (m_SubPages.size() > (size_t)m_KeyCount && m_SubPages[m_KeyCount]) {
            m_SubPages[m_KeyCount]->Print(os, level + 1);
        }
        for (int i = m_KeyCount - 1; i >= 0; i--) {
            for (int t = 0; t < level; t++) {
                os << "\t";
            }
            os << m_Keys[i].key << "->" << m_Keys[i].ObjID << "\n";
            if (m_SubPages.size() > (size_t)i && m_SubPages[i]) {
                m_SubPages[i]->Print(os, level + 1);
            }
        }
    }


    // ForEach 

    void ForEach(lpfnForEach2 lpfn, int level, void *pExtra1) {
        for (int i = 0; i < m_KeyCount; i++) {
            if (m_SubPages[i]) {
                m_SubPages[i]->ForEach(lpfn, level + 1, pExtra1);
            }
            lpfn(m_Keys[i], level, pExtra1);
        }
        if (m_SubPages[m_KeyCount]) {
            m_SubPages[m_KeyCount]->ForEach(lpfn, level + 1, pExtra1);
        }
    }

    void ForEach(lpfnForEach3 lpfn, int level, void *pExtra1, void *pExtra2) {
        for (int i = 0; i < m_KeyCount; i++) {
            if (m_SubPages[i]) {
                m_SubPages[i]->ForEach(lpfn, level + 1, pExtra1, pExtra2);
            }
            lpfn(m_Keys[i], level, pExtra1, pExtra2);
        }
        if (m_SubPages[m_KeyCount]) {
            m_SubPages[m_KeyCount]->ForEach(lpfn, level + 1, pExtra1, pExtra2);
        }
    }

    ObjectInfo *FirstThat(lpfnFirstThat2 lpfn, int level, void *pExtra1) {
        ObjectInfo *pTmp;
        for (int i = 0; i < m_KeyCount; i++) {
            if (m_SubPages[i]) {
                pTmp = m_SubPages[i]->FirstThat(lpfn, level + 1, pExtra1);
                if (pTmp) {
                    return pTmp;
                }
            }
            if (lpfn(m_Keys[i], level, pExtra1)) {
                return &m_Keys[i];
            }
        }
        if (m_SubPages[m_KeyCount]) {
            pTmp = m_SubPages[m_KeyCount]->FirstThat(lpfn, level + 1, pExtra1);
            if (pTmp) {
                return pTmp;
            }
        }
        return nullptr;
    }


    // ITERADORES begin/end

    forward_iterator  begin()  { return forward_iterator(this);  }
    forward_iterator  end()    { return forward_iterator();       }
    backward_iterator rbegin() { return backward_iterator(this); }
    backward_iterator rend()   { return backward_iterator();     }
};

#endif // CBSTARTREEPAGE_H
