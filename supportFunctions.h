//Support functions
int AppendVal2Array(int* array, int val);
double appendMeanIt (double* x, double offsetORvalue, double *std);
int getRandAnswer(int cinterval);
void EnableID(CWnd *hDlg, int id, int onoff);
int GetIndexWithZero(double* array, int len);
int _IsSemiMonotonic (int len, double *x);
int findIndex(double val, int len, double *x);
void doublearray2String(int length, double *x, CString &outStr);
double getmean(int len, double *x, double *std);
float getmax(int len, float *x);
int IsString(const CString str);
void ReplaceVariable (CString& body, const CString& st);
int strscanfINI (CString& errstr, const char *fname, const char *heading, CString& outString);

#include <string>

using namespace std;

void sformat(string& out, size_t nLengthMax, const char* format, ...);
void CollateStrings(string *strIn, int nStrings, string &strOut, const char *breaker);
int IsAllNumeric (string str);
void trim(string& str, char delim);

int randin(int limit);

template <class T>
T * perm(T *out, int m)
{ // permute the order of an array out with length m
	int i, j, k;
	T temp;
	if (out==NULL)
		return out;
	for (i=0; i<m*1000; i++)
	{
		j=randin(m);
		k=randin(m);
		temp = out[j];
		out[j] = out[k];
		out[k] = temp;
	}
	return out;
}