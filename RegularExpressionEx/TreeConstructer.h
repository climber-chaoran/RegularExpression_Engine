#pragma once

#include "NodeOnTree.h"
#include "Lex.h"
#include <stack>

class CTreeConstructer
{
public:
    CTreeConstructer(void);
    ~CTreeConstructer(void);
public:
    BOOL ConstructSyntaxTree(CNodeOnTree **pNode);
    BOOL SetPattern(const CString strPattern);
    void ReleaseNode(CNodeOnTree *&pNode);

private:
    void CleanStack();
    BOOL IsNeedAddConcatToken(CToken *pToken);
    BOOL Evaluate(CToken *pToken);
    BOOL EvaluateOneDestOperation(CToken *pToken);
    BOOL EvaluateTwoDestOperation();

private:
    std::stack<CToken*>         m_stkOperation;
    std::stack<CNodeOnTree*>    m_stkOperateValue;
    node_type                   m_LastTokenType;
    CLex                        m_Lex;
    int                         m_nNodeCount;           // for debug
    int                         m_nTokenCount;          // for debug
    int                         m_nId;

};
