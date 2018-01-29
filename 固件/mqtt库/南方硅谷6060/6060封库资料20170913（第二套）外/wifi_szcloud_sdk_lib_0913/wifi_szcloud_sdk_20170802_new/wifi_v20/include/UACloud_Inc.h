#ifndef	____UACloud_Inc_H____
	   #define	____UACloud_Inc_H____


//--------------------------------------------------------------
////	用法
/*

*/



//--------------------------------------------------------------
////	头文件
////	外部引用时需要使用的
#include	"uRan_Macro_Contiki.h"

#include "net/uip.h"


//--------------------------------------------------------------
////	Define




//--------------------------------------------------------------
////	结构体...定义

//----------------------------------------
//	连接服务器的状态
typedef enum _EUACloudConnectStatus{
	E_UACloudConnectStatus_Disconnect = 0x0,
	E_UACloudConnectStatus_Connected,
	E_UACloudConnectStatus_Login_Ing,
	E_UACloudConnectStatus_Login_Err,
	E_UACloudConnectStatus_Logined,
	E_UACloudConnectStatus_Logout_Ing,
	E_UACloudConnectStatus_Logouted,
}EUACloudConnectStatus;


//
typedef enum _EUACloud_NotifyType{
	E_UACloud_NotifyType_None = 0x00,
	E_UACloud_NotifyType_Sync = 0x01,
	////
	E_UACloud_NotifyType_Point2RAM = 0x02,		//	RAM指针
	E_UACloud_NotifyType_Point2ROM = 0x03,		//	ROM指针, 指向FLASH段
	////
	E_UACloud_NotifyType_Key,

	#ifdef	____D_UA_TIME_CLOCK_SUPPORT____
	E_UACloud_NotifyType_ActionTimer,
	E_UACloud_NotifyType_ActionTimer_ExternalSocket,
	E_UACloud_NotifyType_ActionTimer_ExternalSocket_Delay,
	#endif

	#ifdef	____D_CLOUDMODULE_SUPPORT____
	E_UACloud_NotifyType_UartUpLoad,
	#endif

	E_UACloud_NotifyType_Max,
}EUACloud_NotifyType;

//----------------------------------------
typedef struct _SUACloud_Info
{
	uip_ip4addr_t	sIP;
	u16_t			wPort;
	char *			pURL;
} SUACloud_Info;




//--------------------------------------------------------------
////	声明变量
//----------------------------------------
PROCESS_NAME(F_Proc_UACloudAction);// ATTRIBUTE_SECTION_SRAM;

extern process_event_t R_evUACloud;
extern process_event_t R_evUACloudNotify;
extern process_event_t R_evUACloudNotify2;
extern EUACloudConnectStatus R_eUACloudConnectState;


//--------------------------------------------------------------
////	Macro
//--------------------------------------



//--------------------------------------------------------------
////	函数
//----------------------------------------
extern void F_IOLamp_Init(void);
//	使用AT命令对接 Srv(iDest = 0) / Dev (iDest != 0)
extern ERetErr F_UACloud_ATCmd(int iDest, char * szCmd, char * szVal);


#endif

