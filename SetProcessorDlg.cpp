// SetProcessorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "psynteg.h"
#include "psyntegDlg.h"
#include "SetProcessorDlg.h"


// CSetProcessorDlg dialog

IMPLEMENT_DYNAMIC(CSetProcessorDlg, CDialog)

CSetProcessorDlg::CSetProcessorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetProcessorDlg::IDD, pParent)
{

}

CSetProcessorDlg::~CSetProcessorDlg()
{
}

void CSetProcessorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSetProcessorDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CSetProcessorDlg::OnBnClickedOK)
	ON_BN_CLICKED(IDC_L34, &CSetProcessorDlg::OnBnClickedL34)
	ON_BN_CLICKED(IDC_SP12, &CSetProcessorDlg::OnBnClickedSP12)
	ON_BN_CLICKED(IDC_SPRINT, &CSetProcessorDlg::OnBnClickedSprint)
	ON_BN_CLICKED(IDC_EMUL, &CSetProcessorDlg::OnBnClickedEmul)
END_MESSAGE_MAP()


// CSetProcessorDlg message handlers

void CSetProcessorDlg::SetControls()
{
	char sprocessor[16], errstr[256];
	if (itype>=SPRINT_CPS) // nic1
	{
		EnableID(this, IDC_EMUL, 0);
		EnableID(this, IDC_CIC4, 0);
		EnableID(this, IDC_COMP_EDIT, 1);
		SendDlgItemMessage(IDC_SPRINT, BM_SETCHECK, BST_CHECKED);
		SendDlgItemMessage(IDC_L34, BM_SETCHECK, BST_UNCHECKED);
		SendDlgItemMessage(IDC_SP12, BM_SETCHECK, BST_UNCHECKED);
		SendDlgItemMessage(IDC_CIC3, BM_SETCHECK, BST_CHECKED);
		SendDlgItemMessage(IDC_CIC4, BM_SETCHECK, BST_UNCHECKED);
		SetDlgItemInt(IDC_COMP_EDIT, itype-SPRINT_CPS);
	}
	else if (itype==UNSPECIFIED) // error
	{
		strcpy(sprocessor, "L34-CIC3-0");
			MessageBox (errstr, "Hardware info cannot be set. Application will crash and close.");
		::PostQuitMessage(0);
	}
	else // nic2
	{
		EnableID(this, IDC_EMUL, 1);
		EnableID(this, IDC_CIC4, 1);
		EnableID(this, IDC_COMP_EDIT, 0);
		SendDlgItemMessage(IDC_SPRINT, BM_SETCHECK, BST_UNCHECKED);
		switch(itype)
		{
		case L34_CIC3:
		case L34_CIC4:
			SendDlgItemMessage(IDC_L34, BM_SETCHECK, BST_CHECKED);
			SendDlgItemMessage(IDC_SP12, BM_SETCHECK, BST_UNCHECKED);
			break;
		case SP12_CIC3:
		case SP12_CIC4:
			SendDlgItemMessage(IDC_L34, BM_SETCHECK, BST_UNCHECKED);
			SendDlgItemMessage(IDC_SP12, BM_SETCHECK, BST_CHECKED);
			break;
		}
		switch(itype)
		{
		case L34_CIC3:
		case SP12_CIC3:
			SendDlgItemMessage(IDC_CIC3, BM_SETCHECK, BST_CHECKED);
			SendDlgItemMessage(IDC_CIC4, BM_SETCHECK, BST_UNCHECKED);
			break;
		case L34_CIC4:
		case SP12_CIC4:
			SendDlgItemMessage(IDC_CIC3, BM_SETCHECK, BST_UNCHECKED);
			SendDlgItemMessage(IDC_CIC4, BM_SETCHECK, BST_CHECKED);
			break;
		}
		if (EmulMode) SendDlgItemMessage(IDC_EMUL, BM_SETCHECK, BST_CHECKED);
		else SendDlgItemMessage(IDC_EMUL, BM_SETCHECK, BST_UNCHECKED);
	}
}

int CSetProcessorDlg::GetControls()
{
	int ch, dum;
	if (SendDlgItemMessage(IDC_SPRINT, BM_GETCHECK) == BST_CHECKED)
	{
		dum = GetDlgItemInt(IDC_COMP_EDIT, &ch);
		if (!ch) 
		{
			MessageBox ("Com port number for PPS connection needs to be specified (for CPS, type 0)");
			return 0;
		}
		itype = (InterfaceType)((int)SPRINT_CPS + dum);
		EmulMode = false;
	}
	else if (SendDlgItemMessage(IDC_L34, BM_GETCHECK) == BST_CHECKED)
	{
		if (SendDlgItemMessage(IDC_CIC3, BM_GETCHECK) == BST_CHECKED)
			itype = L34_CIC3;
		else 
			itype = L34_CIC4;
		EmulMode = (SendDlgItemMessage(IDC_EMUL, BM_GETCHECK) == BST_CHECKED);
	}
	else if (SendDlgItemMessage(IDC_SP12, BM_GETCHECK) == BST_CHECKED)
	{
		if (SendDlgItemMessage(IDC_CIC3, BM_GETCHECK) == BST_CHECKED)
			itype = SP12_CIC3;
		else 
			itype = SP12_CIC4;
		EmulMode = (SendDlgItemMessage(IDC_EMUL, BM_GETCHECK) == BST_CHECKED);
	}
	else
	{
		MessageBox ("Invalid control setting");
		return 0;
	}
	return 1;
}

BOOL CSetProcessorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	char sproc[64];
	hPar = GetParent();

	strcpy(sproc, ((CpsyntegDlg*)hPar)->sprocessor);
	itype = ((CpsyntegDlg*)hPar)->itype;
	EmulMode = ((CpsyntegDlg*)hPar)->EmulMode;
	SetControls();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSetProcessorDlg::OnBnClickedOK()
{
	char sproc[64];
	static bool changed(false);
	if (GetControls())
	{
		if ((((CpsyntegDlg*)hPar)->itype != itype ) || (((CpsyntegDlg*)hPar)->EmulMode - EmulMode ) ) changed = true;
		((CpsyntegDlg*)hPar)->itype = itype;
		((CpsyntegDlg*)hPar)->EmulMode = EmulMode;
		((CpsyntegDlg*)hPar)->itype2string(itype, EmulMode, sproc); 
		if (strncmp(sproc,"Invalid",7))	((CpsyntegDlg*)hPar)->sprocessor = sproc;
		if (changed) 
		{
			((CpsyntegDlg*)hPar)->sbar.SetText(sproc, 1, 0);
			((CpsyntegDlg*)hPar)->sbar.SetText("Restart to update the hardware setting", 2, 0);
		}
		EndDialog(IDOK);
	}
}

void CSetProcessorDlg::OnBnClickedL34()
{
	EnableID(this, IDC_CIC4, 1);
	EnableID(this, IDC_COMP_EDIT, 0);
	EnableID(this, IDC_EMUL, 1);
	if (((CpsyntegDlg*)hPar)->EmulMode) SendDlgItemMessage(IDC_EMUL, BM_SETCHECK, BST_CHECKED);
	else SendDlgItemMessage(IDC_EMUL, BM_SETCHECK, BST_UNCHECKED);
}

void CSetProcessorDlg::OnBnClickedSP12()
{
	EnableID(this, IDC_CIC4, 1);
	EnableID(this, IDC_COMP_EDIT, 0);
	EnableID(this, IDC_EMUL, 1);
	if (((CpsyntegDlg*)hPar)->EmulMode) SendDlgItemMessage(IDC_EMUL, BM_SETCHECK, BST_CHECKED);
	else SendDlgItemMessage(IDC_EMUL, BM_SETCHECK, BST_UNCHECKED);
}

void CSetProcessorDlg::OnBnClickedSprint()
{
	CString s;
	EnableID(this, IDC_CIC4, 0);
	EnableID(this, IDC_COMP_EDIT, 1);
	SendDlgItemMessage(IDC_CIC3, BM_SETCHECK, BST_CHECKED);
	SendDlgItemMessage(IDC_CIC4, BM_SETCHECK, BST_UNCHECKED);
	SendDlgItemMessage(IDC_EMUL, BM_SETCHECK, BST_UNCHECKED);
	EnableID(this, IDC_EMUL, 0);
	GetDlgItemText(IDC_COMP_EDIT, s);
	if (s.GetLength()==0)		SetDlgItemInt(IDC_COMP_EDIT, 0);
}


void CSetProcessorDlg::OnBnClickedEmul()
{
	// TODO: Add your control notification handler code here
}
