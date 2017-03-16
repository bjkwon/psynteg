#include <windows.h>
#include "resource.h"

int  WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
	STARTUPINFO sinfo;
	PROCESS_INFORMATION pinfo;
	char envStr[10*MAX_PATH], pathStr[10*MAX_PATH];
	envStr[0]=0;
	envStr[1]=0;

	HANDLE h = LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 0, 0, 0);
    SetClassLongPtr (NULL, GCL_HICON, (LONG_PTR)h);

	LPCH lpvEnv = GetEnvironmentStrings();

	for (LPSTR lpszVar = (LPTSTR) lpvEnv; *lpszVar; lpszVar++)
	{
		LPSTR xx;
		xx = strstr(lpszVar, "PATH");
		if (xx==NULL) lpszVar += strlen(lpszVar) + 1;
		else strcpy(pathStr, xx);
	}
	FreeEnvironmentStrings(lpvEnv);
    ZeroMemory( &sinfo, sizeof(sinfo) );
    sinfo.cb = sizeof(sinfo);
    ZeroMemory( &pinfo, sizeof(pinfo) );

	DWORD dwres = GetEnvironmentVariable("PATH", envStr, sizeof(envStr));
	strcat(envStr, ";c:\\Program Files\\Common Files\\auditorypro");

	dwres = SetEnvironmentVariable("PATH", envStr);
	int res = CreateProcess (NULL, "psynteg2.exe", NULL, NULL, FALSE, 0, 
		NULL, NULL, &sinfo, &pinfo);
	return 0;
}