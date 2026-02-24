
//CBTreePage.h

/*************************
#ifndef BTPage_H
#define BTPage_H
***************************/
#ifndef CBTreePage_H
#define CBTreePage_H
#include <vector>
#include <iostream>
#include <assert.h>
#include "../general/types.h"

template <typename keyType, typename ObjIDType>
class BTree;


using namespace std;
enum bt_ErrorCode {bt_ok, bt_overflow, bt_underflow, bt_duplicate, bt_nofound, bt_rootmerged};

template <typename keyType, typename ObjIDType>
struct tagObjectInfo
{
       keyType                 key;
       ObjIDType               ObjID;
       long                    UseCounter;
       tagObjectInfo(const keyType     &_key, ObjIDType _ObjID)
               : key(_key), ObjID(_ObjID), UseCounter(0) {}
       tagObjectInfo()                          {}
       operator keyType                         ()     { return key; }
       long                    GetUseCounter() { return UseCounter;    }
};


template <typename keyType, typename ObjIDType>
class CBTreePage 

{
       friend class BTree<keyType, ObjIDType>;
       
       template <typename K, typename O>
       friend class ForwardBTreeIterator;
       template <typename K, typename O>
       friend class BackwardBTreeIterator;

       typedef CBTreePage<keyType, ObjIDType>    BTPage;         
       typedef tagObjectInfo<keyType, ObjIDType> ObjectInfo;

       
       using size_type = Size;
       using long_size_type = long;
       using key_type = keyType;
       using obj_id_type = ObjIDType;

       typedef void (*lpfnForEach2)(ObjectInfo &info, int level, void *pExtra1);
       typedef void (*lpfnForEach3)(ObjectInfo &info, int level, void *pExtra1, void *pExtra2);

       typedef ObjectInfo *(*lpfnFirstThat2)(ObjectInfo &info, int level, void *pExtra1);
       typedef ObjectInfo *(*lpfnFirstThat3)(ObjectInfo &info, int level, void *pExtra1, void *pExtra2);
 public:
       CBTreePage(int maxKeys, bool unique = true);
       virtual ~CBTreePage();

       bt_ErrorCode    Insert (const keyType &key, const ObjIDType ObjID);
       bt_ErrorCode    Remove (const keyType &key, const ObjIDType ObjID);
       bool            Search (const keyType &key, long &ObjID);
       void            Print  (ostream &os);
       void            ForEach(lpfnForEach2 lpfn, int level, void *pExtra1);
       void            ForEach(lpfnForEach3 lpfn, int level, void *pExtra1, void *pExtra2);
       ObjectInfo*     FirstThat(lpfnFirstThat2 lpfn, int level, void *pExtra1);
       ObjectInfo*     FirstThat(lpfnFirstThat3 lpfn, int level, void *pExtra1, void *pExtra2);

protected:
       int  m_MinKeys; 
       int  m_MaxKeys, 
                m_MaxKeysForChilds; 
       bool m_Unique;
       bool m_isRoot;
       vector<ObjectInfo> m_Keys;
       vector<BTPage *>m_SubPages;
       int  m_KeyCount;
       void  Create();
       void  Reset ();
       void  Destroy () {   Reset(); delete this;}
       void  clear ();

       bool  Redistribute1   (int &pos);
       bool  Redistribute2   (int pos);
       void  RedistributeR2L (int pos);
       void  RedistributeL2R (int pos);

       bool    TreatUnderflow  (int &pos)
       {       return Redistribute1(pos) || Redistribute2(pos);}

       bt_ErrorCode    Merge  (int pos);
       bt_ErrorCode    MergeRoot ();
       void  SplitChild (int pos);

       ObjectInfo &GetFirstObjectInfo();

       bool Overflow()  { return m_KeyCount > m_MaxKeys; }
       bool Underflow() { return m_KeyCount < MinNumberOfKeys(); }
       bool IsFull()    { return m_KeyCount >= m_MaxKeys; }
       int  MinNumberOfKeys()  { return 2*m_MaxKeys/3.0; }
       int  GetFreeCells()  { return m_MaxKeys - m_KeyCount; }
       int& NumberOfKeys()  { return m_KeyCount; }
       int  GetNumberOfKeys()  { return m_KeyCount; }
       bool IsRoot()  { return m_MaxKeysForChilds != m_MaxKeys; }
       void SetMaxKeysForChilds(int orderforchilds)
       {
               m_MaxKeysForChilds = orderforchilds;
       }

       int GetFreeCellsOnLeft(int pos);
       int GetFreeCellsOnRight(int pos);

private:
       bool SplitRoot();
       void SplitPageInto3(vector<ObjectInfo>   & tmpKeys,
                                               vector<BTPage *>  & SubPages,
                                               BTPage           *& pChild1,
                                               BTPage           *& pChild2,
                                               BTPage           *& pChild3,
                                               ObjectInfo        & oi1,
                                               ObjectInfo        & oi2);
       void MovePage(BTPage *  pChildPage,vector<ObjectInfo> & tmpKeys,vector<BTPage *> & tmpSubPages);
};

template <typename Container, typename ObjType>
int binary_search(Container& container, int first, int last, ObjType &object)
{
       if( first >= last )
               return first;
       while( first < last )
       {
               int mid = (first+last)/2;
               if( object == (ObjType)container[mid ] )
                       return mid;
               if( object > (ObjType)container[mid ] )
                       first = mid+1;
               else
                       last  = mid;
       }
       if( object <= (ObjType)container[first] )
               return first;
       return last;
}

template <typename Container, typename ObjType>
void insert_at(Container& container, const ObjType &object, int pos)
{
       int size = container.size();
       for(int i = size-2 ; i >= pos ; i--)
               container[i+1] = container[i];
       container[pos] =  object;
		
}

template <typename Container>
void remove(Container& container, int pos)
{
       int size = container.size();
       for(int i = pos+1 ; i < size ; i++)
               container[i-1] = container[i];
}

template <typename keyType, typename ObjIDType>
CBTreePage<keyType, ObjIDType>:: CBTreePage(int maxKeys, bool unique)
                                       : m_MaxKeys(maxKeys), m_Unique(unique), m_KeyCount(0)
{
       Create();
       SetMaxKeysForChilds(m_MaxKeys);
}

template <typename keyType, typename ObjIDType>
CBTreePage<keyType, ObjIDType>::~CBTreePage()
{
       Reset();
}

template <typename keyType, typename ObjIDType>
bt_ErrorCode CBTreePage<keyType, ObjIDType>::Insert(const keyType& key, const ObjIDType ObjID)
{
       int pos = binary_search(m_Keys, 0, m_KeyCount, key);
       bt_ErrorCode error = bt_ok;

       if( pos < m_KeyCount && (keyType)m_Keys[pos] == key && m_Unique)
               return bt_duplicate; 

       if( !m_SubPages[pos] ) 
       {
               ::insert_at(m_Keys, ObjectInfo(key, ObjID), pos);
               m_KeyCount++;
               if( Overflow() )
                       return bt_overflow;
               return bt_ok;
       }
       else
       {
               
               error = m_SubPages[pos]->Insert(key, ObjID);
               if( error == bt_overflow )
               {
                       if( !Redistribute1(pos) )
                               SplitChild(pos);
                       if( Overflow() )          
                               return bt_overflow;
                       return bt_ok;
               }
       }
       if( Overflow() ) 
               return bt_overflow;
       return bt_ok;
}

template <typename keyType, typename ObjIDType>
bool CBTreePage<keyType, ObjIDType>::Redistribute1(int &pos)
{
       if( m_SubPages[pos]->Underflow() )
       {
               int nkol = 0,
                       nkor = 0;
               
               if( pos > 0 )
                       nkol = m_SubPages[pos-1]->NumberOfKeys();
               if( pos < NumberOfKeys() )
                       nkor = m_SubPages[pos+1]->NumberOfKeys();

               if( nkol > nkor )
                       if( m_SubPages[pos-1]->NumberOfKeys() > m_SubPages[pos-1]->MinNumberOfKeys() )
                               RedistributeL2R(pos-1); 
                       else
                               if( pos == NumberOfKeys() )
                                       return (--pos, false);
                               else
                                       return false;
               else 
                       if( m_SubPages[pos+1]->NumberOfKeys() > m_SubPages[pos+1]->MinNumberOfKeys() )
                               RedistributeR2L(pos+1); 
                       else
                               if( pos == 0 )
                                       return (++pos, false);
                               else
                                       return false;
       }
       else 
       {
               int fcol = GetFreeCellsOnLeft(pos),   
               fcor = GetFreeCellsOnRight(pos);  

               if( !fcol && !fcor && m_SubPages[pos]->IsFull() )
                       return false;
               if( fcol > fcor )
                       RedistributeR2L(pos);
               else
                       RedistributeL2R(pos);

       }
       return true;
}


template <typename keyType, typename ObjIDType>
bool CBTreePage<keyType, ObjIDType>::Redistribute2(int pos)
{
       assert( pos > 0 && pos < NumberOfKeys()  );
       assert( m_SubPages[pos-1] != 0 && m_SubPages[pos] != 0 && m_SubPages[pos+1] != 0 );
       assert( m_SubPages[pos-1]->Underflow() ||
                       m_SubPages[ pos ]->Underflow() ||
                       m_SubPages[pos+1]->Underflow() );

       if( m_SubPages[pos-1]->Underflow() )
       {       
               RedistributeR2L(pos+1);
               RedistributeR2L(pos);
               if( m_SubPages[pos-1]->Underflow() )
                       return false;
       }
       else if( m_SubPages[pos+1]->Underflow() )
       {       
               RedistributeL2R(pos-1);
               RedistributeL2R(pos);
               if( m_SubPages[pos+1]->Underflow() )
                       return false;
       }
       else 
       {
               
               RedistributeL2R(pos-1);
               RedistributeR2L(pos+1);
               if( m_SubPages[pos]->Underflow() )
                       return false;
       }
       return true;
}

template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::RedistributeR2L(int pos)  
{
       BTPage  *pSource = m_SubPages[ pos ],
                       *pTarget = m_SubPages[pos-1];

       while(pSource->GetNumberOfKeys() > pSource->MinNumberOfKeys() &&
                 pTarget->GetNumberOfKeys() < pSource->GetNumberOfKeys() )
       {
               
               ::insert_at(pTarget->m_Keys, m_Keys[pos-1], pTarget->NumberOfKeys()++);
               
               ::insert_at(pTarget->m_SubPages, pSource->m_SubPages[0], pTarget->NumberOfKeys());

               
               m_Keys[pos-1] = pSource->m_Keys[0];

               
               ::remove(pSource->m_Keys    , 0);
               ::remove(pSource->m_SubPages, 0);
               pSource->NumberOfKeys()--;
       }
}

template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::RedistributeL2R(int pos)
{
       BTPage  *pSource = m_SubPages[pos],
                       *pTarget = m_SubPages[pos+1];
       while(pSource->GetNumberOfKeys() > pSource->MinNumberOfKeys() &&
                 pTarget->GetNumberOfKeys() < pSource->GetNumberOfKeys() )
       {
               
               ::insert_at(pTarget->m_Keys, m_Keys[pos], 0);
               
               ::insert_at(pTarget->m_SubPages, pSource->m_SubPages[pSource->NumberOfKeys()], 0);
               pTarget->NumberOfKeys()++;

               
               m_Keys[pos] = pSource->m_Keys[pSource->NumberOfKeys()-1];

               
               
               pSource->NumberOfKeys()--;
       }
}

template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::SplitChild(int pos)
{
       
       BTPage  *pChild1 = 0, *pChild2 = 0;
       if( pos > 0 )                                   
               if( m_SubPages[pos-1]->IsFull() )
               {
                       pChild1 = m_SubPages[pos-1];
                       pChild2 = m_SubPages[pos--];
               }
       if( pos < GetNumberOfKeys() )   
               if( m_SubPages[pos+1]->IsFull() )
               {
                       pChild1 = m_SubPages[pos];
                       pChild2 = m_SubPages[pos+1];
               }

       

       
       vector<ObjectInfo> tmpKeys;
       
       vector<BTPage *>   tmpSubPages;
       

      
       MovePage(pChild1, tmpKeys, tmpSubPages);
       
       tmpKeys    .push_back(m_Keys[pos]);

       
       MovePage(pChild2, tmpKeys, tmpSubPages);

       BTPage *pChild3 = 0;
       ObjectInfo oi1, oi2;
       SplitPageInto3(tmpKeys, tmpSubPages, pChild1, pChild2, pChild3, oi1, oi2);

       
       m_Keys    [pos] = oi1;
       m_SubPages[pos] = pChild1;

       
       ::insert_at(m_Keys, oi2, pos+1);
       ::insert_at(m_SubPages, pChild2, pos+1);
       NumberOfKeys()++;

       m_SubPages[pos+2] = pChild3;
}

template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::SplitPageInto3(vector<ObjectInfo>& tmpKeys,
                                                vector<BTPage *>  & tmpSubPages,
                                                BTPage*                   &     pChild1,
                                                BTPage*                   &     pChild2,
                                                BTPage*                   &     pChild3,
                                                ObjectInfo                & oi1,
                                                ObjectInfo                & oi2)
{
       assert(tmpKeys.size() >= 8);
       assert(tmpSubPages.size() >= 9);
       if( !pChild1 )
               pChild1 = new BTPage(m_MaxKeysForChilds, m_Unique);

       
       pChild1->clear();
       int nKeys = (tmpKeys.size()-2)/3;
       int i = 0;
       for( ; i < nKeys; i++ )
       {
               pChild1->m_Keys    [i] = tmpKeys    [i];
               pChild1->m_SubPages[i] = tmpSubPages[i];
               pChild1->NumberOfKeys()++;
       }
       pChild1->m_SubPages[i] = tmpSubPages[i];

       
       oi1 = tmpKeys[i++];

       if( !pChild2 )
               pChild2 = new BTPage(m_MaxKeysForChilds, m_Unique);
       pChild2->clear();
       
       nKeys += (tmpKeys.size()-2)/3 + 1;
       int j = 0;
       for(; i < nKeys; i++, j++ )
       {
               pChild2->m_Keys    [j] = tmpKeys    [i];
               pChild2->m_SubPages[j] = tmpSubPages[i];
               pChild2->NumberOfKeys()++;
       }
       pChild2->m_SubPages[j] = tmpSubPages[i];

       
       oi2 = tmpKeys[i++];

       
       if( !pChild3 )
               pChild3 = new BTPage(m_MaxKeysForChilds, m_Unique);
       pChild3->clear();
       nKeys = tmpKeys.size();
       for(j = 0; i < nKeys; i++, j++)
       {
               pChild3->m_Keys    [j] = tmpKeys    [i];
               pChild3->m_SubPages[j] = tmpSubPages[i];
               pChild3->NumberOfKeys()++;
       }
       pChild3->m_SubPages[j] = tmpSubPages[i];
}

template <typename keyType, typename ObjIDType>
bool CBTreePage<keyType, ObjIDType>::SplitRoot()
{
       BTPage  *pChild1 = 0, *pChild2 = 0, *pChild3 = 0;
       ObjectInfo oi1, oi2;
       SplitPageInto3( m_Keys,m_SubPages,pChild1, pChild2, pChild3, oi1, oi2);
       clear();

       
       m_Keys    [0] = oi1;
       m_SubPages[0] = pChild1;
       NumberOfKeys()++;

       
       m_Keys    [1] = oi2;
       m_SubPages[1] = pChild2;
       NumberOfKeys()++;

       m_SubPages[2] = pChild3;
       return true;
}

template <typename keyType, typename ObjIDType>
bool CBTreePage<keyType, ObjIDType>::Search(const keyType &key, long &ObjID)
{
       int pos = binary_search(m_Keys, 0, m_KeyCount, key);
       if( pos >= m_KeyCount ){
               if( m_SubPages[pos] )
                       return m_SubPages[pos]->Search(key, ObjID);
               else
                       return false;
       }
       if( key == m_Keys[pos].key )
       {
               ObjID = m_Keys[pos].ObjID;
               m_Keys[pos].UseCounter++;
               return true;
       }
       if( key < m_Keys[pos].key )
               if( m_SubPages[pos] )
                       return m_SubPages[pos]->Search(key, ObjID);
       return false;
}


template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::ForEach(lpfnForEach2 lpfn, int level, void *pExtra1)
{
       for( int i = 0 ; i < m_KeyCount ; i++)
       {
               if( m_SubPages[i] )
                       m_SubPages[i]->ForEach(lpfn, level+1, pExtra1);
               lpfn(m_Keys[i], level, pExtra1);
       }
       if( m_SubPages[m_KeyCount] )
               m_SubPages[m_KeyCount]->ForEach(lpfn, level+1, pExtra1);
}

template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::ForEach(lpfnForEach3 lpfn, int level, void *pExtra1, void *pExtra2)
{
       for( int i = 0 ; i < m_KeyCount ; i++)
       {
               if( m_SubPages[i] )
                       m_SubPages[i]->ForEach(lpfn, level+1, pExtra1, pExtra2);
               lpfn(m_Keys[i], level, pExtra1, pExtra2);
       }
       if( m_SubPages[m_KeyCount] )
               m_SubPages[m_KeyCount]->ForEach(lpfn, level+1, pExtra1, pExtra2);
}

template <typename keyType, typename ObjIDType>
typename CBTreePage<keyType, ObjIDType>::ObjectInfo *
CBTreePage<keyType, ObjIDType>::FirstThat(lpfnFirstThat2 lpfn,
                                          int level, void *pExtra1)
{
       ObjectInfo *pTmp;
       for( int i = 0 ; i < m_KeyCount ; i++)
       {
               if( m_SubPages[i] ){
                        pTmp = m_SubPages[i]->FirstThat(lpfn, level+1, pExtra1);
                       if( pTmp )
                               return pTmp;
               }
               if( lpfn(m_Keys[i], level, pExtra1) )
                       return &m_Keys[i];
       }
       if( m_SubPages[m_KeyCount] ){
                pTmp = m_SubPages[m_KeyCount]->FirstThat(lpfn, level+1, pExtra1);
               if( pTmp )
                       return pTmp;
       }
       return 0;
}

template <typename keyType, typename ObjIDType>
typename CBTreePage<keyType, ObjIDType>::ObjectInfo *
CBTreePage<keyType, ObjIDType>::FirstThat(lpfnFirstThat3 lpfn,int level, void *pExtra1, void *pExtra2)
{
       ObjectInfo *pTmp;
       for( int i = 0 ; i < m_KeyCount ; i++){
               if( m_SubPages[i] ){
                       pTmp = m_SubPages[i]->FirstThat(lpfn, level+1, pExtra1, pExtra2);
                       if( pTmp )
                           return pTmp;
               }
               if( lpfn(m_Keys[i], level, pExtra1, pExtra2) )
                       return &m_Keys[i];
       }
        if( m_SubPages[m_KeyCount] )
        {       pTmp = m_SubPages[m_KeyCount]->FirstThat(lpfn, level+1, pExtra1, pExtra2);
                if( pTmp ) 
                return pTmp;
        }
        return 0;
}

template <typename keyType, typename ObjIDType>
bt_ErrorCode CBTreePage<keyType, ObjIDType>::Remove(const keyType &key, const ObjIDType ObjID)
{
       bt_ErrorCode error = bt_ok;
       int pos = binary_search(m_Keys, 0, m_KeyCount, key);
       
       if( pos < NumberOfKeys() && key == m_Keys[pos].key ) 
       {
               
               int firstEq = pos;
               while( firstEq > 0 && m_Keys[firstEq - 1].key == key ) firstEq--;
               int lastEq = pos;
               while( lastEq + 1 < NumberOfKeys() && m_Keys[lastEq + 1].key == key ) lastEq++;

               
               bool foundExact = false;
               for( int i = firstEq; i <= lastEq; ++i ) {
                       if( m_Keys[i].ObjID == ObjID ) {
                               pos = i;
                               foundExact = true;
                               break;
                       }
               }
               
               if( !foundExact ) {
                       if( m_SubPages[firstEq] )
                               return m_SubPages[firstEq]->Remove(key, ObjID);
                       return bt_nofound;
               }

               
               if( !m_SubPages[pos+1] )
               {
                       ::remove(m_Keys, pos);
                       NumberOfKeys()--;
                       if( Underflow() )
                               return bt_underflow;
                       return bt_ok;
               }

               
               {
                       
                       ObjectInfo &rFirstFromRight = m_SubPages[pos+1]->GetFirstObjectInfo();
                       
                       swap(m_Keys[pos], rFirstFromRight);
                       
                       error = m_SubPages[++pos]->Remove(key, ObjID);
               }
       }
       else if( pos == NumberOfKeys() ) 
               error = m_SubPages[pos]->Remove(key, ObjID);
       else if( key <= m_Keys[pos].key ){
               if( m_SubPages[pos] )
                       error = m_SubPages[pos]->Remove(key, ObjID);
               else
                       return bt_nofound;
       }
       if( error == bt_underflow ){
               
               if( TreatUnderflow(pos) )
                       return bt_ok;
               
               if( IsRoot() && NumberOfKeys() == 2 )
                       return MergeRoot();
               return Merge(pos);
       }
       if( error == bt_nofound )
               return bt_nofound;
       return bt_ok;
}


template <typename keyType, typename ObjIDType>
bt_ErrorCode CBTreePage<keyType, ObjIDType>::Merge(int pos)
{
       assert( m_SubPages[pos-1]->NumberOfKeys() +
                       m_SubPages[ pos ]->NumberOfKeys() +
                       m_SubPages[pos+1]->NumberOfKeys() ==
                       3*m_SubPages[ pos ]->MinNumberOfKeys() - 1);

       
       vector<ObjectInfo> tmpKeys;
       
       vector<BTPage *>   tmpSubPages;

       BTPage  *pChild1 = m_SubPages[pos-1],
                       *pChild2 = m_SubPages[ pos ],
                       *pChild3 = m_SubPages[pos+1];
       MovePage(pChild1, tmpKeys, tmpSubPages);
       tmpKeys    .push_back(m_Keys[pos-1]);
       MovePage(pChild2, tmpKeys, tmpSubPages);
       tmpKeys    .push_back(m_Keys[ pos ]);
       MovePage(pChild3, tmpKeys, tmpSubPages);
       pChild3->Destroy();;

       
       int nKeys = pChild1->GetFreeCells();
       int i = 0;
       for( ; i < nKeys ; i++ )
       {
               pChild1->m_Keys    [i] = tmpKeys    [i];
               pChild1->m_SubPages[i] = tmpSubPages[i];
               pChild1->NumberOfKeys()++;
       }
       pChild1->m_SubPages[i] = tmpSubPages[i];

       m_Keys    [pos-1] = tmpKeys[i];
       m_SubPages[pos-1] = pChild1;

       ::remove(m_Keys    , pos);
       ::remove(m_SubPages, pos);
       NumberOfKeys()--;

       nKeys = pChild2->GetFreeCells();
       int j = ++i;
       for(i = 0 ; i < nKeys ; i++, j++ )
       {
               pChild2->m_Keys    [i] = tmpKeys    [j];
               pChild2->m_SubPages[i] = tmpSubPages[j];
               pChild2->NumberOfKeys()++;
       }
       pChild2->m_SubPages[i] = tmpSubPages[j];
       m_SubPages[ pos ]          = pChild2;

       if( Underflow() )
               return bt_underflow;
       return bt_ok;
}

template <typename keyType, typename ObjIDType>
bt_ErrorCode CBTreePage<keyType, ObjIDType>::MergeRoot()
{
       int pos = 1;
       assert( m_SubPages[pos-1]->NumberOfKeys() +
                       m_SubPages[ pos ]->NumberOfKeys() +
                       m_SubPages[pos+1]->NumberOfKeys() ==
                       3*m_SubPages[ pos ]->MinNumberOfKeys() - 1);

       BTPage  *pChild1 = m_SubPages[pos-1], *pChild2 = m_SubPages[ pos ], *pChild3 = m_SubPages[pos+1];
       int nKeys = pChild1->NumberOfKeys() + pChild2->NumberOfKeys() + pChild3->NumberOfKeys() + 2;

       
       vector<ObjectInfo> tmpKeys;
      
       vector<BTPage *>   tmpSubPages;

       MovePage(pChild1, tmpKeys, tmpSubPages);
       tmpKeys    .push_back(m_Keys[pos-1]);
       MovePage(pChild2, tmpKeys, tmpSubPages);
       tmpKeys    .push_back(m_Keys[ pos ]);
       MovePage(pChild3, tmpKeys, tmpSubPages);

       clear();
       int i = 0;
       for( ; i < nKeys ; i++ ){
               m_Keys    [i] = tmpKeys    [i];
               m_SubPages[i] = tmpSubPages[i];
               NumberOfKeys()++;
       }
       m_SubPages[i] = tmpSubPages[i];

       
       pChild1->Destroy();
       pChild2->Destroy();
       pChild3->Destroy();

       return bt_rootmerged;
}

template <typename keyType, typename ObjIDType>
typename CBTreePage<keyType, ObjIDType>::ObjectInfo &
CBTreePage<keyType, ObjIDType>::GetFirstObjectInfo()
{
       if( m_SubPages[0] )
               return m_SubPages[0]->GetFirstObjectInfo();
       return m_Keys[0];
}

template <typename keyType, typename ObjIDType>
void Print(tagObjectInfo<keyType, ObjIDType> &info, int level, void *pExtra)
{
        ostream &os = *(ostream *)pExtra;
        for( int i = 0; i < level ; i++)
                os << "\t";
        os << info.key << "->" << info.ObjID << "\n";
}

template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::Print(ostream & os)
{
       lpfnForEach2 lpfn = &::Print<keyType, ObjIDType>;
       ForEach(lpfn, 0, &os);
}

template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::Create()
{
       Reset();
       m_Keys.resize(m_MaxKeys+1);
       m_SubPages.resize(m_MaxKeys+2, NULL);
       m_KeyCount = 0;
       m_MinKeys  = 2 * m_MaxKeys/3;
}

template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::Reset()
{
       for( int i = 0 ; i < m_KeyCount ; i++ )
               delete m_SubPages[i];
       clear();
}

template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::clear()
{
       
       m_KeyCount = 0;
}

template <typename keyType, typename ObjIDType>
CBTreePage<keyType, ObjIDType> * CreateBTreeNode (int maxKeys, int unique)
{
       return new CBTreePage<keyType, ObjIDType> (maxKeys, unique);
}

template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::MovePage(BTPage *pChildPage, vector<ObjectInfo> &tmpKeys,vector<BTPage *> &tmpSubPages)
{
       int nKeys = pChildPage->GetNumberOfKeys();
       int i = 0;
       for( ; i < nKeys; i++ )
       {
               tmpKeys    .push_back(pChildPage->m_Keys[i]);
               tmpSubPages.push_back(pChildPage->m_SubPages[i]);
       }
       tmpSubPages.push_back(pChildPage->m_SubPages[i]);
       pChildPage->clear();
}

template <typename keyType, typename ObjIDType>
int CBTreePage<keyType, ObjIDType>::GetFreeCellsOnLeft(int pos)
{
       if( pos > 0 )                                   
               return m_SubPages[pos-1]->GetFreeCells();
       return 0;
}

template <typename keyType, typename ObjIDType>
int CBTreePage<keyType, ObjIDType>::GetFreeCellsOnRight(int pos)
{
       if( pos < GetNumberOfKeys() )  
               return m_SubPages[pos+1]->GetFreeCells();
       return 0;
}

#endif