// psyntegDlg.cpp : implementation file
//

#include "stdafx.h"
#include "psynteg.h"
#include "psyntegDlg.h"
#include "Settings.h"
#include <math.h>
#include <stdlib.h>

#define LOG175 2.243038048686
#define PI	3.141592

#define INIFILE		"genTaskCI.ini"

#define FILL_PREV_IF_NOT_NULL(x,y,msg) { if (prevBr==NULL) {errstr=msg;return 0;}else x=y;}

// NIC1 path is hard coded here.
#define NIC1PATH "C:\\nic\\1\\binaries"
#define DEF_NIC2PATH "c:\\nic\\2\\binaries"
#define DEF_PROCESSOR "L34-CIC3-0"

#define FONTS_DEF "MS Reference Sans Serif"

#define CHECV(x) {if(!(x)) {errstr = errStr; return;}}
#define CHEC(x) {if(!(x)) {errstr = errStr; return 0;}}
#define CHEC3(X,Y) {int __temp_int = (Y); if(!__temp_int) {errstr = errStr; free(buf); return 0;} else { X = max(X,__temp_int);}}
#define CHEC4(X,Y) {res = Y; if(res>0) { X = max(X,res);}}

int CleanupNIC1Lib();
int LoadNIC1Lib(const char * nic1path, char * errstr);

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CpsyntegDlg dialog

CpsyntegDlg::CpsyntegDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CpsyntegDlg::IDD, pParent), nSeq(2), connected(0), EmulMode(false)
	, defaultpw(25.), defaultpg(8.), trial(-999), m_figProc(NULL)
	, nic1(false), seqq(NULL), report("PsyconCI"), shortnullframe(false)
	, NIC2Path("")
{
	//{{AFX_DATA_INIT(CpsyntegDlg)
	m_bricString = _T("");
	m_bricString2 = _T("");
	m_substr = _T("");
	m_str_pwframe = _T("");
	m_condOn = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_PSY);
	toggle=0; // toggle: 0 for primary sequence 1 for secondary
	
	srand((unsigned int)time(NULL) ^ (unsigned int)GetCurrentThreadId());

	m_bClose=FALSE;
	variable = 0;
	refNext=0;
	initialized=0;
	m_child1=NULL;
	m_child2=NULL;
	m_child3=NULL;
	memset((void*)purt, 0, sizeof(purt));
	scores = new double[1];
	presentingIDorder = new short[1];
	cumTrials = new short[1];
	cumCorr = new short[1];
	SigEditFont.CreateFont(16,0,0,0, FW_DONTCARE, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
		FIXED_PITCH | FF_MODERN, FONTS_DEF);
	brics = new BRIC[MAX_BRICS];
	allocated_max_brics = MAX_BRICS;
}

CpsyntegDlg::~CpsyntegDlg()
{ 
	SigEditFont.DeleteObject();
	delete m_figProc;
}

void CpsyntegDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CpsyntegDlg)
	DDX_Control(pDX, IDC_COND_CL, m_edit_condCL);
	DDX_Control(pDX, IDC_PWFRAME, m_pwframe);
	DDX_Control(pDX, IDC_PLAY2, m_play2);
	DDX_Control(pDX, IDC_LOG, m_logEdit);
	DDX_Control(pDX, IDC_PLAY, m_play);
	DDX_Text(pDX, IDC_TAGS, m_tagStr);
	DDX_Text(pDX, IDC_TAGS2, m_tagStr2);
	DDX_Text(pDX, IDC_BRICS, m_bricString);
	DDX_Text(pDX, IDC_BRICS2, m_bricString2);
	DDX_Text(pDX, IDC_SUBJ_ID, m_substr);
	DDX_CBString(pDX, IDC_PWFRAME, m_str_pwframe);
	DDX_Check(pDX, IDC_COND_ONOFF, m_condOn);
	DDX_Control(pDX, IDC_TASKTAB, m_tab);
	DDX_Check(pDX, IDC_FEEDBACK, feedback);
	DDX_Text(pDX, IDC_NUM_INTERVALS, nIntervals);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_TAGS, m_edit_tag);
	DDX_Control(pDX, IDC_TAGS2, m_edit_tag2);
	DDX_Control(pDX, IDC_BRICS, m_edit_bric1);
	DDX_Control(pDX, IDC_BRICS2, m_edit_bric2);
}

BEGIN_MESSAGE_MAP(CpsyntegDlg, CDialog)
	//{{AFX_MSG_MAP(CpsyntegDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_PLAY, OnPlay1)
	ON_WM_TIMER()
	ON_EN_UPDATE(IDC_TEXT, OnUpdateText)
	ON_BN_CLICKED(IDC_PLAY2, OnPlay2)
	ON_WM_CLOSE()
	ON_CBN_KILLFOCUS(IDC_PWFRAME, OnKillfocusPwframe)
	ON_BN_CLICKED(IDC_COND_ONOFF, OnCondOnoff)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_SETTINGS, OnFileSettings)
	ON_COMMAND(ID_FILE_HARDWARECONFIG, OnFileConfig)
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_COMMAND(ID_FILE_OPENSTIM, OnOpenStim)
	ON_COMMAND(ID_FILE_SAVESTIM, OnSaveStim)
	ON_COMMAND(ID_FILE_RESETPROCESSOR, OnResetProcessor)
	ON_MESSAGE (WM_FL_CONNECTED, OnFlyConnected)
	ON_MESSAGE (WM_FL_CLOSE, OnFlyClosed)
	ON_MESSAGE (WM_FL_ARRIVED, OnFlyArrived)
	ON_MESSAGE (WM_FL_NETWORK_ERR, OnFlyErr)
	ON_MESSAGE (WM__NEXT_INTERVAL, OnNextInterval)
	ON_MESSAGE (WM__SELCHANGE, OnSelChange)
	ON_MESSAGE (WM__COND_ADJUST, OnCondAdj)
	ON_BN_CLICKED(IDC_GO, &CpsyntegDlg::OnBnClickedGo)
	ON_BN_CLICKED(IDC_STOP, &CpsyntegDlg::OnBnClickedStop)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CpsyntegDlg message handlers

BOOL CpsyntegDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// set MENU here
	menu0.LoadMenu(IDR_MENU1);
	SetMenu(&menu0);

	// TODO: Add extra initialization here
	char procname[64], verstr[16], IPaddress[64], buf[128], errstr[256];
	sbar.Create	(WS_CHILD|WS_VISIBLE, CRect(1,2,1,2), this, 1000);
	int i, rtEdges[] = {120, 280, -1};
 	char fname[256], buffer[256];
	string str, str2;

	sbar.SetParts(3, rtEdges);
	GetCurrentProcInfo(AppPath, procname, verstr);
	getVersionStringAndUpdateTitle(m_hWnd, procname, verstr, sizeof(verstr));

	i=PickRemotePC (NULL, IPaddress, "remotePCs.ini");
	i=initfly (m_hWnd, IPaddress, -1, true);

	m_child1 = new CAdaptiveDlg(this);
	m_child2 = new CConstantStim(this);
	m_child3 = new CSliderAdj(this);
	m_child4 = new CCustomScript(this);
	dlgs[0] = m_child1;
	dlgs[1] = m_child2;
	dlgs[2] = m_child3;
	dlgs[3] = m_child4;

	int ID[4];
	ID[0]=IDD_ADAPTIVE;
	ID[1]=IDD_CONSTANT;
	ID[2]=IDD_SLIDER;
	ID[3]=IDD_CUSTOM;

	m_tab.SetDlgs(dlgs, ID, sizeof(ID)/sizeof(ID[0]));

	CString s;
	s.LoadString(IDS_STRING102);
	m_pwframe.AddString (s);
	s.LoadString(IDS_STRING103);
	m_pwframe.AddString (s);

	s.Format("Default pw/pg:%d/%d us", (int)defaultpw, (int)defaultpg);
	SetDlgItemText(IDC_DEFAULT_PWPG, s);

	AstSig.SetPath("", AppPath);	// this should proceed ReadINI()
	LoadINI();
	stopped=1;
	m_tt = new CToolTipCtrl;
	m_tt->Create(this);
	m_tt->AddTool(this, IDS_ABOUTBOX);
	m_tt->Activate(1);
	
	CString str_help("");
	s.LoadString(IDS_STRING_HELP1); s +="\n";	str_help += s;
	s.LoadString(IDS_STRING_HELP2); s +="\n";	str_help += s;
	s.LoadString(IDS_STRING_HELP3); s +="\n";	str_help += s;
	s.LoadString(IDS_STRING_HELP4); s +="\n";	str_help += s;
	s.LoadString(IDS_STRING_HELP5); s +="\n";	str_help += s;
	s.LoadString(IDS_STRING_HELP6); s +="\n";	str_help += s;
	
	CreateMyTooltip(GetDlgItem(IDC_BRICS), str_help.GetBuffer(128));
	CreateMyTooltip(GetDlgItem(IDC_BRICS2), str_help.GetBuffer(128));
	str_help="";
	str_help="";
	s.LoadString(IDS_STRING_HELP7); s +="\n";	str_help += s;
	s.LoadString(IDS_STRING_HELP8); str_help += s;
	CreateMyTooltip(GetDlgItem(IDC_COND_CL), str_help.GetBuffer(256));
	i=m_pwframe.SetCurSel (1);	

	SetTimer(IDT_INIT_TABS, 500, NULL);
	ReadINI(errstr, fname, "PROCESSOR", str);

	strcpy(buf, sprocessor);
	itype = string2itype(buf, EmulMode);
	if (EmulMode) strcat(buf, "(Emulation mode)");
	sbar.SetText(sprocessor, 1, 0);
	switch(itype)
	{
	case SPRINT_CPS:
	case SPRINT_PPS1:
	case SPRINT_PPS2:
	case SPRINT_PPS3:
	case SPRINT_PPS4:
	case SPRINT_PPS5:
	case SPRINT_PPS6:
	case SPRINT_PPS7:
	case SPRINT_PPS8:
	case SPRINT_PPS9:
		if (Append2Path(NIC1PATH, buf)==0){
			GetLastErrorStr(buffer);
			MessageBox (strcat(buf,"(Append2Path error)"), buffer, MB_OK); 
			return 0;			}
		if (!LoadNIC1Lib(NIC1PATH, buf)) {
			MessageBox ("NIC1 Initialization error", buf, MB_OK); 
			return 0;			}
		seqSPrint = (IMP_SEQUENCE**) new IMP_SEQUENCE*[nSeq];
		nic1=true;
		SetTimer (INIT_HARDWARE, 50, NULL);
		break;
	default:
		if (Append2Path(NIC2Path, buf)==0){
			GetLastErrorStr(buffer);
			MessageBox (strcat(buf,"(Append2Path error)"), buffer, MB_OK); 
			return 0;			}
		if (!InitNIC2(NIC2Path, sprocessor, errstr)) /*nicPATH*/{
			MessageBox (this->errstr, "NIC2 Initialization error", MB_OK); 
			return 0;			}
		seqq = (SequenceCommandHandle*) new SequenceCommandHandle[nSeq];
		for (int i=0; i<nSeq; i++) seqq[i] = SequenceNew(1);
		EnableID(this, IDC_COND_ONOFF, 0);
		EnableID(this, IDC_COND_CL, 0);
		EnableID(this, IDC_COND_E, 0);
		EnableID(this, IDC_COND_RATE, 0);
		nic1=false;
		break;
	}
	SetupPowerUpFrameElectrodes(1,2);
	m_edit_bric1.SetFont(&SigEditFont);
	m_edit_bric2.SetFont(&SigEditFont);
	m_edit_tag.SetFont(&SigEditFont);
	m_edit_tag2.SetFont(&SigEditFont);

	return TRUE;  
} 

void CpsyntegDlg::OnFileSettings()
{
	CSettings dlg (this);
	dlg.DoModal();
}

void CpsyntegDlg::OnFileConfig()
{
	CSetProcessorDlg m_processorDlg;
	m_processorDlg.DoModal();
}


void CpsyntegDlg::OnFileExit()
{
	DestroyWindow();
}

void CpsyntegDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CpsyntegDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CpsyntegDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL CpsyntegDlg::DestroyWindow() 
{
	UpdateData();
	m_child1->UpdateDataPlus();
	m_child2->UpdateData();
	m_child3->UpdateData();
	m_child4->UpdateData();
	WriteINI();
	closefly();
	if (initialized)
	{
		ImpCommunicatorDisconnect();
		ImpCommunicatorDelete();
	}
	if (nic1)
	{
		for (int i=0; i<nSeq; i++) ImpSequenceDelete(seqSPrint[i]);
		ImpFrameDelete(nullFrame);
		ImpFrameDelete(condFrame);
		delete[] seqSPrint;
	}
	else if (seqq)
	{
		for (int i=0; i<nSeq; i++) sequenceCommandDelete(seqq[i]);
		delete[] seqq;
		NIC2CleanUp();
	}
	
	delete m_tt;
	delete[] scores;
	delete[] presentingIDorder;
	delete[] cumTrials;
	delete[] cumCorr;
	delete[] brics;
	return CDialog::DestroyWindow();
}

char CheckTimeUnit(string &_str)
{
	//returns 'u' for us, 'm' for ms, ' ' for nothing, '*' for error
	char res;
	char buf[8], token[16];
	CString str(_str.c_str());
	str.Trim();
	CString sub = str.Right(2);
	if (sub.MakeLower()=="us") res = 'u'; 
	else if (sub.MakeLower()=="ms") res = 'm';
	else return ' ';
	str.Delete(str.GetLength()-2, 2);

	buf[0]= str.GetAt(str.GetLength()-1); buf[1]=0;
	strcpy(token, "12345 67890.");
	// If a character is found right before us or ms---error 
	if (strcspn(buf, token)>0) res = '*';
	_str = str.GetString();
	return res;
}

void CpsyntegDlg::HandleCIPulse(const AstNode *pnode, CAstSig *pAst)
{
	// line format: (ae,re) tmark, period, pw, pg, cl, duration
	//              (ae,re) tmark, period, cl, duration
	//              (ae,re) tmark, period, cl
	// duration is either the number of pulses or in ms or us
	if (nBrics > allocated_max_brics-2) {
		allocated_max_brics *=2;
		BRIC* bricsNew = new BRIC[allocated_max_brics];
		for (int k(0); k<nBrics; k++) bricsNew[k] = brics[k];
		delete[] brics;
		brics = bricsNew;
	}
	BRIC *pbr = &brics[nBrics];
	string paramName;
	AstNode *p;
	try {
		paramName = "active_electrode";
		if (!pAst->Compute(p=pnode->child).IsScalar())
			throw "must be a scalar.";
		pbr->ae = (int)pAst->Sig.value();
		paramName = "reference_electrode";
		if (!pAst->Compute(p=p->next).IsScalar())
			throw "must be a scalar.";
		pbr->re = (int)pAst->Sig.value();

		paramName = "time_mark";
		p = p->next;
		if (p->type == NODE_MSEC || p->type == NODE_USEC) {
			if (!pAst->Compute(p->child).IsScalar())
				throw "must be a scalar.";
			pbr->tmark = pAst->Sig.value();
			if (p->type == NODE_MSEC)
				pbr->tmark *= 1000;
		} else {
			if (!pAst->Compute(p).IsScalar())
				throw "must be a scalar.";
			if (pAst->Sig.value() != 0)
				throw "must end with either 'ms' or 'us'.";
			pbr->tmark = 0;
		}

		paramName = "period";
		if (!pAst->Compute(p=p->next).IsScalar())
			throw "must be a scalar.";
		pbr->pam.period = pAst->Sig.value();

		if (pnode->type == NODE_CIPULSE5 || pnode->type == NODE_CIPULSE6) {
			paramName = "phase_width";
			if (!pAst->Compute(p=p->next).IsScalar())
				throw "must be a scalar.";
			pbr->pam.pw = pAst->Sig.value();
			paramName = "phase_gap";
			if (!pAst->Compute(p=p->next).IsScalar())
				throw "must be a scalar.";
			pbr->pam.pg = pAst->Sig.value();
		} else {
			pbr->pam.pw = defaultpw;
			pbr->pam.pg = defaultpg;
		}

		paramName = "current_level";
		if (!pAst->Compute(p=p->next).IsScalar())
			throw "must be a scalar.";
		pbr->cl = (int)pAst->Sig.value();

		if (pnode->type == NODE_CIPULSE4 || pnode->type == NODE_CIPULSE6) {
			paramName = "repeat/duration";
			p = p->next;
			if (p->type == NODE_MSEC || p->type == NODE_USEC) {
				if (!pAst->Compute(p->child).IsScalar())
					throw "must be a scalar.";
				if (p->type == NODE_MSEC)
					pbr->repeat = (unsigned short)(pAst->Sig.value()*1000/pbr->pam.period);
				else
					pbr->repeat = (unsigned short)(pAst->Sig.value()/pbr->pam.period);
			} else {
				if (!pAst->Compute(p).IsScalar())
					throw "must be a scalar.";
				pbr->repeat = (unsigned short)pAst->Sig.value();
			}
		} else
			pbr->repeat = 1;
	} catch (const char *errmsg) {
		throw CAstException(p, "Invalid pulse parameter - " + paramName + " " + errmsg);
	}
	double charge;
	if (!CheckChargeLimit(pbr, &charge)) {
		errstr.Format("Charge Limit Exceeded:%7.2fnC, (%d,%d) pw=%6.1fus, cl=%d, ", charge, pbr->ae, pbr->re, pbr->pam.pw, pbr->cl);
		throw (const char *)errstr;
	}
	nBrics++;
}

LRESULT CpsyntegDlg::OnSelChange(WPARAM wp, LPARAM lparam)
{
	int check;
	taskID=(int)wp;
	switch (taskID)
	{
	case 0:
		variable = m_child1->adap.initialPt;
		break;
	case 1:
		variable = 0;	//m_child2->m_initVal;
		break;
	case 2:
		variable = GetDlgItemDouble(m_child3->m_hWnd, IDC_INIT_VAL, &check);
		if (!check) MessageBox ("Invalid initial value");
		break;
	}
	return 1;
}

void CpsyntegDlg::DoneSession(CString buf2Print)
{
	double jnd, std, jnd2, std2;
	int nItems;
	CString s, buf, buf2, buf3, longBuf;
	static CString filename;
	char zbuf[MAX_PATH], zbuf1[MAX_PATH], zbuf2[MAX_PATH], zbuf3[MAX_PATH];
	char szFilter[] = "Text Files (*.txt)|*.txt|All Files (*.*)|*.*||";

	stopped=1;
	CFileDialog dlg (0, "txt", filename, OFN_HIDEREADONLY, szFilter);
	if (strlen(ResultPath)>0)		dlg.m_ofn.lpstrInitialDir = ResultPath; 

	flySendText (FL_DONE, "Session Done.");
	EnableID (this, IDC_GO, 1);
	EnableID (this, IDC_STOP, 0);
	EnableID (this, IDC_PLAY, 1);
	EnableID (this, IDC_PLAY2, 1);
	EnableID (this, IDC_TASKTAB, 1);

	if (dlgs[taskID]->DoneSession() != 1)
		return;
	switch(taskID)
	{
	case 0:
		nItems = GetIndexWithZero(thresholdCand[0], MAX_TRIALS);
		jnd = getmean(m_child1->adap.finalRev4Avg, thresholdCand[0]+m_child1->adap.reversal[0]+m_child1->adap.reversal[1]-m_child1->adap.finalRev4Avg, &std);
		if (m_child1->adap.dualSeries)
		{
			jnd2 = getmean(m_child1->adap.finalRev4Avg, thresholdCand[1]+m_child1->adap.reversal[0]+m_child1->adap.reversal[1]-m_child1->adap.finalRev4Avg, &std2);
			buf.Format ("Testing complete. Converged values=%5.1f, %5.1f", jnd, jnd2);
			buf2.Format("Results:(Series 1)%5.1f, (Series 2)%5.1f\n", jnd, jnd2);
		}
		else
		{
			buf.Format ("Testing complete. Converged value=%5.1f", jnd);
			buf2.Format("Result=%5.1f\r\n", jnd);
		}
		MessageBox (buf);
		longBuf = buf2;
		m_child1->GetDlgItemText (IDC_INIT_VAL, zbuf, sizeof(zbuf));
		sprintfFloat(m_child1->adap.stepsize[0], 3, zbuf1, sizeof(zbuf1));
		sprintfFloat(m_child1->adap.stepsize[1], 3, zbuf2, sizeof(zbuf2));
		strcpy(zbuf3, feedback ? "y" : "n");
		buf3.Format("%dIFC %dd-%dup, fdback:%s, Instruction:%s\r\nBegin/steps/revs/lastavg:%s    %s,%s    %d,%d    %d\r\n", 
			nIntervals, m_child1->adap.down, m_child1->adap.up, zbuf3, m_child1->instruction, zbuf, zbuf1, zbuf2, m_child1->adap.reversal[0], m_child1->adap.reversal[1], m_child1->adap.finalRev4Avg);
		longBuf += buf3;
		EditPrintf (m_logEdit.m_hWnd, buf2);
		EditPrintf (m_logEdit.m_hWnd, buf3);
		break;
	case 1:
		MessageBox (buf2Print);
		strcpy(zbuf3, feedback ? "y" : "n");
		s.Format("%dIFC, fdback:%s, Instruction:%s\r\n", nIntervals, zbuf3, m_child2->instruction);
		buf.Format("%s%s", buf2Print, s);
		longBuf = buf;
		// Restore tag (remove the local tags from CConstantStim)
//		for (int i=0; i<m_child2->nVars; i++) RemoveTaggedSig(nTags-1);
		break;
	case 2:
		sprintfFloat(variable, 3, zbuf, sizeof(zbuf));
		buf.Format("Instruction: %s\r\nSession done (after played %d times). Adjusted value:%s\r\n",  m_child3->instruction, trial+1, zbuf);
		MessageBox (buf);
		longBuf = buf;
		break;
	case 3:
		GetDlgItemText(IDC_LOG, buf);
		MessageBox (buf);
		longBuf = buf + "\r\n";
		break;
	}

	report.filename = "LastSession.log";
	remove(report.filename);
	SaveLogFile(report.filename, longBuf);
	if (report.fUseEmail || report.fUseFtp)
		_beginthread(uploadLogFile, 0, (void*)&report);

	if (taskID != 3 && dlg.DoModal()==IDOK)		
	{
		CString s, fnameWpath;
		fnameWpath.Format("%s%s", AppPath, INIFILE);

		zbuf[0]=0;
		filename = dlg.GetPathName();
		GetPathFileNameFromFull(zbuf, zbuf1, filename);
		printfINI(zbuf3/*errstr*/, fnameWpath, "RESULT PATH", "%s", zbuf);
	}
	SaveLogFile(filename, longBuf);
	trial=-999;
}


int CpsyntegDlg::SaveLogFile(CString& fname, CString& string2save)
{
	if (fname.GetLength()==0) return 0;

	CTime t = CTime::GetCurrentTime();
	CTimeSpan tspan; 
	CString s, ss, str = t.Format( "%H:%M:%S, " );
	HANDLE hFile;
	char buf[32], procname[256], verstr[16];
	DWORD dw;
	int e1, e2, cl;
	double period;
	LONGLONG iHours;

	if(fname.GetLength()==0)
		return 1;

	if ( (hFile = ::CreateFile (fname, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
				OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ))== INVALID_HANDLE_VALUE)
	{
		errstr.Format("Cannot open %s", fname);
		return -1; //file not found or something like that
	}
	::SetFilePointer(hFile, 0, NULL, FILE_END);
	s = beginTime.Format("%H:%M:%S, %m/%d/%Y");
	tspan = t - beginTime;
	iHours = tspan.GetDays() * 24 + tspan.GetHours();
	ss = tspan.Format("%M:%S");
	if (iHours!=0)
	{
		sprintf (buf, "%d hrs,", iHours);
		ss = buf + ss; 
	}

	GetCurrentProcInfo(AppPath, procname, verstr);

	str.Format( "%s(%s)  Subject:%s, Time/Date: %s (duration: %s)\r\n", procname, verstr, m_substr, s, ss);
	if(!WriteFile (hFile, str, str.GetLength(), &dw, NULL))
	{
		errstr.Format("Cannot write to file %s", fname);
		return -2;
	}
	if (taskID == 3) {
		if (!m_child4->m_Script.IsEmpty()) {
			str = "Custom Script:\r\n" + m_child4->m_Script + "\r\n";
			if(!WriteFile (hFile, str, str.GetLength(), &dw, NULL))
			{
				errstr.Format("Cannot write to file %s", fname);
				return -2;
			}
		}
	} else {
	if (m_tagStr.GetLength()>0)
	{
		str = "Tags Defined:\r\n" + m_tagStr + "\r\n";
		if(!WriteFile (hFile, str, str.GetLength(), &dw, NULL))
		{
			errstr.Format("Cannot write to file %s", fname);
			return -2;
		}
	}
	if (m_tagStr2.GetLength()>0)
	{
		str = "Per-trial Variables:\r\n" + m_tagStr2 + "\r\n";
		if(!WriteFile (hFile, str, str.GetLength(), &dw, NULL))
		{
			errstr.Format("Cannot write to file %s", fname);
			return -2;
		}
	}
	str = "Reference:\r\n" + m_bricString + "\r\n";
	if(!WriteFile (hFile, str, str.GetLength(), &dw, NULL))
	{
		errstr.Format("Cannot write to file %s", fname);
		return -2;
	}
	str = "Varied:\r\n" + m_bricString2 + "\r\n";
	if(!WriteFile (hFile, str, str.GetLength(), &dw, NULL))
	{
		errstr.Format("Cannot write to file %s", fname);
		return -2;
	}
	if (m_condOn)
	{
		GetCondSetting(&period, &cl, &e1, &e2);
		sprintfFloat(1.e6/period, 3, buf, sizeof(buf));
		str.Format("Conditioner: (%d,%d), cl=%d, %s Hz\r\n", e1, e2, cl, buf);
		if(!WriteFile (hFile, str, str.GetLength(), &dw, NULL))
		{
			errstr.Format("Cannot write to file %s", fname);
			return -2;
		}
	}
	}
	str = string2save + "===========================\r\n";
	if(!WriteFile (hFile, str, str.GetLength(), &dw, NULL))
	{
		errstr.Format("Cannot write to file %s", fname);
		return -2;
	}
	if(!::CloseHandle(hFile))
	{
		errstr.Format("Cannot close file %s", fname);
		return -3;
	}
	errstr = "No error";
	return 0;
}

int CpsyntegDlg::WriteINI()
{
	CString s, fnameWpath;
	fnameWpath.Format("%s%s", AppPath, INIFILE);
	char errStr[128], buf[64], buf2[64];

	CHEC(printfINI(errStr, fnameWpath, "DEF_TASK_ID", "%d", taskID))
	CHEC(printfINI(errStr, fnameWpath, "INTERVALDELAY", "%d", intervalDelay))
	CHEC(printfINI(errStr, fnameWpath, "TRIALDELAY", "%d", trialDelay))
	sprintfFloat(charge_limit, 4, buf, sizeof(buf));
	CHEC(printfINI(errStr, fnameWpath, "CHARGE LIMIT", "%s", buf))
	CHEC(printfINI(errStr, fnameWpath, "UDFPATH", "%s", m_udfpath))
	// Log reporting
	if (report.fUseEmail)
		CHEC(printfINI(errStr, fnameWpath, "LOG EMAIL ADDRESS", "%s", report.email_address))
	else
		CHEC(printfINI(errStr, fnameWpath, "LOG EMAIL ADDRESS", ""))
	if (report.fUseFtp) {
		CHEC(printfINI(errStr, fnameWpath, "LOG FTP ADDRESS", "%s", report.ftp_address))
		CHEC(printfINI(errStr, fnameWpath, "LOG FTP ID", "%s", report.ftp_id))
		CHEC(printfINI(errStr, fnameWpath, "LOG FTP PASSWORD", "%s", report.ftp_password))
	} else
		CHEC(printfINI(errStr, fnameWpath, "LOG FTP ADDRESS", ""))

	CHEC(printfINI(errStr, fnameWpath, "ANCHORED STANDARD", "%d", m_anchored_ref))
	CHEC(printfINI(errStr, fnameWpath, "INSTRUCTIONS", "%s", instruction[0]))
	CHEC(printfINI(errStr, fnameWpath, "NUM_INTERVALS", "%d", nIntervals))

	CHEC(printfINI(errStr, fnameWpath, "DOWN_UP", "%d %d", m_child1->adap.down, m_child1->adap.up))
	CHEC(printfINI(errStr, fnameWpath, "FEEDBACK", "%d", feedback))
	sprintfFloat(m_child1->adap.stepsize[0], 6, buf, sizeof(buf));
	sprintfFloat(m_child1->adap.stepsize[1], 6, buf2, sizeof(buf2));
	CHEC(printfINI(errStr, fnameWpath, "STEPSIZES", "%s %s", buf, buf2))
	CHEC(printfINI(errStr, fnameWpath, "FACTOR", "%d", m_child1->adap.factor))
	CHEC(printfINI(errStr, fnameWpath, "REVERSALS", "%d %d", m_child1->adap.reversal[0], m_child1->adap.reversal[1]))
	CHEC(printfINI(errStr, fnameWpath, "FINAL_REV_4_AVG", "%d", m_child1->adap.finalRev4Avg))
	sprintfFloat(m_child1->adap.initialPt , 6, buf, sizeof(buf));
	CHEC(printfINI(errStr, fnameWpath, "INITIAL VALUE", "%s", buf))
	m_child2->GetDlgItemText(IDC_IND_VAR_STR, s);
	CHEC(printfINI(errStr, fnameWpath, "INDEPENDENT VAR", "%s", s))
	CHEC(printfINI(errStr, fnameWpath, "REPEATS FOR EACH VAR", "%d", m_child2->nTrials))
	sprintfFloat(m_child3->val, 6, buf, sizeof(buf));
	sprintfFloat(m_child3->m_step, 6, buf2, sizeof(buf2));
	CHEC(printfINI(errStr, fnameWpath, "INITIAL VALUE AND STEP", "%s %s", buf, buf2))
	CHEC(printfINI(errStr, fnameWpath, "RANGE", "%d %d", m_child3->m_range1, m_child3->m_range2))
	CHEC(printfINI(errStr, fnameWpath, "INSTRUCTION1", "%s", m_child1->instruction))
	CHEC(printfINI(errStr, fnameWpath, "INSTRUCTION2", "%s", m_child2->instruction))
	CHEC(printfINI(errStr, fnameWpath, "INSTRUCTION3", "%s", m_child3->instruction))
	CHEC(printfINI(errStr, fnameWpath, "CUSTOM SCRIPT", "%s", m_child4->m_Script))
	CHEC(printfINI(errStr, fnameWpath, "PROCESSOR", "%s", sprocessor))
	CHEC(printfINI(errStr, fnameWpath, "NIC2 PATH", "%s", NIC2Path))

	ReadWriteStimFile(fnameWpath, 0);

	return 1;
}

#define FROM_INI_OR_DEFAULT(ITEM, FORMAT, VAR) \
if (sscanfINI(errStr, fnameWpath, ITEM, FORMAT, &VAR)<=0) RetrieveDefaultParameter(ITEM);
#define FROM_INI_OR_DEFAULT2(ITEM, FORMAT, VAR, VAR2) \
if (sscanfINI(errStr, fnameWpath, ITEM, FORMAT, &VAR, &VAR2)<=0) RetrieveDefaultParameter(ITEM);

int CpsyntegDlg::LoadINI()
{
	char buf[256];
	CString fnameWpath;
	string sbuf[2];
	int res;
	char errStr[256];
	fnameWpath.Format("%s%s", AppPath, INIFILE);

	if (ReadINI(errStr, fnameWpath, "PROCESSOR", buf, 256)<=0) 		
	{		MessageBox(strcat(errStr, " PROCESSOR"), "Default will be used."); sprocessor = DEF_PROCESSOR; EmulMode=true; }
	else
	{ sprocessor = buf;}

	if (ReadINI(errStr, fnameWpath, "NIC2 PATH", buf, 256)<=0)
	{		MessageBox(strcat(errStr, " NIC2 PATH"), "Default will be used."); NIC2Path = DEF_NIC2PATH; }
	else
	{ NIC2Path = buf;}

	if (ReadINI(errStr, fnameWpath, "RESULT PATH", ResultPath, MAX_PATH)<=0) ResultPath[0] = 0;
	if (ReadINI(errStr, fnameWpath, "STIM PATH", StimPath, MAX_PATH)<=0) StimPath[0] = 0;

	FROM_INI_OR_DEFAULT ("DEF_TASK_ID", "%d", taskID)
	FROM_INI_OR_DEFAULT ("ANCHORED STANDARD", "%d", m_anchored_ref)

	FROM_INI_OR_DEFAULT ("INTERVALDELAY", "%d", intervalDelay)
	FROM_INI_OR_DEFAULT ("TRIALDELAY", "%d", trialDelay)
	FROM_INI_OR_DEFAULT ("ROVING", "%lf", roving)
	FROM_INI_OR_DEFAULT ("CHARGE LIMIT", "%lf", charge_limit)

	if (ReadINI(errStr,fnameWpath,"UDFPATH",buf, 256)>0) {
		m_udfpath = buf;
		AstSig.SetPath("aux", m_udfpath);
	} else {
		m_udfpath = "";
		AstSig.SetPath("aux", AppPath);
	}
	// Log reporting
	if (ReadINI(errStr, fnameWpath, "LOG EMAIL ADDRESS", buf, sizeof(buf))>0) {
		report.fUseEmail = true;
		report.email_address = buf;
	}
	if (ReadINI(errStr, fnameWpath, "LOG FTP ADDRESS", buf, sizeof(buf))>0) {
		report.fUseFtp = true;
		report.ftp_address = buf;
		if (ReadINI(errStr, fnameWpath, "LOG FTP ID", buf, sizeof(buf))>0)
			report.ftp_id = buf;
		if (ReadINI(errStr, fnameWpath, "LOG FTP PASSWORD", buf, sizeof(buf))>0)
			report.ftp_password = buf;
	}

	if (ReadINI(errStr, fnameWpath, "INSTRUCTIONS", buf, 256)>0) {
		str2array(sbuf, 2, buf, "\r\n"); 
		instruction[0] = sbuf[0].c_str(); instruction[0].TrimLeft(" \t");
	} else {
		instruction[0] = "Listen...";
		instruction[1] = "(Not used)";
	}
	if (ReadINI(errStr, fnameWpath, "INSTRUCTION1", buf, 256)>0) 
	{	trimLeft(buf, " "); trimRight(buf, "\r\n"); trimRight(buf, " "); m_child1->instruction = buf; }
	if (ReadINI(errStr, fnameWpath, "INSTRUCTION2", buf, 256)>0) 
	{	trimLeft(buf, " "); trimRight(buf, "\r\n"); trimRight(buf, " "); m_child2->instruction = buf; }
	if (ReadINI(errStr, fnameWpath, "INSTRUCTION3", buf, 256)>0) 
	{	trimLeft(buf, " "); trimRight(buf, "\r\n"); trimRight(buf, " "); m_child3->instruction = buf; }
	string strbuf;
	if (ReadINI(errStr, fnameWpath, "CUSTOM SCRIPT", strbuf)>0) 
	{	 m_child4->m_Script = strbuf.c_str(); }

	FROM_INI_OR_DEFAULT ("NUM_INTERVALS", "%d", nIntervals)
	FROM_INI_OR_DEFAULT2 ("DOWN_UP", "%d %d", m_child1->adap.down, m_child1->adap.up)
	FROM_INI_OR_DEFAULT ("FEEDBACK", "%d", feedback)
	FROM_INI_OR_DEFAULT2 ("STEPSIZES", "%lf %lf", m_child1->adap.stepsize[0], m_child1->adap.stepsize[1])
	FROM_INI_OR_DEFAULT2 ("REVERSALS", "%d %d", m_child1->adap.reversal[0], m_child1->adap.reversal[1])
	FROM_INI_OR_DEFAULT ("FINAL_REV_4_AVG", "%d", m_child1->adap.finalRev4Avg)
	FROM_INI_OR_DEFAULT ("FACTOR", "%d", m_child1->adap.factor)
	FROM_INI_OR_DEFAULT ("INITIAL VALUE", "%lf", m_child1->adap.initialPt)
	res = ReadINI(errStr, fnameWpath,"INDEPENDENT VAR", buf, 256);
	trimLeft(buf, " "); trimRight(buf, "\r\n"); trimRight(buf, " "); 
	if (res>0) m_child2->varlistStr = buf;
	FROM_INI_OR_DEFAULT ("REPEATS FOR EACH VAR", "%d", m_child2->nTrials)
	FROM_INI_OR_DEFAULT2 ("INITIAL VALUE AND STEP", "%lf %lf", m_child3->val, m_child3->m_step)
	FROM_INI_OR_DEFAULT2 ("RANGE", "%d %d", m_child3->m_range1, m_child3->m_range2)

	ReadWriteStimFile(fnameWpath, 1);

	UpdateData(0);

	return 1;
}

void CpsyntegDlg::RetrieveDefaultParameter(char *item)
{
	if (!strcmp(item,"DEF_TASK_ID")) taskID = 0;
	else if (!strcmp(item,"ANCHORED STANDARD")) m_anchored_ref=0;
	else if (!strcmp(item,"INTERVALDELAY")) intervalDelay=400;
	else if (!strcmp(item,"TRIALDELAY")) trialDelay=400;
	else if (!strcmp(item,"ROVING")) roving=0.;
	else if (!strcmp(item,"CHARGE LIMIT")) charge_limit = 44.;
	else if (!strcmp(item,"NUM_INTERVALS")) nIntervals=3;
	else if (!strcmp(item,"DOWN_UP")) {m_child1->adap.down = 2; m_child1->adap.up = 1; }
	else if (!strcmp(item,"FEEDBACK")) feedback=1;
	else if (!strcmp(item,"STEPSIZES")) {m_child1->adap.stepsize[0] = 7; m_child1->adap.stepsize[1] = 2;}
	else if (!strcmp(item,"REVERSALS")) {m_child1->adap.reversal[0] = 8; m_child1->adap.reversal[1] = 6;}
	else if (!strcmp(item,"FINAL_REV_4_AVG")) m_child1->adap.finalRev4Avg = 4;
	else if (!strcmp(item,"FACTOR")) m_child1->adap.factor = false;
	else if (!strcmp(item,"INITIAL VALUE")) m_child1->adap.initialPt = 30;
	else if (!strcmp(item,"REPEATS FOR EACH VAR")) m_child2->nTrials = 5;
	else if (!strcmp(item,"INITIAL VALUE AND STEP")) {m_child3->val = 30; m_child3->m_step = 2;}
	else if (!strcmp(item,"RANGE")) {m_child3->m_range1 = -7; m_child3->m_range2 = 7;}
}

void CpsyntegDlg::ReadWriteStimFile(const char * stimfile, int isRead)
{
	CString buf;
	string strbuf;
	char errStr[128];
	if (isRead)
	{
		m_tagStr = (::ReadINI(errStr, stimfile, "TAGS", strbuf)>=0) ? strbuf.c_str() : "";
		m_tagStr2 = (::ReadINI(errStr, stimfile, "TAGS2", strbuf)>=0) ? strbuf.c_str() : "";
		m_bricString = (::ReadINI(errStr, stimfile, "BRIC_STRING", strbuf)>=0) ? strbuf.c_str() : "";
		m_bricString2 = (::ReadINI(errStr, stimfile, "BRIC_STRING2", strbuf)>=0) ? strbuf.c_str() : "";
		m_substr = (::ReadINI(errStr, stimfile, "SUBJECT", strbuf)>=0) ? strbuf.c_str() : "";
		m_child2->varlistStr = (::ReadINI(errStr, stimfile, "IND_VARIABLES", strbuf)>=0) ? strbuf.c_str() : "";
		UpdateData(0); 
		//m_child2->UpdateData(0);	// makes an error. not initialized?
	}
	else
	{
		m_child2->UpdateData(1);
		CHECV(printfINI(errStr, stimfile, "IND_VARIABLES", "%s", m_child2->varlistStr))
		GetDlgItemText(IDC_TAGS, buf);
		CHECV(printfINI(errStr, stimfile, "TAGS", "%s", buf))
		GetDlgItemText(IDC_TAGS2, buf);
		CHECV(printfINI(errStr, stimfile, "TAGS2", "%s", buf))
		GetDlgItemText(IDC_BRICS, buf);
		CHECV(printfINI(errStr, stimfile, "BRIC_STRING", "%s", buf))
		GetDlgItemText(IDC_BRICS2, buf);
		CHECV(printfINI(errStr, stimfile, "BRIC_STRING2", "%s", buf))
		GetDlgItemText(IDC_SUBJ_ID, buf);
		CHECV(printfINI(errStr, stimfile, "SUBJECT", "%s", buf))
	}
}

void CpsyntegDlg::OnOpenSaveBric(int isOpen) 
{	//isOpen is 1 for open, 0 for save
	char szFilter[] = "Text Files (*.txt)|*.txt|All Files (*.*)|*.*||";
	char zbuf[MAX_PATH], zbuf1[MAX_PATH];

	CFileDialog dlg (isOpen, "txt", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter);
	if (strlen(StimPath)>0)		dlg.m_ofn.lpstrInitialDir = StimPath; 
	if (dlg.DoModal()==IDOK)
	{
		zbuf[0]=0;
		CString filename = dlg.GetPathName();
		GetPathFileNameFromFull(zbuf, zbuf1, filename);
		CString s, fnameWpath;
		fnameWpath.Format("%s%s", AppPath, INIFILE);
		printfINI(zbuf1/*errstr*/, fnameWpath, "STIM PATH", "%s", zbuf);
		if (isOpen)
			ReadWriteStimFile(filename, 1);
		else
		{
			CStdioFile file;
			file.Open(filename, CFile::modeCreate);
			file.Close();
			ReadWriteStimFile(filename, 0);
		}
		m_child2->UpdateData(0);
	}
}

void CpsyntegDlg::OnOpenStim() 
{
	OnOpenSaveBric(1);
}

void CpsyntegDlg::OnSaveStim()
{
	OnOpenSaveBric(0);
}

void CpsyntegDlg::OnUpdateText() 
{
	CString s;
	GetDlgItemText(IDC_TEXT, s);
	FER(flySendText (FL_TEXT, s))
}

CString CpsyntegDlg::Bric2String(BRIC &onebr, CString &str)
{
	double endtime; // endtime is actually the time the last pulse begin
	char buf1[16], buf2[16], buf3[16], buf4[16], buf5[16];
	VERIFY(sprintfFloat(onebr.pam.pw, 3, buf1, sizeof(buf1)));
	VERIFY(sprintfFloat(onebr.pam.pg, 3, buf2, sizeof(buf2)));
	VERIFY(sprintfFloat(onebr.pam.period, 3, buf3, sizeof(buf3)));
	endtime = onebr.tmark + onebr.pam.period * (onebr.repeat-1); 
	VERIFY(sprintfFloat(onebr.tmark/1000., 3, buf4, sizeof(buf4)));
	VERIFY(sprintfFloat(endtime/1000., 3, buf5, sizeof(buf5)));
	str.Format("(%d,%d) %s/%s/%s (%s, %s) %d\r\n", onebr.ae, onebr.re, buf1, buf2, buf3, buf4, buf5, onebr.cl);
	return str;
}


LRESULT CpsyntegDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message==WM_COMMAND && wParam==WM_DESTROY)
	{
		if (m_bClose==FALSE)
			return NULL;
	}
	
	if (message==WM_COMMAND && wParam==IDOK)
	{
			return NULL;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void CpsyntegDlg::OnClose() 
{
	m_bClose = TRUE;
	CDialog::OnClose();
}

BOOL CpsyntegDlg::RecordTrialByTrial(CString s)
{
	FILE*fp;
	char fname[MAX_PATH];
	sprintf(fname, "%s%s", AppPath, LOGFILE);
	fp = fopen(fname, "a");
	if (fp == NULL)
      AfxMessageBox(_T("Couldn't create the log file!"));
	else
   {
	   CStdioFile file (fp);
		file.SeekToEnd();
		file.WriteString(s);
		file.Close();
   }
	return 1;
}

void CpsyntegDlg::OnKillfocusPwframe()
{
	m_pwframe.GetWindowText(m_str_pwframe);
	UpdateNullFrameElectrodes(m_str_pwframe);
}

void CpsyntegDlg::PlotHistory(double *ydata)
{ 
	static double rangex1=0, rangex2=-1, rangey1, rangey2;
	int height;
	COLORREF col;
	col = toggle? RGB(140, 200, 0) : RGB(0, 200, 140);
	CLine *pbj = m_axProc->plot(trial+1, ydata, col, (char)2, 1);
	if ( rangex1 > rangex2 ) // If the range has not been initialized  
	{
		rangex1 = 0.;		rangex2 = 10.;
		rangey1 = variable - 5;
		rangey2 = variable + 5;
	}
	else
	{
		if (trial>rangex2-1)				rangex2 += 10;
		if (variable > rangey2)
			rangey2 = variable + .3 * ( variable - rangey1 );
		else if (variable < rangey1)		
			rangey1 = variable - .3 * ( rangey2 - variable );
	}
	m_axProc->setRange('x',rangex1,rangex2);
	m_axProc->setRange('y',rangey1,rangey2);
	m_axProc->setTick('x',0.,5., 0);
	height = (int) (rangey2 - rangey1 +.5);
	m_axProc->setTick('y',rangey1, rangey1+height, 9);
}


void CpsyntegDlg::CreateMyTooltip(CWnd *hCtrl, char *helpStr)
{
    HWND hwndTT; 
    TOOLINFO ti;// struct specifying info about tool in tooltip control
    unsigned int uid = 0;       
    char strTT[2048];

	HWND hwnd = hCtrl->m_hWnd;

	strcpy(strTT,"help.....");
    RECT rect;                  // for client area coordinates

    hwndTT = ::CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,
        WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,        
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        hwnd, NULL, AfxGetInstanceHandle(), NULL);

    ::SetWindowPos(hwndTT, HWND_TOPMOST, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    /* GET COORDINATES OF THE MAIN CLIENT AREA */
    ::GetClientRect (hwnd, &rect);
	
    /* INITIALIZE MEMBERS OF THE TOOLINFO STRUCTURE */
    ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = TTF_SUBCLASS;
    ti.hwnd = hwnd;
    ti.hinst = AfxGetInstanceHandle();
    ti.uId = uid;
    ti.lpszText = helpStr;
    ti.rect.left = rect.left;    
    ti.rect.top = rect.top;
    ti.rect.right = rect.right;
    ti.rect.bottom = rect.bottom;
    
    /* SEND AN ADDTOOL MESSAGE TO THE TOOLTIP CONTROL WINDOW */
    ::SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);	
	::SendMessage(hwndTT, TTM_SETMAXTIPWIDTH, 0, 400);
}


int CpsyntegDlg::GetCurrentNullE(Electrode & e1, Electrode & e2)
{
	CString s1, s2;
	s1.LoadString(IDS_STRING102); // MP null frames
	s2.LoadString(IDS_STRING103); // all else

	m_pwframe.GetWindowText(m_str_pwframe);
	
	if (s1==m_str_pwframe)
	{
		e1 = ECE1;
		e2 = ECE2;
	}
	else if (s2==m_str_pwframe)
	{
		e1 = e2 = E_POWER;
	}
	else
	{
		int e[2];
		str2array(e, 2, m_str_pwframe, "(),./ ");
		if ( !IsBetween(e[0], 0, 23) || !IsBetween(e[1], 0, 23))
		{
			e1 = e2 = E_POWER;
			MessageBox ("Improper electrod pair.");
			return 0;
		}
		else
		{
			e1 = (Electrode)e[0];
			e2 = (Electrode)e[1];
		}
	}
	return 1;
}

int CpsyntegDlg::PrepareTags()
{
	GetDlgItemText(IDC_TAGS, m_tagStr);
	m_tagStr.Trim();
	try {
		AstSig.SetNewScript(m_tagStr).Compute();
	} catch (const char *errstr) {
		MessageBox(errstr, "Error in Variable Definitions");
		return 0;
	}
	return 1;
}

int CpsyntegDlg::CheckChargeLimit(BRIC *br, double *charge)
{
	double uA = 10. * EXPONENTIAL(175., (double)(int)br->cl/255.);
	*charge = uA * br->pam.pw / 1000.;
	double multiplier(1.);
	int bpwidth;
	if (br->re==101 || br->re==102 || br->re==103)
	{
		//default
	}
	else // BP or CG
	{
		bpwidth = (int)br->re - (int)br->ae;
		switch (bpwidth)
		{
		case 0:
			multiplier = 1;
			break;
		case 1:
		case -1:
			multiplier = 5;
			break;
		case 2:
		case -2:
			multiplier = 4.5;
			break;
		case 3:
		case -3:
			multiplier = 4;
			break;
		default:
			multiplier = 4.5 - (double)bpwidth/6.;
			break;
		}
	}
	if (*charge<multiplier*charge_limit) return 1;
	else return 0;
}


void CpsyntegDlg::OnBnClickedGo()
{
	CAstSig ast(&AstSig);
	CRect rt;
	CString buf, buf2, str;

	UpdateData();
	ClearEditPrintf(m_logEdit.m_hWnd);
	if (m_figProc==NULL)
	{
		rt.SetRect(450, 420, 750, 607);
		m_figProc = OpenChildFigure(&rt, ast.Sig); 
		m_axProc = AddAxis(m_figProc, .15, .13, .75, .84);
	}
	else
	{
		m_axProc->DeleteLine(0);
		m_axProc->setRange('x', 0., -1.);
		m_axProc->setRange('y', 0., -1.);
	}
	EnableID (this, IDC_TASKTAB, 0);
	EnableID (this, IDC_PLAY, 0);
	EnableID (this, IDC_PLAY2, 0);
	EnableID (this, IDC_GO, 0);
	EnableID (this, IDC_STOP, 1);
	stopped=0;
	nTrials = 0;
	trial=-1;
	AstSig.Reset();

	if (dlgs[taskID]->OnBnClickedGo() != 1)
		return;

	if (m_bricString.GetLength()*m_bricString2.GetLength()==0)
	{
		MessageBox ("Stimulus not defined.");
		return ;
	}
	switch(taskID)
	{
	case 0:
		buf = m_bricString2;
		buf.MakeLower();
		if (buf.Find("v") == -1) {
			MessageBox ("At least one variable needs to be defined.");
			return ;
		}
		m_child1->UpdateDataPlus();
		variable = m_child1->adap.initialPt;
		flySendBin (FL_INIT, &nIntervals, sizeof(nIntervals));
		flySendText (FL_INSTRUCTION, instruction[0]);
		flySendText (FL_INSTRUCTION, m_child1->instruction);
		break;
 
	case 1:
		if (m_child2->ProcessList()<0) return;
		nConstSteps = m_child2->nSteps;
		m_child2->OnEnKillfocusIndVarStr();
		nTrials = m_child2->nTrials * nConstSteps;

		//de-alloc and alloc 
		delete[] presentingIDorder; presentingIDorder = new short[nTrials];
		delete[] scores; scores = new double[nConstSteps];		memset((void*)scores, 0, sizeof(double)*nConstSteps);
		delete[] cumTrials; cumTrials = new short[nConstSteps];	memset((void*)cumTrials, 0, sizeof(short)*nConstSteps);
		delete[] cumCorr; cumCorr = new short[nConstSteps];		memset((void*)cumCorr, 0, sizeof(short)*nConstSteps);

		for (int i=0; i<nConstSteps; i++)
		{
			for (int j=0; j<m_child2->nTrials; j++)
				presentingIDorder[i*m_child2->nTrials+j]=i;
		}
		perm (presentingIDorder, nTrials);
		FER(flySendBin(FL_INIT, &nIntervals, sizeof(nIntervals)))
		FER(flySendText(FL_INSTRUCTION, instruction[0]))
		FER(flySendText(FL_INSTRUCTION, m_child2->instruction))
		break;

	case 2:
		m_child3->UpdateData();
		nIntervals = 2;
		variable = GetDlgItemDouble(m_child3->m_hWnd, IDC_INIT_VAL);
		m_child3->SetDlgItemText(IDC_CUR_VAL, "");
		FER(flySendText (FL_INIT_SLIDER, ""))
		FER(flySendBin (FL_RANGE_MAX, &m_child3->m_range2, sizeof(m_child3->m_range1)))
		FER(flySendBin (FL_RANGE_MIN, &m_child3->m_range1, sizeof(m_child3->m_range1)))
		flySendText (FL_INSTRUCTION, m_child3->instruction);
		break;
	}

	beginTime = CTime::GetCurrentTime();
	buf = beginTime.Format("\r\n%H:%M:%S, %m/%d/%Y===================\n");
	str.Format("subj:%s\n", m_substr);
	buf += str;
	RecordTrialByTrial(buf);
	if (taskID == 3)
		return;
	buf="";
	for (int i=0; i<nTrials; i++)
	{
		buf2.Format("%d ", presentingIDorder[i]);
		buf += buf2;
	}
	buf += "\n";
	RecordTrialByTrial(buf);
	if (m_tagStr.GetLength()>0)
	{
		buf2 = m_tagStr;
		buf2.Replace("\r\n", "\n");
		str = "Tags Defined:\n" + buf2 + "\n";
		RecordTrialByTrial(str);
	}
	if (m_tagStr2.GetLength()>0)
	{
		buf2 = m_tagStr2;
		buf2.Replace("\r\n", "\n");
		str = "Per-trial Variables:\n" + buf2 + "\n";
		RecordTrialByTrial(str);
	}
	buf2 = m_bricString;
	buf2.Replace("\r\n", "\n");
	str = "Reference:\n" + buf2 + "\n";
	RecordTrialByTrial(str);
	buf2 = m_bricString2;
	buf2.Replace("\r\n", "\n");
	str = "Varied:\n" + buf2 + "\n";
	RecordTrialByTrial(str);

	if (nSeq<nIntervals)
	{
		nSeq = nIntervals;
		if (nic1)
		{
			delete[] seqSPrint;
			seqSPrint = (IMP_SEQUENCE**) new IMP_SEQUENCE*[nSeq];
		}
		else
		{ // for this version, let's not worry about power-up frames for NIC2 (also conditioner is not a possibility)
		  // then just delete and update with new sequence handles
			for (int i=0; i<nSeq; i++) sequenceCommandDelete(seqq[i]);
			delete[] seqq;
			seqq = (SequenceCommandHandle*) new SequenceCommandHandle[nSeq];
			for (int i=0; i<nSeq; i++) seqq[i] = SequenceNew(1);
		}
	}
}

void CpsyntegDlg::OnBnClickedStop()
{
	char EditStr[8192];
	FER(flySendText (FL_STOP, ""))
	EnableID (this, IDC_TASKTAB, 1);
	EnableID (this, IDC_GO, 1);
	EnableID (this, IDC_STOP, 0);
	EnableID (this, IDC_PLAY, 1);
	EnableID (this, IDC_PLAY2, 1);
	EditStr[0]='\0';
	switch(taskID)
	{
	case 0:
		break;

	case 1:
		nTrials = m_child2->nTrials * m_child2->var.nSamples;
		EditPrintf (m_logEdit.m_hWnd, "===Session Aborted===\n");
		GetDlgItemText(IDC_LOG, EditStr, sizeof(EditStr));
		// Restore tag (remove the local tags from CConstantStim)
//		for (int i=0; i<m_child2->nVars; i++) RemoveTaggedSig(nTags-1);
		break;

	case 3:
	case 4:
		EditPrintf (m_logEdit.m_hWnd, "===Session Aborted===\n");
		GetDlgItemText(IDC_LOG, EditStr, sizeof(EditStr));
		break;
	}
	trial=-999;
	AbortSession(BYOPERATOR);
	if (taskID==1 || taskID==3) DoneSession(EditStr);
}

InterfaceType CpsyntegDlg::string2itype(char *str, bool &emul)
{
	int port;
	InterfaceType res;
	bool _emul(false);
	trimLeft(str, " "); trimRight(str, " ");
	if (!strnicmp(str,"SPrint", 6))	
	{
		if (!strnicmp(str+7, "CPS", 3)) return SPRINT_CPS;
		if (!strnicmp(str+7, "PPS", 3)) 
		{
			if (sscanf(str+10, "%d", &port)!=1) return UNSPECIFIED;
			return (InterfaceType)((int)SPRINT_PPS1+port-1);
		}
	}
	else if (!strnicmp(str,"L34", 3))
	{
		if (!strnicmp(str+4, "CIC3", 4)) res = L34_CIC3;
		else if (!strnicmp(str+4, "CIC4", 4)) res = L34_CIC4;
		else res = UNSPECIFIED;
		if (res != UNSPECIFIED && str[strlen(str)-1]=='0') _emul = true;
	}
	else if (!strnicmp(str,"SP12", 4))
	{
		if (!strnicmp(str+5, "CIC3", 4)) res = SP12_CIC3;
		else if (!strnicmp(str+5, "CIC4", 4)) res = SP12_CIC4;
		else res = UNSPECIFIED;
		if (res != UNSPECIFIED && str[strlen(str)-1]=='0') _emul = true;
	}
	else
	{
		_emul = true;
		res = UNSPECIFIED;
	}
	emul = _emul;
	return res;
}

void CpsyntegDlg::itype2string(InterfaceType itype, bool emul, char *buf)
{
	switch (itype)
	{
	case L34_CIC3:
		strcpy(buf, "L34-CIC3-");
		if (emul) strcat(buf,"0"); else strcat(buf,"1"); 
		break;
	case L34_CIC4:
		strcpy(buf, "L34-CIC4-");
		if (emul) strcat(buf,"0"); else strcat(buf,"1"); 
		break;
	case SP12_CIC3:
		strcpy(buf, "SP12-CIC3-");
		if (emul) strcat(buf,"0"); else strcat(buf,"1"); 
		break;
	case SP12_CIC4:
		strcpy(buf, "SP12-CIC4-");
		if (emul) strcat(buf,"0"); else strcat(buf,"1"); 
		break;
	case SPRINT_CPS:
		strcpy(buf, "SPrint CPS"); 
		break;
	case SPRINT_PPS1:
	case SPRINT_PPS2:
	case SPRINT_PPS3:
	case SPRINT_PPS4:
	case SPRINT_PPS5:
	case SPRINT_PPS6:
	case SPRINT_PPS7:
	case SPRINT_PPS8:
	case SPRINT_PPS9:
		sprintf(buf, "SPrint PPS%d", itype-SPRINT_PPS1+1) ;
		break;
	default:
		strcpy(buf, "Invalid Hardware info"); 
		break;
	}
}
