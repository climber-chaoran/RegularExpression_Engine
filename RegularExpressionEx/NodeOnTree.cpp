#include "StdAfx.h"
#include "NodeOnTree.h"

CNodeInTree::CNodeInTree(void)
{
    m_Node1         = NULL;
    m_Node2         = NULL;
    m_pToken        = NULL;
}

enum 
{
    RIGHT_CHILD,
    LEFT_CHILD,
};

CNodeInTree::~CNodeInTree(void)
{
}

void CNodeInTree::ShowPrefix(int nDeep, int iChild)
{
    for (int i = 0; i < nDeep-1; i++)
        m_str += "       ";

    if (RIGHT_CHILD == iChild)
        m_str += " ©°©¤©¤";
    else
        m_str += " ©¸©¤©¤";
}

CString CNodeInTree::ShowAllNode()
{
    m_str = "";
    // if right chlid is End flag, only show left child
    if (eType_END == this->m_Node2->m_pToken->GetType())
    {
        this->m_Node1->ShowNode(1, RIGHT_CHILD);
        m_str = this->m_Node1->m_str;
    }
    else
    {
        ShowNode(1, RIGHT_CHILD);
    }
    return m_str;
}

void CNodeInTree::ShowNode(int nDeep, int iChild)
{
    if (m_Node2)
    {
        m_Node2->ShowNode(nDeep+1, RIGHT_CHILD);
        m_str += m_Node2->m_str;
    }

    ShowPrefix(nDeep, iChild);
    switch (m_pToken->GetType())
    {
    case eType_END:
    case eType_NORMAL:
    case eType_STAR:
    case eType_UNION:
    case eType_WILDCARD:
        m_str += "'";
        m_str += m_pToken->GetChar();
        m_str += "'\r\n";
        break;
    case eType_CONCAT:
        m_str += "'+'\r\n";
        break;
    default:
        assert(FALSE);
    }

    if (m_Node1)
    {
        m_Node1->ShowNode(nDeep+1, LEFT_CHILD);
        m_str += m_Node1->m_str;
    }
}

BOOL CNodeInTree::CalculateAllFunction()
{
    return CalculateFunction(this);
}

BOOL CNodeInTree::CopyVector(vector<CNodeInTree*> &m_vecDest, vector<CNodeInTree*> &m_vecScr)
{
    try
    {
        for (vector<CNodeInTree*>::iterator it = m_vecScr.begin();
            it != m_vecScr.end();
            it++)
        {
            m_vecDest.push_back(*it);
        }
    }
    catch (...)
    {
        return FALSE;
    }
    return TRUE;
}

BOOL CNodeInTree::CalculateFunction(CNodeInTree *pNode)
{
    BOOL bRet = FALSE;
    if (!pNode)
        return TRUE;

    CHECK_BOOL ( CalculateFunction(pNode->m_Node1) );
    CHECK_BOOL ( CalculateFunction(pNode->m_Node2) );
    
    switch (pNode->m_pToken->GetType())
    {
    case eType_END:
    case eType_NORMAL:
    case eType_WILDCARD:
        pNode->m_bNullAble = FALSE;
        try 
        {
            pNode->m_vecFirstPos.push_back(pNode);
            pNode->m_vecLastPos.push_back(pNode);
        }
        catch (...)
        {
            goto Exit0;
        }
    	break;
    case eType_STAR:
        pNode->m_bNullAble = TRUE;
        CHECK_BOOL ( CopyVector(pNode->m_vecFirstPos, pNode->m_Node1->m_vecFirstPos) );
        CHECK_BOOL ( CopyVector(pNode->m_vecLastPos, pNode->m_Node1->m_vecLastPos) );
        CHECK_BOOL ( CalcFollowPos(pNode) );
        break;
    case eType_UNION:
        pNode->m_bNullAble = pNode->m_Node1->m_bNullAble || pNode->m_Node2->m_bNullAble;
        CHECK_BOOL ( CopyVector(pNode->m_vecFirstPos, pNode->m_Node1->m_vecFirstPos) );
        CHECK_BOOL ( CopyVector(pNode->m_vecFirstPos, pNode->m_Node2->m_vecFirstPos) );
        CHECK_BOOL ( CopyVector(pNode->m_vecLastPos, pNode->m_Node1->m_vecLastPos) );
        CHECK_BOOL ( CopyVector(pNode->m_vecLastPos, pNode->m_Node2->m_vecLastPos) );
        break;
    case eType_CONCAT:
        pNode->m_bNullAble = pNode->m_Node1->m_bNullAble && pNode->m_Node2->m_bNullAble;
        // firstpos(n)
        CHECK_BOOL ( CopyVector(pNode->m_vecFirstPos, pNode->m_Node1->m_vecFirstPos) );
        if (pNode->m_Node1->m_bNullAble)
        {
            CHECK_BOOL ( CopyVector(pNode->m_vecFirstPos, pNode->m_Node2->m_vecFirstPos) );
        }
        // lastpos(n)
        if (pNode->m_Node2->m_bNullAble)
        {
            CHECK_BOOL ( CopyVector(pNode->m_vecLastPos, pNode->m_Node1->m_vecLastPos) );
        }
        CHECK_BOOL ( CopyVector(pNode->m_vecLastPos, pNode->m_Node2->m_vecLastPos) );
        CHECK_BOOL ( CalcFollowPos(pNode) );
        break;
    default:
        goto Exit0;
    }

    bRet = TRUE;
Exit0:
    return bRet;
}

BOOL CNodeInTree::CalcFollowPos(CNodeInTree *pNode)
{
    BOOL bRet = FALSE;

    switch (pNode->m_pToken->GetType())
    {
    case eType_STAR:
        for (vector<CNodeInTree*>::iterator it = pNode->m_vecLastPos.begin();
             it != pNode->m_vecLastPos.end();
             it++)
        {
            CopyVector((*it)->m_vecFollowPos, pNode->m_vecFirstPos);
        }
    	break;
    case eType_CONCAT:
        for (vector<CNodeInTree*>::iterator it = pNode->m_Node1->m_vecLastPos.begin();
            it != pNode->m_Node1->m_vecLastPos.end();
            it++)
        {
            CopyVector((*it)->m_vecFollowPos, pNode->m_Node2->m_vecFirstPos);
        }
        break;
    default:
        goto Exit0;
    }

    bRet = TRUE;
Exit0:
    return bRet;
}