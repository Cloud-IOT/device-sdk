#ifndef	____UACloudModule_H____
	   #define	____UACloudModule_H____


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
#include	"UAVersion.h"

#include	"UACloudModule_Inc.h"


//--------------------------------------------------------------
////	Option & Version
#ifdef	DEBUG_VERSION
#define	____D_CloudModule_Dbg____
#define	____D_CloudModule_UART_Dbg____
#endif

#define	____D_Dbg_Test_NoCheckSum____



//--------------------------------------------------------------
////	Define
#define	D_CloudModule_CmdDataSize			16
#define	D_CloudModule_DataSize				64
#define	D_CloudModule_PacketSize_Min		(5)		//	最小长度时, data部分长度为0
#define	D_CloudModule_PacketSize_Max		(D_CloudModule_DataSize + D_CloudModule_PacketSize_Min)

#define	D_CloudModule_UARTPacket_Header		0xAA
#define	D_CloudModule_UARTPacket_Ender		0xBB


//--------------------------------------------------------------
////	结构体...定义


typedef struct _SCloudModuleUARTDataBuf
{
	TBYTE	btCount;
	TBYTE	btSize;
	TBYTE 	Buf[D_CloudModule_UARTDataBuffer_Size];
} SCloudModuleUARTDataBuf;


typedef enum _ECloudModule_PacketType{
	E_CloudModule_PacketType_Module = 0,
	E_CloudModule_PacketType_UpLoad,
	E_CloudModule_PacketType_DownLoad,

	E_CloudModule_PacketType_Max
} ECloudModule_PacketType;

typedef enum _ECloudModule_Cmd{
	E_CloudModule_Cmd_Reboot				= 0x00,
	E_CloudModule_Cmd_GetChkMode,		//	此不检查CHK
	E_CloudModule_Cmd_SetChkMode,
	E_CloudModule_Cmd_SaveCfg,
	E_CloudModule_Cmd_RestoreCfg,		//	恢复默认值
	E_CloudModule_Cmd_GetID,
	E_CloudModule_Cmd_GetIMEI,
	E_CloudModule_Cmd_GetVersion,
	E_CloudModule_Cmd_GetMAC,
	E_CloudModule_Cmd_GetState,
	E_CloudModule_Cmd_GetIP,
	E_CloudModule_Cmd_SetIP,
	E_CloudModule_Cmd_GetDHCP,
	E_CloudModule_Cmd_SetDHCP,
	E_CloudModule_Cmd_GetIfConfig,
	E_CloudModule_Cmd_SetIfConfig,
	E_CloudModule_Cmd_GetOprationMode,
	E_CloudModule_Cmd_SetOprationMode,
	E_CloudModule_Cmd_GetSLinkMode,
	E_CloudModule_Cmd_SetSLinkMode,
	E_CloudModule_Cmd_GetAutoConnect,
	E_CloudModule_Cmd_SetAutoConnect,

	E_CloudModule_Cmd_GetNotifyType,
	E_CloudModule_Cmd_SetNotifyType,
	E_CloudModule_Cmd_GetTimerAction,
	E_CloudModule_Cmd_SetTimerAction,

	E_CloudModule_Cmd_SetWifiConnect,
	E_CloudModule_Cmd_GetTime,

	E_CloudModule_Cmd_Max,


	E_CloudModule_Cmd_Notify_Ready			= 0x70,			//	初始化完成通知
	E_CloudModule_Cmd_Notify_ConnState,		//	连接状态改变通知
	E_CloudModule_Cmd_Notify_Timer,			//	定时通知
	E_CloudModule_Cmd_Notify_Delay,			//	延时通知
	E_CloudModule_Cmd_Notify_Key,			//	延时通知

	E_CloudModule_Cmd_All_Max,

	E_CloudModule_Cmd_Err = 0xFF,
} ECloudModule_Cmd;


typedef enum _ECloudModule_ProtocolChkMode{
	E_CloudModule_ProtocolChkMode_None = 0,
	E_CloudModule_ProtocolChkMode_XOR,
	E_CloudModule_ProtocolChkMode_CRC8,
	E_CloudModule_ProtocolChkMode_Max

} ECloudModule_ProtocolChkMode;


#define	E_CloudModule_NotifyEn_None				0x00
#define	E_CloudModule_NotifyEn_Ready			0x01
#define	E_CloudModule_NotifyEn_ConnState		0x02
#define	E_CloudModule_NotifyEn_Timer			0x04
#define	E_CloudModule_NotifyEn_Delay			0x08
#define	E_CloudModule_NotifyEn_Key				0x10
#define	E_CloudModule_NotifyEn_Mask			(E_CloudModule_NotifyEn_Ready | E_CloudModule_NotifyEn_ConnState \
	| E_CloudModule_NotifyEn_Timer | E_CloudModule_NotifyEn_Delay | E_CloudModule_NotifyEn_Key)


//	返回: 0, 成功;
typedef ERetErr (*PF_CloudModule_CmdHdl)(TBYTE * pVal, TBYTE btLen);
typedef struct _SCloudModule_CmdHdl
{
	ECloudModule_Cmd eCmd;
	PF_CloudModule_CmdHdl pfHdl;
} SCloudModule_CmdHdl;


#define	D_CloudModule_CfgSave_Mask		0x5541434D		//	'UACM'
#define	D_CloudModule_CfgSave_Mask1		0x5541436D

//--------------------------------------------------------------
////	函数声明
ERetErr F_CloudModule_CmdHdl_Reboot(TBYTE * pVal, TBYTE btLen);
ERetErr F_CloudModule_CmdHdl_GetChkMode(TBYTE * pVal, TBYTE btLen);
ERetErr F_CloudModule_CmdHdl_SetChkMode(TBYTE * pVal, TBYTE btLen);
ERetErr F_CloudModule_CmdHdl_SaveCfg(TBYTE * pVal, TBYTE btLen);
ERetErr F_CloudModule_CmdHdl_RestoreCfg(TBYTE * pVal, TBYTE btLen);
ERetErr F_CloudModule_CmdHdl_GetState(TBYTE * pVal, TBYTE btLen);
ERetErr F_CloudModule_CmdHdl_GetID(TBYTE * pVal, TBYTE btLen);
ERetErr F_CloudModule_CmdHdl_GetVersion(TBYTE * pVal, TBYTE btLen);
ERetErr F_CloudModule_CmdHdl_GetIMEI(TBYTE * pVal, TBYTE btLen);
ERetErr F_CloudModule_CmdHdl_GetIP(TBYTE * pVal, TBYTE btLen);
ERetErr F_CloudModule_CmdHdl_GetMAC(TBYTE * pVal, TBYTE btLen);

ERetErr F_CloudModule_CmdHdl_GetSLinkMode(TBYTE * pVal, TBYTE btLen);
ERetErr F_CloudModule_CmdHdl_SetSLinkMode(TBYTE * pVal, TBYTE btLen);
ERetErr F_CloudModule_CmdHdl_GetAutoConnect(TBYTE * pVal, TBYTE btLen);
ERetErr F_CloudModule_CmdHdl_SetAutoConnect(TBYTE * pVal, TBYTE btLen);

ERetErr F_CloudModule_CmdHdl_GetNotifyType(TBYTE * pVal, TBYTE btLen);
ERetErr F_CloudModule_CmdHdl_SetNotifyType(TBYTE * pVal, TBYTE btLen);
ERetErr F_CloudModule_CmdHdl_GetTimerAction(TBYTE * pVal, TBYTE btLen);
ERetErr F_CloudModule_CmdHdl_SetTimerAction(TBYTE * pVal, TBYTE btLen);
ERetErr F_CloudModule_CmdHdl_GetTime(TBYTE * pVal, TBYTE btLen);

//--------------------------------------------------------------
////	变量
int R_bCloudModule_KeyToSmartLink = D_FALSE;

process_event_t R_evCloudModule_Uart;
D_DefineProcess(F_Proc_UACloudModule_UartHdl);

static SCloudModuleUARTDataBuf S_UARTData_TX;
static SCloudModuleUARTDataBuf S_UARTData_RX;
static SCloudModuleUARTDataBuf S_UARTData_RX_Valid;

TBYTE R_bufCloudModule_Data[D_CloudModule_UARTDataBuffer_Size];


static const SCloudModule_CmdHdl S_aCloudModule_CmdHdlTbl[] = {
	{E_CloudModule_Cmd_Reboot,				F_CloudModule_CmdHdl_Reboot},
	{E_CloudModule_Cmd_GetChkMode,			F_CloudModule_CmdHdl_GetChkMode},
	{E_CloudModule_Cmd_SetChkMode,			F_CloudModule_CmdHdl_SetChkMode},
	{E_CloudModule_Cmd_SaveCfg,				F_CloudModule_CmdHdl_SaveCfg},
	{E_CloudModule_Cmd_RestoreCfg,			F_CloudModule_CmdHdl_RestoreCfg},
	{E_CloudModule_Cmd_GetID, 				F_CloudModule_CmdHdl_GetID},
	{E_CloudModule_Cmd_GetIMEI, 			F_CloudModule_CmdHdl_GetIMEI},
	{E_CloudModule_Cmd_GetVersion, 			F_CloudModule_CmdHdl_GetVersion},
	{E_CloudModule_Cmd_GetMAC, 				F_CloudModule_CmdHdl_GetMAC},
	{E_CloudModule_Cmd_GetState, 			F_CloudModule_CmdHdl_GetState},
	{E_CloudModule_Cmd_GetIP, 				F_CloudModule_CmdHdl_GetIP},

	{E_CloudModule_Cmd_GetSLinkMode,		F_CloudModule_CmdHdl_GetSLinkMode},
	{E_CloudModule_Cmd_SetSLinkMode,		F_CloudModule_CmdHdl_SetSLinkMode},
	{E_CloudModule_Cmd_GetAutoConnect,		F_CloudModule_CmdHdl_GetAutoConnect},
	{E_CloudModule_Cmd_SetAutoConnect,		F_CloudModule_CmdHdl_SetAutoConnect},


	{E_CloudModule_Cmd_GetNotifyType, 		F_CloudModule_CmdHdl_GetNotifyType},
	{E_CloudModule_Cmd_SetNotifyType, 		F_CloudModule_CmdHdl_SetNotifyType},
	{E_CloudModule_Cmd_GetTimerAction, 		F_CloudModule_CmdHdl_GetTimerAction},
	{E_CloudModule_Cmd_SetTimerAction, 		F_CloudModule_CmdHdl_SetTimerAction},
	{E_CloudModule_Cmd_GetTime, 			F_CloudModule_CmdHdl_GetTime},


};

static SCloudModule_Cfg S_CloudModule_Cfg;

//--------------------------------------------------------------
////	Macro


//----------------------------------------






//--------------------------------------------------------------
////	变量



#endif


