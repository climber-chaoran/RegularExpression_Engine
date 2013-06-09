#pragma once

#include "Token.h"

#include <vector>
using namespace std;


class CNodeOnTree
{
public:
    CNodeOnTree(void);
    ~CNodeOnTree(void);

public:
    CNodeOnTree             * m_Node1;
    CNodeOnTree             * m_Node2;
    CToken                  *m_pToken;
    CString                 m_str;

    BOOL                    m_bNullAble;
    vector<CNodeOnTree*>    m_vecFirstPos;
    vector<CNodeOnTree*>    m_vecLastPos;
    vector<CNodeOnTree*>    m_vecFollowPos;

public:
    CString ShowAllNode();
    BOOL CalculateAllFunction();
    static BOOL CalculateFunction(CNodeOnTree *);

private:
    void ShowNode(int nDeep, int iChild);
    void ShowPrefix(int nDeep, int iChild);
    static BOOL CopyVector(vector<CNodeOnTree*> &m_vecDest, vector<CNodeOnTree*> &m_vecScr);
    static BOOL CalcFollowPos(CNodeOnTree *pNode);
};
