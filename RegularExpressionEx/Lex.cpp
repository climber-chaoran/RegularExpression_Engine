#include "StdAfx.h"
#include "Lex.h"

unsigned char g_SpecialChar[] = {
    '.',
    '|',
    '*',
    '(',
    ')',
    '\\',
};

CLex::CLex(void)
{
}

CLex::~CLex(void)
{
}

BOOL CLex::IsSpecialChar(unsigned char ch)
{
    for (int i = 0; i < sizeof(g_SpecialChar)/sizeof(unsigned char); i++)
    {
        if (ch == g_SpecialChar[i])
            return TRUE;
    }
    return FALSE;
}

char CLex::GetNextChar()
{
    return m_strPattern.GetAt(m_CurrentIdx++);
}

CToken* CLex::GetToken()
{
    CToken *pToken = new (std::nothrow)CToken;
    CHECK_POINT(pToken);

    pToken->SetType(eType_NORMAL);

    if (m_CurrentIdx >= m_strPattern.GetLength())
    {
        pToken->SetType(eType_END);
        pToken->SetChar((unsigned char)(0x08));
        goto Exit0;
    }

    char ch = GetNextChar();
    if (IsSpecialChar(ch))
    {
        switch (ch)
        {
        case '.':
            pToken->SetType(eType_WILDCARD);
        	break;
        case '*':
            pToken->SetType(eType_STAR);
            break;
        case '|':
            pToken->SetType(eType_UNION);
            break;
        case '(':
            pToken->SetType(eType_LEFTPARENTHESIS);
            break;
        case ')':
            pToken->SetType(eType_RIGHTPARENTHESIS);
            break;
        case '\\':
            if (m_CurrentIdx >= m_strPattern.GetLength())
            {
                pToken->SetType(eType_ERROR);
            }
            else
            {
                if (!IsSpecialChar(ch = GetNextChar()))
                {
                    pToken->SetType(eType_ERROR);
                }
            }
            break;
        default:
            ;
        }
    }
    pToken->SetChar(ch);

Exit0:
    return pToken;
}

BOOL CLex::SetPattern(const CString strPattern)
{
    m_CurrentIdx = 0;
    m_strPattern = strPattern;
    return TRUE;
}