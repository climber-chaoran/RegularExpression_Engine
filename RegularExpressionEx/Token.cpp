#include "StdAfx.h"
#include "Token.h"

int CToken::s_nTokenCount = 0;
CToken::~CToken(void)
{
    s_nTokenCount--;
}
