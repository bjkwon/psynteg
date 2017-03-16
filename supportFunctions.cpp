#include "stdafx.h"
#include <math.h>
#include <stdlib.h>
#include <math.h>
#include "audfret.h"
#include "audstr.h"

#include <string>
#include "supportFunctions.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CHAR_NUMERIC ":.+-0123456789"


int randin(int limit)
{// generated a random number with uniform distr (from 0 thru limit-1)
	if (limit==0) return 0;
	return (int)fmod((double)rand(),(double)limit);
}


int GetIndexWithZero(double* array, int len)
{
	int i;
	for (i=0; i<len;i++)
	{
		if (array[i] == 0)
			return i;
	}
	return -1; // no element is zero
}

int AppendVal2Array(int* array, int val)
{// array should have at least one member with the value zero.
	UINT ind=0;
	srand((unsigned)GetCurrentTime() );
	while (1)
	{
		if (array[ind] == 0)
		{
			array[ind] = val;
			return ind;
		}
		else 
			ind++;
		if (ind > 65536*2-1)
			return -1; //error!! 
	}
}

double appendMeanIt (double* x, double offsetORvalue, double *std)
{
	// if the first argument is null, it means initializing currentID
	// if the 3rd argument is null, it means append to array
	
	// offset: the number to skip in calculation of the average.

	static int currentID;
	int i; 
	double mean, sqSum=0., sum=0.;

	if (x==NULL)
	{
		currentID = 0;
	}
	else if (std==NULL)
	{
		x[currentID++]=offsetORvalue;
	}
	else
	{
		for (i=(int)offsetORvalue; i<currentID; i++)
		{
			sum += x[i];
			sqSum += (x[i] * x[i]) ; 
		}
		mean = (double)sum / ((double)i-offsetORvalue);
		*std = sqrt ( (double)sqSum / ((double)i-offsetORvalue) - mean*mean);
		return mean;
	}
	return 0.;
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

void EnableID(CWnd *hDlg, int id, int onoff)
{
	CWnd *h = hDlg->GetDlgItem(id);
	h->EnableWindow(onoff);
}

int _belowabove (double val, int len, double *x)
{
	if (val<x[0]) 
		return -1;
	if (val>=x[len-1]) 
		return len-1;
	return 0;
}

int _IsSemiMonotonic (int len, double *x)
{
	int i, updown;
	updown = x[1]>x[0];
	for (i=2; i<len; i++)
	{
		if ( updown != (x[i]>x[i-1]) )
			return 0;
	}
	return 1;
}

int findIndex (double val, int len, double *x)
{
	// Find the greatest index of the array x, which has the element smaller than (or the same as) val.
	ASSERT(len>0);
	int lastID, curId;
	curId=0;
	lastID = _belowabove (val,len,x);
	if (lastID!=0)
		return lastID;
	while ( len>1)
	{
		len /= 2;
		if (_belowabove (val,len,x+curId)>0)
			curId += len + _belowabove (val,len,x+curId+len);
	}
	return curId;
}

void doublearray2String(int length, double *x, CString &outStr)
{
	int i;
	CString res;
	char buf[16];
	res = "";
	for (i=0; i<length; i++)
	{
		sprintfFloat(x[i], 2, buf, sizeof(buf));
		strcat(buf, " ");
		res += buf;
	}
	outStr = res;
}

double getmean(int len, double *x, double *std)
{
	int i;
	double mean, sqSum=0., sum=0.;
	for (i=0; i<len; i++)
	{
		sum += x[i];
		sqSum += (x[i] * x[i]) ; 

	}
	mean = sum / (double)len;
	*std = sqrt ( sqSum/(double)len - mean*mean );
	return mean;
}

float getmax(int len, float *x)
{
	int i;
	float maax=(float)-1.e-5;
	for (i=0; i<len; i++)
		if (x[i]>maax)
			maax = x[i];
	return maax;
}

int GetSurroundedBy(char c1, char c2, const char* in, CString& out, int iStart)
{
	// This extracts the string bounded by c1 and c2 (the first boundary)
	// returns the index of the ending border
	// c1 and c2 must be different
	CString _in(in);
	bool inside(false), loop(true);
	int i, len, cumInt(0);
	if (in==NULL) return -1;
	len = _in.GetLength();
	if (len>65535)  return -1;
	for (i=iStart; i<len && loop; i++)
	{
		if (in[i]==c1) { cumInt++; inside=true;}
		else if (in[i]==c2) cumInt--; 
			if (cumInt<0) return -1; // unblanced parenthesis 
		if (inside && cumInt==0) loop=false;
	}
	if (cumInt!=0 || !inside) return -1;
	int first = _in.Find(c1, iStart)+1;
	int count = i - first-1;
	out = _in.Mid(first, count);
	return i-1;
}

int IsString(const CString str)
{
	CString _str(str);
	_str.Trim();

	if (_str[0]=='\x22' && _str[_str.GetLength()-1]=='\x22') return 1;
	else return 0;
}

int FindFirstNonNumeric(const CString& str)
{
	CString s(CHAR_NUMERIC);
	for (int i=0; i<str.GetLength(); i++)
	{
		if (s.Find(str[i])==-1)
			return i;
	}
	return -1;
}


int IsNumeric(const CString str)
{
	CString _str(str);
	_str.Trim();
	_str.Replace("[",".");
	_str.Replace("]",".");
	_str.Replace(" ",".");

	if (FindFirstNonNumeric(_str)==-1) return 1;
	else return 0;
}

void ReplaceVariable (CString& body, const CString& newstr)
{ // For now, 'v' is the variable.
	int id, idq1(0), idq2(-1);
	CString str(body), inside, idV;
	char dquot('\x22'); // double-quotation mark
	str.Replace("wav", "QQ$5232Q");
	str.Replace("nvoc", "QQ$5232K");

	if ((id=str.Find('v'))==-1) return ;
	while ((idq1 = str.Find(dquot, idq2+1))!=-1) 
	{
		if ( (idq2 = str.Find(dquot, idq1+1))==-1) return; //quotation not matching
		if ( (id = str.Find('v', idq1+1)) < idq2)		
			str.SetAt(id, '\x1f'); // MEANING: DO NOT TOUCH
		else
			str.SetAt(id, '\x01'); // MEANING: this is true 'v' that needs replacing
	}
	id=idq2;
	while ((id = str.Find('v', id+1))!=-1)  		str.SetAt(id, '\x01');
	body = str;
	body.Replace("\x01", newstr);
	body.Replace("\x1f", "v");
	body.Replace("QQ$5232Q","wav");
	body.Replace("QQ$5232K","nvoc");
}

void CollateStrings(string *strIn, int nStrings, string &strOut, const char *breaker)
{
	strOut="";
	for (int i=0; i<nStrings; i++)
	{
		strOut += strIn[i];
		strOut += breaker;
	}
}