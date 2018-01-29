#ifndef	____UACloud_H____
	   #define	____UACloud_H____


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

#include	"atcmdlib.h"

//#undef	____D_uRan_Macro_Dbg_En____

//#include	"uRan_Macro.h"
#include	"uRan_Macro_Contiki.h"
#include	"UA_Func.h"

#if (FUNC_OTA_ON == 1)
#include	"ota_api.h"
#endif

/*
 *add by lijing 0826 debug mark UACloudBuildMsg:
 */
 #ifdef ____D_LED_4PWM____

#include "debug_mark.h"

#endif

#include	"UACloud_Inc.h"
#include	"UAVersion.h"


//--------------------------------------------------------------
////	Option & Version
#ifdef	DEBUG_VERSION
#define	____D_DBg_UCCloud____


	#ifdef	____D_UACloud_Support_LocalConnect____
//#define	____D_Dbg_LocalCloud____
//#define	____D_UACloud_TCP_Queue_Dbg____
#define	____D_URSocket_Dbg____
	#endif

#endif

#define	____D_UACloudMsg_NewMethod____

//	#define	____D_TCP_Use_StringData____		//	tcp流数据处理



//--------------------------------------------------------------
////	Define
#ifdef	____D_UACloud_Protocol_CmdSerialID____
#define	D_CmdSerialIDStr_Length		4
#else
#define	D_CmdSerialIDStr_Length		0
#endif
#define	D_szUACloud_IPSev_URL		"cloudip.uascent.com"
#define	D_szUACloud_URL				"cloud.uascent.com"


#define	D_URSocket_Port_Dbg_HP	11011		//
#define	D_URSocket_Port_Dbg		11000		//

#define	D_UACloud_JumpIPPort	9999		//	重定服务器向端口
#define	D_UACloud_DebugPort		6666		//	调试服务器端口
#if (defined(____D_Project_UA402_CloudModule____) || defined(____D_Project_UA420_STYLE____))
#define	D_UACloud_Port			D_UACloud_DebugPort // 9898		//	运营服务器端口
#else
#define	D_UACloud_Port			9898		//	运营服务器端口
#endif
#define	D_UACloud_LocalPort		9899		//	本地监听端口
#define	D_UACloud_LocalUDPPort	19899		//	本地UDP监听端口, 用于局域网UDP查找设备
#define	D_UACloud_LocalSrvDebugPort		5000		//	局域网调试服务器端口

#define	D_iTCP_Send_MonitorTime			(5)	//	单位: S, 超时后socket重发数据
#define	D_iUACloudTCP_Send_Monitor_Timer_RetryTime		2



//--------------------------------------------------------------
////	结构体...定义
typedef enum _EUACloudAction{
	E_UACloudAction_Init = 0x0,
	E_UACloudAction_LogIn,
	E_UACloudAction_LogOut,
	E_UACloudAction_Notify,
	E_UACloudAction_Idle,

	E_UACloudAction_Connected,
	E_UACloudAction_Disconnect,
	E_UACloudAction_Sent,
	E_UACloudAction_Received,
	E_UACloudAction_Max,
}EUACloudAction;


//----------------------------------------
typedef enum _EUACloud_Cmd{
	E_UACloudCmd_None_CMD											= 0,
	E_UACloudCmd_LOGIN_CMD								/*"010"*/	= 10,			//	C->S | S->C
	E_UACloudCmd_LOGIN_RSP_CMD							/*"011"*/	,				//	S->C | C->S		//	当为局域网连接时, 返回2, SRV为1
	E_UACloudCmd_HEART_CMD								/*"014"*/	= 14,			// 心跳消息
	E_UACloudCmd_HEART_RSP_CMD							/*"015"*/	,				// 心跳消息回应
	E_UACloudCmd_NOTIFY_CMD								/*"016"*/	,				// 消息上报指令
	E_UACloudCmd_NOTIFY_RSP_CMD							/*"017"*/	,				// 消息上报指令回应
	E_UACloudCmd_SWITCH_CMD								/*"018"*/	,				// 开关命令
	E_UACloudCmd_SWITCH_RSP_CMD							/*"019"*/	,				// 开关命令回应
	E_UACloudCmd_GETSTATUS_CMD							/*"020"*/	,				// 获取状态 S->C
	E_UACloudCmd_GETSTATUS_RSP_CMD						/*"021"*/	,				// 获取状态返回 C->S
	E_UACloudCmd_PING_CMD								/*"022"*/	,				//  S->C
	E_UACloudCmd_PING_RSP_CMD							/*"023"*/	,				//  C->SC->S
	E_UACloudCmd_GetLightList_CMD						/*"024"*/	,				//  S->C
	E_UACloudCmd_GetLightList_RSP_CMD					/*"025"*/	,				//  C->S
	E_UACloudCmd_SetLight_CMD							/*"026"*/	,				//  S->C
	E_UACloudCmd_SetLight_RSP_CMD						/*"027"*/	,				//  C->S
	E_UACloudCmd_GetLightInfo_CMD						/*"028"*/	,				//  S->C
	E_UACloudCmd_GetLightInfo_RSP_CMD					/*"029"*/	,				//  C->S
	E_UACloudCmd_Pair_CMD								/*"030"*/	,				//  S->C
	E_UACloudCmd_Pair_RSP_CMD							/*"031"*/	,				//  C->S
	E_UACloudCmd_NotifyLightChange_CMD					/*"032"*/	,				//  C->S
	E_UACloudCmd_NotifyLightChange_RSP_CMD				/*"033"*/	,				//  S->C
	E_UACloudCmd_GetIP_CMD								/*"034"*/	,				//  C->S
	E_UACloudCmd_GetIP_RSP_CMD							/*"035"*/	,				//  S->C
	////	 GetIP_RSP_CMD Value = 12002421913009898, IP:PORT = 120.024.219.130:9898
	E_UACloudCmd_GetModuleInfo_CMD						/*"036"*/	,				//  S->C
	E_UACloudCmd_GetModuleInfo_RSP_CMD					/*"037"*/	,				//  C->S
	E_UACloudCmd_GetLEDLightInfo_CMD					/*"038"*/	,				//  S->C
	E_UACloudCmd_GetLEDLightInfo_RSP_CMD				/*"039"*/	,				//  C->S
	E_UACloudCmd_GetLEDLightAttrib_CMD					/*"040"*/	,				//  S->C
	E_UACloudCmd_GetLEDLightAttrib_RSP_CMD				/*"041"*/	,				//  C->S
	E_UACloudCmd_SetLEDLightAttrib_CMD					/*"042"*/	,				//  S->C
	E_UACloudCmd_SetLEDLightAttrib_RSP_CMD				/*"043"*/	,				//  C->S
	E_UACloudCmd_GetTimerTask_CMD						/*"044"*/	,				//  S->C
	E_UACloudCmd_GetTimerTask_RSP_CMD					/*"045"*/	,				//  C->S
	E_UACloudCmd_NotifyTimer_CMD						/*"046"*/	,				//  C->S
	E_UACloudCmd_NotifyTimer_RSP_CMD					/*"047"*/	,				//  S->C
	E_UACloudCmd_RemoveLight_CMD						/*"048"*/	,				//  S->C
	E_UACloudCmd_RemoveLight_RSP_CMD					/*"049"*/	,				//  C->S

	E_UACloudCmd_GetActTimerList_CMD					/*"050"*/	,				//  S->C
	E_UACloudCmd_GetActTimerList_RSP_CMD				/*"051"*/	,				//  C->S
	E_UACloudCmd_AddActTimer_CMD						/*"052"*/	,				//  S->C
	E_UACloudCmd_AddActTimer_RSP_CMD					/*"053"*/	,				//  C->S
	E_UACloudCmd_DelActTimer_CMD						/*"054"*/	,				//  S->C
	E_UACloudCmd_DelActTimer_RSP_CMD					/*"055"*/	,				//  C->S
	E_UACloudCmd_StopActTimer_CMD						/*"056"*/	,				//  S->C
	E_UACloudCmd_StopActTimer_RSP_CMD					/*"057"*/	,				//  C->S
	E_UACloudCmd_StartActTimer_CMD						/*"058"*/	,				//  S->C
	E_UACloudCmd_StartActTimer_RSP_CMD					/*"059"*/	,				//  C->S
	E_UACloudCmd_NOTIFY_ActTimerTimeout_CMD				/*"060"*/	,				//  C->S
	E_UACloudCmd_NOTIFY_ActTimerTimeout_RSP_CMD			/*"061"*/	,				//  S->C
	E_UACloudCmd_NOTIFY_ActTimerDelayTimeout_CMD		/*"062"*/	,				//  C->S
	E_UACloudCmd_NOTIFY_ActTimerDelayTimeout_RSP_CMD	/*"063"*/	,				//  S->C
	E_UACloudCmd_SetLight_Batch_CMD						/*"064"*/	,				//  S->C
	E_UACloudCmd_SetLight_Batch_RSP_CMD					/*"065"*/	,				//  C->S
	E_UACloudCmd_GetLightInfo_Batch_CMD					/*"066"*/	,				//  S->C
	E_UACloudCmd_GetLightInfo_Batch_RSP_CMD				/*"067"*/	,				//  C->S

	E_UACloudCmd_OTA_GetVer_CMD							/*"068"*/	,				//  S->C
	E_UACloudCmd_OTA_GetVer_RSP_CMD						/*"069"*/	,				//  C->S
	E_UACloudCmd_OTA_Start_CMD							/*"070"*/	,				//  S->C
	E_UACloudCmd_OTA_Start_RSP_CMD						/*"071"*/	,				//  C->S

	////	以下不做应答
	//	val format is hex string
	E_UACloudCmd_Remote2Uart_CMD						/*"100"*/	= 100,			//  S->C
	E_UACloudCmd_Uart2Remote_CMD						/*"101"*/	,				//  C->S






	E_UACloudCmd_Max,
}EUACloud_Cmd;



#define	D_IOLamp_PWM_MaxLv 		10
typedef struct _SIOLamp
{
	unsigned char btPin;		//	Pin
	unsigned char btMode;		//	O, PWM,
	unsigned char btOnOff;		//	0, 1
	unsigned char btOnOff_Reverse;	//	0, 1: 为1时, btOnOff为1则输出低; 为0时相反
	unsigned char btLevel;		//	0 ~ D_IOLamp_PWM_MaxLv
} SIOLamp;

//----------------------------------------
#define	D_UACloud_Msg_Header_Length		2
#define	D_UACloud_Msg_Token_Length		23
#define	D_UACloud_Msg_Cmd_Length		3
#define END_CHAR '$'
#define SEP_CHAR '@'
#define	D_iUACmd_Cmd_Index			4
#define	D_iUACmd_Token_Index		8
#define	D_iUACmd_IMEI_Index			32
#define	D_iUACmd_Value_Index		48
typedef struct _SUACloud_Msg
{
	char caHeader[D_UACloud_Msg_Header_Length];		//	"@QC"
	char cSep0;										//	"&"
	char caCmd[D_UACloud_Msg_Cmd_Length];
	char cSep1;
	char caToken[D_UACloud_Msg_Token_Length];
	char cSep2;
	char caIMEI[D_IMEI_Length];
	char cSep3;
	char cVal;									//	从这开始是可变长度, 确保此位置前字节结构固定
												//	由于cVal为可变长度的数组, 申请缓存时请结合D_UACloud_Msg_Val_MaxLength
	char cShap;										//	"#"
} SUACloud_Msg;

#define	D_UACloud_Msg_MaxLength			0x100		//	LAMPGATE need: 128-->256

#define	D_UACloud_Msg_Val_MaxLength		(D_UACloud_Msg_MaxLength - sizeof(SUACloud_Msg) + 1)

typedef struct _SUACloud_CmdValue
{
	#ifdef	____D_UACloud_Support_LocalConnect____
	EURSocketIndex	eSocket;
	TDWORD	dwTimeStamp;
	#endif

	char * pToken;
	//char * pCmd;
	EUACloud_Cmd eCmd;
	char * pValue;
} SUACloud_CmdValue;

#define	D_UACloud_UDPDiscoverPacket_Mask 		0x50445555
typedef struct _SUACloud_UDPDiscoverPacket
{
	TDWORD dwMask;
	TBYTE  caLen[4];
	TBYTE  caID[8];
	TBYTE  caVal[0x20];
} SUACloud_UDPDiscoverPacket;




//----------------------------------------
//	需要等待设备回应的cmd 用, 如Pair
//	返回值: 0, 成功; 0x10, 消息暂不处理, 等待下一次. other, 失败
#define	D_UACloud_CmdHdl_DonotDelMsg		0x10

typedef int (*PF_UACloud_CmdHdl)(STCPDataBuffer *p, SUACloud_CmdValue * psCmdVal, char * pBuf, int iBufSize);
typedef struct _SUACloud_CmdHdl
{
	//char * pCmd;
	EUACloud_Cmd eCmd;
	PF_UACloud_CmdHdl pfHdl;
} SUACloud_CmdHdl;

typedef int (*PF_UACloud_ProcRspHdl)(int iMsg, void * pPara);
typedef struct _SUACloud_ProcRspHdl
{
	//process_event_t ev;
	TBYTE btType;
	PF_UACloud_ProcRspHdl pfHdl;
} SUACloud_ProcRspHdl;


#ifdef	____D_UACloud_Support_LocalConnect____

#define	D_iTCPLocal_MonitorTime			(D_iTimer_HeartTime * 3 + 3)
//	局域网连接方法
typedef enum _EURSocketConnectMethod{
	E_URSocketConnectMethod_LockforTheFirst = 0,		//	保留上一个连接, 踢掉新的连接.
	E_URSocketConnectMethod_TheLast,					//	保留新的连接, 踢掉上一个连接.
	//E_URSocketConnectMethod_2Input,						//	最多可以2个连接

}EURSocketConnectMethod;


typedef enum _EURSocketStatus{
	E_URSocketStatus_Disconnect = 0,
	E_URSocketStatus_Listening,
	E_URSocketStatus_Connecting,
	E_URSocketStatus_Connected,

}EURSocketStatus;


typedef enum _EURSocketMode{
	E_URSocketMode_UnConfig = 0,
	E_URSocketMode_TCPListen,
	E_URSocketMode_TCPStopListen,
	E_URSocketMode_TCPClient,
	E_URSocketMode_UDP,

}EURSocketMode;

typedef struct _SURSockets
{
	EURSocketMode 		eMode;
	EURSocketStatus		eStatus;
	int 		bAutoReConnect;		//	自动重连
	NETSOCKET	iSocket;
	uip_ip4addr_t	sIP;
	TWORD			wPort;
	struct process * pCBProcess;
	void *		pQueue_TX;
	void *		pQueue_RX;
} SURSockets;

#endif


//--------------------------------------------------------------
////	函数声明
int F_URSocket_GetPort(EURSocketIndex eSocketIndex);
NETSOCKET F_URSocket_GetSocket(EURSocketIndex eSocketIndex);
int F_URSocket_IsConnected(EURSocketIndex eSocketIndex);
void F_URSocket_SetClient(EURSocketIndex eSocketIndex, NETSOCKET iSocket, EURSocketStatus eStatus);
void F_URSocket_Print(EURSocketIndex eSocketIndex);
void F_URSocket_PrintAll(void);


int F_UACloud_Cmd_Heart_RspHdl(STCPDataBuffer *p, SUACloud_CmdValue * psCmdVal, char * pBuf, int iBufSize);
int F_UACloud_Cmd_Switch_Rsp(STCPDataBuffer *p, SUACloud_CmdValue * psCmdVal, char * pBuf, int iBufSize);
int F_UACloud_Cmd_GetStatus_Rsp(STCPDataBuffer *p, SUACloud_CmdValue * psCmdVal, char * pBuf, int iBufSize);
int F_UACloud_Cmd_Ping_Rsp(STCPDataBuffer *p, SUACloud_CmdValue * psCmdVal, char * pBuf, int iBufSize);


#ifdef	____D_UA_LAMPGATE_SUPPORT____
int F_UACloud_Cmd_GetLightList(STCPDataBuffer *p, SUACloud_CmdValue * psCmdVal, char * pBuf, int iBufSize);
int F_UACloud_Cmd_SetLight(STCPDataBuffer *p, SUACloud_CmdValue * psCmdVal, char * pBuf, int iBufSize);
int F_UACloud_Cmd_SetLight_Batch(STCPDataBuffer *p, SUACloud_CmdValue * psCmdVal, char * pBuf, int iBufSize);
int F_UACloud_Cmd_GetLightInfo(STCPDataBuffer *p, SUACloud_CmdValue * psCmdVal, char * pBuf, int iBufSize);
int F_UACloud_Cmd_GetLightInfo_Batch(STCPDataBuffer *p, SUACloud_CmdValue * psCmdVal, char * pBuf, int iBufSize);
int F_UACloud_Cmd_Pair(STCPDataBuffer *p, SUACloud_CmdValue * psCmdVal, char * pBuf, int iBufSize);
int F_UACloud_Cmd_RemoveLight(STCPDataBuffer *p, SUACloud_CmdValue * psCmdVal, char * pBuf, int iBufSize);

#endif

#ifdef ____D_LED_4PWM____
int F_UACloud_Cmd_SetLEDLightAttrib(STCPDataBuffer * p, SUACloud_CmdValue * psCmdVal, char * pBuf, int iBufSize);
#endif

int F_UACloud_Cmd_GetTimerTask_Rsp(STCPDataBuffer * p, SUACloud_CmdValue * psCmdVal, char * pBuf, int iBufSize);
int F_UACloud_Cmd_GetIP_Rsp(STCPDataBuffer *p, SUACloud_CmdValue * psCmdVal, char * pBuf, int iBufSize);
int F_UACloud_Cmd_LogIn_Rsp(STCPDataBuffer *p, SUACloud_CmdValue * psCmdVal, char * pBuf, int iBufSize);
int F_UACloud_Cmd_LogIn_RspHdl(STCPDataBuffer *p, SUACloud_CmdValue * psCmdVal, char * pBuf, int iBufSize);

#ifdef	____D_CLOUDMODULE_SUPPORT____
int F_UACloud_Cmd_Remote2Uart_Rsp(STCPDataBuffer *p, SUACloud_CmdValue * psCmdVal, char * pBuf, int iBufSize);
#endif

#ifdef	____D_CLOUDMODULE_SUPPORT____
int F_UACloud_Cmd_GetActTimerList_Rsp(STCPDataBuffer *p, SUACloud_CmdValue * psCmdVal, char * pBuf, int iBufSize);
int F_UACloud_Cmd_AddActTimer_Rsp(STCPDataBuffer *p, SUACloud_CmdValue * psCmdVal, char * pBuf, int iBufSize);
int F_UACloud_Cmd_DelActTimer_Rsp(STCPDataBuffer *p, SUACloud_CmdValue * psCmdVal, char * pBuf, int iBufSize);
int F_UACloud_Cmd_StopActTimer_Rsp(STCPDataBuffer *p, SUACloud_CmdValue * psCmdVal, char * pBuf, int iBufSize);
int F_UACloud_Cmd_StartActTimer_Rsp(STCPDataBuffer *p, SUACloud_CmdValue * psCmdVal, char * pBuf, int iBufSize);
#endif

int F_UACloud_Cmd_OTA_GetVer_Rsp(STCPDataBuffer *p, SUACloud_CmdValue * psCmdVal, char * pBuf, int iBufSize);
#if (FUNC_OTA_ON == 1)
int F_UACloud_Cmd_OTA_Start_Rsp(STCPDataBuffer *p, SUACloud_CmdValue * psCmdVal, char * pBuf, int iBufSize);
#endif

//----------------------------------------
//----------------------------------------
int F_UACloud_Notify_IOLampState(int iMsg, void *pPara);

#ifdef	____D_UA_TIME_CLOCK_SUPPORT____
int F_UACloud_NotifyHdl_ActionTimer(int iMsg, void *pPara);
int F_UACloud_NotifyHdl_ActionTimer_ExternalSocket(int iMsg, void *pPara);
int F_UACloud_NotifyHdl_ActionTimer_ExternalSocket_Delay(int iMsg, void *pPara);

#endif

#ifdef	____D_CLOUDMODULE_SUPPORT____
int F_UACloud_NotifyHdl_UartUpLoad(int iMsg, void *pPara);
#endif

//----------------------------------------
//----------------------------------------
#ifdef	____D_UA_LAMPGATE_SUPPORT____
int F_UACloud_Cmd_SetLight_Rsp(int iMsg, void *pPara);
int F_UACloud_Cmd_SetLight_Batch_Rsp(int iMsg, void *pPara);
int F_UACloud_Cmd_GetLightInfo_Rsp(int iMsg, void *pPara);
int F_UACloud_Cmd_GetLightInfo_Batch_Rsp(int iMsg, void *pPara);
int F_UACloud_Cmd_Pair_Rsp(int iMsg, void *pPara);
int F_UACloud_NotifyLightChange(int iMsg, void *pPara);
int F_UACloud_Cmd_RemoveLight_Rsp(int iMsg, void *pPara);

#endif

//--------------------------------------------------------------
////	变量
process_event_t R_evUACloud;
process_event_t R_evUACloudNotify;
process_event_t R_evUACloudNotify2;

//----------------------------------------
D_DefineProcess(F_Proc_UACloudTCP);
D_DefineProcess(F_Proc_UACloudTCP_Send_Monitor);
D_DefineProcess(F_Proc_UACloudUDP_Discover);
D_DefineProcess(F_Proc_UACloudAction);

//----------------------------------------
static int R_iUACloudRunning = FALSE;
EUACloudConnectStatus R_eUACloudConnectState = E_UACloudConnectStatus_Disconnect;
static EUACloudAction R_eUACloud_Action = E_UACloudAction_Init;

static int R_iUACloudTCP_Send_Monitor_Timer_RetryTime;
static struct etimer R_sUACloudTCP_Send_Monitor_Timer;
static NETSOCKET R_sock_UACloud = D_Socket_Invalid;

static NETSOCKET R_sock_UACloud_UDP = D_Socket_Invalid;
static uip_ip4addr_t	R_sIP_UDP;		//	UDP 目的地址
static TWORD	R_wPort_peer;			//	UDP 目的主机的端口
#define	D_UACloud_UDP_Send_IntervalTime		100		//	unit: mS
#define	D_UACloud_UDP_Send_ReDoTime			10000	//	unit: mS, 重复发送多长时间

static int R_bUACloud_GotIPed = D_TRUE;////D_FALSE;
static SUACloud_Info * R_psUACloud_Info = D_NULL;
//	UACloud 重定向IP服务器地址
static SUACloud_Info S_UACloud_IPSrv_Info = {
	.sIP = {{120, 24, 219, 130}}, //{127, 0, 0, 1},
	.wPort = D_UACloud_JumpIPPort,
	.pURL = D_szUACloud_IPSev_URL
	};

//	Dbgport 由 R_bDebugModeInReleaseVer 来决定是否使用

static SUACloud_Info S_UACloud_Info = {
	.sIP = {{120, 24, 219, 130}}, //{127, 0, 0, 1},
#if 0 //(defined(__HTTP_SUPPORT__) && defined(DEBUG_VERSION))
	//	Not use  -- u 20160906
	.wPort = 8888,
#else
    #if defined(____D_LED_4PWM____) //(defined(____D_LED_4PWM____) || defined(DEBUG_VERSION))
	.wPort = D_UACloud_DebugPort,
	#else
    .wPort = D_UACloud_Port,
    #endif
#endif
	.pURL = D_szUACloud_URL
	};

static STCPDataBuffer S_bufTCP_TX = {
	.wSize = D_TCPDataBuffer_Size
	};
static STCPDataBuffer S_bufTCP_RX = {
	.wSize = D_TCPDataBuffer_Size
	};
static STCPDataBuffer S_bufUDP_TX = {
	.wSize = D_TCPDataBuffer_Size
	};
static STCPDataBuffer S_bufUDP_RX = {
	.wSize = D_TCPDataBuffer_Size
	};

#ifdef	____D_TCP_Use_StringData____
#define	D_TCP_RX_StringData_BufSize			(D_TCPDataBuffer_Size + D_UACloud_Msg_MaxLength * 2)
static int R_iTCP_RX_StringData_Count[E_URSocketIndex_Max];
static TBYTE R_bufTCP_RX_StringData[E_URSocketIndex_Max][D_TCP_RX_StringData_BufSize];		//	流数据处理
#endif

#ifdef	____D_UACloud_Protocol_CmdSerialID____
static TBYTE	R_btaCmdSerialID[D_CmdSerialIDStr_Length];
#endif


//----------------------------------------
#define	D_Timer_UACloud_IntervalTime	(CLOCK_SECOND * 2 / 2)
#define	D_Timer_UACloud_TimeOut			(CLOCK_SECOND * 5)

#define	D_iTimer_HeartTime				10	// 2	//	unit: S, 心跳包周期时间, 由Srv传递参数下来
#define	D_iTimer_HeartTime_Increase		100		//	unit: mS, 心跳包周期时间增量
static int R_iTimer_HeartTime	= D_iTimer_HeartTime; //120;
static clock_time_t R_tTimer_HeartTime = D_iTimer_HeartTime * CLOCK_SECOND + (D_iTimer_HeartTime_Increase * (CLOCK_SECOND / 1000));


static int R_bHoldToDealLastCmd = D_FALSE;		//	为 D_TRUE 上个cmd未处理完, 需要等待处理完成
static char R_szUACloud_Token[D_UACloud_Msg_Token_Length + 1] = "00000000000000000000000";
static char R_szUACloud_Token_Z[D_UACloud_Msg_Token_Length + 1] = "00000000000000000000000";

//----------------------------------------
#define	D_szUACmdFmtHeader		"QC@"
static char R_szUACmdFmt[] = "QC@999@00000000000000000000000@123456789012345@";
#define	D_UACloud_Msg_MinLength			(sizeof(R_szUACmdFmt))	//	要算上最后的 '$'

//----------------------------------------
const char	D_szUACloud_Cmd_ClientHeader			[]="QC";
/*
const char	D_szUACloud_Cmd_LOGIN_CMD					[D_UACloud_Msg_Cmd_Length+1]="010";
const char	D_szUACloud_Cmd_LOGIN_RSP_CMD			[D_UACloud_Msg_Cmd_Length+1]="011";
const char	D_szUACloud_Cmd_HEART_CMD				[D_UACloud_Msg_Cmd_Length+1]="014";				// 心跳消息
const char	D_szUACloud_Cmd_HEART_RSP_CMD			[D_UACloud_Msg_Cmd_Length+1]="015";				// 心跳消息回应
const char	D_szUACloud_Cmd_NOTIFY_CMD				[D_UACloud_Msg_Cmd_Length+1]="016";				// 消息上报指令
const char	D_szUACloud_Cmd_NOTIFY_RSP_CMD			[D_UACloud_Msg_Cmd_Length+1]="017";				// 消息上报指令回应
const char	D_szUACloud_Cmd_SWITCH_CMD				[D_UACloud_Msg_Cmd_Length+1]="018";				// 开关命令
const char	D_szUACloud_Cmd_SWITCH_RSP_CMD			[D_UACloud_Msg_Cmd_Length+1]="019";				// 开关命令回应
const char	D_szUACloud_Cmd_GETSTATUS_CMD			[D_UACloud_Msg_Cmd_Length+1]="020";				// 获取状态 S->C
const char	D_szUACloud_Cmd_GETSTATUS_RSP_CMD		[D_UACloud_Msg_Cmd_Length+1]="021";				// 获取状态返回 C->S
const char	D_szUACloud_Cmd_PING_CMD				[D_UACloud_Msg_Cmd_Length+1]="022";				//  S->C
const char	D_szUACloud_Cmd_PING_RSP_CMD			[D_UACloud_Msg_Cmd_Length+1]="023";				//  C->SC->S
const char	D_szUACloud_Cmd_GetLightList_CMD		[D_UACloud_Msg_Cmd_Length+1]="024";				//  S->C
const char	D_szUACloud_Cmd_GetLightList_RSP_CMD	[D_UACloud_Msg_Cmd_Length+1]="025";				//  C->S
const char	D_szUACloud_Cmd_SetLight_CMD			[D_UACloud_Msg_Cmd_Length+1]="026";				//  S->C
const char	D_szUACloud_Cmd_SetLight_RSP_CMD		[D_UACloud_Msg_Cmd_Length+1]="027";				//  C->S
const char	D_szUACloud_Cmd_GetLightInfo_CMD		[D_UACloud_Msg_Cmd_Length+1]="028";				//  S->C
const char	D_szUACloud_Cmd_GetLightInfo_RSP_CMD	[D_UACloud_Msg_Cmd_Length+1]="029";				//  C->S
const char	D_szUACloud_Cmd_Pair_CMD				[D_UACloud_Msg_Cmd_Length+1]="030";				//  S->C
const char	D_szUACloud_Cmd_Pair_RSP_CMD			[D_UACloud_Msg_Cmd_Length+1]="031";				//  C->S
const char	D_szUACloud_Cmd_NotifyLightChange_CMD	[D_UACloud_Msg_Cmd_Length+1]="032";				//  C->S
const char	D_szUACloud_Cmd_NotifyLightChange_RSP_CMD	[D_UACloud_Msg_Cmd_Length+1]="033";				//  S->C
const char	D_szUACloud_Cmd_GetIP_CMD				[D_UACloud_Msg_Cmd_Length+1]="034";				//  C->S
const char	D_szUACloud_Cmd_GetIP_RSP_CMD			[D_UACloud_Msg_Cmd_Length+1]="035";				//  S->C
////	 GetIP_RSP_CMD Value = 12002421913009898, IP:PORT = 120.024.219.130:9898
const char	D_szUACloud_Cmd_GetModuleInfo_CMD				[D_UACloud_Msg_Cmd_Length+1]="036";				//  S->C
const char	D_szUACloud_Cmd_GetModuleInfo_RSP_CMD			[D_UACloud_Msg_Cmd_Length+1]="037";				//  C->S
const char	D_szUACloud_Cmd_GetLEDLightInfo_CMD				[D_UACloud_Msg_Cmd_Length+1]="038";				//  S->C
const char	D_szUACloud_Cmd_GetLEDLightInfo_RSP_CMD			[D_UACloud_Msg_Cmd_Length+1]="039";				//  C->S
const char	D_szUACloud_Cmd_GetLEDLightAttrib_CMD			[D_UACloud_Msg_Cmd_Length+1]="040";				//  S->C
const char	D_szUACloud_Cmd_GetLEDLightAttrib_RSP_CMD		[D_UACloud_Msg_Cmd_Length+1]="041";				//  C->S
const char	D_szUACloud_Cmd_SetLEDLightAttrib_CMD			[D_UACloud_Msg_Cmd_Length+1]="042";				//  S->C
const char	D_szUACloud_Cmd_SetLEDLightAttrib_RSP_CMD		[D_UACloud_Msg_Cmd_Length+1]="043";				//  C->S
const char	D_szUACloud_Cmd_GetTimerTask_CMD				[D_UACloud_Msg_Cmd_Length+1]="044";				//  S->C
const char	D_szUACloud_Cmd_GetTimerTask_RSP_CMD			[D_UACloud_Msg_Cmd_Length+1]="045";				//  C->S
const char	D_szUACloud_Cmd_NotifyTimer_CMD					[D_UACloud_Msg_Cmd_Length+1]="046";				//  C->S
const char	D_szUACloud_Cmd_NotifyTimer_RSP_CMD				[D_UACloud_Msg_Cmd_Length+1]="047";				//  S->C
const char	D_szUACloud_Cmd_RemoveLight_CMD					[D_UACloud_Msg_Cmd_Length+1]="048";				//  S->C
const char	D_szUACloud_Cmd_RemoveLight_RSP_CMD				[D_UACloud_Msg_Cmd_Length+1]="049";				//  C->S

const char	D_szUACloud_Cmd_GetActTimerList_CMD				[D_UACloud_Msg_Cmd_Length+1]="050";				//  S->C
const char	D_szUACloud_Cmd_GetActTimerList_RSP_CMD			[D_UACloud_Msg_Cmd_Length+1]="051";				//  C->S
const char	D_szUACloud_Cmd_AddActTimer_CMD				[D_UACloud_Msg_Cmd_Length+1]="052";				//  S->C
const char	D_szUACloud_Cmd_AddActTimer_RSP_CMD			[D_UACloud_Msg_Cmd_Length+1]="053";				//  C->S
const char	D_szUACloud_Cmd_DelActTimer_CMD				[D_UACloud_Msg_Cmd_Length+1]="054";				//  S->C
const char	D_szUACloud_Cmd_DelActTimer_RSP_CMD			[D_UACloud_Msg_Cmd_Length+1]="055";				//  C->S
const char	D_szUACloud_Cmd_StopActTimer_CMD				[D_UACloud_Msg_Cmd_Length+1]="056";				//  S->C
const char	D_szUACloud_Cmd_StopActTimer_RSP_CMD			[D_UACloud_Msg_Cmd_Length+1]="057";				//  C->S
const char	D_szUACloud_Cmd_StartActTimer_CMD				[D_UACloud_Msg_Cmd_Length+1]="058";				//  S->C
const char	D_szUACloud_Cmd_StartActTimer_RSP_CMD			[D_UACloud_Msg_Cmd_Length+1]="059";				//  C->S
const char	D_szUACloud_Cmd_NOTIFY_ActTimerTimeout_CMD			[D_UACloud_Msg_Cmd_Length+1]="060";				//  C->S
const char	D_szUACloud_Cmd_NOTIFY_ActTimerTimeout_RSP_CMD		[D_UACloud_Msg_Cmd_Length+1]="061";				//  S->C
const char	D_szUACloud_Cmd_NOTIFY_ActTimerDelayTimeout_CMD		[D_UACloud_Msg_Cmd_Length+1]="062";				//  C->S
const char	D_szUACloud_Cmd_NOTIFY_ActTimerDelayTimeout_RSP_CMD	[D_UACloud_Msg_Cmd_Length+1]="063";				//  S->C
const char	D_szUACloud_Cmd_SetLight_Batch_CMD			[D_UACloud_Msg_Cmd_Length+1]="064";				//  S->C
const char	D_szUACloud_Cmd_SetLight_Batch_RSP_CMD		[D_UACloud_Msg_Cmd_Length+1]="065";				//  C->S
const char	D_szUACloud_Cmd_GetLightInfo_Batch_CMD			[D_UACloud_Msg_Cmd_Length+1]="066";				//  S->C
const char	D_szUACloud_Cmd_GetLightInfo_Batch_RSP_CMD		[D_UACloud_Msg_Cmd_Length+1]="067";				//  C->S

const char	D_szUACloud_Cmd_OTA_GetVer_CMD			[D_UACloud_Msg_Cmd_Length+1]="068";				//  S->C
const char	D_szUACloud_Cmd_OTA_GetVer_RSP_CMD		[D_UACloud_Msg_Cmd_Length+1]="069";				//  C->S
const char	D_szUACloud_Cmd_OTA_Start_CMD			[D_UACloud_Msg_Cmd_Length+1]="070";				//  S->C
const char	D_szUACloud_Cmd_OTA_Start_RSP_CMD		[D_UACloud_Msg_Cmd_Length+1]="071";				//  C->S



////	以下不做应答
//	val format is hex string
const char	D_szUACloud_Cmd_Remote2Uart_CMD					[D_UACloud_Msg_Cmd_Length+1]="100";				//  S->C
const char	D_szUACloud_Cmd_Uart2Remote_CMD					[D_UACloud_Msg_Cmd_Length+1]="101";				//  C->S
*/

#ifdef	____D_UA_TIME_CLOCK_SUPPORT____
int R_bHaveSyncSrvTime = D_FALSE;

#endif




//----------------------------------------
#ifdef	____D_UACloud_Support_LocalConnect____
D_DefineProcess(F_Proc_UACloudTCP_Local);
D_DefineProcess(F_Proc_UACloudTCP_Local_Monitor);
static EURSocketIndex	R_eUACloud_LastCmd_SocketIndex = E_URSocketIndex_Invalid;// E_URSocketIndex_UACloud;
static struct etimer R_sUACloudTCP_Local_Monitor_Timer;


static SURSockets	R_saSockets[E_URSocketIndex_Max] = {
	//	E_URSocketIndex_UACloud
	{
		.eMode = E_URSocketMode_TCPClient,
		.eStatus = E_URSocketStatus_Disconnect,
		.bAutoReConnect = D_TRUE,
		.iSocket = D_Socket_Invalid,
		.pCBProcess = &F_Proc_UACloudTCP,

	},
	//	E_URSocketIndex_UACloud_Local
	{
		.eMode = E_URSocketMode_TCPListen,
		.eStatus = E_URSocketStatus_Disconnect,
		.bAutoReConnect = D_FALSE,
		.iSocket = D_Socket_Invalid,
		//.wPort = 9899,
		.pCBProcess = &F_Proc_UACloudTCP_Local,

	},

};

typedef struct _SSocketData
{
	//TBYTE		bValid;						//	当前数据是否有效
	TWORD		wLen;						//	当前要发送数据的长度
	#ifdef	DEBUG_VERSION
	TDWORD		dwTimeStamp;						//	时间戳
	#endif

	//SocketData * pNext;						//	下一个SocketData
	TWORD		wSerialID;					//	命令序列, 调试用
	//TBYTE *		pBuf;						//	要发送的数据
	TBYTE		btSocketIndex;				//	所用socket在数组中的位置
	TBYTE		Buf[D_UACloud_Msg_MaxLength];

} SSocketData;





#define	D_TCP_RX_Buf_DeepLv		32
static TDWORD R_dwTCPRecv_SerialID = 0;

static int R_TCP_RX_BufIndex = -1;
static int R_TCP_RX_BufIndex_R = 0;
static SSocketData R_saSocketRXDataQueue[D_TCP_RX_Buf_DeepLv];

//	用于dbg识别
static TDWORD R_dwTCPSend_SerialID = 0;
static TDWORD R_dwTCPSend_SerialID_Doing[E_URSocketIndex_Max];

//	附加该值到cmd msg中, 用于区分多网段时不同顺序的命令
static TWORD R_wTCPSendCmd_SerialID = 0;

#define	D_TCP_TX_Buf_DeepLv		32
static int R_TCP_TX_BufIndex[E_URSocketIndex_Max];
static int R_TCP_TX_BufIndex_R[E_URSocketIndex_Max];
static SSocketData R_saSocketTXDataQueue[E_URSocketIndex_Max][D_TCP_TX_Buf_DeepLv];

static EURSocketConnectMethod R_eLocalConnectMethod = E_URSocketConnectMethod_LockforTheFirst;


#else
static TDWORD R_dwTCPSend_SerialID = 0;
static TDWORD R_dwTCPSend_SerialID_Doing = 0;

#define	D_TCP_TX_Buf_DeepLv		16
static int R_TCP_TX_BufIndex = -1;
static int R_TCP_TX_BufIndex_R = 0;
TBYTE	R_TCP_TX_BufCount[D_TCP_TX_Buf_DeepLv][2];		//count
TBYTE 	R_buf_TXBuf[D_TCP_TX_Buf_DeepLv][256];
#endif

/*
 *  add by lijing 0829
 */
#ifdef ____D_LED_4PWM____
static const TBYTE RGB_level_map[] = { 0, 1, 25, 50, 76, 101, 127, 151, 178, 203, 229, 256};//{ 0, 1, 25, 50, 76, 101, 127, 151, 178, 203, 229, 256};//L10
#endif
/*end of add*/


#if defined(____D_Project_UA402_CloudModule____)
static SIOLamp S_Lamp[] = {
	{GPIO_2, 0, 0, 1, 0},
};
#else
static SIOLamp S_Lamp[] = {
#ifdef ____D_Project_UA402_WIFI_GN____
	#ifdef ____D_LED_4PWM____
	{GPIO_20, 0, 0, 0, 0}
	#else
	{GPIO_2, 0, 0, 0, 0}
	#endif
#elif defined(____D_Project_UA420_STYLE____)
	{GPIO_18, 0, 0, 1, 0},
#else
	{GPIO_8, 0, 0, 1, 0},
	#ifdef	____D_UA_POWERJACK_DEMO_SUPPORT____
	{GPIO_1, 0, 0, 1, 0},
	#else
	{GPIO_2, 0, 0, 1, 0},
	#endif

	{GPIO_3, 0, 0, 1, 0},
	#ifndef	____D_Module_IO_Indicator____
	{GPIO_15, 0, 0, 1, 0},
	{GPIO_18, 0, 0, 1, 0}
	#endif
#endif
};
#endif


static const char R_caDec2Ascii[] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
};



//--------------------------------------------------------------
////	Macro


//----------------------------------------







//--------------------------------------------------------------
////	变量

//----------------------------------------
//----------------------------------------
//	响应SRV传下来的CMD的处理函数
static const SUACloud_CmdHdl S_aUACloud_CmdHdl[] = {
	{E_UACloudCmd_SWITCH_CMD, F_UACloud_Cmd_Switch_Rsp},
	{E_UACloudCmd_GETSTATUS_CMD, F_UACloud_Cmd_GetStatus_Rsp},
	{E_UACloudCmd_HEART_RSP_CMD, F_UACloud_Cmd_Heart_RspHdl},

#ifdef	____D_UA_LAMPGATE_SUPPORT____
	{E_UACloudCmd_GetLightList_CMD, F_UACloud_Cmd_GetLightList},
	{E_UACloudCmd_SetLight_CMD, F_UACloud_Cmd_SetLight},
	{E_UACloudCmd_SetLight_Batch_CMD, F_UACloud_Cmd_SetLight_Batch},
	{E_UACloudCmd_GetLightInfo_CMD, F_UACloud_Cmd_GetLightInfo},
	{E_UACloudCmd_GetLightInfo_Batch_CMD, F_UACloud_Cmd_GetLightInfo_Batch},
	{E_UACloudCmd_Pair_CMD, F_UACloud_Cmd_Pair},
	//{E_UACloudCmd_NotifyLightChange_RSP_CMD, F_UACloud_Cmd_NotifyLightChange_Rsp},
	{E_UACloudCmd_RemoveLight_CMD, F_UACloud_Cmd_RemoveLight},
#endif

	{E_UACloudCmd_PING_CMD, F_UACloud_Cmd_Ping_Rsp},
	{E_UACloudCmd_GetTimerTask_CMD, F_UACloud_Cmd_GetTimerTask_Rsp},

#ifdef	____D_CLOUDMODULE_SUPPORT____
	{E_UACloudCmd_Remote2Uart_CMD, F_UACloud_Cmd_Remote2Uart_Rsp},
#endif


#ifdef ____D_LED_4PWM____
    {E_UACloudCmd_SetLEDLightAttrib_CMD, F_UACloud_Cmd_SetLEDLightAttrib},
#endif

#ifdef	____D_CLOUDMODULE_SUPPORT____
	{E_UACloudCmd_GetActTimerList_CMD, F_UACloud_Cmd_GetActTimerList_Rsp},
	{E_UACloudCmd_AddActTimer_CMD, F_UACloud_Cmd_AddActTimer_Rsp},
	{E_UACloudCmd_DelActTimer_CMD, F_UACloud_Cmd_DelActTimer_Rsp},
	{E_UACloudCmd_StopActTimer_CMD, F_UACloud_Cmd_StopActTimer_Rsp},
	{E_UACloudCmd_StartActTimer_CMD, F_UACloud_Cmd_StartActTimer_Rsp},
#endif


	//--------------------------------
	//	不频繁的命令
	{E_UACloudCmd_GetIP_CMD, F_UACloud_Cmd_GetIP_Rsp},
	{E_UACloudCmd_LOGIN_CMD, F_UACloud_Cmd_LogIn_Rsp},
	{E_UACloudCmd_LOGIN_RSP_CMD, F_UACloud_Cmd_LogIn_RspHdl},
	{E_UACloudCmd_OTA_GetVer_CMD, F_UACloud_Cmd_OTA_GetVer_Rsp},
#if (FUNC_OTA_ON == 1)
	{E_UACloudCmd_OTA_Start_CMD, F_UACloud_Cmd_OTA_Start_Rsp},
#endif

	//--------------------------------
	//{0, NULL}//{NULL, NULL}
};

//----------------------------------------
//	响应其他线程发过来的notify 类型的处理函数, 事件是R_evUACloudNotify2
static const SUACloud_ProcRspHdl S_aUACloud_NotifyHdl[] = {
	#ifdef	____D_UA_TIME_CLOCK_SUPPORT____
	{E_UACloud_NotifyType_ActionTimer, F_UACloud_NotifyHdl_ActionTimer},
	{E_UACloud_NotifyType_ActionTimer_ExternalSocket, F_UACloud_NotifyHdl_ActionTimer_ExternalSocket},
	{E_UACloud_NotifyType_ActionTimer_ExternalSocket_Delay, F_UACloud_NotifyHdl_ActionTimer_ExternalSocket_Delay},
	#endif

	#ifdef	____D_CLOUDMODULE_SUPPORT____
	{E_UACloud_NotifyType_UartUpLoad, F_UACloud_NotifyHdl_UartUpLoad},
	#endif



	//-----------------------------------
	//	Add Above this
	{E_UACloud_NotifyType_None, NULL}	//{NULL, NULL}
};

#ifdef	____D_UA_LAMPGATE_SUPPORT____
//----------------------------------------
static const SUACloud_ProcRspHdl S_aUACloud_LampGate_ProcRspHdl[] = {
	{E_LampGate_Step_Set, F_UACloud_Cmd_SetLight_Rsp},
	{E_LampGate_Step_Set_LvCT_Batch, F_UACloud_Cmd_SetLight_Batch_Rsp},
	{E_LampGate_Step_GetInfo, F_UACloud_Cmd_GetLightInfo_Rsp},
	{E_LampGate_Step_GetInfo_Batch, F_UACloud_Cmd_GetLightInfo_Batch_Rsp},
	{E_LampGate_Step_Pair, F_UACloud_Cmd_Pair_Rsp},
	//{E_LampGate_Step_Change_LvCT, F_UACloud_NotifyLightChange},
	{E_LampGate_Step_Remove, F_UACloud_Cmd_RemoveLight_Rsp},
	{E_LampGate_Step_NotifyChange, F_UACloud_NotifyLightChange},

	//{NULL, NULL}
};

#endif



#endif


