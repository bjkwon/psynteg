#define FER(x) {\
char linestr[256], estr[128]; int ecode = (x); \
if (ecode<0) { flyErrMsg(ecode, estr); wsprintf (linestr, "Error occurred at "#x", code=%d", ecode); MessageBox (hwnd, linestr, estr, MB_OK);} }
