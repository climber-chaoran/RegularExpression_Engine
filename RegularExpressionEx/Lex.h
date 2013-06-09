#pragma once

#include "Token.h"

class CLex
{
public:
    CLex(void);
    ~CLex(void);

public:
    CToken* GetToken();
    BOOL SetPattern(const CString strPattern);
    char GetNextChar();
    BOOL IsSpecialChar(unsigned char ch);

private:
    CString         m_strPattern;
    int             m_CurrentIdx;


};
