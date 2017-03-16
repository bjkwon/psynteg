#define FL_FILE			(WORD)0XFAFF
#define FL_FILEINFO		(WORD)0XFAFE

// Command Codes
#define 	FL_RANGE_MIN		(WORD)0x1234
#define 	FL_RANGE_MAX		(WORD)0x1235
#define 	FL_CLEAN_WINDOW		(WORD)0x6345
#define 	FL_PLAY				(WORD)0x34AA
#define 	FL_PLAY_SAMPLE		(WORD)0x340A
#define 	FL_DONE		  		(WORD)0x34BB
#define		FL_INIT				(WORD)0x0000
#define 	FL_ABOUT2PRESENT	(WORD)0x4111
#define 	FL_ABOUT2STOP  		(WORD)0x4113
#define 	FL_PARAMSONDECK		(WORD)0X615A
#define		FL_END_OF_PRSENTATION (WORD)0X6E50
#define		FL_STOP				(WORD)0x14d2
#define		FL_TEXT 			(WORD)0xf34a
#define		FL_ADJUSTING		(WORD)0x7345
#define		FL_NEED_MORE		(WORD)0x8908
// FL_NEED_MORE sends binary.... int array of need_more_up_or_down... (1:up, -1:down, 0: no need) for each item
#define		FL_BATCH_COUNT		(WORD)0x5687
#define		FL_APP_MATCH		(WORD)0x010f

#define		FL_BEGIN					(WORD)0x0001
#define		FL_RESPONSE					(WORD)0x5412
#define		FL_RESULT					(WORD)0x8412
#define		FL_FEEDBACK			(WORD)0x1412
#define		FL_COUNT				(WORD)0xf343
#define		FL_TEXT 				(WORD)0xf34a
#define		FL_RESUME 				(WORD)0x7886
#define		FL_INSTRUCTION 				(WORD)0xfaa4
#define		FL_INIT_SLIDER 			(WORD)0xfca4
#define		FL_PLAYAGAIN 			(WORD)0x34ab
#define		FL_PLAY_SHORT 			(WORD)0x64CF
#define		FL_INIT_PRANK		(WORD)0x234C
#define		FL_READYVALUE		(WORD)0x234D
#define		FL_PRANK_PLAY		(WORD)0x234E
#define		FL_WAIT4NEXT		(WORD)0x2448

typedef struct
{
	int itemAdjusted;
	int newVal;
} ADJUSTING;


