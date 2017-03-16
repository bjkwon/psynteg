// CustomScript.cpp : implementation file
//

#include "stdafx.h"
#include "psynteg.h"
#include "psyntegDlg.h"
#include "CustomScript.h"

/////////////////////////////////////////////////////////////////////////////
// CCustomScript dialog


CCustomScript::CCustomScript(CpsyntegDlg* pParent /*=NULL*/)
	: CProcDialog(CCustomScript::IDD, pParent),
	hPar(pParent),
	m_Script(_T("")), hEvent(NULL)
{
}

CCustomScript::~CCustomScript()
{
}

void CCustomScript::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCustomScript)
	//DDX_Text(pDX, IDC_TRIALS, nTrials);
	DDX_Text(pDX, IDC_CUSTOM_SCRIPT, m_Script);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCustomScript, CDialog)
	//{{AFX_MSG_MAP(CCustomScript)
//	ON_EN_KILLFOCUS(IDC_IND_VAR_STR, &CCustomScript::OnEnKillfocusIndVarStr)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomScript message handlers

BOOL CCustomScript::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CCustomScript::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message==WM_COMMAND)
	{
		if (wParam==IDCANCEL || wParam==IDOK)
			return NULL;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CCustomScript::OnClose() 
{
	CDialog::OnClose();
}


void ThreadCustomScript(void *pCCustomScript)
{
	CCustomScript *pDlg = (CCustomScript *)pCCustomScript;
	srand((unsigned int)time(NULL) ^ (unsigned int)GetCurrentThreadId());
	pDlg->hPar->Prepare(pDlg->m_Script, CString("Custom Script"));
	pDlg->hPar->DoneSession();
	_endthread();
}

int CCustomScript::OnBnClickedGo()
{
	UpdateData();
	if (CString(m_Script).Trim().IsEmpty()) {	// make a copy of m_Script so that we can check for emptiness without affecting the line number
		MessageBox("Script is empty.", "Error", MB_ICONERROR);
		return 0;
	}

	if (hEvent)
		CloseHandle(hEvent);
	hEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("PsycnoCI_CustomScriptEvent")); 
	if (hEvent == NULL) { 
		CString errstr;
		errstr.Format("CreateEvent failed (%d)", GetLastError());
		MessageBox(errstr, "Error", MB_ICONERROR);
		return 0;
	}

	((CpsyntegDlg *)hPar)->nBrics = 0;
	LastResponse = -1;
	_beginthread(ThreadCustomScript, 0, (void*)this);
	return 1;
}

LRESULT CCustomScript::OnFlyArrived(WPARAM code, LPARAM lParam)
{
	switch (LOWORD(code)) {
	case FL_RESPONSE:
		LastResponse = *(int*)lParam + 1;
		// go through
	case FL_BEGIN:
		if (!SetEvent(hEvent)) {
			CString errstr;
			errstr.Format("SetEvent failed (%d)\n", GetLastError());
			MessageBox(errstr, "Error", MB_ICONERROR);
		}
		return 0;
	}
	return 1;
}

int CCustomScript::DoneSession()
{
	return 1;
}