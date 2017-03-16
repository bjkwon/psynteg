#if !defined(AFX_SLIDERADJ_H__1BBDD729_72D1_4D5D_8CB0_01B7DF7A92B1__INCLUDED_)
#define AFX_SLIDERADJ_H__1BBDD729_72D1_4D5D_8CB0_01B7DF7A92B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SliderAdj.h : header file
//

#include "EditPlus.h"

/////////////////////////////////////////////////////////////////////////////
// CSliderAdj dialog

class CSliderAdj : public CProcDialog
{
// Construction
public:
	CWnd* hPar;
	CSliderAdj(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSliderAdj)
	enum { IDD = IDD_SLIDER };
	CEditPlus	m_valEdit;
	double	val;
	double	m_step;
	int		m_range1;
	int		m_range2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSliderAdj)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSliderAdj)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CString instruction;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SLIDERADJ_H__1BBDD729_72D1_4D5D_8CB0_01B7DF7A92B1__INCLUDED_)
