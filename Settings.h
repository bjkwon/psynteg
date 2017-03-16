#if !defined(AFX_SETTINGS_H__610459E4_7F6B_4044_BD43_6385FD446503__INCLUDED_)
#define AFX_SETTINGS_H__610459E4_7F6B_4044_BD43_6385FD446503__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Settings.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSettings dialog

class CSettings : public CDialog
{
// Construction
public:
	CSettings(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSettings)
	enum { IDD = IDD_SETTINGS };
	int		m_intervalDelay;
	int		m_trialDelay;
	CString	m_instructions;
	double	m_roving;
	BOOL	m_freeze_rov;
	CString	m_proc_var;
	CString	m_stim_var;
	BOOL	m_anchored_ref;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSettings)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	double m_charge_limit; // nC
	// Log reporting
	BOOL   m_log_email;
	BOOL   m_log_ftp;
	CString m_email_address;
	CString m_ftp_address;
	CString m_ftp_id;
	CString m_ftp_password;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETTINGS_H__610459E4_7F6B_4044_BD43_6385FD446503__INCLUDED_)
