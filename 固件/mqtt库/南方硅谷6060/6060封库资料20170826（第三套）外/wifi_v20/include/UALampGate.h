#ifndef	____UALampGate_H____
	   #define	____UALampGate_H____


//--------------------------------------------------------------
////	说明



//--------------------------------------------------------------
////	头文件
////	该头文件会用到的
#include "ssv_config.h"

//contiki include
#include "contiki.h"
#include "contiki-net.h"
#include "contiki-lib.h"

#include <string.h>
#include <stdio.h> /* For printf() */
#include "gpio_api.h"
#include "atcmd.h"



#include <stdlib.h>
#include	"pwm_api.h"
#include	"swpwm.h"
//#include	"..\\icomlib\\include\\sys\\ctimer.h"
//#include	"sys/ctimer.h"
//#include	"sys/rtimer.h"
#include	"dbg-atcmd-api.h"
#include	"drv_uart.h"

#include	"Cabrio-conf.h"
//	IP
#include "net/uip.h"
#include "net/tcpip.h"
#include "net/dhcpc.h"
#include "net/uip_arp.h"
#include "mac.h"

#include	"socket_driver.h"
#include	"sys_status_api.h"
#include	"atcmd_icomm.h"
#include	"drv_gpio.h"

#include	"serial_api.h"
#include	"drv_uart.h"
#include	"flash_api.h"

#include	"uRan_Macro_Contiki.h"
#include	"UA_Func.h"

#include	"UALampGate_Inc.h"


//--------------------------------------------------------------
////	Option & Version
#ifdef	DEBUG_VERSION
#define	____D_LampGate_Dbg____
#define	____D_LampGate_UART_Dbg____
//#define	____D_Dbg_Test_Uart____
//#define	____D_Dbg_Test_NoCheckSum____
//#define	____D_DBg_LampGate____
//#define	____D_DBg_LampGate_Lv2____
#endif

#define	____D_UartRsp_HandleInThread____
#define	____D_Uart_Use_StringData____


#define	____D_LampGate_NewLampStruct____		//	only use this macro by u 20161024
#define	____D_LampGate_Scan_NewMethod____

//	cfg 保存使用
//#define	____D_LampGate_Cfg_Use_Cfs____		//	fs有问题, 用回custarea -u 20160914

#define	____D_LampGate_FixBug_befault1116____			//	临时解决 remove outline lamp return op_err code 01, 后续版本取消



//--------------------------------------------------------------
////	Define

#define	D_LampGate_Cmd_Notify_Code_InitOK		0x7000
#define	D_LampGate_CfgSave_Mask		0x55414c47		//	'UALG'

#ifdef	____D_LampGate_Cfg_Use_Cfs____
#include	"cfs/cfs.h"
#define	D_szLampGate_Cfg_FileName		"LampGate.cfg"
#endif

//	操作回复
#define	D_iLampGate_OpRsp_Sucess			0
#define	D_iLampGate_OpRsp_Err				1
#define	D_iLampGate_OpRsp_Outline			2


#define	D_LampGata_2G4_Power_Pin	GPIO_3


//----------------------------------------
#ifdef	____D_DBg_LampGate____
#define	D_LampGate_Time_Interval		(CLOCK_SECOND/5)
#else
#define	D_LampGate_Time_Interval		(CLOCK_SECOND/5)
#endif
#define	D_LampGate_CmdTimeout			(5) //(1)			//	unit: D_LampGate_Time_Interval
#define	D_LampGate_ScanTime				(2*20)		//	unit: D_LampGate_Time_Interval
#define	D_LampGate_Cmd_ReSendTimes		3			//	cmd重复发送次数

#define	D_LampGate_UartPacketData_DataMinSize		1
#define	D_LampGate_UartPacketData_PacketMinSize		(D_LampGate_UartPacketData_DataMinSize + 3)

#define	D_LampGate_UARTDataBuffer_Size	(8+24*5) //8

#define	D_LampStateChange_None			0x00
#define	D_LampStateChange_Online		0x01
#define	D_LampStateChange_Lv			0x02
#define	D_LampStateChange_ColorTemp		0x04
#define	D_LampStateChange_LvCT			(D_LampStateChange_Lv | D_LampStateChange_ColorTemp)
#define	D_LampStateChange_Err			0x08	//	Lamp Err
#define	D_LampStateChange_DevErr		0x10	//	LampGate Err, No Rsp
#define	D_LampStateChange_All			(D_LampStateChange_Online | D_LampStateChange_LvCT | D_LampStateChange_Err | D_LampStateChange_DevErr)

#define	D_LampGate_SetRetryTimes			1		//	设置出错后重试的次数(只是重试的次数)

//--------------------------------------------------------------
////	结构体...定义

typedef struct _SLampState
{
	#ifdef	____D_LampGate_NewLampStruct____
	TBYTE	btNo;
	#endif
	TBYTE	bOnLine;			//	D_TRUE: online.
	TBYTE	bErr;				//	D_TRUE: Error.
	TBYTE	btChanged;			//	状态改变	RF -> Wifi -> Srv
								//	0x01, Online; 0x2, Lv; 0x04, CT; 0x8, Err
	TBYTE	bToUpdate_FromDev;	//	Dev -> Wifi, D_TRUE, 从Dev取状态
	TBYTE	btOnMode;
	TBYTE	btGroup;
	TBYTE	btLv;				//	亮度
	TBYTE	btColorTemp;		//	色温
	TBYTE	btLv_R;				//	RGB
	TBYTE	btLv_G;				//	RGB
	TBYTE	btLv_B;				//	RGB
} SLampState;

typedef struct _SLampGateUARTDataBuf
{
	TBYTE	btCount;
	TBYTE	btSize;					//	buf大小
	TBYTE 	Buf[D_LampGate_UARTDataBuffer_Size];
} SLampGateUARTDataBuf;

typedef enum _ELampGate_Cmd{
	E_LampGate_Cmd_None = 0,
	//	Wifi -> 2.4G
	E_LampGate_Cmd_SetLight_LvCT = 0x01,
	E_LampGate_Cmd_SetLight_LvCT_Batch,
	E_LampGate_Cmd_GetLight_Info,
	E_LampGate_Cmd_GetAmount,
	E_LampGate_Cmd_Query,
	E_LampGate_Cmd_Pair,
	E_LampGate_Cmd_Remove,
	E_LampGate_Cmd_GetLight_Info_Batch,

	//	2.4G -> Wifi
	//	采用轮询方式, 此无效
	E_LampGate_Cmd_Change = 0x11,
	E_LampGate_Cmd_Change_LvCT,

	//------------------------------
	//	Protocol V2
	E_LampGate_Cmd_GetInfo = 0x20,

	E_LampGate_Cmd_Notify = 0x30,

	E_LampGate_Cmd_Max
}ELampGate_Cmd;

typedef enum _ELampGate_Cmd_GetInfoType{
	E_LampGate_Cmd_GetInfo_None = 0,

	E_LampGate_Cmd_GetInfo_LampVer = 0xFC,
	E_LampGate_Cmd_GetInfo_LampID = 0xFD,
	E_LampGate_Cmd_GetInfo_GateID = 0xFE,
	E_LampGate_Cmd_GetInfo_GateVer = 0xFF,
}ELampGate_Cmd_GetInfoType;

typedef enum _ELampGateType{
	E_LampGateType_MCUDeal = 0,		//	MCU处理
	E_LampGateType_ThisDeal,		//	6060处理, MCU做透传
}ELampGateType;


//----------------------------------------
typedef enum _ELampGate_2G4Status{
	E_LampGate_2G4Status_Off = 0,
	E_LampGate_2G4Status_Reset,
	E_LampGate_2G4Status_On,
	E_LampGate_2G4Status_Ready,
	E_LampGate_2G4Status_Error,
}ELampGate_2G4Status;


//----------------------------------------
//	返回: 0, 成功;
typedef int (*PF_LampGate_CmdRspHdl)(TBYTE * pVal, TBYTE btLen);
typedef struct _SLampGate_CmdRspHdl
{
	ELampGate_Cmd eCmd;
	PF_LampGate_CmdRspHdl pfHdl;
} SLampGate_CmdRspHdl;






//--------------------------------------------------------------
////	函数声明

//----------------------------------------
//	return: D_TRUE, In Scan Mode
L2S F_LampGate_IsInScanMode(void);

//----------------------------------------
int F_LampGate_CmdRspHdl_GetInfo(TBYTE * pVal, TBYTE btLen);
int F_LampGate_CmdRspHdl_SetLight_LvCT(TBYTE * pVal, TBYTE btLen);
int F_LampGate_CmdRspHdl_SetLight_LvCT_Batch(TBYTE * pVal, TBYTE btLen);
////int F_LampGate_CmdRspHdl_SetLight_ColorTemp(TBYTE * pVal, TBYTE btLen);
int F_LampGate_CmdRspHdl_GetLight_Info(TBYTE * pVal, TBYTE btLen);
int F_LampGate_CmdRspHdl_GetLight_Info_Batch(TBYTE * pVal, TBYTE btLen);
int F_LampGate_CmdRspHdl_GetAmount(TBYTE * pVal, TBYTE btLen);
int F_LampGate_CmdRspHdl_Query(TBYTE * pVal, TBYTE btLen);
int F_LampGate_CmdRspHdl_Pair(TBYTE * pVal, TBYTE btLen);
int F_LampGate_CmdRspHdl_Change(TBYTE * pVal, TBYTE btLen);
int F_LampGate_CmdRspHdl_Change_LvCT(TBYTE * pVal, TBYTE btLen);
int F_LampGate_CmdRspHdl_Remove(TBYTE * pVal, TBYTE btLen);
int F_LampGate_CmdRspHdl_Notify(TBYTE * pVal, TBYTE btLen);


//----------------------------------------
void F_LampGate_Pair_Done_toUACloud(void);
void F_LampGate_Change_LvCT_toUACloud(void);
void F_LampGate_PrintLampState(void);


//----------------------------------------
void F_LampGate_UartSend(void);
L3S F_LampGate_Cfg_SetPairedLampToList(TBYTE btNo, int bPaired);
void F_LampGate_Cfg_Save(void);

//----------------------------------------
void F_LampGate_UartRead(void * pPara) ATTRIBUTE_SECTION_SRAM;



//--------------------------------------------------------------
////	变量

static int iScanTime_Count = 0;

//----------------------------------------
process_event_t R_evUALampGate;
process_event_t R_evUALampGate_Uart;
static TDWORD	R_btLampAmount;
static TDWORD	R_btLampCount;
static int R_bLampStateChanged;					//	总的状态改变
static SLampState	S_aLampsState[D_LampGate_Lamp_MaxAmount];
static SLampGateUARTDataBuf S_UARTData_RX;
static SLampGateUARTDataBuf S_UARTData_RX_Valid;
static SLampGateUARTDataBuf S_UARTData_TX;
static ELampGate_Step	R_eLampGate_Step;
static ELampGate_Step	R_eLampGate_Step_Bak;
static int	R_bLampGate_IsReady = D_FALSE;
static int	R_bLampGate_UART_CommOK = D_FALSE;
static SLampGate_CfgSave S_LampGateCfg;
static int	R_bLampGate_Cfg_BeSave = D_FALSE;

//	WIFI -> 2.4G
static ELampGate_Cmd R_eCmd_Current;
static TBYTE R_btLampNo_Current;
static TBYTE R_btLampNo_Scan;		//	空闲扫描用
static int R_iQuery_GroupIndex;


//	2.4G -> WIFI
static TBYTE R_btLampNo_FromDev;

//	SRV -> WIFI
static TBYTE R_btLampNo_FromSrv;
static SLampState	S_LampsState_ToBeUpdate;
static ELampGate_Step	R_eLampGate_State_ToSrv;

//	Scan
static ELampGate_Step	R_eLampGate_Step_ToDoWhileScan;
static TBYTE R_btLampNo_FromSrv_ToDoWhileScan;

static int	R_iLampGate_SetRetryTime;
static int	R_iLampGate_SetRetryTime_Batch;

#ifdef	____D_DBg_LampGate_Lv2____
static clock_time_t R_64Time_UartSend = 0;
#endif

static ELampGate_2G4Status	R_eLampGate_2G4_Status = E_LampGate_2G4Status_Off;



static const SLampGate_CmdRspHdl S_aLampGate_CmdRspHdlTbl[] = {
	{E_LampGate_Cmd_SetLight_LvCT, 			F_LampGate_CmdRspHdl_SetLight_LvCT},
	{E_LampGate_Cmd_SetLight_LvCT_Batch, 	F_LampGate_CmdRspHdl_SetLight_LvCT_Batch},
	////{E_LampGate_Cmd_SetLight_ColorTemp, F_LampGate_CmdRspHdl_SetLight_ColorTemp},
	{E_LampGate_Cmd_GetLight_Info, 			F_LampGate_CmdRspHdl_GetLight_Info},
	{E_LampGate_Cmd_GetLight_Info_Batch,	F_LampGate_CmdRspHdl_GetLight_Info_Batch},
	//{E_LampGate_Cmd_GetAmount, 				F_LampGate_CmdRspHdl_GetAmount},
	//{E_LampGate_Cmd_Query, 					F_LampGate_CmdRspHdl_Query},
	{E_LampGate_Cmd_Pair, 					F_LampGate_CmdRspHdl_Pair},
	{E_LampGate_Cmd_Remove, 				F_LampGate_CmdRspHdl_Remove},

	//	采用轮询方式, 此无效
	//{E_LampGate_Cmd_Change, 				F_LampGate_CmdRspHdl_Change},
	//{E_LampGate_Cmd_Change_LvCT, 		F_LampGate_CmdRspHdl_Change_LvCT},

	//------------------------------
	//	Protocol V2
	{E_LampGate_Cmd_GetInfo, 				F_LampGate_CmdRspHdl_GetInfo},
	{E_LampGate_Cmd_Notify, 				F_LampGate_CmdRspHdl_Notify},

};

static int R_iBatchSetLamp_Count;
static int R_iBatchSetLamp_ToBeRetry = D_FALSE;
static SLampGate_SetLampAttrib S_BatchSetLampAttrib_Back[D_LampGate_Lamp_BatchDeal_MaxNumber];
//static int R_iBatchSetLamp_Result;

//----------------------------------------
#ifdef	____D_UartRsp_HandleInThread____
D_DefineProcess(F_Proc_UALampGate_UartHdl);
#endif
D_DefineProcess(F_Proc_UALampGate);



//--------------------------------------------------------------
////	Macro

#ifdef	____D_LampGate_Dbg____
//#define	M_Dbg	atcmdprintf
#define	M_Dbg	MF_DbgPrint_F
#else
#define	M_Dbg(...)
#endif

//----------------------------------------
#define	M_FillData(_buf,_btVal,_pos)	\
	_buf[_pos] = _btVal; \
	_pos++;


#define	M_IsDevBusy()		\
	(R_iBatchSetLamp_Count || R_btLampNo_FromSrv || R_btLampNo_FromDev)



//--------------------------------------------------------------
////	变量



#endif


