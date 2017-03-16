#pragma once

#include "sigproc.h"

#ifndef GRAPHY_EXPORT
#define GRAPHY_EXPORT __declspec (dllexport)
#endif 

class CPosition
{
public:
	//all the values in this class is scaled by 1 ("normalized" as in Matlab)
	double x0;
	double y0;
	double width;
	double height;
	GRAPHY_EXPORT CPosition(double x, double y, double w, double h);
	CPosition();
};

class CGobj
{
public:
	COLORREF color;
	CPosition pos;
	CGobj *hPar;
#ifdef __AFXWIN_H__
	CDialog* hDlg;
#endif
	char type[16];
	int nChildren;
	GRAPHY_EXPORT CGobj();
	virtual ~CGobj();
};

class CTick : public CGobj
{
public:
	POINT gap4next;
	int size; // in pixel
	int labelPos; // in pixel
	double a1;
	double a2;
	int nDiv;
	int nLabelSkip;
	char format[32];
	double mult;
	double offset;
#ifdef __AFXWIN_H__
	CFont font;
#endif
	GRAPHY_EXPORT CTick();
	~CTick();
};

class CLine : public CGobj
{
public:
	double *xdata, *ydata;
	int len;
	char symbol;
	int lineWidth;
	GRAPHY_EXPORT CLine(CGobj* pParent = NULL, int len=0);   // standard constructor
	~CLine();   
};

class CText : public CGobj
{
public:
#ifdef __AFXWIN_H__
	CRect textRect; // in Client coordiate
	CFont font;
#endif
	int fontsize; // in pixel
	char string[512];
	char fontname[64];
	int SetFont(LPCTSTR fontName, int fontSize=15);
	CText(CGobj* pParent = NULL, const char* strInit=NULL, CPosition posInit=CPosition(0,0,0,0));   // standard constructor
	~CText();   
};

class CAxis : public CGobj
{
public:
#ifdef __AFXWIN_H__
	CRect axRect;
	CRect rcAx;
#endif
	COLORREF colorBack;
	COLORREF colorAx;
	CPosition pos;
	double xlim[2], ylim[2];
	double xlimFull[2], ylimFull[2];
	CTick xtick;
	CTick ytick;
	CLine** m_ln;
	int nLines;
	GRAPHY_EXPORT void GetCoordinate(POINT* pt, double& x, double& y);
	GRAPHY_EXPORT CLine * plot(int length, double *y, COLORREF col=0xff0000, char cymbol=0, int lineWidth=1);
	GRAPHY_EXPORT CLine * plot(int length, double *x, double *y, COLORREF col=0xff0000, char cymbol=0, int lineWidth=1);
	GRAPHY_EXPORT void GetRange(double *x, double *y);
	GRAPHY_EXPORT void DeleteLine(int index);
	GRAPHY_EXPORT void setRangeFromLines(char xy);
	GRAPHY_EXPORT POINT GetPixelAbs(double x, double y);
	GRAPHY_EXPORT int GetPixelAbs(double a, char xy);
	GRAPHY_EXPORT int GetPixelRel(double a, char xy);
	GRAPHY_EXPORT void setRange(const char xy, double x1, double x2);
	GRAPHY_EXPORT void setTick(const char xy, double const x1, double const x2, int const nDiv=1, int const nSkips=0, char const *format=NULL, double const coeff=1., double const offset=0.);
	GRAPHY_EXPORT void setPos(CPosition &posIn);
	GRAPHY_EXPORT void setPos(double x0, double y0, double width, double height);
	GRAPHY_EXPORT CAxis(CGobj* pParent = NULL);   // standard constructor
	virtual ~CAxis();
};

class CFigure : public CGobj
{
public:
	void DeleteAxis(int index);
	CText **text;
	int nTexts;
	CAxis **ax;
	int nAxes;
	GRAPHY_EXPORT CText *AddText(const char* string, CPosition pos);
	GRAPHY_EXPORT CAxis *axes(double x0, double y0, double width, double height);
	GRAPHY_EXPORT CAxis *axes(CPosition pos);
#ifdef __AFXWIN_H__
	GRAPHY_EXPORT CFigure(CDialog* pParent);
#endif
	GRAPHY_EXPORT int Show(int showCode);
	virtual ~CFigure();
};

#define GRAPHY_REPLACE_SAME_KEY		(DWORD)0x0000
#define GRAPHY_REPLACE_NO_KEY		(DWORD)0x0001
#define GRAPHY_RETURN_ERR_SAME_KEY	(DWORD)0x0002


#ifdef __AFXWIN_H__
GRAPHY_EXPORT CFigure* OpenFigure(CRect *rt, const CSignals &data, int devID=0);
GRAPHY_EXPORT CFigure* OpenFigure(CRect *rt, const char* caption, const CSignals &data, int devID=0);
GRAPHY_EXPORT CFigure* OpenChildFigure(CRect *rt, const CSignals &data, CWnd* pParent=NULL);
#endif
GRAPHY_EXPORT HACCEL GetAccel(CFigure* hFig);
GRAPHY_EXPORT HWND GetHWND_PlotDlg(CFigure* hFig);
GRAPHY_EXPORT CFigure* OpenFigure(RECT *rt, const CSignals &data, int devID=0);
GRAPHY_EXPORT CFigure* OpenFigure(RECT *rt, const char* caption, const CSignals &data, int devID=0);
GRAPHY_EXPORT CFigure* OpenChildFigure(RECT *rt, const CSignals &data);
GRAPHY_EXPORT int ShowFigure(CFigure *hFig, int showCode);
GRAPHY_EXPORT int closeFigure(int figId);
GRAPHY_EXPORT CAxis * AddAxis(CFigure* fig, double x0, double y0, double wid, double hei);
GRAPHY_EXPORT CLine * PlotDouble(CAxis *ax, int len, double *x, double *y, COLORREF col=0xff0000, char cymbol=0, int lineWidth=1);
GRAPHY_EXPORT void SetRange(CAxis *ax, const char xy, double x1, double x2);
GRAPHY_EXPORT void SetTick(CAxis *ax, const char xy, double x1, double x2, int nDiv=0, int nSkips=0, char const *format=NULL, double coeff=1., double offset=0.);
GRAPHY_EXPORT void deleteObj (CFigure *hFig);
GRAPHY_EXPORT void deleteObj (CAxis *hAx);
GRAPHY_EXPORT void deleteObj (CLine *hLine);
GRAPHY_EXPORT void deleteObj (CText *hText);

template <class T>
GRAPHY_EXPORT void deleteObj (T *hObj)
{
	int i, j, ind2Del;
	CAxis *hParent = (CAxis *) hLine->hPar;
	for (i=0; i<hParent->nLines; i++)
		if (hLine==hParent->m_ln[i]) {ind2Del=i; break;}
	if (i==hParent->nLines) return ; // If hLine is a ghost, do nothing.
	CLine **pnew;
	pnew = new CLine*[hParent->nLines-1];
	//storing objects to keep
	for (i=0, j=0; i<hParent->nLines; i++)
		if (i!=ind2Del) pnew[j++] = hParent->m_ln[i];
	delete hLine;
	delete[] hParent->m_ln;
	hParent->m_ln = pnew;
	hParent->nLines--;
}