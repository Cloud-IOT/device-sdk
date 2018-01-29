#ifndef	____UACloudModule_Inc_H____
	   #define	____UACloudModule_Inc_H____


//--------------------------------------------------------------
////	用法
/*

*/



//--------------------------------------------------------------
////	头文件
////	外部引用时需要使用的
#include	"uRan_Macro_Contiki.h"


//--------------------------------------------------------------
////	Define

#define	D_CloudModule_UARTDataBuffer_Size	(256-4)



//--------------------------------------------------------------
////	结构体...定义


//----------------------------------------

typedef struct _SCloudModuleCfg
{
	TBYTE	btChkMode;		//	E_CloudModule_ProtocolChkMode
	TBYTE	btTimerAction_En;
	TBYTE	btNotify_En;
	TBYTE	btRev;
	TBYTE	btRev1;
	TBYTE	btRev2;
	TBYTE	btRev3;
	TBYTE	btRev4;
} SCloudModule_Cfg;

typedef struct _SCloudModuleCfgSave
{
	TDWORD	dwMask;
	SCloudModule_Cfg	sCfg;
	TDWORD	dwChk;
	TDWORD	dwMask1;
} SCloudModule_CfgSave;

//--------------------------------------------------------------
////	声明变量
//----------------------------------------
extern int R_bCloudModule_KeyToSmartLink;
extern TBYTE R_bufCloudModule_Data[D_CloudModule_UARTDataBuffer_Size];
PROCESS_NAME(F_Proc_UACloudModule_UartHdl);

//--------------------------------------------------------------
////	Macro
//--------------------------------------



//--------------------------------------------------------------
////	函数
//----------------------------------------
extern void F_CloudModule_Init(void);
extern ERetErr F_CloudModule_DownLoad(TBYTE * pVal, TBYTE btLen);

extern void F_CloudModule_DisableSmartlink(int bChangeNow);
extern void F_CloudModule_ReSmartlink(void);
extern void F_CloudModule_Smartlink(void);

extern ERetErr F_CloudModule_CmdHdl_Notify_Ready(void);
extern ERetErr F_CloudModule_CmdHdl_Notify_ConnState(EConnectStatus eStatus);
extern ERetErr F_CloudModule_CmdHdl_Notify_Timer(TBYTE btTimerID, TBYTE btSwitchActive, TBYTE btSwitchAction);
extern ERetErr F_CloudModule_CmdHdl_Notify_Delay(TBYTE btTimerID, TBYTE btSwitchActive, TBYTE btSwitchAction);
extern ERetErr F_CloudModule_CmdHdl_Notify_Key(TBYTE btKey, TBYTE btKeyEvent);

#endif

