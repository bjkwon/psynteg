// psynteg.h : main header file for the psynteg application
//

#pragma once

#include "resource.h"		// main symbols
#include "bric.h"		
#include "flyplus.h"		
#include "audfret.h"
#include "audstr.h"
#include "protocol.h"		
#include "supportFunctions.h"
#include "cer.h"		
#include "assert.h"		
#include "sigproc.h"
#include "graphy.h"

//nic2 STUFF
#include "nic/c_interface/CSequence.h"
#include "services/c_interface/CNICStreamClient.h"

BOOL CALLBACK SmallDlg (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam);
void UpdateSProcessor(HWND hDlg);
BOOL CALLBACK SetSpProc (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam);

// For NIC2
int InitNIC2(HWND hDlg, char *errstr);
int NIC2CleanUp();
//int NIC2StreamIt (const char *streamFile, char *errstr, bool readandstream=0);
//int NIC2StreamIt (ACESEQ *seq, char *errstr, bool readandstream);
extern HINSTANCE hInst;
extern bool EmulMode;
extern HWND hMainDlg, hPipeLog;
extern char ApplicationPath[];
extern unsigned int streamThreadID;

typedef enum
{
	UNSPECIFIED = 0,
	L34_CIC3,
	L34_CIC4,
	SP12_CIC3,
	SP12_CIC4,
	SPRINT_CPS,
	SPRINT_PPS1,
	SPRINT_PPS2,
	SPRINT_PPS3,
	SPRINT_PPS4,
	SPRINT_PPS5,
	SPRINT_PPS6,
	SPRINT_PPS7,
	SPRINT_PPS8,
	SPRINT_PPS9,
} InterfaceType;

// due to inevitable gap before the stimulation, 
// this duration is inserted before the stimulation
// when conditioner is in effect.
#define COND_DUR_BEFORE_STIM_INTERVAL_MS	400
 
#define MAX_BRICS		256
#define MAX_TRIALS	200
#define WM__NEXT_INTERVAL		WM_APP+410
#define WM__GO					WM_APP+423
#define WM__STOP				WM_APP+433
#define WM__COND_ADJUST			WM_APP+435
#define WM__RETRY_PRESENT		WM_APP+437
#define WM__SELCHANGE			WM_APP+440

#define CONTROLLED			300
#define REFERENCE			400
#define ALL					-1

#define QNAN -8888.7777
#define INITVAL 1.2345e-100

#define BYOPERATOR			0
#define BYSUBJECT			1

#define FAILURE_DOUBLE -9999.999
#define	LISTPLUSV  -9949.999
#define LISTMINUSV -9959.999

#define LOGFILE  "psynteg.log"
#define REGISTRY_KEY4HW "SOFTWARE\\AuditoryPro\\psynteg"
#define REGISTRY_SUBKEY_4HW "SpeechProcessor"

/////////////////////////////////////////////////////////////////////////////
// CpsyntegApp:
// See psynteg.cpp for the implementation of this class
//

class CpsyntegApp : public CWinApp
{
public:
	int byList;
	CpsyntegApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CpsyntegApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CpsyntegApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CProcDialog : public CDialog
{
public:
	virtual int OnBnClickedGo() {return 1;}
	virtual LRESULT OnFlyArrived(WPARAM code, LPARAM lParam) {return 1;}
	virtual int DoneSession() {return 1;}
	CProcDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL) : CDialog(nIDTemplate, pParentWnd) {}
};