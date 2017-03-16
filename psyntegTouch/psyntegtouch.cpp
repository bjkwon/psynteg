#include "psyntegTouch.h"
#include "fer.h"

#define WM__INIT_DISPLAY	WM_APP+110
#define ShowWindows(a,b,c) {	ShowWindow(hPlay, a); ShowWindow(hDone, b); ShowWindow(hMore, c); }

HMODULE hDllModule;
WNDPROC hOrgProc, OldSliderProc;
HINSTANCE hInst;
int stopped ;
int autoResponse ;

int DisplayCaptions (int pos, HWND hCap1, HWND hCap2, HWND hCap3)
{
	switch (pos)
	{
	case 0:
		SetWindowText (hCap1, "FIRST ONE HIGH"); 	SetWindowText (hCap3, "SECOND ONE LOW"); 
		SetWindowText (hCap2, "Very clearly different pitch."); 
		break;
	case 10:
		SetWindowText (hCap1, "FIRST ONE LOW"); 	SetWindowText (hCap3, "SECOND ONE HIGH"); 
		SetWindowText (hCap2, "Very clearly different pitch."); 
		break;
	case 1:
		SetWindowText (hCap1, "FIRST ONE HIGH"); 	SetWindowText (hCap3, "SECOND ONE LOW"); 
		SetWindowText (hCap2, "Clearly different pitch."); 
		break;
	case 9:
		SetWindowText (hCap1, "FIRST ONE LOW"); 	SetWindowText (hCap3, "SECOND ONE HIGH"); 
		SetWindowText (hCap2, "Clearly different pitch."); 
		break;
	case 2:
		SetWindowText (hCap1, "FIRST ONE HIGH"); 	SetWindowText (hCap3, "SECOND ONE LOW"); 
		SetWindowText (hCap2, "Somewhat different pitch."); 
		break;
	case 8:
		SetWindowText (hCap1, "FIRST ONE LOW"); 	SetWindowText (hCap3, "SECOND ONE HIGH"); 
		SetWindowText (hCap2, "Somewhat different pitch."); 
		break;
	case 3:
		SetWindowText (hCap1, "FIRST ONE HIGH"); 	SetWindowText (hCap3, "SECOND ONE LOW"); 
		SetWindowText (hCap2, "A little different pitch."); 
		break;
	case 7:
		SetWindowText (hCap1, "FIRST ONE LOW"); 	SetWindowText (hCap3, "SECOND ONE HIGH"); 
		SetWindowText (hCap2, "A little different pitch."); 
		break;
	case 4:
		SetWindowText (hCap1, "FIRST ONE low"); 	SetWindowText (hCap3, "SECOND ONE high"); 
		SetWindowText (hCap2, "Similar but still can tell the difference"); 
		break;
	case 6:
		SetWindowText (hCap1, "FIRST ONE low"); 	SetWindowText (hCap3, "SECOND ONE high"); 
		SetWindowText (hCap2, "Similar but still can tell the difference"); 
		break;
	case 5:
		SetWindowText (hCap1, ""); 	SetWindowText (hCap3, ""); 
		SetWindowText (hCap2, "Cannot tell which one is higher--about the same."); 
		break;
	default:
		SetWindowText (hCap1, ""); 	SetWindowText (hCap3, ""); 
		SetWindowText (hCap2, ""); 
		return 0;
	}
	return 1;
}

int getRandAnswer(int cinterval)
{
	int x;

	if (cinterval==0)
		return 0;
	else
	{
		x = rand();
		return x % cinterval;
	}
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

__declspec (dllexport) HANDLE SubClass(HWND hwnd)
{
	hOrgProc = (WNDPROC)(LONG_PTR)SetWindowLong (hwnd, GWL_WNDPROC, (LONG)WndProc);
	PostMessage(hwnd, WM__INIT_DISPLAY, 0, 0);
	return LoadAccelerators(hDllModule, "ACCEL");
}

BOOL APIENTRY DllMain( HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	hDllModule = (HMODULE)hModule;
	return TRUE;     
}

LRESULT CALLBACK  TabbingProc (HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	int i = GetWindowLong (hwnd, GWL_ID);
	static int pressedCount;
	static HWND hPlay, hDone, hMore;
	switch(umsg)
	{
	case WM_KEYUP: 
		if (wParam == VK_LEFT)
			SendMessage (hwnd, WM_KEYDOWN, VK_UP, 0);
		else if (wParam == VK_RIGHT)
			SendMessage (hwnd, WM_KEYDOWN, VK_DOWN, 0);
		break;

	case WM_KEYDOWN: 
		if (wParam==VK_F12) // responds only to F11 and F12 key
		{
			if (IsWindowEnabled(hPlay))
					SendMessage (GetParent(hwnd), WM_COMMAND, MAKELONG(ID_PLAY,BN_CLICKED), (LPARAM)hPlay);
		}
		else if (wParam == VK_F11)
		{
			if (IsWindowEnabled(hDone))
				SendMessage (GetParent(hwnd), WM_COMMAND, MAKELONG(ID_DONE, BN_CLICKED), (LPARAM)hDone);
		}
		else if (wParam == VK_F10 || wParam == VK_F9)
		{
			if (IsWindowVisible(hMore))
				SendMessage (GetParent(hwnd), WM_COMMAND, MAKELONG(ID_NEED_MORE, BN_CLICKED), (LPARAM)hMore);
		}
		break;

/* ?????
	case WM_TIMER: // single-click (double-click time window expired)
		KillTimer (hwnd, DELAYEDINTERPRET);
		pressedCount=0;
		break;
*/
	case WM_SETFOCUS:
		hPlay = GetDlgItem(GetParent(hwnd), ID_PLAY);
		hDone = GetDlgItem(GetParent(hwnd), ID_DONE);
		hMore = GetDlgItem(GetParent(hwnd), ID_NEED_MORE);
		break;
	}
	return CallWindowProc (OldSliderProc, hwnd, umsg, wParam, lParam);
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static int taskID, adjusted, playedItem, range_max, range_min;
	static HWND hSlider, hSliderVal, hPlay, hDone, hMore, hCap1, hCap2, hCap3;
	static char versionstr[16], buf[1024], inst1[256], inst2[256];
	static HWND hbut[MAX_INTERVALS];
	static int ready4Respond, ready4StartButton(0), intvl, instID, prestop, stopRequested(0);
	static int nTrials(0), trial(0);
	static HBRUSH hBackgrnd;
	static BOOL freedSession, beginNewPair(true);
	static HWND hFreak, hMsg, hFeed, hBegin, hMsgOperator, hCount, hCap;
	static HFONT hFont, hFont2, hFont3, hFont4;

	ADJUSTING adjusting;
	int i, answer, j, id;
	LONG_PTR pos;

	WORD command, code;
	char cody[64];

	HDC hdc;
	int bltx[]={30, 30, 30, 30, 25, 25,}, 
		 blty[]={25, 50, 50, 25, 25, 25,};
	COLORREF col;

	static int sizex, sizey, cintl, marginx, spacing, width, height, marginy, icount;

	switch (uMsg)
	{
	case WM__INIT_DISPLAY:
		autoResponse=0;
		InitCommonControls();
		sizex = GetSystemMetrics (SM_CXSCREEN)-10;
		sizey = GetSystemMetrics (SM_CYSCREEN)-30;
		hMsg=CreateWindow ("Static", "", WS_CHILD|WS_VISIBLE, 50, 20, 800, 100, hwnd,
									(HMENU)ID_HMSG, hInst, NULL);
		hMsgOperator=CreateWindow ("Static", "", WS_CHILD|WS_VISIBLE, 50, 120, 800, 100, hwnd,
									(HMENU)ID_HMSG2, hInst, NULL);
		hFeed=CreateWindow ("Static", "", WS_CHILD, 50, 400, 390, 45, hwnd,
									(HMENU)ID_FEEDBACK, hInst, NULL);
		hCount = CreateWindow ("Static", "", WS_CHILD, 400, 335, 150, 150, hwnd,
									(HMENU)ID_COUNTER, hInst, NULL);
		LoadString(hDllModule, IDS_INFOLINE1, cody, sizeof(cody));
		hCap = CreateWindow ("Static", cody, WS_CHILD|WS_VISIBLE, 
			70, 760, 850, 100, hwnd, (HMENU)ID_CAP, hInst, NULL);
			// Creating the buttons
		hPlay = CreateWindow ("Button", "PLAY", WS_CHILD, 550, 300, 150, 75, hwnd,
									(HMENU)ID_PLAY, hDllModule, NULL);
		hDone= CreateWindow ("Button", "DONE", WS_CHILD|WS_DISABLED, 725, 300, 150, 75, hwnd,
									(HMENU)ID_DONE, hDllModule, NULL);
		hMore= CreateWindow ("Button", "NEED MORE", WS_CHILD, 900, 300, 120, 75, hwnd,
									(HMENU)ID_NEED_MORE, hDllModule, NULL);
		for (j=0; j<MAX_INTERVALS; j++)
			hbut[j] = CreateWindow ("Button", itoa(j+1, buf, 10), WS_CHILD|WS_DISABLED, 
				0, 0, 0, 0, hwnd, (HMENU)(BUTT1+(LONG_PTR)j), NULL, NULL);
		
		hBegin = CreateWindow ("button", "Press to start (Or Hit the Enter Key)", WS_CHILD|WS_DISABLED, sizex/3, sizey*3/5, sizex*35/100, sizey*35/100,
			hwnd, (HMENU)ID_HBEGIN, hDllModule, NULL);
		hFont = CreateFont (30, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_ROMAN,
			"Verdana");
		hFont2 = CreateFont (20, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_ROMAN,
			"Times New Roman");
		hFont3 = CreateFont (20, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, 
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_ROMAN,
			"Verdana");
		hFont4 = CreateFont (25, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_ROMAN,
			"Arial");

		hSlider = CreateWindow (TRACKBAR_CLASS, "", WS_CHILD|TBS_AUTOTICKS|TBS_BOTH|TBS_HORZ|TBS_FIXEDLENGTH, 
				100, 250, 400, 60, hwnd, (HMENU)(ID_SLIDER), hDllModule, NULL);
		OldSliderProc = (WNDPROC)(LONG_PTR)SetWindowLong (hSlider, GWL_WNDPROC, (LONG)TabbingProc);
	    hSliderVal = CreateWindow ("Static", "", WS_CHILD|WS_VISIBLE, 400, 480, 90, 60,
							hwnd, (HMENU)(ID_SLIDERVAL), hDllModule, NULL);
		hCap1 = CreateWindow ("Static", "", WS_CHILD, 100, 370, 130, 80, hwnd, (HMENU)(ID_CAP+1), hDllModule, NULL);
		hCap2 = CreateWindow ("Static", "", WS_CHILD, 370, 370, 200, 80, hwnd, (HMENU)(ID_CAP+2), hDllModule, NULL);
		hCap3 = CreateWindow ("Static", "", WS_CHILD, 620, 370, 200, 80, hwnd, (HMENU)(ID_CAP+3), hDllModule, NULL);
		hBackgrnd = CreateSolidBrush(GetSysColor (COLOR_WINDOW));
		ready4Respond = 0;
		break;

	case WM_SETFOCUS:
		SetFocus (hSlider);
		break;

	case WM_CTLCOLORSTATIC:
		hdc = (HDC)wParam;
		col = GetSysColor (COLOR_WINDOW);
		id = (int)GetWindowLong ((HWND)lParam, GWL_ID);
		switch (id)
		{
		case ID_FEEDBACK:
			SelectObject (hdc, hFont2);
			SetTextColor (hdc, RGB(113, 30, 225));
			SetBkColor (hdc, col);
			return (LRESULT) hBackgrnd;
		case ID_HMSG:
			SelectObject (hdc, hFont);
			SetTextColor (hdc, RGB(100, 80, 64));
			SetBkColor (hdc, col);
			return (LRESULT) hBackgrnd;
		case ID_HMSG2:
			SelectObject (hdc, hFont);
			SetTextColor (hdc, RGB(190, 30, 64));
			SetBkColor (hdc, col);
			return (LRESULT) hBackgrnd;
		case ID_SLIDERVAL:
			SelectObject (hdc, hFont3);
			SetTextColor (hdc, RGB(0, 222, 0));
			SetBkColor (hdc, col);
			return (LRESULT) hBackgrnd;
		case ID_CAP+1:
		case ID_CAP+2:
		case ID_CAP+3:
			SelectObject (hdc, hFont4);
			SetTextColor (hdc, RGB(0, 150, 120));
			SetBkColor (hdc, col);
			return (LRESULT) hBackgrnd;
		}
		break; // make sure to return hBackgrnd only for those above.

	case WM_FL_CLOSE:
	case WM_CLOSE:
//		SendMessage (hwnd, WM_FL_ARRIVED, FL_SESSION_STOP, 0);
		for (j=0; j<cintl; j++)
			DestroyWindow (hbut[j]);
		DestroyWindow(hBegin);
		DestroyWindow(hCap);
		DestroyWindow(hCount);
		DestroyWindow(hFeed); 
		DestroyWindow(hMsg); 
		DestroyWindow(hMsgOperator);
		DestroyWindow(hPlay);
		DestroyWindow(hDone);
		DestroyWindow(hMore);
		DestroyWindow(hSlider);
		DestroyWindow(hSliderVal);
		DestroyWindow(hCap1);
		DestroyWindow(hCap2);
		DestroyWindow(hCap3);
		DeleteObject(hFont);
		DeleteObject(hFont2);
		DeleteObject(hFont3);
		DeleteObject(hFont4);
		SetWindowLong (hwnd, GWL_WNDPROC, (LONG)hOrgProc);
		beginNewPair = true;
		break;
	case WM_FL_ARRIVED:
		command = LOWORD(wParam);
		switch (command)
		{
		case FL_INIT:
			ready4StartButton = 1;
			taskID = 0;
			// Receiving the number of interval from O-PC
			memcpy((void*)&cintl, (void*)lParam, sizeof(int)); 

			// Calculating the width, spacing, etc... of the buttons
			width = 58 * sizex / cintl / 100;
			spacing = 30 * sizex /  cintl / 100;
			marginx = 3 * sizex / cintl / 10;
			height = 2 * sizey / 10;
			marginy = 35 * sizey / 100;

			intvl = stopped = prestop = 0; 
			// Arranging the buttons
			for (j=0; j<cintl; j++)
			{
				MoveWindow(hbut[j], marginx+j*(width+spacing), marginy, width, height, 0);
				ShowWindow(hbut[j], SW_SHOW);
			} 
			ShowWindow(hBegin, SW_SHOW);
			SetWindowText (hMsg, "");
			ShowWindow (hMsg, SW_SHOW);
			ShowWindow (hCount, SW_HIDE);
			ShowWindow (hCap1, SW_HIDE); 	ShowWindow (hCap2, SW_HIDE); 	ShowWindow (hCap3, SW_HIDE); 
			icount = 5;
			instID=0;
			if (autoResponse) SetTimer(hwnd, PRESS_BEGIN, AUTORESP, NULL);
			break;
		case FL_PLAY_SHORT:
			taskID = 3;
			break;

		case FL_INIT_SLIDER:
			taskID = 2;
			freedSession = FALSE;
			ShowWindows (SW_SHOW, SW_SHOW, SW_HIDE);
			SetFocus (hSlider);
			ShowWindow (hSliderVal, SW_HIDE); 
			SetWindowText (hMsg, "Press ""PLAY"" button (or ""done"" when it's done)");
			SetWindowText (hDone, "DONE");
			ShowWindow (hCap1, SW_HIDE); 	ShowWindow (hCap2, SW_HIDE); 	ShowWindow (hCap3, SW_HIDE); 
			break;
		case FL_WAIT4NEXT:
			j = *(int*)(void*)lParam;
			sprintf(buf, "Wait for the next pair.     %d", j);
			SetWindowText (hMsg, buf);
			break;
		case FL_INIT_PRANK:
			taskID = 3;
			j = *(int*)(void*)lParam;
			if (j>0) j = nTrials;
			freedSession = FALSE; //?
			ShowWindows (SW_SHOW, SW_SHOW, SW_HIDE);
			SetFocus (hSlider);
			ShowWindow (hSlider, SW_SHOW);
			ShowWindow (hSliderVal, SW_SHOW); 
			ShowWindow (hCap1, SW_SHOW); 	ShowWindow (hCap2, SW_SHOW); 	ShowWindow (hCap3, SW_SHOW); 
			SetWindowText (hMsg, "Press ""PLAY"" to play the pair and compare the pitch");
			SetWindowText (hDone, "SUBMIT");
			EnableWindow (hDone, FALSE);
			break;
		case FL_ABOUT2PRESENT:
			prestop++;
			if (taskID==0) // taskID is 0 for both adaptive and constant stim 
			{
				ready4Respond = 0; // Just to re-assure
				if (!stopped)
				{
					ShowWindow (hbut[intvl], SW_SHOW);
					if (intvl>0)
						ShowWindow (hbut[intvl-1], SW_HIDE);
				}
			}
			else if (taskID==2 || taskID==3) // slider OR PRANK
			{
				EnableWindow (hSlider, FALSE);
				if (playedItem==0)
					SetWindowText (hMsg, "Listen to SOUND 1...");
				else
					SetWindowText (hMsg, "Listen to SOUND 2...");
				EnableWindow (hPlay, FALSE);
			}
			break;
		case FL_ABOUT2STOP:
			if (prestop!=0) // if FL_ABOUT2STOP somehow comes before FL_ABOUT2PRESENT, this block is ignored
			{
				if (taskID==0)
				{
					intvl++;
					if (intvl==cintl) 
					{	
						for (j=0; j<cintl; j++)
						{
							ShowWindow (hbut[j], SW_SHOW); 
							EnableWindow (hbut[j], TRUE);
						}
						intvl = 0; 
						SetWindowText (hMsg, inst2);
						ready4Respond = 1;
						if (autoResponse) SetTimer(hwnd, PRESS_RESPONSE, AUTORESP, NULL);
					}
				}
				else if (taskID==2 || taskID==3) // slider OR PRANK
				{
					EnableWindow (hSlider, TRUE);
					if (taskID==2) SetWindowText (hMsg, "");
					else if (playedItem==1) 
					{
						SetWindowText (hMsg, "Which one is higher in pitch? (Use Left/Right (or Up/Down) arrow keys)");
					}
					playedItem++;
				}
			}
			break;

		case FL_END_OF_PRSENTATION:
			ShowWindow (hPlay, SW_SHOW);
			EnableWindow (hPlay, TRUE);
			EnableWindow (hDone, TRUE);
			SetFocus (hSlider);
			ShowWindow (hMore, SW_HIDE);
			pos = -SendMessage (hSlider, TBM_GETPOS, 0, 0);
			if (taskID==2)
			{
				SetWindowText (hMsg, inst1);
				if (pos==-SendMessage (hSlider, TBM_GETRANGEMAX, 0, 0) ||
					pos==-SendMessage (hSlider, TBM_GETRANGEMIN, 0, 0) )
				{
					ShowWindow (hMore, SW_SHOW);
					break;
				}
			}
			break;

		case FL_RANGE_MAX:
			memcpy((void*)&range_max, (void*)lParam, sizeof(int));
			break;

		case FL_RANGE_MIN:
			memcpy((void*)&range_min, (void*)lParam, sizeof(int));
			SendMessage (hSlider, TBM_SETRANGE, TRUE, MAKELONG(range_min, range_max));
			SendMessage (hSlider, TBM_SETPAGESIZE, 0, (LPARAM)1);
			SendMessage (hSlider, TBM_SETPOS, TRUE, 0);
			SetDlgItemInt (hwnd, ID_SLIDERVAL, 0, TRUE);
			ShowWindow (hSlider, SW_SHOW);
			break;

		case FL_TEXT:
			ShowWindow (hMsgOperator, SW_SHOW);
			SetWindowText (hMsgOperator, (char*)lParam);
			if (!strcmp((char*)lParam,"autoresp777")) autoResponse=1;
			else if (!strcmp((char*)lParam,"autoresp000")) autoResponse=0;
			break;

		case FL_DONE:
			stopped=1;
			SetWindowText (hMsg, "Session done....");
			ShowWindow(hMsg, SW_SHOW);
			break;

		case FL_FEEDBACK:
			ShowWindow (hFeed, SW_SHOW);
			SetWindowText (hFeed, (char*)lParam);
			SetTimer (hwnd, FDBCKTIMER, 1500, NULL);
			break;
		case FL_STOP:
			PostMessage (hwnd, WM__STOPPED, 0, 1);
			break;
		case FL_RESUME:
			stopped=0;
			break;

		case FL_COUNT:
			ShowWindow (hCount, SW_SHOW);
			SetWindowText (hMsg, "Wait for the next one....");
			SetWindowText (hCount, itoa(icount--, buf, 10));
			break;

		case FL_INSTRUCTION:
			if (instID==0)
			{
				strcpy(inst1, (char*)lParam);
				instID++;
			}
			else
			{
				strcpy(inst2, (char*)lParam);
				EnableWindow (hBegin, TRUE);
				instID=0;
			}
			break;
		}
		break;

	case WM_FL_SENT_OK:
		command = LOWORD(wParam);
		switch (command)
		{
		case FL_BEGIN:
		case FL_RESPONSE:
		case FL_PLAYAGAIN:
			for (j=0; j<cintl; j++)
				ShowWindow (hbut[j], SW_HIDE);
			SetWindowText (hMsg, inst1);
			break;
		}
		break;

	case WM_FL_NETWORK_ERR:
		itoa(LOWORD(wParam), cody, 10);
		MessageBox(hwnd, (char*)lParam, cody, MB_OK);
		break ;

	case WM__STOPPED:
		stopped=1;
		intvl = 0;
		if ((int)lParam==1) // "panic" pressed
			strcpy(buf,"Stop pressed.");
		else // stopped by the operator
			strcpy(buf,"Stopped by the operator");
		SetWindowText (hMsg, buf);
		ShowWindow (hBegin, SW_HIDE); 
		for (j=0; j<cintl; j++)
			ShowWindow (hbut[j], SW_HIDE); 
		ShowWindows(SW_HIDE, SW_HIDE, SW_HIDE);
		ShowWindow (hSlider, SW_HIDE); 
		ShowWindow (hSliderVal, SW_HIDE); 
		EnableWindow (hPlay, TRUE);
		EnableWindow (hDone, TRUE);
		beginNewPair = true; // for taskID = 3
		SetWindowText (hCap1, ""); 	SetWindowText (hCap3, ""); SetWindowText (hCap2, ""); 
		break;

	case WM_KEYDOWN:
		switch (wParam) 
		{
		case VK_LEFT:
			pos = SendMessage ((HWND)lParam, TBM_GETPOS, 0, 0);
			break;
		}
		break;

	case WM_HSCROLL:
		code = LOWORD(wParam);
		if (code==SB_LEFT) 
			i=9;
		else if (code==SB_LINELEFT|| TB_LINEUP)
			i=19;
		i = GetWindowLong ((HWND)lParam, GWL_ID) - ID_SLIDER;
		pos = SendMessage ((HWND)lParam, TBM_GETPOS, 0, 0);
		GetDlgItem(hwnd, ID_SLIDERVAL+i);
		SetDlgItemInt (hwnd, ID_SLIDERVAL+i, (UINT)pos, TRUE);
		adjusting.itemAdjusted = i;
		adjusting.newVal = (int)pos;
		sendflyn (FL_ADJUSTING, &adjusting, sizeof(ADJUSTING), 0);
		EnableWindow (hDone, FALSE);
		pos = -SendMessage (hSlider, TBM_GETPOS, 0, 0);
		if ( taskID!=3 &&
			(pos==-SendMessage (hSlider, TBM_GETRANGEMAX, 0, 0) ||
			pos==-SendMessage (hSlider, TBM_GETRANGEMIN, 0, 0) ) )
			ShowWindow (hMore, SW_SHOW);
		else
			ShowWindow (hMore, SW_HIDE);
		if (taskID==3) DisplayCaptions (-(int)pos, hCap1, hCap2, hCap3);
		break;

	case WM_COMMAND:
 		answer = LOWORD(wParam)-BUTT1;
		switch (LOWORD(wParam))
		{
		case ID_PLAY:
			ShowWindow (hPlay, SW_HIDE);
			playedItem = 0;
			if (taskID==2)
			{
				adjusted = (int)-SendMessage (hSlider, TBM_GETPOS, 0, 0);
				FER(i=sendflyn (FL_PLAY, &adjusted, sizeof(adjusted), 0))
			}
			else //3
			{
				if (beginNewPair)
				{
					FER(i=sendfly (FL_PRANK_PLAY, "new", 0))
					beginNewPair = false;
					SendMessage (hSlider, TBM_SETPOS, TRUE, 5);
				}
				else if (trial< nTrials)
					FER(i=sendfly (FL_PRANK_PLAY, "", 0))
				else
					FER(i=sendfly (FL_PRANK_PLAY, "", 0))
			}
			break;
		case ID_DONE:
			if (taskID==2)
			{
				ShowWindows(SW_HIDE, SW_HIDE, SW_HIDE);
				SetWindowText (hMsg, "Done. Wait for the next trial.");
				FER(sendfly(FL_DONE, "", 0))
				EnableWindow (hDone, FALSE);
				ShowWindows (SW_HIDE, SW_HIDE, SW_HIDE);
				ShowWindow (hSlider, SW_HIDE); 
				ShowWindow (hSliderVal, SW_HIDE); 
			}
			else
			{
				command = (WORD)SendMessage (hSlider, TBM_GETPOS, 0, 0);
				FER(sendflyn(FL_READYVALUE, &command, sizeof(command),0))
				trial++;
				ShowWindows (SW_HIDE, SW_HIDE, SW_HIDE);
				ShowWindow (hSlider, SW_HIDE); 
				ShowWindow (hSliderVal, SW_HIDE); 
				if (trial< nTrials)
					SetWindowText (hMsg, "Wait for the next pair.");
				else
					SetWindowText (hMsg, "Session done.");
				beginNewPair = true;
				SetWindowText (hCap1, ""); 	SetWindowText (hCap3, ""); SetWindowText (hCap2, ""); 
			}

			break;
		case ID_NEED_MORE:
			int moreupdown;
				pos = SendMessage (hSlider, TBM_GETPOS, 0, 0);
				if (pos==SendMessage (hSlider, TBM_GETRANGEMAX, 0, 0))
					moreupdown=1;
				else if ( pos==-SendMessage (hSlider, TBM_GETRANGEMIN, 0, 0) )
					moreupdown=-1;
				else
					moreupdown = 0;
			FER(flySendBin (FL_NEED_MORE, (void*)&moreupdown, sizeof(moreupdown)));
			ShowWindow (hMore, SW_HIDE);
			EnableWindow (hDone, FALSE);
			ShowWindow (hSlider, SW_HIDE);
			ShowWindow (hSliderVal, SW_HIDE);
			break;
		case BUTT1:
		case BUTT2:
		case BUTT3:
		case BUTT4:
		case BUTT5:
		case BUTT6:
		case BUTT7:
		case BUTT8:
		case BUTT9:
		case BUTT10:
		case BUTT11:
		case BUTT12:
		case BUTT13:
			if (autoResponse)  KillTimer(hwnd, PRESS_RESPONSE); // for the auto response mode
			if(ready4Respond)
			{
				for (j=0; j<cintl; j++)
				{
					ShowWindow (hbut[j], SW_HIDE); 
					EnableWindow (hbut[j], 0);
				}
				FER(sendflyn (FL_RESPONSE, &answer, sizeof(int), 0))
			}
			ready4Respond = 0;
			break;
	
		case ID_HBEGIN:
			if (ready4StartButton)
			{
				ready4StartButton = 0;
				if (autoResponse)  KillTimer(hwnd, PRESS_BEGIN); // for the auto response mode
				answer=10;
				FER(sendfly (FL_BEGIN, "", 0))
				ShowWindow(hBegin, SW_HIDE);
				SetWindowText (hMsg, inst1);
				SetTimer (hwnd, BEGINDELAY, 500, NULL);
				for (j=0; j<cintl; j++)
				{
					ShowWindow (hbut[j], SW_HIDE); 
					EnableWindow (hbut[j], 0);
				}
			}
			break;

		case ID_RESPONSE_1:
		case ID_RESPONSE_2:
		case ID_RESPONSE_3:
		case ID_RESPONSE_4:
			SendMessage (hwnd, WM_COMMAND, BUTT1+LOWORD(wParam)-ID_RESPONSE_1, 0);
			break;
		case ID_STOP:
			if (stopRequested==2)
			{
				FER(sendfly(FL_STOP, "", 0))
				SendMessage(hwnd, WM__STOPPED, 0, STOPPED_BY_USER);
				stopRequested=0;
			}
			else
				stopRequested++;
			break;

		case ID_START:
			SendMessage (hwnd, WM_COMMAND, ID_HBEGIN, 0);
			break;

		default:
			MessageBox(hwnd, "UNKNOWN", "ERROR", MB_OK);
			break;
		}
		if (LOWORD(wParam)!=ID_STOP)	stopRequested=0;
		break ;

	case WM_TIMER:
		KillTimer(hwnd, wParam);
		switch (wParam)
		{
		case FDBCKTIMER:
			ShowWindow (hFeed, SW_HIDE);
			break;
		case PRESS_BEGIN:
			SendMessage (hwnd, WM_COMMAND, ID_HBEGIN, NULL); 
			break;
		case PRESS_RESPONSE:
			SendMessage (hwnd, WM_COMMAND, getRandAnswer(3)+BUTT1, NULL); 				
			break;
		}
		break;
	}
    return CallWindowProc(hOrgProc, hwnd, uMsg, wParam, lParam);
}
