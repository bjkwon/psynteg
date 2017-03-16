// FIFO.h: interface for the CDoubleFIFO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIFO_H__0DBBAD4B_1EF3_422F_B604_5F5E6156C3CD__INCLUDED_)
#define AFX_FIFO_H__0DBBAD4B_1EF3_422F_B604_5F5E6156C3CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_FIFO 16

class CDoubleFIFO  
{
public:
	inline void SetLength(int newLen) { length = min (newLen, MAX_FIFO); }
	inline double get() {return pt[0];}
	void set(double newval);
	CDoubleFIFO(int len=1, double initVal=0.);
	virtual ~CDoubleFIFO();
private:
	int length;
	double pt[MAX_FIFO];
};

class CShortFIFO
{
public:
	inline void SetLength(int newLen) { length = min (newLen, MAX_FIFO); }
	inline short get() {return pt[0];}
	void set(short newval);
	CShortFIFO(int len=1, short initVal=-1);
	virtual ~CShortFIFO();
private:
	int length;
	short pt[MAX_FIFO];
};

#endif // !defined(AFX_FIFO_H__0DBBAD4B_1EF3_422F_B604_5F5E6156C3CD__INCLUDED_)
