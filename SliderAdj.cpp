// SliderAdj.cpp : implementation file
//

#include "stdafx.h"
#include "psynteg.h"
#include "SliderAdj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSliderAdj dialog


CSliderAdj::CSliderAdj(CWnd* pParent /*=NULL*/)
	: CProcDialog(CSliderAdj::IDD, pParent)
	, instruction(_T(""))
{
	//{{AFX_DATA_INIT(CSliderAdj)
	val = 0.0;
	m_step = 0.0;
	m_range1 = 0;
	m_range2 = 0;
	//}}AFX_DATA_INIT
	hPar = pParent;
}


void CSliderAdj::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSliderAdj)
	DDX_Control(pDX, IDC_INIT_VAL, m_valEdit);
	DDX_Text(pDX, IDC_INIT_VAL, val);
	DDX_Text(pDX, IDC_STEP, m_step);
	DDX_Text(pDX, IDC_RANGE1, m_range1);
	DDV_MinMaxInt(pDX, m_range1, -255, 0);
	DDX_Text(pDX, IDC_RANGE2, m_range2);
	DDV_MinMaxInt(pDX, m_range2, 0, 255);
	DDX_Text(pDX, IDC_INST, instruction);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSliderAdj, CDialog)
	//{{AFX_MSG_MAP(CSliderAdj)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSliderAdj message handlers


LRESULT CSliderAdj::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message==WM_COMMAND)
	{
		if (wParam==IDCANCEL || wParam==IDOK)
		return NULL;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}
