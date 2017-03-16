// psyntegDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "psynteg.h"

#include "AdaptiveDlg.h"	// Added by ClassView
#include "SliderAdj.h"	
#include "FIFO.h"
#include "EditPlus.h"
#include "SetProcessorDlg.h"
#include "tabber.h"
#include "ConstantStim.h"
#include "CustomScript.h"
#include "LogReport.h"

/////////////////////////////////////////////////////////////////////////////
// CpsyntegDlg dialog

void OnSeqEnd();

#define HALFDONE			16
#define DUALDONE			 8
#define SINGLEDONE			 1
#define JUST_CONNECTED		100
#define TS_DLL_NAME			"psyntegTouch2.dll"
#define REGISTER_END_EVENT 1000
#define INIT_HARDWARE 2000
#define RECONNECTED_REPLAY 2051
#define IDT_WAIT4NEXTPAIR		8900
#define IDT_INIT_TABS			2300

#define COND_PW			 25.0

#define CHECKNIC2_0(X) \
	if ((X)!=Call_Succeeded) {errstr.Format("Error in "#X " Code=%d", lastServicesError()); return 0;}
#define CHECKNIC2_V(X) \
	if ((X)!=Call_Succeeded) {errstr.Format("Error in "#X " Code=%d", lastServicesError()); return ;}

// A to the X
#define EXPONENTIAL(A,X)	exp((X)*log(A))

class CpsyntegDlg : public CDialog
{
	friend class CConstantStim;
// Construction
public:
	bool shortnullframe; 
	CAstSig AstSig;
	CString m_udfpath;
	void HandleCIPulse(const AstNode *pnode, CAstSig *pAst);
	int HandleHookAST(CAstSig &ast, const AstNode *pnode, const AstNode *p);
	CFont SigEditFont;
	CMenu menu0;
	double duration;
	BRIC *brics;
	int nBrics;
	int allocated_max_brics;
	CProcDialog *dlgs[4];
	CTabber m_tab;
	CString sprocessor, NIC2Path;
	char ResultPath[MAX_PATH], StimPath[MAX_PATH];
	InterfaceType itype;
	bool EmulMode;
	int nIntervals;
	int refNext;
	double purt[6];
	void CreateMyTooltip(CWnd *hCtrl, char *helpStr);
	CToolTipCtrl * m_tt;
	BOOL	feedback;
	int Prepare(int id, bool RecalcTag=false);
	int Prepare(CString &strIn, CString &msgTitle);
	int UpdateBricsWithConditioner(BRIC* brics, int& nBrics);
	IMP_SEQUENCE* SetConditioner(double rate=0., int play=0, int cl=0, int aeCond=0, int reCond=0);
	int GetCondSetting(double *period, int *cl, int *aeCond, int *reCond);
	void UpdateNullFrameElectrodes(const char *str_pw);
	CShortFIFO corr;
	CDoubleFIFO var;
	bool toggle;
	bool nic1;
	void PlotHistory(double *ydata);
	int AdapProceed(int corrThis);
	BOOL RecordTrialByTrial(CString s);
	CTime beginTime;
	BOOL m_bClose;
	int interleaved;
	int stopped;
	int connected;
	CFigure* m_figProc;
	CAxis* m_axProc;
	CString Bric2String(BRIC &onebr, CString& str);
	double procVariable;
	double sliderReference;
	void AbortSession(int bywhom);
	int m_anchored_ref;
	double rovedVal;
	void RetrieveDefaultParameter(char *item);
	CString instruction[2];
	int trialDelay;
	int intervalDelay;
	double roving; 
	double charge_limit; // nC
	LogReport report;
	int playedItem;
	ADJUSTING adj;
	int SaveLogFile(CString& fname, CString& string2save);
	int WriteINI();
	int nTrials;
	int nConstSteps;
	string tagIDs[2];
	short* presentingIDorder;
	double* scores;
	short* cumTrials;
	short* cumCorr;
	void DoneSession(CString buf2Print="");
	double variable;
	double varHistory[2][MAX_TRIALS];
	double thresholdCand[2][MAX_TRIALS];
	int corrHist[2][MAX_TRIALS];
	int interval;
	int answer;
	int trial;
	int Present(int playID);
	CStatusBarCtrl sbar;
	int taskID;
	int LoadINI();
	char AppPath[MAX_PATH];
	CCustomScript* m_child4;
	CSliderAdj* m_child3;
	CConstantStim* m_child2;
	CAdaptiveDlg* m_child1;
	CString errstr;
	IMP_SEQUENCE** seqSPrint;
	IMP_FRAME *nullFrame, *condFrame;
	SequenceCommandHandle *seqq;
	NICStreamClientHandle streamClient;
	int NIC2StreamStart (SequenceCommandHandle seq2Play, int fNonBlock=0);
	int NIC2StreamStop ();
	int NIC2CleanUp();
	int InitNIC2(const char *nicpath, const char *initializeStr, char *errstr);
	int nSeq;
	int initialized;
	int Initialize(CString param);
	int OnRetryPresent();
	IMP_SEQUENCE* MakeNewPowerUpSeq();
	IMP_SEQUENCE* MakePowerOnSeq();
	IMP_SEQUENCE* BuildSeq(IMP_FRAME* powerframe, BRIC* br, int nBrics);
	int UpdateSeq(IMP_SEQUENCE** seq, IMP_FRAME* powerframe, BRIC* br, int nBrics);
	IMP_FRAME* MakeNullFrame();
	int UpdateConditionerFrame(double rate, int cl, int aeCond, int reCond);
	int CheckChargeLimit(BRIC *br, double *charge);
	InterfaceType string2itype(char *str, bool &emul);
	void itype2string(InterfaceType itype, bool emul, char *str);
	void OnPlay(int id);
	CpsyntegDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CpsyntegDlg();


// Dialog Data
	//{{AFX_DATA(CpsyntegDlg)
	enum { IDD = IDD_psynteg_DIALOG };
	CEditPlus	m_edit_condCL;
	CComboBox	m_pwframe;
	CButton	m_play2;
	CEdit	m_logEdit;
	CButton	m_play;
	CString	m_tagStr;
	CString	m_tagStr2;
	CString	m_bricString;
	CString	m_bricString2;
	CString	m_substr;
	CString	m_str_pwframe;
	BOOL	m_condOn;
	double defaultpw;
	double defaultpg;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CpsyntegDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CpsyntegDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnFileSettings();
	afx_msg void OnFileConfig();
	afx_msg void OnOpenStim();
	afx_msg void OnSaveStim();
	afx_msg void OnResetProcessor();
	afx_msg void OnFileExit();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
//	afx_msg void OnCommonSet();
	afx_msg void OnOpenBric();
	afx_msg void OnSaveBric();
	afx_msg void OnUpdateText();
	afx_msg void OnPlay1();
	afx_msg void OnPlay2();
	afx_msg void OnClose();
	afx_msg void OnKillfocusPwframe();
	afx_msg void OnCondOnoff();
	//}}AFX_MSG
	afx_msg LRESULT OnFlyConnected(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnFlyClosed(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnFlyArrived(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnFlyErr(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnNextInterval(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnSelChange(WPARAM wp, LPARAM lp=0);
	afx_msg LRESULT OnCondAdj (WPARAM wp, LPARAM lp);
	DECLARE_MESSAGE_MAP()
private:
	double ReadComputeVal(const CString _str, const CString _key, double val, bool isdB=0);
	int GetSignV(const CString _str);
	void OnOpenSaveBric(int isOpen);
	void ReadWriteStimFile(const char * stimfile, int isRead);
private:
	int GetCurrentNullE(Electrode & e1, Electrode & e2);
	afx_msg void OnEnKillfocusTags();
	int PrepareTags();
	afx_msg void OnBnClickedGo();
	afx_msg void OnBnClickedStop();
	CEdit m_edit_tag;
	CEdit m_edit_tag2;
	CEdit m_edit_bric1;
	CEdit m_edit_bric2;
};
