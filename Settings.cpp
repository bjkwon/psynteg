// Settings.cpp : implementation file
//

#include "stdafx.h"
#include "psynteg.h"
#include "Settings.h"
#include "psyntegDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
\
/////////////////////////////////////////////////////////////////////////////
// CSettings dialog


CSettings::CSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CSettings::IDD, pParent)
	, m_charge_limit(44.), m_intervalDelay(0), m_trialDelay(0), m_roving(0.), m_freeze_rov(0), m_anchored_ref(0)
{
	//{{AFX_DATA_INIT(CSettings)
	m_instructions = _T("");
	m_proc_var = _T("");
	m_stim_var = _T("");
	//}}AFX_DATA_INIT
}


void CSettings::DoDataExchange(CDataExchange* pDX)
{
	CpsyntegDlg* hPar = (CpsyntegDlg*)GetParent();

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettings)
	DDX_Text(pDX, IDC_DELAY_INTERVALS, m_intervalDelay);
	DDX_Text(pDX, IDC_DELAY_TRIALS, m_trialDelay);
	DDX_Text(pDX, IDC_INST, m_instructions);
	DDX_Text(pDX, IDC_LIMIT_CHARGE, m_charge_limit);
	DDX_Text(pDX, IDC_UDFPATH, hPar->m_udfpath);
	// Log reporting
	DDX_Check(pDX, IDC_LOG_EMAIL, m_log_email);
	DDX_Text(pDX, IDC_EMAIL_ADDRESS, m_email_address);
	DDX_Check(pDX, IDC_LOG_FTP, m_log_ftp);
	DDX_Text(pDX, IDC_FTP_ADDRESS, m_ftp_address);
	DDX_Text(pDX, IDC_FTP_ID, m_ftp_id);
	DDX_Text(pDX, IDC_FTP_PASSWORD, m_ftp_password);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettings, CDialog)
	//{{AFX_MSG_MAP(CSettings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettings message handlers

void CSettings::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	CpsyntegDlg* hPar = (CpsyntegDlg*)GetParent();
	hPar->intervalDelay = m_intervalDelay ;
	hPar->trialDelay = m_trialDelay ;
	hPar->charge_limit = m_charge_limit ;
	string sbuf[2];
	str2array(sbuf, 2, m_instructions, "\r\n"); hPar->instruction[0] = sbuf[0].c_str(); hPar->instruction[1] = sbuf[1].c_str(); hPar->instruction[0].TrimLeft(" \t");
	if (hPar->m_udfpath.GetLength()>0)
		hPar->AstSig.SetPath("aux", hPar->m_udfpath);
	else
		hPar->AstSig.SetPath("aux", hPar->AppPath);
	// Log reporting
/*	if ((hPar->report.fUseEmail = m_log_email))
		hPar->report.email_address = m_email_address;
	if ((hPar->report.fUseFtp = m_log_ftp)) {
		hPar->report.ftp_address = m_ftp_address;
		hPar->report.ftp_id = m_ftp_id;
		hPar->report.ftp_password = m_ftp_password;
	} */
	CDialog::OnOK();
}

BOOL CSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CpsyntegDlg* hPar = (CpsyntegDlg*)GetParent();
	m_intervalDelay = hPar->intervalDelay;
	m_trialDelay = hPar->trialDelay;
	m_charge_limit = hPar->charge_limit;
	m_instructions.Format("%s\r\n%s", hPar->instruction[0], hPar->instruction[1]);
	// Log reporting
//	if ((m_log_email = hPar->report.fUseEmail))
//		m_email_address = hPar->report.email_address;
//	if ((m_log_ftp = hPar->report.fUseFtp)) {
//		m_ftp_address = hPar->report.ftp_address;
//		m_ftp_id = hPar->report.ftp_id;
//		m_ftp_password = hPar->report.ftp_password;
//	}
	UpdateData(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
