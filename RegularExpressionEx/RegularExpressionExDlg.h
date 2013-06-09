
// RegularExpressionExDlg.h : header file
//

#pragma once

#include "DFA.h"

// CRegularExpressionExDlg dialog
class CRegularExpressionExDlg : public CDialog
{
// Construction
public:
	CRegularExpressionExDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_REGULAREXPRESSIONEX_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedFind();

public:
    CDFA m_DFA;

};
