#include "stdafx.h"

#pragma once
#include <string>
#include <sstream>
#include <windows.h>

using namespace std;

void sformat(string& out, size_t nLengthMax, const char* format, ...) 
{
	char *buffer = new char[nLengthMax];
	va_list vl;
	va_start(vl, format);
	vsnprintf(buffer, nLengthMax, format, vl);
	buffer[nLengthMax-1] =0;
	out = string(buffer);
	delete[] buffer;
}

void triml(string& str, string delim)
{
  string::size_type pos = str.find_first_not_of(delim);
  if(pos != string::npos) str.erase(0, pos);
  else str.erase(str.begin(), str.end());
}

void trimr(string& str, string delim)
{
  string::size_type pos = str.find_last_not_of(delim);
  if(pos != string::npos) str.erase(pos + 1);
  else str.erase(str.begin(), str.end());
}

void trim(string& str, string delim)
{
	triml(str, delim);
	trimr(str, delim);
}

void triml(string& str, char delim)
{
	string _delim(1,delim);
	triml(str, _delim);
}

void trimr(string& str, char delim)
{
	string _delim(1,delim);
	trimr(str, _delim);
}

void trim(string& str, char delim)
{
	string _delim(1,delim);
	trim(str, _delim);
}

void triml(string& str, char *delim)
{
	string _delim(delim);
	triml(str, _delim);
}

void trimr(string& str, char *delim)
{
	string _delim(delim);
	trimr(str, _delim);
}

void trim(string& str, char *delim)
{
	string _delim(delim);
	trim(str, _delim);
}


/*
int ReadINI (char *errstr, const char *fname, const char *heading, string &strOut)
{
	size_t id, nextid, headlen;
	LONG filesize;
	HANDLE		hFile;
	DWORD		dw;
	string str;

	errstr[0]=0;
	char *head = new char[strlen(heading)+3];
	sprintf (head, "[%s]", heading);
	if ( (hFile = CreateFile (fname, GENERIC_READ, FILE_SHARE_READ, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL ))==INVALID_HANDLE_VALUE)
	{ sprintf(errstr,"%s cannot be opened for reading.", fname); delete[] head; return -2; }
	filesize = GetFileSize(hFile, &dw);
	char *buffer = new char[filesize+2];
	if (!ReadFile (hFile, buffer, filesize+1, &dw, NULL))
	{
		sprintf(errstr, "ReadFile() fails, code=%d", GetLastError()); 
		CloseHandle(hFile);
		delete[] buffer; delete[] head; 
		return -3;
	}
	buffer[dw]=0;
	CloseHandle(hFile);
	strOut.clear();
	strOut.insert(0, buffer, dw);
	delete[] buffer;

	id = strOut.find(head);
	if (id==string::npos) // not found.... 
	{	sprintf(errstr, "Heading %s not found", head); 
		delete[] head;  return -4;	}
	else
	{
		headlen = strlen(head); 
		strOut.erase(0, id + headlen);
		nextid = strOut.find(head);
		if (nextid!=string::npos) 
		{	sprintf(errstr, "%s occurs more than once.", head); 
			delete[] head;  return -5;	}
		nextid = strOut.find('[');
		if (nextid!=string::npos) 
		{
			str = strOut.substr(0, nextid-1);
			trimr(str, " \t");
			if (str.length()==0) str = "dummy";
			while ( str.at(str.length()-1)!='\n' && str.at(str.length()-1)!='\r')
			{	
				if ((nextid = strOut.find('[',nextid+1))==string::npos) break;
				str = strOut.substr(0, nextid-1);
				trimr(str, " \t");
			}
		}

		if (nextid!=string::npos) strOut.erase(nextid);
		trimr(strOut, "\r\n");
		trim(strOut, ' ');
	}
	delete[] head; 
	if (strOut.length()==0) strcpy(errstr, "(empty string)");
	return (int)strOut.length();
}
*/