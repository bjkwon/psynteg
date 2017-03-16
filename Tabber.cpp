// Tabber.cpp : implementation file
//

#include "stdafx.h"
#include "psynteg.h"
#include "Tabber.h"


// CTabber

IMPLEMENT_DYNAMIC(CTabber, CTabCtrl)

CTabber::CTabber()
{

}

void CTabber::SetDlgs(CProcDialog** dlgs, int *IDs, int nDlgs)
{
   m_nPageCount = nDlgs;
   for (int i=0; i<m_nPageCount; i++)
   {
	   m_Dialog[i] = dlgs[i];
	   m_DialogID[i] = IDs[i];
   }
}

CTabber::~CTabber()
{
	for (int i=0; i<m_nPageCount; i++)
		delete m_Dialog[i];
}


BEGIN_MESSAGE_MAP(CTabber, CTabCtrl)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, &CTabber::OnTcnSelchange)
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

// CTabber message handlers

void CTabber::ActivateTabDialogs(int isel)
{int nSel;
  if (isel==-1)
	nSel = GetCurSel();
  else
	  nSel = isel;
  if(m_Dialog[nSel]->m_hWnd)
     m_Dialog[nSel]->ShowWindow(SW_HIDE);

  CRect rcClient, rcWnd;

  GetClientRect(rcClient);
  AdjustRect(FALSE,rcClient);
  GetWindowRect(rcWnd);
  GetParent()->ScreenToClient(rcWnd);
  rcClient.OffsetRect(rcWnd.left,rcWnd.top);
  for(int i=0; i < m_nPageCount; i++)
  {
	  if (m_Dialog[i]->m_hWnd!=NULL)
	  m_Dialog[i]->SetWindowPos(&wndTop, rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), i==nSel? SWP_SHOWWINDOW:SWP_HIDEWINDOW);
  }
  if(m_Dialog[nSel]->m_hWnd)  m_Dialog[nSel]->ShowWindow(SW_SHOW);

}

void CTabber::InitDialogs(CWnd *hPar)
{
   for (int i=0; i<m_nPageCount; i++)	
	   m_Dialog[i]->Create(m_DialogID[i],hPar);
}


void CTabber::OnTcnSelchange(NMHDR *pNMHDR, LRESULT *pResult)
{
	ActivateTabDialogs();
	int i = GetCurSel();
	GetParent()->PostMessage(WM__SELCHANGE, (WPARAM)i);
	*pResult = 0;
}

LRESULT CTabber::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_GETDLGCODE:
		LRESULT res = CTabCtrl::WindowProc(message, wParam, lParam);
		res |= DLGC_WANTMESSAGE;
		return res;
	}

	return CTabCtrl::WindowProc(message, wParam, lParam);
}

void CTabber::OnKillFocus(CWnd* pNewWnd)
{
		CTabCtrl::OnKillFocus(pNewWnd);

	// TODO: Add your message handler code here
}
