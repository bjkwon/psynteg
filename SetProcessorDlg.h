#pragma once
#include "afxwin.h"


// CSetProcessorDlg dialog

class CSetProcessorDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetProcessorDlg)

public:
	CWnd *hPar;
	InterfaceType itype;
	bool EmulMode;
	CSetProcessorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetProcessorDlg();
	void SetControls(); // set window controls from itype and EmulMode
	int GetControls(); // read from windows controls and update itype and EmulMode
	int CheckEMULOK();

// Dialog Data
	enum { IDD = IDD_SET_PROCESSOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOK();
	
	afx_msg void OnBnClickedL34();
	afx_msg void OnBnClickedSP12();
	
public:
	afx_msg void OnBnClickedSprint();
	afx_msg void OnBnClickedEmul();
};
