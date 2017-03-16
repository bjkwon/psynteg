#pragma once


// CTabber

class CTabber : public CTabCtrl
{
	DECLARE_DYNAMIC(CTabber)

public:
	void SetDlgs(CProcDialog** dlgs, int *IDs, int nDlgs);
	CTabber();
	virtual ~CTabber();
	int m_nPageCount;
	int m_DialogID[4];
	CDialog *m_Dialog[4];
	void InitDialogs(CWnd *hPar);
	void ActivateTabDialogs(int isel=-1);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTcnSelchange(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
};


