#include <process.h>

class LogReport {
public:
	BOOL   fUseEmail;
	BOOL   fUseFtp;
	CString email_address;
	CString ftp_address;
	CString ftp_id;
	CString ftp_password;

	CString filename;
	CString NamePrefix;
	LogReport(char *namePrefix);
};

void uploadLogFile(void *p);
