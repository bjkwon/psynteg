#if !defined(AFX_EDITPLUS_H__71A0C1DA_670E_4C3D_85BF_D6F1C0037F68__INCLUDED_)
#define AFX_EDITPLUS_H__71A0C1DA_670E_4C3D_85BF_D6F1C0037F68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditPlus.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditPlus window

class CEditPlus : public CEdit
{
// Construction
public:
	CEditPlus();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditPlus)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditPlus();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditPlus)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITPLUS_H__71A0C1DA_670E_4C3D_85BF_D6F1C0037F68__INCLUDED_)
