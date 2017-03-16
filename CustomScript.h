#pragma once
#include "psynteg.h"

/////////////////////////////////////////////////////////////////////////////
// CCustomScript dialog
class CpsyntegDlg;

class CCustomScript : public CProcDialog
{
// Construction
public:
	CCustomScript(CpsyntegDlg* pParent = NULL);   // standard constructor
	virtual ~CCustomScript();
	virtual BOOL OnInitDialog();
	int OnBnClickedGo();
	LRESULT OnFlyArrived(WPARAM code, LPARAM lParam);
	int DoneSession();

// Dialog Data
	//{{AFX_DATA(CCustomScript)
	enum { IDD = IDD_CUSTOM };
	//}}AFX_DATA
	CpsyntegDlg* hPar;
	CString m_Script;
	HANDLE hEvent;
	int LastResponse;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomScript)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCustomScript)
	afx_msg void OnClose();
public:
	//afx_msg void OnEnKillfocusIndVarStr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
