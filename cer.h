#include <stdio.h>
#include "Miscellaneous.h"


// Check ERror macro (returns integer)
#define CER(e)                                                  \
{                                                               \
    Boolean errorCode = (e);                                    \
    if (errorCode != NICCInterface__Function_Success )         \
    {                                                           \
     char zbuf[128];                                              \
     sprintf (zbuf, "Error at: "#e", code: %s, %d", NICerrorMsg[GetLastNICError()], GetLastNICError());                 \
	  MessageBox (zbuf, "", MB_OK);         \
	  return 0;                                               \
    }                                                          \
}                                                               

// Check ERror macro (returns void)
#define CERV(e)                                                  \
{                                                               \
    Boolean errorCode = (e);                                    \
    if (errorCode != NICCInterface__Function_Success )         \
    {                                                           \
     char zbuf[256];                                              \
     sprintf (zbuf, "Error at: "#e", code: %s, %d", NICerrorMsg[GetLastNICError()], GetLastNICError());                 \
	  MessageBox (zbuf, "", MB_OK);         \
	  return;                                               \
    }                                                          \
}                                                               

#define CER_BRC(e)           \
{                                                               \
    int errorCode = (e);                                    \
    if (errorCode < 0 )         \
    {                                                           \
	     char zbuf[256]; char errmsg[256]; sprintf(zbuf, "Error at: "#e);                \
         GetBRErrorString (errmsg, errorCode); 		 \
		 errstr.Format("%s - %s", zbuf, errmsg); MessageBox (errstr); return 0;  \
    }                                                          \
}                                                               

#define CER_BRCV(e)           \
{                                                               \
    int errorCode = (e);                                    \
    if (errorCode < 0 )         \
    {                                                           \
	     char zbuf[256], errstr[256];                            \
	     sprintf(zbuf, "Error at: "#e);                \
         GetBRErrorString (errstr, errorCode); 		 \
		 MessageBox (zbuf, errstr, MB_OK);       return;  \
    }                                                          \
}                                                               

#define CER_BRCNULL(e)           \
{                                                               \
    int errorCode = (e);                                    \
    if (errorCode < 0 )         \
    {                                                           \
	     char zbuf[256], errstr[256];                            \
	     sprintf(zbuf, "Error at: "#e);                \
         GetBRErrorString (errstr, errorCode); 		 \
		 MessageBox (zbuf, errstr, MB_OK);       return NULL;  \
    }                                                          \
}                                                               

#define FER(x) {\
char linestr[256], estr[128]; int ecode = (x); \
if (ecode<0) { flyErrMsg(ecode, estr); wsprintf (linestr, "Error occurred at "#x", code=%d", ecode); MessageBox (linestr, estr, MB_OK);} }

