#include "StdAfx.h"
#include "DFA.h"

void TestMinimizeDFA()
{
    int                         nSetSize = 5;
    list<DFANodeRelation>       lstNodeRelation;
    set<int>                    setAcceptingIdx;
    list<set<int>>              lstSet;
    CDFA                        dfa;

    lstNodeRelation.push_back(DFANodeRelation(0, 'a', 1));
    lstNodeRelation.push_back(DFANodeRelation(0, 'b', 2));
    lstNodeRelation.push_back(DFANodeRelation(1, 'a', 1));
    lstNodeRelation.push_back(DFANodeRelation(1, 'b', 3));
    lstNodeRelation.push_back(DFANodeRelation(2, 'a', 1));
    lstNodeRelation.push_back(DFANodeRelation(2, 'b', 2));
    lstNodeRelation.push_back(DFANodeRelation(3, 'a', 1));
    lstNodeRelation.push_back(DFANodeRelation(3, 'b', 4));
    lstNodeRelation.push_back(DFANodeRelation(4, 'a', 1));
    lstNodeRelation.push_back(DFANodeRelation(4, 'b', 2));

    setAcceptingIdx.insert(4);

    dfa.MinimizeDFA(nSetSize, lstNodeRelation, setAcceptingIdx, lstSet);

    return;
}

CDFA::CDFA(void)
{
    m_pSyntaxNode = NULL;
}

CDFA::~CDFA(void)
{
    m_TreeConstructer.ReleaseNode(m_pSyntaxNode);
}

BOOL CDFA::RegExpToDFA(CString strPattern)
{
    BOOL bRet = FALSE;

    if ("" == strPattern)
        goto Exit0;

    if (strPattern == m_strPattern)
        return TRUE;

    CHECK_BOOL ( m_TreeConstructer.SetPattern(strPattern) );
    CHECK_BOOL ( ConstructDFA() );

    bRet = TRUE;
Exit0:
    return bRet;
}

BOOL CDFA::FindRelationNode(list<DFANodeRelation> &lstNodeRelation, 
                            int nIdx, unsigned char ch, int &nMapToIdx)
{
    list<DFANodeRelation>::iterator it = lstNodeRelation.begin();
    for ( ; it != lstNodeRelation.end(); it++)
    {
        if (it->m_nIdxCur == nIdx && it->m_ch == ch)
        {
            nMapToIdx = it->m_nIdxNext;
            return TRUE;
        }
    }
    return FALSE;
}

int CDFA::FindIdxInListSet(int nMapToIdx, list<set<int>> &lstSet)
{
    int i = 0;
    for (list<set<int>>::iterator it = lstSet.begin(); it != lstSet.end(); it++, i++)
    {
        set<int> & setIdx = *it;
        for (set<int>::iterator itInt = setIdx.begin(); itInt != setIdx.end(); itInt++)
        {
            if (nMapToIdx == *itInt)
            {
                return i;
            }
        }
    }
    return -1;
}

BOOL CDFA::PartitionGroup(list<set<int>> &lstSet, set<int> &setStates, 
                          list<DFANodeRelation> &lstNodeRelation, 
                          map<int, set<int>> &mapTemp)
{
    BOOL            bRet            = FALSE;
    list<DFANodeRelation>::iterator itRelation;
    set<unsigned char>              setChar;
    set<int>                        setMapToIdx;

    try
    {
        // collect each node's translation char in the set
        for (set<int>::iterator it = setStates.begin(); it != setStates.end(); it++)
        {
            for (itRelation = lstNodeRelation.begin(); itRelation != lstNodeRelation.end(); itRelation++)
            {
                if (itRelation->m_nIdxCur == *it)
                {
                    setChar.insert(itRelation->m_ch);
                }
            }
        }
        // end collect

        for (set<unsigned char>::iterator it = setChar.begin(); it != setChar.end(); it++)
        {
            mapTemp.clear();
            int nMapToIdx = -1; // indicate map to a dead state, there no translation for this pair of node/char
            for (set<int>::iterator itSet = setStates.begin(); itSet != setStates.end(); itSet++)
            {
                if (FindRelationNode(lstNodeRelation, *itSet, *it, nMapToIdx))
                {
                    int nIdx = FindIdxInListSet(nMapToIdx, lstSet);
                    if (nIdx == -1)
                        assert(FALSE);
                    mapTemp[nIdx].insert(*itSet);
                }
                else
                    mapTemp[-1].insert(*itSet);
            }
            if (mapTemp.size() > 1)// had distinguish
            {
                break;
            }
        }
    }
    catch (...)
    {
        goto Exit0;
    }

    bRet = TRUE;
Exit0:
    return bRet;
}

BOOL CDFA::PartitionGroups(list<set<int>> &lstSet, list<DFANodeRelation> &lstNodeRelation)
{
    BOOL                        bRet   = FALSE;
    list<set<int>>::iterator    it     = lstSet.begin();
    map<int, set<int>>          mapTemp;

    for ( ; it != lstSet.end(); )
    {
        mapTemp.clear();
        set<int> &setStates = *it;
        CHECK_BOOL ( PartitionGroup(lstSet, setStates, lstNodeRelation, mapTemp) );
        if (mapTemp.size() > 1)
        {
            for (map<int, set<int>>::iterator itM = mapTemp.begin(); itM != mapTemp.end(); itM++)
            {
                try
                {
                    lstSet.push_back(itM->second);
                }
                catch (...)
                {
                    goto Exit0;
                }
            }
            it = lstSet.erase(it);

        }
        else
             it++;
    }

    bRet = TRUE;
Exit0:
    return bRet;
}

BOOL CDFA::MinimizeDFA(int                     nSetSize,
                       list<DFANodeRelation>   &lstNodeRelation,
                       set<int>                &setAcceptingIdx,
                       list<set<int>>          &lstSet
)
{
    BOOL            bRet            = FALSE;
    set<int>        setUnAccepting;

    assert(nSetSize >= 1);
    assert(setAcceptingIdx.size() != 0);
    assert(lstNodeRelation.size() != 0);

    lstSet.clear();

    try 
    {
        // get unAccepting set
        for (int i = 0; i < nSetSize; i++)
        {
            if (setAcceptingIdx.find(i) == setAcceptingIdx.end())
            {
                setUnAccepting.insert(i);
            }
        }

        lstSet.push_back(setAcceptingIdx);
        if (setUnAccepting.size() > 0)
        {
            lstSet.push_back(setUnAccepting);
        }
    }
    catch (...)
    {
        goto Exit0;
    }

    CHECK_BOOL ( PartitionGroups(lstSet, lstNodeRelation) );

    bRet = TRUE;
Exit0:
    
    return bRet;
}

BOOL CDFA::ConstructDFA()
{
    BOOL bRet = FALSE;
    CHECK_BOOL ( CreateSyntaxTree() );
    CHECK_BOOL ( m_pSyntaxNode->CalculateAllFunction() );
    m_pSyntaxNode->ShowAllFunction(m_pSyntaxNode);
    CHECK_BOOL ( CreateDFA(m_pSyntaxNode) );
    CHECK_BOOL ( MinimizeDFA(m_lstSet.size(), m_lstNodeRelation, 
                             m_setAcceptingIdx, m_lstFinalSet) );

    assert(m_lstFinalSet.size() <= m_lstSet.size());

    bRet = TRUE;
Exit0:
    return bRet;
}

BOOL CDFA::CreateSyntaxTree()
{
    BOOL bRet = FALSE;
    m_TreeConstructer.ReleaseNode(m_pSyntaxNode);
    CHECK_BOOL ( m_TreeConstructer.ConstructSyntaxTree(&m_pSyntaxNode) );
    m_strTree = m_pSyntaxNode->ShowAllNode();

    bRet = TRUE;
Exit0:

    return bRet;
}

BOOL CDFA::IsNodeSetInList(set<CNodeInTree*> &setNodeNext, int &nIdx)
{
    list<set<CNodeInTree*>>::iterator it = m_lstSet.begin();
    for (nIdx = 0; it != m_lstSet.end(); it++, nIdx++)
    {
        if (*it == setNodeNext)
        {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL CDFA::IsContainAcceptingState(set<CNodeInTree*> &setNode)
{
    for (set<CNodeInTree*>::iterator it = setNode.begin(); it != setNode.end(); it++)
    {
        if (eType_END == (*it)->m_pToken->GetType())
        {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL CDFA::CreateDFA(CNodeInTree *pNode)
{
    int                                     nIdxCur = 0;
    int                                     nIdxNext= 0;
    BOOL                                    bRet    = FALSE;
    vector<CNodeInTree*>::iterator          itNode  = pNode->m_vecFirstPos.begin();
    set<CNodeInTree*>                       setNode;
    map<unsigned char, set<CNodeInTree*>>   mapSet;

    m_lstSet.clear();
    m_lstNodeRelation.clear();
    m_setAcceptingIdx.clear();

    try 
    {
        // prepare start state
        for ( ; itNode != pNode->m_vecFirstPos.end(); itNode++ )
        {
            setNode.insert(*itNode);
        }
        m_lstSet.push_back(setNode);
        if (IsContainAcceptingState(setNode))
        {
            m_setAcceptingIdx.insert(nIdxCur);// the set is an accepting state
        }
        // end prepare start state

        // Construct DFA set, translation relation.
        list<set<CNodeInTree*>>::iterator it = m_lstSet.begin();
        for ( ; it != m_lstSet.end(); it++, nIdxCur++)
        {
            mapSet.clear();
            setNode.clear();
            setNode = *it;
            // collect all translation char into mapSet
            set<CNodeInTree*>::iterator itSet = setNode.begin();
            for ( ; itSet != setNode.end(); itSet++ )
            {
                unsigned char ch = (*itSet)->m_pToken->GetChar();
                mapSet[ch].insert(*itSet);
            }
            // end collect

            // for each translation char, get the set that correspond to translation char
            map<unsigned char, set<CNodeInTree*>>::iterator itMap = mapSet.begin();
            for ( ; itMap != mapSet.end(); itMap++ )
            {
                unsigned char ch = itMap->first;
                set<CNodeInTree*> & setNodeTemp = itMap->second;
                set<CNodeInTree*> setNodeNext;
                // get the union of followpos(p) for all p in the setNodeTemp
                CHECK_BOOL ( GetNextSet(setNodeTemp, setNodeNext) );
                if (setNodeNext.empty())
                    continue;
                if (!IsNodeSetInList(setNodeNext, nIdxNext))
                {
                    //if the set not in list, add it into list
                    m_lstSet.push_back(setNodeNext);
                    if (IsContainAcceptingState(setNodeNext))
                    {
                        m_setAcceptingIdx.insert(nIdxNext);// the set is an accepting state
                    }
                }
                DFANodeRelation stNodeRelation(nIdxCur, ch, nIdxNext);
                m_lstNodeRelation.push_back(stNodeRelation);
            }
            // end for
        }
    }
    catch (...)
    {
        goto Exit0;
    }

    bRet = TRUE;
Exit0:
    return bRet;
}

BOOL CDFA::GetNextSet(set<CNodeInTree*> & setNodeTemp, set<CNodeInTree*> & setNodeNext)
{
    BOOL                        bRet    = FALSE;
    set<CNodeInTree*>::iterator it      = setNodeTemp.begin();

    for ( ; it != setNodeTemp.end(); it++ )
    {
        vector<CNodeInTree*> & vecFollowPos = (*it)->m_vecFollowPos;
        vector<CNodeInTree*>::iterator itVec = vecFollowPos.begin();
        for ( ; itVec != vecFollowPos.end(); itVec++ )
        {
            try 
            {
                setNodeNext.insert(*itVec);
            }
            catch (...)
            {
                goto Exit0;
            }
        }

    }
    bRet = TRUE;
Exit0:
    return bRet;
}