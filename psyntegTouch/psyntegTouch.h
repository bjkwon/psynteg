#include <windows.h>
#include <commctrl.h>  
#include <stdio.h>
#include "flyplus.h"
#include "resource.h"
#include "..\protocol.h"


#define	BUTT1	888
#define	BUTT2	889
#define	BUTT3	890
#define	BUTT4	891
#define	BUTT5	892
#define	BUTT6	893
#define	BUTT7	894
#define	BUTT8	895
#define	BUTT9	896
#define	BUTT10	897
#define	BUTT11	898
#define	BUTT12	899
#define	BUTT13	900
#define ID_HMSG		928
#define ID_FEEDBACK	929
#define ID_HBEGIN		1928
#define ID_HMSG2		948
#define ID_COUNTER		9248
#define ID_CAP			1200
#define ID_SLIDERVAL	2000
#define ID_PLAY			2001
#define ID_DONE			2002
#define ID_NEED_MORE	2003
#define ID_SLIDER		2004
#define ID_EDIT			2005

#define WM__STOPPED		WM_APP+229


#define FDBCKTIMER	1000
#define BEGINDELAY  1001
#define PRESS_BEGIN  1002
#define PRESS_RESPONSE  1003

#define MAX_INTERVALS  13
#define STOPPED_BY_USER  1


#define AUTORESP  2000
