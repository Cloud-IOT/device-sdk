#ifndef	____UA_TIME_clock_H____
	   #define	____UA_TIME_clock_H____


//--------------------------------------------------------------
////	说明



//--------------------------------------------------------------
////	头文件
////	该头文件会用到的
#include "contiki.h"
#include <string.h>
#include <stdio.h> /* For printf() */
#include "gpio_api.h"
#include "atcmd.h"

#include <stdlib.h>
#include	"pwm_api.h"
#include	"swpwm.h"
//#include	"..\\icomlib\\include\\sys\\ctimer.h"
#include	"sys/ctimer.h"
#include	"sys/rtimer.h"
#include	"dbg-atcmd-api.h"
#include	"drv_uart.h"

#include	"sys_status_api.h"

#include	"systemconf.h"
#include	"flash_api.h"

#include	"uRan_Macro_Contiki.h"
#include	"UA_Func.h"

#include	"key.h"
#include "atcmd_icomm.h"
#include "sys/etimer.h"
#include "wdog_api.h"
#include <string.h>

#include	"process.h"




#include	"cfs/cfs.h"

#include	"uRan_Macro_Contiki.h"
#include	"UA_Func.h"

#include	"UA_TIME_clock_Inc.h"


//--------------------------------------------------------------
////	Option & Version

//#define	____D_SaveData_In_FS____


//--------------------------------------------------------------
////	Define
#define	D_szActionTimer_Data_FileName		"ActTimer.dat"



//--------------------------------------------------------------
////	结构体...定义


//----------------------------------------
typedef enum _ETimeChagneFlag{
	E_TimeChagneFlag_None = 0,
	E_TimeChagneFlag_Sec = 1,
	E_TimeChagneFlag_Min = 2,
	E_TimeChagneFlag_Hour = 4,
	E_TimeChagneFlag_Day = 8,
	E_TimeChagneFlag_Month = 16,
	E_TimeChagneFlag_Year = 32,
	E_TimeChagneFlag_Week = 64,

	E_TimeChagneFlag_Max,
}ETimeChagneFlag;




//--------------------------------------------------------------
////	函数声明
void	F_ActionTimer_Print(void);




//--------------------------------------------------------------
////	变量
D_DefineProcess(F_Proc_UA_TIME_CLOCK);

//----------------------------------------
process_event_t event_TIME_CLOCK_post;
struct tm system_time_now;
static unsigned int Month[13] ={0,31,28,31,30,31,30,31,31,30,31,30,31};

//----------------------------------------
//static TDWORD R_dwTimerID = 1;
static SActionTimer	R_saActionTimers[D_ActionTimer_Amount];
static int R_iTimePowerOnInit = D_FALSE;
TDWORD	R_dwSecCounter;		//	从19700101算起秒数
static int R_iTimeZone = 8;
static ETimeChagneFlag R_eTimeChangeFlag = E_TimeChagneFlag_None;		//	计时变化
static int R_bActionTimer_Enable = D_FALSE;			//



//--------------------------------------------------------------
////	Macro
#define	M_CmpTime(__tm1,__tm2) \
	(((__tm1).tm_hour == (__tm2).tm_hour) && ((__tm1).tm_min == (__tm2).tm_min) && ((__tm1).tm_sec == (__tm2).tm_sec))


//----------------------------------------
#ifdef	DEBUG_VERSION
#define	MF_ActionTimer_Print() F_ActionTimer_Print()
#else
#define	MF_ActionTimer_Print()
#endif





//--------------------------------------------------------------
////	变量



#endif


