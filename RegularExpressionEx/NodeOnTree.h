#pragma once

#include "Token.h"

#include <vector>
using namespace std;


class CNodeInTree
{
public:
    CNodeInTree(void);
    ~CNodeInTree(void);

public:
    CNodeInTree             * m_Node1;
    CNodeInTree             * m_Node2;
    CToken                  *m_pToken;
    CString                 m_str;

    BOOL                    m_bNullAble;
    vector<CNodeInTree*>    m_vecFirstPos;
    vector<CNodeInTree*>    m_vecLastPos;
    vector<CNodeInTree*>    m_vecFollowPos;

public:
    CString ShowAllNode();
    BOOL CalculateAllFunction();
    void ShowAllFunction(CNodeInTree *pNode);
    static BOOL CalculateFunction(CNodeInTree *);

private:
    void ShowNode(int nDeep, int iChild);
    void ShowPrefix(int nDeep, int iChild);
    static BOOL AppendVector(vector<CNodeInTree*> &m_vecDest, vector<CNodeInTree*> &m_vecScr);
    static BOOL CalcFollowPos(CNodeInTree *pNode);
};
