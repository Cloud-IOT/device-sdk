#ifndef	____UA_TIME_clock_Inc_H____
	   #define	____UA_TIME_clock_Inc_H____


//--------------------------------------------------------------
////	用法
/*

*/



//--------------------------------------------------------------
////	头文件
////	外部引用时需要使用的
#include	"gcc_time.h"

#include	"uRan_Macro_Contiki.h"


//--------------------------------------------------------------
////	Define

#define	D_ActionTimer_Amount	5	//	定时器个数
#define	D_ActionTimer_InvalidID	0xffffffff



//--------------------------------------------------------------
////	结构体...定义

typedef enum _EActionTimerMode{
	E_ActionTimerMode_Off = 0,
	E_ActionTimerMode_OneTime,
	E_ActionTimerMode_DayRepeat,
	E_ActionTimerMode_WeekRepeat,
	E_ActionTimerMode_MonthRepeat,
	E_ActionTimerMode_Delay,

	E_ActionTimerMode_Max,

	E_ActionTimerMode_StopBit = 0x80,
}EActionTimerMode;

typedef void (*PF_ActionTimer_CallBack)(void * pPara);
typedef struct _SActionTimer
{
	UID128	uID;
	EActionTimerMode eMode;
	TDWORD	dwRepeatFlag;		//	周|月循环用flag
	union {
	struct tm	sTm;			//	比较日历用
	TDWORD	dwSec;				//	unix时间, 只用于设置时
	} u;
	PF_ActionTimer_CallBack pfHdl;
	TBYTE	btSocket;			//	插口编号
	TBYTE	btOnOff;
	TBYTE	btRev0;
	TBYTE	btRev1;
} SActionTimer;

// 只用于设置/读取
typedef struct _SActionSetTimerInfo
{
	TBYTE	btIndex;
	TBYTE	btMode;				//	EActionTimerMode
	TBYTE	btSocket;			//	插口编号
	TBYTE	btOnOff;
	TDWORD	dwSec;				//	unix时间/延时,
	TDWORD	dwRepeatFlag;		//	周|月循环用flag
} SActionSetTimerInfo;


//--------------------------------------------------------------
////	声明变量
//----------------------------------------
extern TDWORD	R_dwSecCounter;
extern struct tm system_time_now;

//--------------------------------------------------------------
////	Macro
//--------------------------------------



//--------------------------------------------------------------
////	函数
//----------------------------------------

extern void system_time_init(TDWORD dwUnixTime, char * szTime);
extern int	F_ActionTimer_GetTimerList(SActionSetTimerInfo * pTimer, int * piCount);
extern int	F_ActionTimer_Add2(UID128 *puID,  TDWORD dwUnixTime, EActionTimerMode eMode, TBYTE btSocket, TBYTE btOnOff);
extern int	F_ActionTimer_Add3(SActionSetTimerInfo *pTimer);
extern int	F_ActionTimer_DelAll(void);
extern ERetErr	F_ActionTimer_Del3(TBYTE btIndex);
extern ERetErr	F_ActionTimer_Stop3(TBYTE btIndex);
extern ERetErr	F_ActionTimer_Start3(TBYTE btIndex);
extern void F_ActionTimer_Enable(int bEn);

extern void	F_ActionTimer_ATcmd_InitClock(TDWORD dwUnixTime, char * szTime);
extern void	F_ActionTimer_ATcmd_Add(int iMode, int iOnOff);


#endif

