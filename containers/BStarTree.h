#ifndef CBSTARTREE_H
#define CBSTARTREE_H

#include <iostream>
#include <fstream>
#include <mutex>
#include <utility>
#include <stdexcept>
#include "BStarTreePage.h"
#include "../foreach.h"

#define DEFAULT_BSTAR_ORDER 3

using namespace std;

template <typename Traits>
class CBStarTree {
public:
    using value_type        = typename Traits::T;
    using BTPage            = CBStarTreePage<Traits>;
    using ObjectInfo        = BStarObjectInfo<value_type, ref_type>;
    using forward_iterator  = BStarForwardIterator<Traits>;
    using backward_iterator = BStarBackwardIterator<Traits>;
    using lpfnForEach2      = typename BTPage::lpfnForEach2;
    using lpfnForEach3      = typename BTPage::lpfnForEach3;
    using lpfnFirstThat2    = typename BTPage::lpfnFirstThat2;
    using lpfnFirstThat3    = typename BTPage::lpfnFirstThat3;

private:
    BTPage             m_Root;
    long               m_NumKeys = 0;
    bool               m_Unique  = true;
    int                m_Order   = DEFAULT_BSTAR_ORDER;
    int                m_Height  = 1;
    mutable std::mutex m_mutex;

public:
    
    // CONSTRUCTOR
    
    CBStarTree(int order = DEFAULT_BSTAR_ORDER, bool unique = true)
        : m_Root(2 * order + 1, unique),
          m_NumKeys(0),
          m_Unique(unique),
          m_Order(order),
          m_Height(1) {
        m_Root.SetMaxKeysForChilds(order);
    }

    
    // MOVE CONSTRUCTOR

    CBStarTree(CBStarTree<Traits> &&other) noexcept
        : m_Root(other.m_Order * 2 + 1, other.m_Unique),
          m_NumKeys(0),
          m_Unique(other.m_Unique),
          m_Order(other.m_Order),
          m_Height(1) {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        std::swap(m_Root.m_Keys,     other.m_Root.m_Keys);
        std::swap(m_Root.m_SubPages, other.m_Root.m_SubPages);
        m_Root.m_KeyCount         = std::exchange(other.m_Root.m_KeyCount, 0);
        m_Root.m_MaxKeys          = other.m_Root.m_MaxKeys;
        m_Root.m_MinKeys          = other.m_Root.m_MinKeys;
        m_Root.m_MaxKeysForChilds = other.m_Root.m_MaxKeysForChilds;
        m_NumKeys                 = std::exchange(other.m_NumKeys, 0);
        m_Height                  = std::exchange(other.m_Height,  1);
    }

    CBStarTree(const CBStarTree &) = delete;
    CBStarTree &operator=(const CBStarTree &) = delete;

    // DESTRCUTOR
    virtual ~CBStarTree() = default;



    bool Insert(const value_type &key, const ref_type ObjID = -1) {
        std::lock_guard<std::mutex> lock(m_mutex);
        bt_ErrorCode error = m_Root.Insert(key, ObjID);
        if (error == bt_duplicate) {
            return false;
        }
        ++m_NumKeys;
        if (error == bt_overflow) {
            m_Root.SplitRoot();
            ++m_Height;
        }
        return true;
    }

    bool Remove(const value_type &key, const ref_type ObjID = -1) {
        std::lock_guard<std::mutex> lock(m_mutex);
        bt_ErrorCode error = m_Root.Remove(key, ObjID);
        if (error == bt_duplicate || error == bt_nofound) {
            return false;
        }
        --m_NumKeys;
        if (error == bt_rootmerged) {
            --m_Height;
        }
        return true;
    }

    ref_type Search(const value_type &key) {
        std::lock_guard<std::mutex> lock(m_mutex);
        ref_type ObjID = -1;
        m_Root.Search(key, ObjID);
        return ObjID;
    }



    long size()     const { std::lock_guard<std::mutex> lock(m_mutex); return m_NumKeys; }
    long height()   const { std::lock_guard<std::mutex> lock(m_mutex); return m_Height;  }
    long GetOrder() const { return m_Order; }

    bool isEmpty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_NumKeys == 0;
    }



    std::mutex &getMutex() { return m_mutex; }

    forward_iterator begin() {
        return forward_iterator(&m_Root);
    }

    forward_iterator end() {
        return forward_iterator();
    }

    backward_iterator rbegin() {
        return backward_iterator(&m_Root);
    }

    backward_iterator rend() {
        return backward_iterator();
    }


    // RECORRIDOS VARIADIC
   


    template <typename Func, typename... Args>
    void inorden(Func fn, Args&&... args) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_Root.internoInorden(&m_Root, fn, std::forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    void preorden(Func fn, Args&&... args) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_Root.internoPreorden(&m_Root, fn, std::forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    void postorden(Func fn, Args&&... args) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_Root.internoPostorden(&m_Root, fn, std::forward<Args>(args)...);
    }


    // FOREACH VARIADIC 


    template <typename ObjFunc, typename... Args>
    void Foreach(ObjFunc of, Args&&... args) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_Root.internoInorden(&m_Root, of, std::forward<Args>(args)...);
    }


    // FIRSTTHAT VARIADIC


    template <typename Func, typename... Args>
    bool firstThat(ObjectInfo &resultado, Func fn, Args&&... args) {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_Root.internoPrimerQue(&m_Root, resultado, fn, std::forward<Args>(args)...);
    }


    template <typename ObjFunc, typename... Args>
    auto FirstThat(ObjFunc of, Args&&... args) {
        return ::FirstThat(begin(), end(), of, std::forward<Args>(args)...);
    }

    
    // ForEach / FirstThat 
    

    void ForEach(lpfnForEach2 lpfn, void *pExtra1) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_Root.ForEach(lpfn, 0, pExtra1);
    }

    void ForEach(lpfnForEach3 lpfn, void *pExtra1, void *pExtra2) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_Root.ForEach(lpfn, 0, pExtra1, pExtra2);
    }

    ObjectInfo *FirstThat(lpfnFirstThat2 lpfn, void *pExtra1) {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_Root.FirstThat(lpfn, 0, pExtra1);
    }

    
    // PERSISTENCIA
    

    void SaveToFile(const string &filename) {
        std::lock_guard<std::mutex> lock(m_mutex);
        ofstream outFile(filename);
        if (!outFile.is_open()) {
            throw std::runtime_error("CBStarTree::SaveToFile: No se puede abrir el archivo.");
        }
        outFile << m_NumKeys << "\n";
        m_Root.ForEach(
            [](BStarObjectInfo<value_type, ref_type> &info, int, void *pExtra) {
                ostream &os = *(ostream *)pExtra;
                os << info.key << " " << info.ObjID << "\n";
            },
            0, &outFile);
        outFile.close();
    }

    void LoadFromFile(const string &filename) {
        std::lock_guard<std::mutex> lock(m_mutex);
        ifstream inFile(filename);
        if (!inFile.is_open()) {
            throw std::runtime_error("CBStarTree::LoadFromFile: No se puede abrir el archivo.");
        }
        long n;
        inFile >> n;
        value_type key;
        ref_type   ref;
        while (inFile >> key >> ref) {
            bt_ErrorCode error = m_Root.Insert(key, ref);
            if (error != bt_duplicate) {
                ++m_NumKeys;
                if (error == bt_overflow) {
                    m_Root.SplitRoot();
                    ++m_Height;
                }
            }
        }
        inFile.close();
    }

    
    // OPERADOR <<
    

    friend ostream &operator<<(ostream &os, CBStarTree<Traits> &tree) {
        std::lock_guard<std::mutex> lock(tree.m_mutex);
        os << "CBStarTree: size=" << tree.m_NumKeys
           << " | height=" << tree.m_Height
           << " | order="  << tree.m_Order << "\n";
        os << "--- Estructura (inorden inverso con tabulacion) ---\n";
        tree.m_Root.Print(os, 0);
        os << "---------------------------------------------------\n";
        return os;
    }

    
    // OPERADOR >>
    

    friend istream &operator>>(istream &is, CBStarTree<Traits> &tree) {
        value_type key;
        ref_type   ref;
        if (is >> key >> ref) {
            std::lock_guard<std::mutex> lock(tree.m_mutex);
            bt_ErrorCode error = tree.m_Root.Insert(key, ref);
            if (error != bt_duplicate) {
                ++tree.m_NumKeys;
                if (error == bt_overflow) {
                    tree.m_Root.SplitRoot();
                    ++tree.m_Height;
                }
            }
        }
        return is;
    }
};

#endif // CBSTARTREE_H
