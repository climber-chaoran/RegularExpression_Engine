#pragma once

#include "NodeOnTree.h"
#include "TreeConstructer.h"
#include <list>
#include <set>
#include <map>
using namespace std;

void TestMinimizeDFA();

struct _DFANodeRelation
{
    int             m_nIdxCur;
    unsigned char   m_ch;
    int             m_nIdxNext;
    _DFANodeRelation::_DFANodeRelation(int nIdxCur, unsigned char ch, int nIdxNext)
        :m_nIdxCur(nIdxCur), m_ch(ch), m_nIdxNext(nIdxNext)
    {}
    //_DFANodeRelation::_DFANodeRelation();
};
typedef _DFANodeRelation DFANodeRelation;
typedef _DFANodeRelation* PDFANodeRelation;


class CDFA
{
public:
    CDFA(void);
    ~CDFA(void);

public:
    BOOL RegExpToDFA(CString strPattern);
    BOOL ConstructDFA();
    BOOL CreateSyntaxTree();
    BOOL CreateDFA(CNodeInTree *pNode);
    BOOL MinimizeDFA(int                     nSetSize,
                     list<DFANodeRelation>   &lstNodeRelation,
                     set<int>                &setAcceptingIdx,
                     list<set<int>>          &lstSet
        );
    CString GetTreeString()
    {
        return m_strTree;
    }

private:
    BOOL GetNextSet(set<CNodeInTree*> & setNodeTemp, set<CNodeInTree*> & setNodeNext);
    BOOL IsNodeSetInList(set<CNodeInTree*> &setNodeNext, int &nIdx);
    BOOL IsContainAcceptingState(set<CNodeInTree*> &setNode);
    BOOL PartitionGroups(list<set<int>> &lstSet, list<DFANodeRelation> &lstNodeRelation);
    BOOL PartitionGroup(list<set<int>> &lstSet, set<int> &setStates, 
                        list<DFANodeRelation> &lstNodeRelation, map<int, set<int>> &mapTemp);
    BOOL FindRelationNode(list<DFANodeRelation> &lstNodeRelation, int nIdx, unsigned char ch, int &nMapToIdx);
    int  FindIdxInListSet(int nMapToIdx, list<set<int>> &lstSet);

private:
    CNodeInTree             *m_pSyntaxNode;
    CTreeConstructer        m_TreeConstructer;
    CString                 m_strPattern;
    CString                 m_strTree;

    list<set<CNodeInTree*>> m_lstSet;
    list<set<int>>          m_lstFinalSet;
    list<DFANodeRelation>   m_lstNodeRelation;
    set<int>                m_setAcceptingIdx;

};
