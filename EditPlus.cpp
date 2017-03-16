// EditPlus.cpp : implementation file
//

#include "stdafx.h"
#include "psynteg.h"
#include "EditPlus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditPlus

CEditPlus::CEditPlus()
{
}

CEditPlus::~CEditPlus()
{
}


BEGIN_MESSAGE_MAP(CEditPlus, CEdit)
	//{{AFX_MSG_MAP(CEditPlus)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditPlus message handlers

void CEditPlus::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	int id;
	double val;
	char buf[16];

	if ( (nChar-33)*(nChar-34)*(nChar-38)*(nChar-40)*
		(nChar-36)*(nChar-35)==0 )
	{
		CWnd *hPar = GetParent();
		id=GetDlgCtrlID();
		val = GetDlgItemDouble (hPar->m_hWnd, id);
		switch (nChar)
		{
		case 36://Home
			val +=5.;
			break;
		case 35://End
			val -=5.;			if (val<0) val=0;
			break;
		case 33: // Page Up
			val +=10.;
			break;
		case 34: // Page Down
			val -=10.;			if (val<0) val=0;
			break;
		case 38: // Up Arrow
			val += 1.;
			break;
		case 40: // Down Arrow
			val -= 1.;			if (val<0) val=0;
			break;
		}
		sprintfFloat(val, 4, buf, sizeof(buf));
		hPar->SetDlgItemText(id, buf); 
		if (id==IDC_COND_CL) // if conditioner, adjust the cl while conditioner is playing
			hPar->SendMessage(WM__COND_ADJUST);
	}
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}
