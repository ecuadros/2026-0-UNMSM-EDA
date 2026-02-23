#ifndef BTreePage_H
#define BTreePage_H

#include <utility>
#include <iostream>
#include <vector>
#include <algorithm>
#include <assert.h>

using namespace std;

enum bt_ErrorCode { bt_ok, bt_overflow, bt_underflow, bt_duplicate, bt_nofound, bt_rootmerged };

template <typename keyType, typename ObjIDType>
struct tagObjectInfo {
    keyType key;
    ObjIDType ObjID;
    int count = 0;
    int UseCounter = 0;

    tagObjectInfo() : count(0), UseCounter(0) {}
    tagObjectInfo(keyType k, ObjIDType id) : key(k), ObjID(id), count(1), UseCounter(0) {}
    operator keyType() const { return key; }
};

template <typename keyType, typename ObjIDType>
bool operator>=(const tagObjectInfo<keyType, ObjIDType>& o1, const tagObjectInfo<keyType, ObjIDType>& o2)
{ return o1.key >= o2.key; }

template <typename keyType, typename ObjIDType>
bool operator<=(const tagObjectInfo<keyType, ObjIDType>& o1, const tagObjectInfo<keyType, ObjIDType>& o2)
{ return o1.key <= o2.key; }

template <typename keyType, typename ObjIDType> class BTree;

template <typename keyType, typename ObjIDType>
class CBTreePage {
    friend class BTree<keyType, ObjIDType>;
    typedef CBTreePage<keyType, ObjIDType> BTPage;         
    typedef tagObjectInfo<keyType, ObjIDType> ObjectInfo;

public:
        CBTreePage(int maxKeys, bool unique = true);
        virtual ~CBTreePage();

        bt_ErrorCode Insert(const keyType &key, const ObjIDType ObjID);
        bool Search(const keyType &key, ObjIDType &ObjID);
        
        int GetNumberOfKeys() { return m_KeyCount; }
        bool Overflow() { return m_KeyCount > m_MaxKeys; }

        template <typename Func, typename... Args>
        void InOrderVariadic(Func f, int level, Args... args) {
        for (int i = 0; i < m_KeyCount; i++) {
                if (m_SubPages[i]) m_SubPages[i]->InOrderVariadic(f, level + 1, args...);
                f(m_Keys[i], level, args...);
        }
        if (m_SubPages[m_KeyCount]) m_SubPages[m_KeyCount]->InOrderVariadic(f, level + 1, args...);
        }

protected:
        int m_MaxKeys;
        bool m_Unique;
        vector<ObjectInfo> m_Keys;
        vector<BTPage *> m_SubPages;
        int m_KeyCount;

        void Create();
        void Reset();
        bool Redistribute1(int &pos);
        void RedistributeR2L(int pos);
        void RedistributeL2R(int pos);
        void SplitChild(int pos);
};

template <typename Container, typename ObjType>
int binary_search(Container& container, int first, int last, const ObjType &object) {
    while (first < last) {
        int mid = (first + last) / 2;
        if (object == (ObjType)container[mid]) return mid;
        if (object > (ObjType)container[mid]) first = mid + 1;
        else last = mid;
    }
    return first;
}

template <typename Container, typename ObjType>
void insert_at(Container& container, const ObjType &object, int pos, int count) {
    for (int i = count; i > pos; i--) container[i] = container[i-1];
    container[pos] = object;
}

template <typename keyType, typename ObjIDType>
CBTreePage<keyType, ObjIDType>::CBTreePage(int maxKeys, bool unique) 
    : m_MaxKeys(maxKeys), m_Unique(unique), m_KeyCount(0) {
    Create();
}

template <typename keyType, typename ObjIDType>
CBTreePage<keyType, ObjIDType>::~CBTreePage() { Reset(); }

template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::Create() {
    m_Keys.resize(m_MaxKeys + 1);
    m_SubPages.assign(m_MaxKeys + 2, nullptr);
}

template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::Reset() {
    for (auto page : m_SubPages) if (page) delete page;
    m_SubPages.assign(m_MaxKeys + 2, nullptr);
    m_KeyCount = 0;
}

template <typename keyType, typename ObjIDType>
bool CBTreePage<keyType, ObjIDType>::Search(const keyType &key, ObjIDType &ObjID) {
    int pos = ::binary_search(m_Keys, 0, m_KeyCount, key);
    if (pos < m_KeyCount && m_Keys[pos].key == key) {
        ObjID = m_Keys[pos].ObjID;
        return true;
    }
    if (m_SubPages[pos]) return m_SubPages[pos]->Search(key, ObjID);
    return false;
}

template <typename keyType, typename ObjIDType>
bt_ErrorCode CBTreePage<keyType, ObjIDType>::Insert(const keyType& key, const ObjIDType ObjID) {
    int pos = ::binary_search(m_Keys, 0, m_KeyCount, key);
    if (pos < m_KeyCount && (keyType)m_Keys[pos] == key && m_Unique) return bt_duplicate;

    if (!m_SubPages[pos]) {
        ::insert_at(m_Keys, ObjectInfo(key, ObjID), pos, m_KeyCount);
        m_KeyCount++;
        return Overflow() ? bt_overflow : bt_ok;
    } else {
        bt_ErrorCode error = m_SubPages[pos]->Insert(key, ObjID);
        if (error == bt_overflow) {
            if (!Redistribute1(pos)) SplitChild(pos);
            return Overflow() ? bt_overflow : bt_ok;
        }
        return error;
    }
}

template <typename keyType, typename ObjIDType>
bool CBTreePage<keyType, ObjIDType>::Redistribute1(int &pos) {
    if (pos > 0 && m_SubPages[pos-1]->m_KeyCount < m_MaxKeys) {
        RedistributeR2L(pos-1); return true;
    }
    if (pos < m_KeyCount && m_SubPages[pos+1]->m_KeyCount < m_MaxKeys) {
        RedistributeL2R(pos); return true;
    }
    return false;
}

template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::RedistributeL2R(int pos) {
    BTPage *L = m_SubPages[pos], *R = m_SubPages[pos+1];
    ::insert_at(R->m_Keys, m_Keys[pos], 0, R->m_KeyCount);
    for(int i=R->m_KeyCount+1; i>0; i--) R->m_SubPages[i] = R->m_SubPages[i-1];
    R->m_SubPages[0] = L->m_SubPages[L->m_KeyCount];
    m_Keys[pos] = L->m_Keys[L->m_KeyCount-1];
    R->m_KeyCount++; L->m_KeyCount--;
}

template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::RedistributeR2L(int pos) {
    BTPage *L = m_SubPages[pos], *R = m_SubPages[pos+1];
    L->m_Keys[L->m_KeyCount] = m_Keys[pos];
    L->m_SubPages[L->m_KeyCount+1] = R->m_SubPages[0];
    m_Keys[pos] = R->m_Keys[0];
    for(int i=0; i<R->m_KeyCount-1; i++) R->m_Keys[i] = R->m_Keys[i+1];
    for(int i=0; i<R->m_KeyCount; i++) R->m_SubPages[i] = R->m_SubPages[i+1];
    L->m_KeyCount++; R->m_KeyCount--;
}



template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::SplitChild(int pos) {
    BTPage* child = m_SubPages[pos];
    BTPage* newNode = new BTPage(m_MaxKeys, m_Unique);
    int mid = child->m_KeyCount / 2;
    
    int j = 0;
    for (int i = mid + 1; i < child->m_KeyCount; i++) newNode->m_Keys[j++] = child->m_Keys[i];
    if (child->m_SubPages[0]) {
        for (int i = mid + 1; i <= child->m_KeyCount; i++) newNode->m_SubPages[i-(mid+1)] = child->m_SubPages[i];
    }
    newNode->m_KeyCount = j;
    ObjectInfo upKey = child->m_Keys[mid];
    child->m_KeyCount = mid;

    ::insert_at(m_Keys, upKey, pos, m_KeyCount);
    for (int i = m_KeyCount + 1; i > pos + 1; i--) m_SubPages[i] = m_SubPages[i-1];
    m_SubPages[pos+1] = newNode;
    m_KeyCount++;
}

#endif