#include "stdafx.h"
#include "psynteg.h"
#include "psyntegDlg.h"
#include "nic.h"
#include "constants.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
 
extern CpsyntegApp theApp;

static int eventNotyHold=0;
static int retryPlayID=0;
static IMP_SEQUENCE* turnonseq;

void OnSeqEnd()
{
	CpsyntegDlg *cwnd = (CpsyntegDlg*)theApp.m_pMainWnd;
	if (!eventNotyHold)
	{
		sendfly (FL_ABOUT2STOP, "", 0);
		cwnd->m_play.EnableWindow();
		cwnd->m_play2.EnableWindow();
		if (!cwnd->stopped)
		{
			switch(cwnd->taskID)
			{
			case 0:
				cwnd->SetTimer(PLAY_NEXT_INTERVAL, cwnd->intervalDelay, NULL);
				break;
			case 1:
				cwnd->SetTimer(PLAY_NEXT_INTERVAL, cwnd->intervalDelay, NULL);
				break;
			case 2:
				cwnd->SetTimer(PLAY_NEXT_INTERVAL, cwnd->intervalDelay, NULL);
				break;
			case 3:
				cwnd->SetTimer(PLAY_NEXT_INTERVAL, cwnd->intervalDelay, NULL);
				break;
			}
		}
	}
	else
	{
//		cwnd->SendMessage(WM__RETRY_PRESENT);
	}
}

void CpsyntegDlg::OnTimer(UINT nIDEvent) 
{
	CString buf, interfaceParam;
	BRIC br;
	static int count(4);
	
	//If you need any REPETITIVE (such as time-ticking), move KillTimer inside.
	KillTimer(nIDEvent);
	switch(nIDEvent)
	{
	case JUST_CONNECTED:
		FER(flySendText (FL_RUNDLL, TS_DLL_NAME))
		break;
	case INIT_HARDWARE:
		switch (itype) 
		{
		case SPRINT_CPS:
			interfaceParam = "CPS 0x300";
			break;
		default: // PPS
			int port = (int)(itype - SPRINT_CPS);
			interfaceParam.Format ("PPS %d 115200", port);
			break;
		}
		if (Initialize(interfaceParam)==-1)  { OnClose(); return;}
		nullFrame = MakeNullFrame(); // This must be cleaned.
		condFrame = MakeNullFrame(); // This must be cleaned.

		br.ae=ECE1;		br.re=ECE2;		br.cl=0;
		br.pam.pw=25;		br.pam.pg=8;
		br.tmark=0;		br.repeat=1500;	br.pam.period=200; // 300-ms long

		seqSPrint[0] = BuildSeq(nullFrame, &br, 1);
		seqSPrint[1] = BuildSeq(nullFrame, &br, 1);
		if (!ImpCommunicatorWriteSequence(ProgramSlot2, seqSPrint[0])) MessageBox ("Error in ImpCommunicatorWriteSequence","psynteg, OnInitDialog");
		if (!ImpCommunicatorStartSequence(ProgramSlot2)) MessageBox ("Error in ImpCommunicatorStartSequence","psynteg, OnInitDialog");
		SetTimer (REGISTER_END_EVENT, 1000, NULL);
		break;
	case RECONNECTED_REPLAY:
		OnRetryPresent();
		break;
	case REGISTER_END_EVENT:
		RegisterOnSequenceEndFunction(OnSeqEnd);
		break;
	case PLAY_NEXT_TRIAL:
		if (taskID==0)
		{
			buf.Format("%d: ", trial+1);
			EditPrintf (m_logEdit.m_hWnd, buf);
		}
		else 
		{
			if (taskID!=3) trial++;
		}
	case PLAYAGAIN:
		refNext = interval = 0;
		PostMessage(WM__NEXT_INTERVAL, interval);
		break;
	case PLAY_NEXT_INTERVAL:
		if (taskID==2 || taskID==3) // slider or prank
		{
			if (++interval==1)
			{
				PostMessage(WM__NEXT_INTERVAL, interval);
			}
			else
			{
				FER(flySendText (FL_END_OF_PRSENTATION, ""))
				interval=0;
			}
		}
		else
		{
			if (++interval<nIntervals)
				PostMessage(WM__NEXT_INTERVAL, interval);
		}
		break;
	case IDT_WAIT4NEXTPAIR:
		if (count>0)
		{
			FER(flySendBin (FL_WAIT4NEXT, &count, sizeof(count)));
			count--;
			SetTimer(IDT_WAIT4NEXTPAIR, 1000, NULL);
		}
		else
		{
			FER(flySendBin (FL_INIT_PRANK, &nTrials, sizeof(nTrials)));
			count=3;
		}
		break;
	case IDT_INIT_TABS:
		m_tab.InitDialogs(this);
		m_tab.InsertItem(0,"Adaptive");
		m_tab.InsertItem(1,"Constant");
		m_tab.InsertItem(2,"Adjustment");
		m_tab.InsertItem(3,"Custom");
		m_tab.SetCurSel(taskID);
		m_tab.ActivateTabDialogs(taskID);
		OnSelChange(taskID);
		break;
	}
	CDialog::OnTimer(nIDEvent);
}

void CpsyntegDlg::AbortSession(int bywhom)
{
	if (nic1)
	{
		CERV(ImpCommunicatorStopSequence(1));
	}
	else
		CHECKNIC2_V(nscStopStream(streamClient))
	stopped=1;
	if (taskID==3 && !SetEvent(m_child4->hEvent)) {
		CString errstr;
		errstr.Format("SetEvent failed (%d)\n", GetLastError());
		MessageBox(errstr, "Error", MB_ICONERROR);
	}
	if (bywhom==BYSUBJECT)
	{
		MessageBox ("Stop pressed by subject");
	}
	else
	{
		MessageBox ("Session stopped.");
	}
	RecordTrialByTrial("===Session Aborted===\n");
}

int CpsyntegDlg::Initialize(CString param)
{
	initialized = 1;
	int i, address, port, baud;
	char stimmode[5];
	char *parsed[3], buf0[16], buf1[16], buf2[16];
	CString st;

	strcpy(stimmode, "MP");
	CER(ImplantSelectType (CIC3, EMBEDDED, 5.0));
	parsed[0] = buf0; parsed[1] = buf1; parsed[2] = buf2;
	if (param.GetAt(0)=='C')
	{
		str2array(parsed, 2, param, " ");
		sscanf(parsed[1],"%x",&address);
		CER(ImpCommunicatorSetCPSConfiguration(address)); // 0x300 default
		sbar.SetText(param, 1, 0);
	}
	else if (param.GetAt(0)=='P')
	{
		str2array(parsed, 3, param, " ");
		sscanf(parsed[1],"%d",&port);
		sscanf(parsed[2],"%d",&baud); // 115200 default
		CER(ImpCommunicatorSetPPSConfiguration(port, baud));
		st.Format("PPS com%d", port);
		sbar.SetText(st, 1, 0);
	}
	else
	{
		st.Format("%s:Invalid interface parameters.", param);
		MessageBox (st);
		return NULL;
	}
	//if (!ImpCommunicatorInit())
	//{
	//	while (!ImpCommunicatorReset())
	//	{
	//		if (MessageBox ("ImpCommunicatorInit failed. Retry=yes, Abort=no", "psynteg", MB_YESNO)==IDNO)
	//			return -1;
	//	}
	//}
	if ( (i = ImpCommunicatorConnect()) == 0)
	{
		CER(ImpCommunicatorForceConnect());		
		sbar.SetText("Speech Processor Connected in 2nd try", 2, 0);
	}
	else
		sbar.SetText("Speech Processor Connected", 2, 0);
	return 1;
}

IMP_SEQUENCE* CpsyntegDlg::MakePowerOnSeq()
{
	IMP_SEQUENCE* tempseq;
	if (nic1)
	{
		Electrode nullAE, nullRE;
		GetCurrentNullE(nullAE, nullRE);
		IMP_FRAME* pw_frame = ImpFrameNew();
		CER(ImpFrameSetPhaseWidth (pw_frame, 25.));
		CER(ImpFrameSetPhaseGap (pw_frame, 8.));
		CER(ImpFrameSetPeriod (pw_frame, POWER_FRAME_PERIOD));
		CER(ImpFrameSetCurrentLevel (pw_frame, 0));
		CER(ImpFrameSetElectrodes (pw_frame, nullAE, nullRE));

		tempseq = ImpSequenceNew(pw_frame);

		CER(ImpFrameSetCurrentLevel (pw_frame, 1));
		CER(ImpFrameSetElectrodes (pw_frame, ICE1, ECE2));
		CER(ImpFrameSetPeriod (pw_frame, 200.));
		CER(ImpSequenceAppendFrames (tempseq, pw_frame, 1000));
		CER(ImpFrameDelete(pw_frame));
	}
	else
	{
	}
	return tempseq;
}

void CpsyntegDlg::OnPlay(int id) 
{
	int id1 = id? CONTROLLED : REFERENCE;
	UpdateData();
	sendfly (FL_PLAY_SHORT, "", 0);
	if (Prepare(id1, true))
	{
		m_play.EnableWindow(0);		m_play2.EnableWindow(0);
		if (!Present(id)) 
		{
			m_play.EnableWindow(1);	m_play2.EnableWindow(1);
		}
	}
	else
	{
		m_play.EnableWindow(1);	m_play2.EnableWindow(1);
	}
}

void CpsyntegDlg::OnPlay1() 
{
	OnPlay(0);
}

void CpsyntegDlg::OnPlay2() 
{ // controlled is played only when slider is in use.
	if (taskID==2)
		variable = GetDlgItemDouble(m_child3->m_hWnd, IDC_INIT_VAL);
	OnPlay(1);
}
 

LRESULT CpsyntegDlg::OnFlyConnected(WPARAM code, LPARAM lParam)
{
	sbar.SetText("Connection: ON", 0, 0);
	if (!connected) 
	{
		SetTimer(JUST_CONNECTED, 500, NULL);
	}
	return (connected=1);
}

LRESULT CpsyntegDlg::OnFlyClosed(WPARAM code, LPARAM lParam)
{
	sbar.SetText("Connection: OFF", 0, 0);
	connected=0;
	return 1;
}

LRESULT CpsyntegDlg::OnFlyErr(WPARAM code, LPARAM lParam)
{
	sbar.SetText("Communication Error", 0, 0);
	return 1;
}

int CpsyntegDlg::AdapProceed(int corrThis)
{
	static CAdaptive *obj=&m_child1->adap; // toggles between m_child1->adap and m_child1->adap2
	static int onceDual;
	int res;
	double val = var.get();
	if (corrThis<0)
		onceDual = m_child1->adap.dualSeries;
	if (m_child1->adap.dualSeries)
	{
		if (toggle) ++trial;
		toggle = !toggle;
		obj = toggle ? &m_child1->adap2 : &m_child1->adap;
		if (res = obj->Proceed (corrThis, &val)) 
		{
			toggle = !toggle;
			obj = toggle ? &m_child1->adap2 : &m_child1->adap;
			var.set(-9999.9);
			corr.set(8);
			val = var.get();
			if (res = obj->Proceed (corr.get(), &val))  return DUALDONE;
			variable = val;
			m_child1->adap.dualSeries=0;
			corr.SetLength(1);
			var.SetLength(1);
			varHistory[toggle][trial] = val;
			return HALFDONE;
		}
		varHistory[toggle][trial] = val;
	}
	else
	{
		++trial;
		if (res = obj->Proceed (corrThis, &val)) 
		{
			if (onceDual)		return DUALDONE;
			else				return SINGLEDONE;

		}
		varHistory[toggle][trial] = val;
	}
	variable = val;
	return res;
}

LRESULT CpsyntegDlg::OnFlyArrived(WPARAM code, LPARAM lParam)
{
	int res, response;
	double initval;
	static int clipped;
	bool curCorr;
	short sd;
	char valbuf[64], strbuf[64];
	CString zbuf, buf, temp[2];

	if (dlgs[taskID]->OnFlyArrived(code, lParam) != 1)
		return 1;

	switch (LOWORD(code))
	{
	case FL_STOP:
		AbortSession(BYSUBJECT);
		OnBnClickedStop();
		break;

	case FL_BEGIN:
		stopped=0;
		if(taskID==0)
		{
			memset((void*)varHistory, 0, sizeof(varHistory));
			m_child1->adap.InitAdaptive (corrHist[0], thresholdCand[0]);
			if (m_child1->adap.dualSeries)
			{
				toggle = 1;
				if (nIntervals!=2)
				{
					MessageBox ("Only 2AFC is allowed for dual-series adaptive procedure.");
					OnBnClickedStop();
					break;
				}
				m_child1->adap2.InitAdaptive (corrHist[1], thresholdCand[1]);
				corr.SetLength(2);
				corr.set(-2); corr.set(-1);
				var.SetLength(2);
				var.set(m_child1->adap.initialPt); 
				var.set(m_child1->adap2.initialPt);
			}
			else
			{
				toggle = 0;
				corr.SetLength(1);
				corr.set(-1);
				var.SetLength(1);
				var.set(m_child1->adap.initialPt); 
			}
			AdapProceed (corr.get());
			PlotHistory(varHistory[toggle]);
		}
		answer = getRandAnswer(nIntervals);
		if (Prepare(ALL,true))
		{
			if (taskID==1)
			{
				strcpy(strbuf,"");
				strcat(strbuf, tagIDs[0].c_str()); strcat(strbuf," ");
				if (m_child2->nVars>1)	strcat(strbuf, tagIDs[1].c_str()); 
				m_child2->GenLogStr(buf, strbuf, cumCorr, cumTrials);
				EditPrintf (m_logEdit.m_hWnd, buf);
			}
			SetTimer(PLAY_NEXT_TRIAL, trialDelay, NULL);
		}
		else
			OnBnClickedStop();
		break;
	case FL_RESPONSE:
		response = *(int*)lParam;
		curCorr = answer ==response;
		answer = getRandAnswer(nIntervals);
		if (toggle)			curCorr = !curCorr;
		corr.set(curCorr);
		if (curCorr)
		{	zbuf = "Correct"; buf = "  O\r\n";	}
		else
		{	zbuf = "Incorrect"; buf = "  X\r\n";	}
		RecordTrialByTrial(curCorr ? " O\n" : " X\n");
		
		if (taskID==0)
		{
			if (feedback)				flySendText (FL_FEEDBACK, zbuf);
			EditPrintf (m_logEdit.m_hWnd, buf);
			
			res = AdapProceed (corr.get());
			if (!Prepare(ALL)) return 1; // just exist on error
			if (res==DUALDONE)				m_child1->adap.dualSeries=1;
			if (res==DUALDONE || res==SINGLEDONE)
				DoneSession();
			else if (res==-1)
			{
				MessageBox ("Reached the boundary. Session aborted.");
				OnBnClickedStop();
			}
			else // continue
			{
				SetTimer(PLAY_NEXT_TRIAL, trialDelay, NULL);
				if (m_child1->adap.dualSeries)
				{
					if (m_axProc->nLines==2)
						m_axProc->DeleteLine(0);
				} 
				else
					m_axProc->DeleteLine(m_axProc->nLines-1);
				PlotHistory(varHistory[toggle]);
			}
		}
		else
		{
			if (!Prepare(ALL)) return 1; // just exist on error
			if (feedback)				flySendText (FL_FEEDBACK, zbuf);
			cumTrials[presentingIDorder[trial]]++;
			cumCorr[presentingIDorder[trial]] += curCorr;
			scores[presentingIDorder[trial]] = (double) cumCorr[presentingIDorder[trial]] / (double)cumTrials[presentingIDorder[trial]] * 100.;
			ClearEditPrintf(m_logEdit.m_hWnd);
			strcpy(strbuf,"");
			strcat(strbuf, tagIDs[0].c_str()); strcat(strbuf," ");
			if (m_child2->nVars>1) strcat(strbuf, tagIDs[1].c_str()); 
			m_child2->GenLogStr(buf, strbuf, cumCorr, cumTrials);			
			EditPrintf (m_logEdit.m_hWnd, buf);
			
			if (trial==nTrials-1)
			{
				m_axProc->DeleteLine(-1);
				m_axProc->plot(nConstSteps, scores, RGB(240, 50, 200), (char)2, 1);
				DoneSession(buf);
			}
			else
				SetTimer(PLAY_NEXT_TRIAL, trialDelay, NULL);
		}
		break;
		
	case FL_PLAYAGAIN:
		SetTimer(PLAYAGAIN, trialDelay, NULL);
		if (taskID==0)
			EditPrintf (m_logEdit.m_hWnd, "\r\n");
		break;

	case FL_READYVALUE://taskID:3
		sd = *(short*)lParam;
		cumTrials[trial++] = sd;
		buf.Format("%d ", sd);
		EditPrintf (m_logEdit.m_hWnd, buf);
		if (trial==nTrials) DoneSession();
		else 
			SetTimer(IDT_WAIT4NEXTPAIR, 1000, NULL);
		break;

	case FL_PLAY: //taskID is always 2 (method of adjustment)
		if (++trial==0) curCorr = true;
		else curCorr = false;
		answer=1;
		stopped=0;
		interval = 0;
		if (Prepare(ALL, curCorr))
			PostMessage(WM__NEXT_INTERVAL, interval);
		else
			OnBnClickedStop(); 
		break;

	case FL_ADJUSTING:
		adj = *(ADJUSTING*)lParam;
		variable = m_child3->GetDlgItemInt(IDC_INIT_VAL) + m_child3->m_step * adj.newVal;
		sprintfFloat (variable, 1,  valbuf, sizeof(valbuf));
		m_child3->SetDlgItemText(IDC_CUR_VAL, valbuf);
		break;

	case FL_NEED_MORE:
		initval = GetDlgItemDouble(m_child3->m_hWnd, IDC_INIT_VAL, &res);
		if (!res) MessageBox ("Invalid initial value");
		else
		{
			if (*(int*)lParam)
				initval += m_child3->m_range2;
			else
				initval += m_child3->m_range1;
			sprintfFloat (initval, 1,  valbuf, sizeof(valbuf));
			m_child3->SetDlgItemText(IDC_INIT_VAL, valbuf);
			FER(flySendBin (FL_RANGE_MAX, &m_child3->m_range2, sizeof(m_child3->m_range1)))
			FER(flySendBin (FL_RANGE_MIN, &m_child3->m_range1, sizeof(m_child3->m_range1)))
		}
		break;

	case FL_DONE:
		DoneSession();
		break;
	}
	return 1;
}

LRESULT CpsyntegDlg::OnNextInterval(WPARAM wp, LPARAM lparam)
{
	static double rangex1, rangex2, rangey1, rangey2, varHistory[256];
	CString buf;
	int i, k;
	char varstr[16], buf1[64], zbuf[256];
	FER(sendfly (FL_ABOUT2PRESENT, "", 0));
	switch(taskID)
	{
	case 0:
		strcpy(buf1," ");
		if (wp==answer)
		{
			k=sprintfFloat(variable, 4, zbuf, sizeof(zbuf));
			buf.Format(" %s%s", zbuf, buf1);
		}
		else
			buf.Format (" ref%s ", buf1);
		EditPrintf (m_logEdit.m_hWnd, buf);
		RecordTrialByTrial(buf);
		break;
	case 1:
		if (interval==0)
		{
			if (m_child2->nVars<2)
			{
				sprintfFloat(AstSig.GetTag(tagIDs[0].c_str()).value(), 3, varstr, sizeof(varstr));
			} 
			else // assume that it is 2
			{
				sprintfFloat(AstSig.GetTag(tagIDs[0].c_str()).value(), 3, varstr, sizeof(varstr));
				strcat(varstr, "|");
				sprintfFloat(AstSig.GetTag(tagIDs[1].c_str()).value(), 3, varstr+strlen(varstr), sizeof(varstr)-strlen(varstr));
			}
			buf.Format("Trial %d,%s: ", trial+1, varstr);
			CString st="";
			for (i=0; i<nIntervals; i++)
			{
				st = (answer==i)? "odd " : "ref ";
				buf += st;
			}
			RecordTrialByTrial(buf);
			buf += "\r\n";
			EditPrintf (m_logEdit.m_hWnd, buf);
			m_axProc->DeleteLine(-1);
			m_axProc->plot(nConstSteps, scores, RGB(40, 150, 200), (char)2, 1);
			if (trial==0)
			{
				rangex1 = -.5;		rangex2 = nConstSteps -.5;
				rangey1 = 0;		rangey2 = 110.;
				m_axProc->setRange('x', rangex1,rangex2);
				m_axProc->setRange('y', rangey1,rangey2);
				m_axProc->setTick('x',0., 1., 0);
				m_axProc->setTick('y',0., 110., 10);
				m_axProc->xtick.offset=1.;
			}
		}
		break;
	case 2:
		break;
	}
	if (!Present(wp))
	{
		if (nic1)
		{
			OnResetProcessor();
			trial--;
			SetTimer(PLAY_NEXT_TRIAL, trialDelay, NULL);
		}
	}
	return 1;
}

CpsyntegDlg *pPsyntegDlg;
void handleCIPulseInAST(const AstNode *pnode, CAstSig *pAst)
{
	pPsyntegDlg->HandleCIPulse(pnode, pAst);
}


int handleHookAST(CAstSig &ast, const AstNode *pnode, const AstNode *p)
{
	return pPsyntegDlg->HandleHookAST(ast, pnode, p);
}

int CpsyntegDlg::HandleHookAST(CAstSig &ast, const AstNode *pnode, const AstNode *p)
{
	string HookName;
	const AstNode *args;

	if (stopped)
		throw "Script execution has been interrupted.";

	if (pnode->str[0] == '#') {
		HookName = pnode->str+1;
		args = p;
	} else {
		const char *fnsigs[] = {
			"(name_string, ...)", 0};
		HookName = ast.ComputeString(p);
		checkNumArgs(pnode, p, fnsigs, 1, 0);
		args = p->next;
	}

	if (HookName == "INIT_RESPONSE") {
		const char *fnsigs[] = {
			"(num_interval [, instruction_string])", 0};
		checkNumArgs(pnode, args, fnsigs, 1, 2);
		ast.Compute(args);
		if (!ast.Sig.IsScalar())
			throw CAstException(pnode, args, fnsigs, "argument must be a scalar.");
		int n = round(ast.Sig.value());

#define FLY_ERR(x) {\
	char linestr[256], estr[128]; int ecode = (x); \
	if (ecode<0) { flyErrMsg(ecode, estr); wsprintf (linestr, "Error occurred at "#x", code=%d", ecode); throw CAstException(pnode, "flySend...() failed:", linestr);} \
}
		FLY_ERR(flySendBin(FL_INIT, &n, sizeof(n)))
		FLY_ERR(flySendText(FL_INSTRUCTION, instruction[0]))
		if (args->next) {
			string instruction = ast.ComputeString(args->next);
			FLY_ERR(flySendText(FL_INSTRUCTION, instruction.c_str()))
		}
		if (WaitForSingleObject(m_child4->hEvent, INFINITE) != WAIT_OBJECT_0)
			throw CAstException(pnode, "WaitForSingleObject() failed", GetLastError());
	} else if (HookName == "BUTTON") {
		const char *fnsigs[] = {
			"(1:show | 0:hide)", 0};
		checkNumArgs(pnode, args, fnsigs, 1, 1);
		ast.Compute(args);
		if (!ast.Sig.IsScalar())
			throw CAstException(pnode, args, fnsigs, "argument must be a scalar.");
		int n = round(ast.Sig.value());

		if (n)
			FLY_ERR(sendfly(FL_ABOUT2PRESENT, "", 0))
		else
			FLY_ERR(sendfly(FL_ABOUT2STOP, "", 0))
	} else if (HookName == "RESPONSE" || HookName == "RECEIVE" ) {
		const char *fnsigs[] = {
			"( )", 0};
		checkNumArgs(pnode, args, fnsigs, 0, 0);
		while (m_child4->LastResponse < 0) {
			if (WaitForSingleObject(m_child4->hEvent, INFINITE) != WAIT_OBJECT_0)
				throw CAstException(pnode, "WaitForSingleObject() failed", GetLastError());
			if (stopped)
				throw "Script execution has been interrupted.";
		}
		ast.Sig.SetValue(m_child4->LastResponse);
		m_child4->LastResponse = -1;
	} else if (HookName == "FEEDBACK") {
		const char *fnsigs[] = {
			"(message_string)", 0};
		checkNumArgs(pnode, args, fnsigs, 1, 1);
		string msg = ast.ComputeString(args);
		FLY_ERR(flySendText(FL_FEEDBACK, msg.c_str()))
	} else if (HookName == "STREAM") {
		const char *fnsigs[] = {
			"([0:blocking | 1:non-blocking | 2:stop])", 0};
		checkNumArgs(pnode, args, fnsigs, 0, 1);
		int mode = 0;	// default is blocking
		if (args) {
			ast.Compute(args);
			if (!ast.Sig.IsScalar())
				throw CAstException(pnode, args, fnsigs, "argument must be a scalar.");
			mode = round(ast.Sig.value());
		}
		switch (mode) {
		case 0:	// blocking
			Sleep(100);
			// go through
		case 1:	// start non-blocking stream
			if (nic1) {
				CString str;
				CER(ImpSequenceReset(seqSPrint[0]));
				if (m_condOn) {
					UpdateBricsWithConditioner(brics, nBrics);
					UpdateSeq(&seqSPrint[0], condFrame, brics, nBrics);
				} else
					UpdateSeq(&seqSPrint[0], nullFrame, brics, nBrics);
				if (ImpCommunicatorWriteSequence(CombinedSlots, seqSPrint[0])!= NICCInterface__Function_Success) {
					OnResetProcessor();
					str.Format("slot=%d,seqSPrint[%d]=%x", CombinedSlots, 0, seqSPrint[0]);
					MessageBox (str, "ImpCommunicatorWriteSequence Error", MB_OK);
					while (ImpCommunicatorWriteSequence(CombinedSlots, seqSPrint[0]) != NICCInterface__Function_Success) {
						Sleep(400);
						str.Format("Error2, code=%d", GetLastNICError());
						if (MessageBox ("ImpCommunicatorWriteSequence Error, Retry=Yes, Abort=No", str, MB_YESNO)==IDNO)
							return -1;
					}
				}
				CER(ImpCommunicatorStartSequence(CombinedSlots));
			} else {
				if (!sequenceCommandDelete(seqq[0]))
					throw "sequenceCommandDelete( ) failed.";
				seqq[0] = SequenceNew(1);
				// null frames must be indicated. 
				double nullframePeriod = 100.;
#ifdef NDEBUG
				int nullFrameRepeat = 1000;
#else
				int nullFrameRepeat = 1;
#endif
				brics[nBrics].ae=1; brics[nBrics].re=1;
				brics[nBrics].cl=0; brics[nBrics].pam.pw = 25.;
				brics[nBrics].pam.period = nullframePeriod;
				brics[nBrics].tmark = 0.;
				double min_pw = brics[0].pam.pw;
				for (int i=0; i<nBrics; i++) {
					brics[i].tmark += nullframePeriod*nullFrameRepeat;
					if (brics[i].pam.pw < min_pw)
						min_pw = brics[i].pam.pw;
				}
				if (min_pw < 25.) {
					brics[nBrics].pam.pw = 12.; brics[nBrics].pam.pg=6.;
				} else {
					brics[nBrics].pam.pw = 25.; brics[nBrics].pam.pg=8.;
				}
				brics[nBrics].repeat = nullFrameRepeat;
				++nBrics;
				CER_BRC(Bric2Seqq (seqq[0], brics, nBrics, BR_PERIOD_CHECK))
				nBrics = 0;
				if (!NIC2StreamStart(seqq[0], mode))
					throw "NIC2StreamStart( ) failed.";
			}
			break;
		case 2:	// stop stream
			if (nic1)
				ImpCommunicatorStopSequence(1);
			else
				NIC2StreamStop();
		}
	} else
		return -1;
	if (stopped)
		throw "Script execution has been interrupted.";
	return 0;
}


int CpsyntegDlg::Prepare(CString &strIn, CString &msgTitle)
{
	try {
		CAstSig astsig(strIn, &AstSig);
		pPsyntegDlg = this;
		astsig.CallbackCIPulse = handleCIPulseInAST;
//		astsig.CallbackHook = handleHookAST;
		astsig.Compute();
	} catch (const char *errstr) {
		MessageBox (errstr, msgTitle); 
		return 0;
 	}
	return 1;
}

int CpsyntegDlg::Prepare(int id, bool RecalcTag)
{
	//Main entry function
	// id in Prepare(id) is either the interval or REFERENCE/CONTROLLED.
	BRIC runbr;
	CString strIn, msgtitle, buf, loopTagStr;
	CSignals varlist;
	static double val;
	char *_estr[1], estr[256];
	_estr[0]=estr;

	if (RecalcTag)
	{
		if (!PrepareTags()) 
			return 0;
		if (taskID==1)
		{
			if (m_child2->nVars>0) // if a local tag (a tag used for ConstantStim procedure) was defined 
				for (int i=0; i<m_child2->nVars; i++) 
					tagIDs[i] = m_child2->varNames[i];
			else
				tagIDs[0] = "v";
		}
	}
	nBrics = 0;
	if (id==ALL)	
	{
		if (taskID==1)
		{// to check irregularities here and record in the log file (if trial==nTrials-1, don't do it---the last one that won't be used)
			if (trial!=nTrials-1)
			{
				if (trial>nTrials-1 || trial<-1)
				{ buf.Format("trial=%d\r\n", trial);		RecordTrialByTrial(buf);			}
				// This is where value selection for independent variables is done in each trial from presentingIDorder;
				// currentInd: the index in the order of m_child2->var from CConstantStim::ProcessList()
				int currentInd = presentingIDorder[trial+1];

				if (m_child2->nVars>1) {	// assume that it is 2
					AstSig.SetTag(tagIDs[0].c_str(), m_child2->var.buf[currentInd * m_child2->nVars]);
					AstSig.SetTag(tagIDs[1].c_str(), m_child2->var.buf[currentInd * m_child2->nVars+1]);
				} else
					AstSig.SetTag(tagIDs[0].c_str(), m_child2->var.buf[currentInd]);
				if (currentInd>=m_child2->var.nSamples || currentInd<0) // check this m_child2->var.nSamples
				{ buf.Format("currentInd=%d, variable=%lf\r\n", currentInd, variable);		RecordTrialByTrial(buf);			}
			}
		} else {
			var.set(variable);
			AstSig.SetTag("v", variable);
		}
		AstSig.SetTag("n", trial+1.0);	
		try {
			AstSig.SetNewScript(m_tagStr2).Compute();
		} catch (char *errmsg) {
			MessageBox(errmsg, "Error in Per-trial Variables", MB_ICONERROR);
			return 0;
		}		
		for (int i=0 ; i<nIntervals; i++) 
			if (!Prepare(i)) return 0;
		return 1;
	}
	else if (id==answer || id==CONTROLLED)
	{
		strIn = m_bricString2;
		msgtitle = "Controlled Signal";
	}
	else // id must be REFERENCE or non-answer interval
	{
		strIn = m_bricString;
		msgtitle = "Reference Signal";
	}
	if (trial==-999) // trial -999 means this is from OnPlay or OnPlay2
	{
		switch(taskID)
		{
		case 0:
		case 3:
			AstSig.SetTag("v", variable = GetDlgItemDouble(m_child1->m_hWnd, IDC_INIT_VAL));
			break;
		case 1:
			AstSig.SetTag(tagIDs[0].c_str(), m_child2->var.buf[0]);
			if (m_child2->nVars>1)	// assume that it is 2
				AstSig.SetTag(tagIDs[1].c_str(), m_child2->var.buf[1]);
			break;
		case 2:
			AstSig.SetTag("v", variable = GetDlgItemDouble(m_child3->m_hWnd, IDC_INIT_VAL));
			break;
		default:
			MessageBox ("not prepared");
			return 0;
		}
		AstSig.SetTag("n", 1.0);	
		try {
			AstSig.SetNewScript(m_tagStr2).Compute();
		} catch (char *errmsg) {
			MessageBox(errmsg, "Error in Per-trial Variables", MB_ICONERROR);
			return 0;
		}		
	}

	if (!Prepare(strIn, msgtitle)) return 0;

	if (id==REFERENCE ) id=0;
	else if (id==CONTROLLED) id=1;
	if (nic1)
	{
		CER(ImpSequenceReset(seqSPrint[id]));
		if (m_condOn)	
		{
			UpdateBricsWithConditioner(brics, nBrics);
			UpdateSeq(&seqSPrint[id], condFrame, brics, nBrics);
		}
		else
			UpdateSeq(&seqSPrint[id], nullFrame, brics, nBrics);
	}
	else
	{
		if (!sequenceCommandDelete(seqq[id])) {errstr.Format("sequenceCommandDelete"); return 0;}
		seqq[id] = SequenceNew(1);
		// null frames must be indicated. 
		double nullframePeriod = 100.;
		int nullFrameRepeat;
#ifdef NDEBUG
		if (shortnullframe)
			nullFrameRepeat = 1;
		else
			nullFrameRepeat = 1000;
#else
		nullFrameRepeat = 1;
#endif
		brics[nBrics].ae=1; brics[nBrics].re=1;
		brics[nBrics].cl=0; brics[nBrics].pam.pw = 25.;
		brics[nBrics].pam.pg=8.; brics[nBrics].pam.period = nullframePeriod;
		brics[nBrics].tmark = 0.;
		for (int i=0; i<nBrics; i++)			brics[i].tmark += nullframePeriod*nullFrameRepeat;
		brics[nBrics++].repeat = nullFrameRepeat;

		CER_BRC(Bric2Seqq (seqq[id], brics, nBrics, BR_NO_PERIOD_CHECK))
		GetDuration(brics, nBrics, &duration);
	}
	return 1;
}
 
int CpsyntegDlg::OnRetryPresent()
{
	eventNotyHold=0;
	while (ImpCommunicatorWriteSequence(CombinedSlots, seqSPrint[retryPlayID]) != NICCInterface__Function_Success)
	{
		//wow.... IT DID IT AGAIN WITH THIS SEQUENCE.....!!!!!! LETS DO THIS FROM HERE.
		Sleep(400);
		if (MessageBox ("ImpCommunicatorWriteSequence Error, Retry=Yes, Abort=No", "Error2", MB_YESNO)==IDNO)
			return 0;
	}
	return 0;
}

void CpsyntegDlg::OnResetProcessor()
{
	CString str, tstr, buf;
	CTime markedTime;
	int res;
	str.Format("SPrint Disconnecting");
	sbar.SetText(str, 2, 0);		RecordTrialByTrial(str);
	if (!ImpCommunicatorDisconnect())
	{
		markedTime = CTime::GetCurrentTime();
		tstr = markedTime.Format("\r\n%H:%M:%S ");
		str.Format("SPrint Disconnecting failed");
		sbar.SetText(str, 2, 0);	
		buf.Format("%s%s", tstr, str);
		RecordTrialByTrial(buf+"\r\n");
		MessageBox("SPrint Resetting failed");
		return;
	}
	markedTime = CTime::GetCurrentTime();
	tstr = markedTime.Format("\r\n%H:%M:%S ");
	str.Format("SPrint Disconnected");
	sbar.SetText(str, 2, 0);	
	buf.Format("%s%s", tstr, str);
	RecordTrialByTrial(buf+"\r\n");

	Sleep(200);

	if (!ImpCommunicatorConnect())
	{
		res = ImpCommunicatorForceConnect(); 
		markedTime = CTime::GetCurrentTime();
		tstr = markedTime.Format("\r\n%H:%M:%S ");
		if (res== NICCInterface__Function_Success)
		{
			str.Format("SPrint Re-FORCEConnected");
			sbar.SetText(str, 2, 0);
			buf.Format("%s%s", tstr, str);
			RecordTrialByTrial(buf+"\r\n");
		}
		else
		{
			str.Format("SPrint ForceConnection Failed");
			sbar.SetText(str, 2, 0);
			buf.Format("%s%s", tstr, str);
			RecordTrialByTrial(buf+"\r\n");
			MessageBox(str);
			return;
		}
	}
	else
	{
		markedTime = CTime::GetCurrentTime();
		tstr = markedTime.Format("\r\n%H:%M:%S ");
		str.Format("SPrint Re-Connected");
		sbar.SetText(str, 2, 0);
		buf.Format("%s%s", tstr, str);
		RecordTrialByTrial(buf+"\r\n");
	}
	BRIC br;
	br.ae=ECE1;		br.re=ECE2;		br.cl=0;
	br.pam.pw=25;		br.pam.pg=8;
	br.tmark=0;		br.repeat=1500;	br.pam.period=200; // 300-ms long

	seqSPrint[0] = BuildSeq(nullFrame, &br, 1);
	seqSPrint[1] = BuildSeq(nullFrame, &br, 1);
	if (!ImpCommunicatorWriteSequence(ProgramSlot3, seqSPrint[0])) MessageBox ("Error in ImpCommunicatorWriteSequence","psynteg, OnInitDialog");
	if (!ImpCommunicatorStartSequence(ProgramSlot3)) MessageBox ("Error in ImpCommunicatorStartSequence","psynteg, OnInitDialog");
}

int CpsyntegDlg::Present(int playID)
{
	CString str;
	if (nic1)
	{
		if (ImpCommunicatorWriteSequence(CombinedSlots, seqSPrint[playID])!= NICCInterface__Function_Success)
		{
			OnResetProcessor();
			str.Format("slot=%d,seqSPrint[%d]=%x", CombinedSlots, playID, seqSPrint[playID]);
			MessageBox (str, "ImpCommunicatorWriteSequence Error", MB_OK);
			while (ImpCommunicatorWriteSequence(CombinedSlots, seqSPrint[playID]) != NICCInterface__Function_Success)
			{
				Sleep(400);
				str.Format("Error2, code=%d", GetLastNICError());
				if (MessageBox ("ImpCommunicatorWriteSequence Error, Retry=Yes, Abort=No", str, MB_YESNO)==IDNO)
					return 0;
			}
		}
		CER(ImpCommunicatorStartSequence(CombinedSlots));
	}
	else
	{
		//NIC2 doesn't have event notification so I am doing it manually. 
		// NIC2StreamStart is a blocking function, so when it returns, the stream ending can be assumed
		if (!NIC2StreamStart(seqq[playID])) {MessageBox (errstr); return 0;}
		OnSeqEnd();
	}
	return 1;
}


void CpsyntegDlg::UpdateNullFrameElectrodes(const char *str_pw)
{
	Electrode nullAE, nullRE;
	IMP_FRAME* pw_frame;
	BRIC br;

	GetCurrentNullE(nullAE, nullRE);

	pw_frame = ImpFrameNew();
	CERV(ImpFrameSetElectrodes(pw_frame, nullAE, nullRE));
	for (int i=0; i<nSeq; i++) CERV(ImpSequenceDelete(seqSPrint[i]));
	for (int i=0; i<nSeq; i++) seqSPrint[i] = ImpSequenceNew(pw_frame);
	CERV(ImpFrameDelete(pw_frame));

	br.ae=0;
	br.re=0;
	br.cl=0;
	br.pam.pw=25;
	br.pam.pg=14;
	br.tmark=0;
	br.repeat=1;
	br.pam.period=1000;
	Bric2Sequence(seqSPrint[0], &br, 1, 0);
	CERV(ImpCommunicatorWriteSequence(CombinedSlots, seqSPrint[0]));
	CERV(ImpCommunicatorStartSequence(CombinedSlots));
}

void CpsyntegDlg::OnCondOnoff() 
{
	BRIC tempbr;
	double period;
	int e1, e2, cl;
	bool turn(false);
	IMP_SEQUENCE* temp;
	IMP_FRAME* baseFrame;
	CString str;
	UpdateData();
	if (m_condOn)
	{
		if (!GetCondSetting(&period, &cl, &e1, &e2))
		{
			MessageBox (errstr, "GetCondSetting Error (in OnCondOnoff)"); m_condOn=0; UpdateData(0); return;
		}
		if (!UpdateConditionerFrame(1.e6/period, cl, e1, e2))
		{
			MessageBox (errstr); return;
		}
		baseFrame = condFrame;
	}
	else
	{
		baseFrame = nullFrame;
		turn = true;
	}

	tempbr.ae = ECE1; tempbr.re = ECE2;
	tempbr.cl = 0; tempbr.pam.period = 100.;
	tempbr.pam.pw = 25; tempbr.pam.pg = 8;
	tempbr.tmark=0.;
	tempbr.repeat = 100;

	temp = BuildSeq(baseFrame, &tempbr, 1);
	int j = ImpCommunicatorWriteSequence(ProgramSlot4, temp);
	int k = ImpCommunicatorStartSequence(ProgramSlot4);
	int m = ImpSequenceDelete(temp);
	if (temp==NULL)
	{
		MessageBox("BuildSeq error 1928");	m_condOn = (m_condOn==1)? 0:1;	UpdateData(0); return;
	}
	if (j==0)
	{
		MessageBox("ImpCommunicatorWriteSequence error");	m_condOn = (m_condOn==1)? 0:1;	UpdateData(0); return;
	}
	if (k==0)
	{
		MessageBox("ImpCommunicatorStartSequence error");	m_condOn = (m_condOn==1)? 0:1;	UpdateData(0); return;
	}

	EnableID(this, IDC_COND_E, !turn? 0:1);
	EnableID(this, IDC_COND_RATE, !turn? 0:1);
}


int CpsyntegDlg::UpdateBricsWithConditioner(BRIC *brics, int& nBrics)
{
	// Also conditioner frame is updated here **** IMPORTANT
	BRIC tempbr;
	double gap, period;
	int e1, e2, cl;
	int Cond2TargetPeriodRatio, addPrecedCondPulse;
	if (nBrics!=1)
	{ errstr = "Only one stim should exist."; return 0;}
	if (!GetCondSetting(&period, &cl, &e1, &e2))
	{		MessageBox (errstr); m_condOn=0; UpdateData(0); return 0;	}

	if (!UpdateConditionerFrame(1.e6/period, cl, e1, e2))
	{
		MessageBox (errstr); return 0;
	}
	tempbr.ae = e1; tempbr.re = e2;
	tempbr.cl = cl; tempbr.pam.period = period;
	tempbr.pam.pw = COND_PW; tempbr.pam.pg = 8;
	tempbr.tmark=0.;

	Cond2TargetPeriodRatio = (int)(brics[0].pam.period/period+.5);
	addPrecedCondPulse = (int)(COND_DUR_BEFORE_STIM_INTERVAL_MS*1000/period+.5);
	tempbr.repeat = brics[0].repeat * Cond2TargetPeriodRatio + addPrecedCondPulse;
	gap = .5*(period-tempbr.pam.pg-brics[0].pam.pg) - tempbr.pam.pw-brics[0].pam.pw;
	brics[0].tmark = gap + 2*tempbr.pam.pw+tempbr.pam.pg + addPrecedCondPulse*period;
	//This gap should be greater than 19.2 because of GetCondSetting()
	brics[1] = tempbr;
	nBrics++;
	return 1;
}

int CpsyntegDlg::GetCondSetting(double *period, int *cl, int *aeCond, int *reCond)
{ // adjusting period of conditioner to accommodate with stim on deck
	BRIC brOnDeck;
	string lines[2]; 
	CString str; 
	bool b=1;
	int numPeriods, trans, tempcl, el[2];
	double reqPeriod = 1.e6/GetDlgItemDouble (m_hWnd, IDC_COND_RATE);
	double maxPw, gap, per;

	GetDlgItemText (IDC_BRICS, str);
	if (countDeliminators(str, "\r\n")>1)
	{
		errstr = "Only one sequence can be used with a conditioner";
		return 0;
	}
	str2array(lines,2,str,"\r\n");
	numPeriods = (int)(brOnDeck.pam.period / reqPeriod +.5);
	maxPw = brOnDeck.pam.pw ;
	while(b)
	{
		per = brOnDeck.pam.period / (double)numPeriods;
		gap = .5*(per-8-brOnDeck.pam.pg) - COND_PW - maxPw;
		if (gap>19)
			b=0;
		else
			if (numPeriods--==0)
			{
				errstr = "Cannot accommodate stim with this conditioner";
				return 0;
			}
	}
	*period = per;
	tempcl = GetDlgItemInt (IDC_COND_CL, &trans);
	if (trans)	*cl = tempcl;
	else { errstr = "Invalid conditioner current level."; return 0;}
	GetDlgItemText (IDC_COND_E, str);
	if (str2array(el,2,str,", \t")<2) { errstr = "Invalid conditioner electrodes."; return 0;}
	*aeCond = el[0];
	*reCond = el[1];
	return 1;
}


LRESULT CpsyntegDlg::OnCondAdj(WPARAM wp, LPARAM lp)
{
	
	double period;
	int e1, e2;
	int cl;
	if (m_condOn)
	{
		UpdateData();
		cl=GetDlgItemInt(IDC_COND_CL);
		if (!GetCondSetting(&period, &cl, &e1, &e2))
		{MessageBox (errstr); m_condOn=0; UpdateData(0);}
		else 
		{
			OnCondOnoff();
		}
	}
	return 1;
}

IMP_SEQUENCE* CpsyntegDlg::BuildSeq(IMP_FRAME* powerframe, BRIC* br, int nBrics)
{ // This creates a new IMP_SEQUENCE* object, which must be deleted.
	if (powerframe==NULL) {errstr="Null pointer"; return NULL;}
	IMP_SEQUENCE* temp = ImpSequenceNew(powerframe);
	if (temp==NULL) {errstr="ImpSequenceNew failed (343)"; return NULL;}
	CER_BRCNULL(Bric2Sequence (temp, br, nBrics, 0))
	return temp;
}

int CpsyntegDlg::UpdateSeq(IMP_SEQUENCE** seq, IMP_FRAME* powerframe, BRIC* br, int nBrics)
{
	if (powerframe==NULL || seq==NULL) {errstr="Null pointer"; return 0;}
	CER(ImpSequenceDelete(*seq))
	IMP_SEQUENCE* temp = ImpSequenceNew(powerframe);
	if (temp==NULL) {errstr="ImpSequenceNew failed (393)"; return 0;}
	CER_BRC(Bric2Sequence (temp, br, nBrics, 0))
	*seq = temp;
	return 1;
}

IMP_FRAME* CpsyntegDlg::MakeNullFrame()
{ // This creates a new IMP_FRAME* object, which must be deleted when the application closes
	Electrode nullAE, nullRE;
	GetCurrentNullE(nullAE, nullRE);
	IMP_FRAME* pw_frame = ImpFrameNew();
	if (pw_frame==NULL) {errstr="ImpFrameNew failed (493)"; return NULL;}
	CER_BRCNULL(ImpFrameSetPhaseWidth (pw_frame, 25.));
	CER_BRCNULL(ImpFrameSetPhaseGap (pw_frame, 8.));
	CER_BRCNULL(ImpFrameSetPeriod (pw_frame, POWER_FRAME_PERIOD));
	CER_BRCNULL(ImpFrameSetCurrentLevel (pw_frame, 0));
	CER_BRCNULL(ImpFrameSetElectrodes (pw_frame, nullAE, nullRE));
	return pw_frame;
}

int CpsyntegDlg::UpdateConditionerFrame(double rate, int cl, int aeCond, int reCond)
{ 
	double period;
	Electrode nullAE, nullRE;

	GetCurrentNullE(nullAE, nullRE);
	IMP_FRAME* pw_frame = ImpFrameNew();
	if (pw_frame==NULL) {errstr="ImpFrameNew failed (493)"; return 0;}
	int i = ImpFrameDelete(condFrame);
	period = 1.e6/rate;
	CER(ImpFrameSetPhaseWidth (pw_frame, COND_PW));
	CER(ImpFrameSetPhaseGap (pw_frame, 8.));
	CER(ImpFrameSetPeriod (pw_frame, period));
	CER(ImpFrameSetCurrentLevel (pw_frame, cl));
	CER(ImpFrameSetElectrodes (pw_frame, (Electrode)aeCond, (Electrode)reCond));
	condFrame = pw_frame;
	return 1;
}