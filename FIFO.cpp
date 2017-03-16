// FIFO.cpp: implementation of the CDoubleFIFO class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "psynteg.h"
#include "FIFO.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDoubleFIFO::CDoubleFIFO(int len, double initVal)
{
	length=min(len, MAX_FIFO);
	int i;
	for (i=0; i<length; i++)
		pt[i] = initVal;
}

void CDoubleFIFO::set(double newval)
{
	int i;
	if (length==0) length=1;
	for (i=0; i<length-1; i++)
		pt[i] = pt[i+1];
	pt[length-1] = newval;
}

CDoubleFIFO::~CDoubleFIFO()
{
}

CShortFIFO::CShortFIFO(int len, short initVal)
{
	length=min(len, MAX_FIFO);
	int i;
	for (i=0; i<length; i++)
		pt[i] = initVal;
}

void CShortFIFO::set(short newval)
{
	int i;
	if (length==0) length=1;
	for (i=0; i<length-1; i++)
		pt[i] = pt[i+1];
	pt[length-1] = newval;
}

CShortFIFO::~CShortFIFO()
{
}
