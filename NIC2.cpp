#include "stdafx.h"
#include "audfret.h"
#include "psynteg.h"
#include "psyntegDlg.h"
#include "services/c_interface/CNICStreamClient.h"
#include "nic/c_interface/CSequence.h"
#include "nic/c_interface/CStimulusCommand.h"
#include "nic/c_interface/CBiphasicStimulus.h"
#include "services/c_interface/CServicesErrors.h"


int LoadNIC2Lib(const char *nicpath, char * errstr);
int CleanupNIC2Lib();

DWORD mainThreadID;


BOOL CALLBACK SmallDlg (HWND hDlg, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	static char *strPt;
	switch (umsg)
	{
	case WM_INITDIALOG:
		strPt = (char*)lParam;
		SetWindowText(GetDlgItem(hDlg, IDC_STATIC_DIRECTION), (char*)lParam);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			GetWindowText(GetDlgItem(hDlg, IDC_EDIT_GET), strPt, 512);
			EndDialog(hDlg, IDOK);
		}
		else if (LOWORD(wParam) == IDCANCEL) 
			EndDialog(hDlg, IDCANCEL);
		break;
	default:
		return 0;
	}
	return 1;
}

int Mode2RE(int mode)
{
	switch(mode)
	{
	case 101:
		return -1;
	case 102:
		return -2;
	case 103:
		return -3;
	default:
		return mode;
	}
}

int CpsyntegDlg::InitNIC2(const char *nicpath, const char *initializeStr, char *errstr)
{
	if (!LoadNIC2Lib(nicpath,errstr)) return 0;

//	if ((seqq[0]=SequenceNew(1))==NULL) {
//		sprintf(errstr, "Error in SequenceNew, code=%d", lastServicesError());
//		CleanupNIC2Lib();
//		return 0;		}
	if ((streamClient=NICStreamClientNew(initializeStr))==NULL) {
		sprintf(errstr, "Error in NICStreamClientNew, param=%s, code=%d", initializeStr, lastServicesError());
		CleanupNIC2Lib();
		return 0;		}

	return 1;
}

int CpsyntegDlg::NIC2CleanUp()
{
	int res;

	res = nicStreamClientDelete(streamClient);
	CleanupNIC2Lib();
	return res;
}

int CpsyntegDlg::NIC2StreamStart (SequenceCommandHandle seq2Play, int fNonBlock)
{
	ErrorCode res;
	StreamStatus status;

	CHECKNIC2_0(nscStopStream(streamClient))
	CHECKNIC2_0(nscSendData(streamClient, seq2Play))
	CHECKNIC2_0(nscStartStream(streamClient))
	if (fNonBlock)
		return 1;
	do
	{
		/* Only check the status every 200 ms or so. */
		Sleep(200);
		res = nscGetStreamStatus(streamClient, &status);
		res = lastServicesError();
	} while (status != Idle && status != Error);
	return 1;
}


int CpsyntegDlg::NIC2StreamStop ()
{
	CHECKNIC2_0(nscStopStream(streamClient))
	return 1;
}