#pragma once

enum node_type
{
    eType_NORMAL,
    eType_WILDCARD, // wildcards character
    eType_UNION,    // | operation
    eType_CONCAT,
    eType_STAR,     // * operation
    eType_LEFTPARENTHESIS, // left parenthesis
    eType_RIGHTPARENTHESIS, // right parenthesis
    eType_END,
    eType_ERROR,
};



class CToken
{
    static int s_nTokenCount;
public:
    CToken()
    {
        s_nTokenCount++;
        m_Type  = eType_NORMAL;
        m_nId   = -1;
        m_char  = '\0';
    }
    ~CToken(void);
    CToken(node_type type)
    {
        s_nTokenCount++;
        m_Type  = type;
        m_nId   = -1;
        m_char  = '\0';
    }
    node_type GetType()
    {
        return m_Type;
    }
    void SetType(node_type type)
    {
        m_Type = type;
    }
    unsigned char GetChar()
    {
        return m_char;
    }
    void SetChar(unsigned char ch)
    {
        m_char = ch;
    }
    BOOL IsOperation()
    {
        if (eType_NORMAL == this->GetType())
        {
            return FALSE;
        }
        return ('|' == m_char) || ('*' == m_char) ||
               ('(' == m_char) || (')' == m_char);
    }
    void SetId(int nId)
    {
        m_nId = nId;
    }
    int GetId()
    {
        return m_nId;
    }

private:
    int                 m_nId;
    node_type           m_Type;
    unsigned char       m_char;
};
