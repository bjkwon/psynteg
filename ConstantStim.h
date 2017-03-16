#pragma once
#include "psynteg.h"
#include "EditPlus.h"

/////////////////////////////////////////////////////////////////////////////
// CConstantStim dialog

class CConstantStim : public CProcDialog
{
// Construction
public:
	CWnd* hPar;
	void GenLogStr(CString& str, string tagStrSpaceDelim, short *cumCorr, short *cumTrials);
	void UpdateEndValue();
	int ProcessList();
	int GetValues(int id, double **out);
	CConstantStim(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConstantStim();

// Dialog Data
	//{{AFX_DATA(CConstantStim)
	enum { IDD = IDD_CONSTANT };
	CEditPlus	m_valEdit;
	CString	varlistStr;
	CSignals var;
	int		nTrials;
	int		nSteps;
	int		nVars;
	string	*varNames;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConstantStim)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConstantStim)
	afx_msg void OnClose();
public:
	afx_msg void OnEnKillfocusIndVarStr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	CString instruction;
};
