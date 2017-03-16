#pragma once

/////////////////////////////////////////////////////////////////////////////
// CAdaptiveDlg dialog

#include "Adaptive.h"

#define PLAY_NEXT_TRIAL			2200
#define PLAY_NEXT_INTERVAL		2201
#define PLAYAGAIN				2202

class CAdaptiveDlg : public CProcDialog
{
// Construction
public:
	void SyncAdap();
	void InitAdaptive(CAdaptive *adapObj, int *corrHist, double* thrCand);
	BOOL UpdateDataPlus(BOOL bSaveAndValidate = TRUE);
	CWnd* hPar;
	CAdaptive adap;
	CAdaptive adap2;
	CAdaptiveDlg(CWnd* pParent = NULL);   // standard constructor
	CString instruction;
	//virtual ~CAdaptiveDlg();

// Dialog Data
	//{{AFX_DATA(CAdaptiveDlg)
	enum { IDD = IDD_ADAPTIVE };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdaptiveDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAdaptiveDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnUpdateInitVal();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int maxReversalStage;
	int reversalUpdate (double variable, double thresholdCandidate[], int *rev, int *reversalStage, int *reversals);
};
