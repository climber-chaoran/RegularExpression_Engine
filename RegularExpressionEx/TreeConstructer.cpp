#include "StdAfx.h"
#include "TreeConstructer.h"

node_type g_NeedAddConcat[][2] = {
    {eType_NORMAL, eType_NORMAL},
    {eType_NORMAL, eType_LEFTPARENTHESIS},
    {eType_STAR, eType_NORMAL},
    {eType_STAR, eType_LEFTPARENTHESIS},
    {eType_RIGHTPARENTHESIS, eType_LEFTPARENTHESIS},
    {eType_RIGHTPARENTHESIS, eType_NORMAL},

};

CTreeConstructer::CTreeConstructer(void)
{
    m_nNodeCount    = 0;
    m_LastTokenType = eType_END;
    m_nTokenCount   = NULL;
}

CTreeConstructer::~CTreeConstructer(void)
{
    CleanStack();
}

BOOL CTreeConstructer::SetPattern(const CString strPattern)
{
    return m_Lex.SetPattern(strPattern);
}

BOOL CTreeConstructer::EvaluateOneDestOperation(CToken *pToken)
{    
    BOOL        bRet    = TRUE;
    CNodeInTree *pNode  = NULL;

    pNode = new (std::nothrow)CNodeInTree;
    m_nNodeCount++;
    CHECK_POINT(pNode);
    pNode->m_pToken = pToken;
    pNode->m_Node1 = m_stkOperateValue.top();
    m_stkOperateValue.pop();
    try
    {
        m_stkOperateValue.push(pNode);
    }
    catch (...)
    {
        bRet = FALSE;
        goto Exit0;
    }
Exit0:
    if (!bRet)
    {
        delete pNode;
    }
    return bRet;
}

BOOL CTreeConstructer::EvaluateTwoDestOperation()
{
    BOOL        bRet            = FALSE;
    CNodeInTree *pNode          = NULL;
    CToken      *pToken         = NULL;

    pToken = m_stkOperation.top();
    if (eType_UNION != pToken->GetType() && eType_CONCAT != pToken->GetType())
    {
        pToken = NULL;
        goto Exit0;
    }
    m_stkOperation.pop();

    pNode = new (std::nothrow)CNodeInTree;
    CHECK_POINT(pNode);
    m_nNodeCount++;
    pNode->m_pToken = pToken;
    pToken = NULL;

    if (m_stkOperateValue.size() < 2)
    {
        goto Exit0;
    }
    pNode->m_Node2 = m_stkOperateValue.top();
    m_stkOperateValue.pop();
    pNode->m_Node1 = m_stkOperateValue.top();
    m_stkOperateValue.pop();
    try
    {
        m_stkOperateValue.push(pNode);
    }
    catch (...)
    {
        goto Exit0;
    }
    bRet = TRUE;
Exit0:
    if (!bRet)
    {
        ReleaseNode(pNode);
    }
    if (pToken)
    {
        delete pToken;
        pToken = NULL;
    }
    return bRet;

}

BOOL CTreeConstructer::Evaluate(CToken *pCurrentToken)
{
    BOOL    bRet                    = FALSE;
    CToken  *pTokenLeftParenthesis  = NULL;

    switch (pCurrentToken->GetType())
    {
    case eType_LEFTPARENTHESIS:
        try
        {
            m_stkOperation.push(pCurrentToken);
        }
        catch (...)
        {
            goto Exit0;
        }
    	break;
    case eType_NORMAL:
        goto Exit0;
        break;
    case eType_STAR:
        if (m_stkOperateValue.empty() || 
            eType_STAR == m_stkOperateValue.top()->m_pToken->GetType() )
        {
            goto Exit0;
        }
        CHECK_BOOL ( EvaluateOneDestOperation(pCurrentToken) );
        break;
    case eType_UNION:
    case eType_CONCAT:
        while (!m_stkOperation.empty() && 
            eType_LEFTPARENTHESIS != m_stkOperation.top()->GetType() &&
            (int)(pCurrentToken->GetType()) <= (int)(m_stkOperation.top()->GetType()))
        {
            // current operation priority less or equal last operation
            CHECK_BOOL ( EvaluateTwoDestOperation() );
        }
        try
        {
            m_stkOperation.push(pCurrentToken);
        }
        catch (...)
        {
            goto Exit0;
        }
        break;
    case eType_RIGHTPARENTHESIS:
        // if there are nothing between a pair of parentheses, it is wrong
        if (eType_LEFTPARENTHESIS == m_LastTokenType)
        {
            goto Exit0;
        }
        while (!m_stkOperation.empty() &&
            eType_LEFTPARENTHESIS != (m_stkOperation.top()->GetType()))
        {
            CHECK_BOOL ( EvaluateTwoDestOperation() );
        }
        if (m_stkOperation.empty() || eType_LEFTPARENTHESIS != m_stkOperation.top()->GetType())
        {
            goto Exit0;
        }
        pTokenLeftParenthesis = m_stkOperation.top();
        delete pTokenLeftParenthesis;
        m_nTokenCount--;
        m_stkOperation.pop();
        delete pCurrentToken;
        m_nTokenCount--;
        break;
    default:
        bRet = FALSE;
    }
    bRet = TRUE;
Exit0:
    return bRet;
}

BOOL CTreeConstructer::IsNeedAddConcatToken(CToken *pToken)
{
    node_type type = pToken->GetType();
    if (eType_WILDCARD == type)
    {
        type = eType_NORMAL;
    }

    if (eType_END == m_LastTokenType)
        return FALSE;

    if (eType_END == pToken->GetType())
    {
        return TRUE;
    }

    for (int i = 0; i < sizeof(g_NeedAddConcat)/sizeof(node_type[2]); i++)
    {
        if (g_NeedAddConcat[i][0] == m_LastTokenType && 
            g_NeedAddConcat[i][1] == type)
        {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL CTreeConstructer::ConstructSyntaxTree(CNodeInTree **pNode)
{
    BOOL        bRet            = FALSE;
    CToken      *pTokenConcat   = NULL;
    CToken      *pToken         = NULL;
    CNodeInTree *pNormalNode    = NULL;
    BOOL        bLoopFlag       = TRUE;
    node_type   nodeType;

    m_LastTokenType = eType_END;
    m_nId           = 0;

    CleanStack();

    while (bLoopFlag)
    {
        pToken = m_Lex.GetToken();
        CHECK_POINT(pToken);
        m_nTokenCount++;
        
        // eType_END is not a part of pattern, but we add one eType_END token
        // to indicate the accepting states (like # flag in BOOK)
        if (eType_END == pToken->GetType())
            bLoopFlag = FALSE;
        else if (eType_ERROR == pToken->GetType())
            goto Exit0;

        if (eType_END == pToken->GetType())
        {
            // analyze TwoDestOperations in stack
            while (0 != m_stkOperation.size())
                CHECK_BOOL ( EvaluateTwoDestOperation() );
        }

        if (IsNeedAddConcatToken(pToken))
        {
            pTokenConcat = new (std::nothrow)CToken(eType_CONCAT);
            CHECK_POINT ( pTokenConcat );
            m_nTokenCount++;
            CHECK_BOOL ( Evaluate(pTokenConcat) );
            pTokenConcat = NULL;
        }

        if (pToken->IsOperation())
        {
            nodeType = pToken->GetType();
            CHECK_BOOL ( Evaluate(pToken) );
            m_LastTokenType = nodeType;
            pToken = NULL;
        }
        else
        {
            // normal char
            pToken->SetId(++m_nId);
            pNormalNode = new (std::nothrow) CNodeInTree;
            m_nNodeCount++;
            CHECK_POINT(pNormalNode);
            pNormalNode->m_pToken = pToken;
            m_LastTokenType = pToken->GetType();
            if (eType_WILDCARD == m_LastTokenType)
                m_LastTokenType = eType_NORMAL;

            pToken = NULL;
            try 
            {
                m_stkOperateValue.push(pNormalNode);
                pNormalNode = NULL;
            }
            catch (...)
            {
                goto Exit0;
            }
        }
    }

    // analyze TwoDestOperations in stack
    while (0 != m_stkOperation.size())
        CHECK_BOOL ( EvaluateTwoDestOperation() );

    if (!m_stkOperation.empty() || m_stkOperateValue.size() != 1)
    {
        goto Exit0;
    }
    *pNode = m_stkOperateValue.top();
    m_stkOperateValue.pop();
    bRet = TRUE;
Exit0:
    if (pTokenConcat)
    {
        delete pTokenConcat;
        m_nTokenCount--;
        pTokenConcat = NULL;
    }
    if (pToken)
    {
        delete pToken;
        m_nTokenCount--;
        pToken = NULL;
    }
    ReleaseNode(pNormalNode);
    return bRet;
}

void CTreeConstructer::ReleaseNode(CNodeInTree *&pNode)
{
    if (!pNode)
        return;

    if (pNode->m_pToken)
    {
        delete pNode->m_pToken;
        m_nTokenCount--;
        pNode->m_pToken = NULL;
    }

    ReleaseNode(pNode->m_Node1);
    ReleaseNode(pNode->m_Node2);

    delete pNode;
    m_nNodeCount--;
    pNode = NULL;
}

void CTreeConstructer::CleanStack()
{
    while (!m_stkOperateValue.empty())
    {
        CNodeInTree *pNode = m_stkOperateValue.top();
        m_stkOperateValue.pop();
        ReleaseNode(pNode);
    }
    while (!m_stkOperation.empty())
    {
        CToken *pToken = m_stkOperation.top();
        m_stkOperation.pop();
        delete pToken;
        m_nTokenCount--;
        pToken = NULL;
    }
}