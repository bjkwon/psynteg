#include "nic.h"
#include "nic/c_interface/CSequence.h"
#include "nic/c_interface/CStimulusCommand.h"
#include "nic/c_interface/CBiphasicStimulus.h"
#include "services/c_interface/CServicesErrors.h"

#ifdef  __cplusplus
extern "C" {
#endif

#define BRICVERSION  (float)2.2
#define GetBricVersion(x) getBricVersion(x) // for backward compatibility


/*  Change in version 2.2

  modulation can be not only cl (AM) but also pw (phasewidth mod)

*/

/*  About memory allocation of penv
	
	Whenever mod.cycle is changing, make sure free previous penv 
	and re-allocate penv accorindg to the new mod.cycle value.

    Other than that... user doesn't need to worry about memory management.
	Sep 2, 2005. BK
*/

typedef	struct 
{
	unsigned short cycle;
	short which; // 0 for cl mod, 1 for pw mod.
	union {
	int* i;
	float* f;
	} env;
} MODULATION;

class BRIC
{
public:
	unsigned char   ae;
	unsigned char   re;
	double			pw;
	double			pg;
	double			period;
	unsigned char	cl;
	unsigned short	repeat;
	double			tmark;
	MODULATION mod;
	BRIC();
	~BRIC();
	const BRIC& operator=(const BRIC& rhs);
};

                                                             
#define BUFFERSIZE_BRC_FILE		 1024
#define MAX_ALLOWABLE_SEQUENCE	 16380
#define MINIMUM_SHORTING		 7.
#define POWER_FRAME_PERIOD		 10000.
#define MAX_PHASEWIDTH			 3276.8
#define MIN_PHASEWIDTH			 24.8
#define MAX_PHASEGAP  			 3276.8
#define MIN_PHASEGAP  			 8.
#define MAX_PERIOD    			 26214.4 // 131072 divided by 5 (17-bit representation of period rf counts)
#define MIN_PERIOD    			 78.
#define MAX_CL    				 255
#define MAX_REPETITION			 65535

#define BR_NO_ERR				 0
#define BR_ERR_FILE_NOT_OPEN	-1
#define BR_ERR_FILE_EXISTS		-2
#define BR_ERR_CANNOT_CLOSE		-3
#define BR_ERR_CANNOT_WRITE		-4
#define BR_ERR_CANNOT_READ		-5
#define BR_ERR_TOO_MANY_ELEMENTS	-6
#define BR_ERR_INVALID_NUMBRICS  -7
#define BR_ERR_NULL_POINTER		-10
#define BR_ERR_INVALID_INDEX	-11
#define BR_ERR_INVALD_BRC_FILE			-12
#define BR_ERR_INTERNAL			-16
#define BR_ERR_MEMORY_EXCESS	-20
#define BR_ERR_TIMING			-21
#define BR_ERR_INVALID_ELECTRODE		-30
#define BR_ERR_INVALID_PHASEWIDTH		-31
#define BR_ERR_INVALID_PHASEGAP			-32
#define BR_ERR_INVALID_PERIOD			-33
#define BR_ERR_INVALID_CL				-34
#define BR_ERR_INVALID_TMARK			-35
#define BR_ERR_INVALID_MODULATION_CYCLE	-36
#define BR_ERR_INVALID_REPEAT			-37
#define BR_ERR_INVALID_DELAY 			-38
#define	BR_ERR_FILE_NOT_FOUND			-39

// Some of these NIC errors are unlikely to happen with BRIC, 
// but just leaving all them here...
#define BR_ERR_NIC_0			-100
#define BR_ERR_NIC_1			-101
#define BR_ERR_NIC_2			-102
#define BR_ERR_NIC_3			-103
#define BR_ERR_NIC_4			-104
#define BR_ERR_NIC_5			-105
#define BR_ERR_NIC_6			-106
#define BR_ERR_NIC_7			-107
#define BR_ERR_NIC_8			-108
#define BR_ERR_NIC_9			-109
#define BR_ERR_NIC_10			-110
#define BR_ERR_NIC_11			-111
#define BR_ERR_NIC_12			-112
#define BR_ERR_NIC_13			-113
#define BR_ERR_NIC_14			-114
#define BR_ERR_NIC_15			-115

#define BR_UNKNOWN_ERR			-9999



#define BR_NOREPLACE			(unsigned long)0x00000000
#define BR_REPLACE				(unsigned long)0x00000001

#define BR_PERIOD_CHECK	  	    (unsigned long)0x00000000
#define BR_NO_PERIOD_CHECK		(unsigned long)0x00000001


int WriteBric (const char* fname, BRIC *br, int num_element, unsigned long flag);
int ReadBric (const char* fname, BRIC *br, int *num_element);
int IsValidBRIC (BRIC *br, int num);
int Bric2Sequence (IMP_SEQUENCE* seqOut, BRIC brIn[], int num, unsigned long flag);
void SortBR (BRIC *br, int m);
int RemoveItem (BRIC *br, int *num, int k);
int BricTimingCheck (BRIC *br, int num, int *x, int *y);
int InterleaveBrics (BRIC *brDest, int *numDest, BRIC *brSecond, int numSecond, double offset);
float getBricVersion ();
int SetupPowerUpFrameElectrodes (int e1, int e2);
int RepeatedBR2Sequence (IMP_SEQUENCE* seqOut, BRIC *br, int Nbrics, double delay, int repeat);
void PowerBric (BRIC *br);
char* GetBRErrorString(char* errStr, int errorCode);
int Bric2Seqq (SequenceCommandHandle seqOut, BRIC *brIn, int num, unsigned long flag);
int GetDuration(BRIC *br, int num, double *duration);

#ifdef  __cplusplus
}
#endif
