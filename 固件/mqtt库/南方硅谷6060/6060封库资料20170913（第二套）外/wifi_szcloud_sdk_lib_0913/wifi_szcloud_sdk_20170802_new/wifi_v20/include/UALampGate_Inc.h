#ifndef	____UALampGate_Inc_H____
	   #define	____UALampGate_Inc_H____


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
#define	D_LampGate_Lamp_MaxAmount		255				//	灯数
#define	D_LampGate_Lamp_MaxGroup		4				//	分组
#define	D_LampGate_Lamp_BatchDeal_MaxNumber		8				//	分组


#define	____D_LampGate_NewLampStruct_NewList____

#define	D_LampGate_LightListSize_inDWORD	((2*D_LampGate_Lamp_MaxAmount + 31) / 8 / sizeof(TDWORD))


//--------------------------------------------------------------
////	结构体...定义

typedef struct _SLampGate_CfgSave
{
	TDWORD	dwMask;				//	'UALG'
	TBYTE	eLampGateType;		//	类型: ELampGateType.
	TBYTE	btVer;
	TBYTE	btAmount_Max;		//	最大支持数量
	TBYTE	btAmount;			//	记录的灯数
	TDWORD	dwLampList[(D_LampGate_Lamp_MaxAmount + 31) / sizeof(TDWORD) / 8];
	TDWORD	dwChk;
} SLampGate_CfgSave;

//----------------------------------------
typedef enum _ELampGate_Step{
	E_LampGate_Step_Init = 0,
	E_LampGate_Step_GetAmount,
	E_LampGate_Step_GetAmount_Done,
	E_LampGate_Step_Query,
	E_LampGate_Step_Query_Done,
	E_LampGate_Step_1stGetInfo,
	E_LampGate_Step_1stGetInfo_Done,
	E_LampGate_Step_GetInfo,
	E_LampGate_Step_GetInfo_Done,
	E_LampGate_Step_GetInfo_Done_OK,
	E_LampGate_Step_GetInfo_Done_Err,
	E_LampGate_Step_GetInfo_Done_Outline,
	E_LampGate_Step_GetInfo_Batch,
	E_LampGate_Step_GetInfo_Batch_Done,
	E_LampGate_Step_GetInfo_Batch_Retry,
	E_LampGate_Step_ScanGetInfo,
	E_LampGate_Step_ScanGetInfo_Done,
	//	0x10
	E_LampGate_Step_Set,
	E_LampGate_Step_Set_Busy,
	E_LampGate_Step_Set_Timeout,
	E_LampGate_Step_Set_LvCT_Done,
	E_LampGate_Step_Set_Done_OK,
	E_LampGate_Step_Set_Done_Err,
	E_LampGate_Step_Set_Done_Outline,
	E_LampGate_Step_Set_LvCT_Batch,
	E_LampGate_Step_Set_LvCT_Batch_Done,
	E_LampGate_Step_Set_LvCT_Batch_Retry,
	E_LampGate_Step_Set_LvCT_Batch_Done_OK,
	E_LampGate_Step_Set_LvCT_Batch_Done_Err,
	E_LampGate_Step_Pair,
	E_LampGate_Step_Pair_Done,
	E_LampGate_Step_Pair_Done_OK,
	E_LampGate_Step_Pair_Done_Err,
	E_LampGate_Step_Change,
	//	0x20
	E_LampGate_Step_Change_Done,
	E_LampGate_Step_Change_LvCT,
	E_LampGate_Step_Change_LvCT_Done,
	E_LampGate_Step_Remove,
	E_LampGate_Step_Remove_Done,
	E_LampGate_Step_Remove_Done_OK,
	E_LampGate_Step_Remove_Done_Err,
	E_LampGate_Step_Remove_Done_Outline,
	E_LampGate_Step_NotifyChange,


	E_LampGate_Step_Idle,

	E_LampGate_Step_Max
}ELampGate_Step;

typedef enum _ELampGate_LampState{
	E_LampGate_LampState_OK = 0,
	E_LampGate_LampState_DevErr,
	E_LampGate_LampState_DevOutline,

	E_LampGate_LampState_DevBusy,
	E_LampGate_LampState_DevTimeout,

	E_LampGate_LampState_NoDev,

	E_LampGate_LampState_Max
}ELampGate_LampState;



typedef struct _SLampGate_Msg32{
	TBYTE	btType;
	TBYTE	btNo;
	TBYTE	btState;		//	取值范围 = ELampGate_LampState
	TBYTE	btDat0;
}SLampGate_Msg32;

typedef union _UUAMsg32{
	int iData;
	SUAMsg32	msg;
	SLampGate_Msg32	msg_lg;
}UUAMsg32;

typedef enum _ELampMode{
	E_LampMode_ErrInfo = -1,		//	用于传输错误信息
	E_LampMode_Lv,
	E_LampMode_LvCT,
	E_LampMode_RGB,

}ELampMode;

#define	D_LampGate_GetLampAttribSize_ForProtocal		1	//	协议上传输的大小
#define	D_LampGate_ReturnLampAttribSize_ForProtocal		7	//	协议上传输的大小
#define	D_LampGate_SetLampAttribSize_ForProtocal		7	//	协议上传输的大小
typedef struct _SLampGate_SetLampAttrib{
	TBYTE	btNo;
	TBYTE	btMode;		//	ELampMode
	TBYTE	btLv;		//	btMode = E_LampMode_ErrInfo时, btLv 为错误信息(对应 ELampGate_LampState ), 其他元素无效
	TBYTE	btColorTemp;
	TBYTE	btLvR;
	TBYTE	btLvG;
	TBYTE	btLvB;
	TBYTE	btRet;		//	操作返回值 ELampGate_LampState
}SLampGate_SetLampAttrib;


//--------------------------------------------------------------
////	声明变量
//----------------------------------------
extern process_event_t R_evUALampGate;

PROCESS_NAME(F_Proc_UALampGate);

//--------------------------------------------------------------
////	Macro
//--------------------------------------



//--------------------------------------------------------------
////	函数
//----------------------------------------
//	恢复出厂设置
extern void F_LampGate_Cfg_Restore(void);
extern TBYTE F_LampGate_Uart_CheckSum(TBYTE *pVal, TBYTE btLen);
extern void F_LampGate_SetCmd_forATCmd(TBYTE btCmd);
extern int F_LampGate_GetLightList(TDWORD *pdwList);
extern int F_LampGate_BuildChangeList(TDWORD * pdwList);
extern int F_LampGate_SetLight(TBYTE btNo, TBYTE btLv, TBYTE btColorTemp);
extern int F_LampGate_SetLight_Batch(TBYTE btCount, SLampGate_SetLampAttrib * pInfo);
extern int F_LampGate_SetLight_Batch_GetResult(TBYTE * pbtCount, SLampGate_SetLampAttrib * pInfo);
extern int F_LampGate_GetLightInfo(TBYTE btNo, TBYTE *pbtLv, TBYTE *pbtColorTemp);
extern int F_LampGate_GetLightInfo_Batch(TBYTE btCount, SLampGate_SetLampAttrib * pInfo);
extern int F_LampGate_GetLightInfo_Batch_GetResult(TBYTE * pbtCount, SLampGate_SetLampAttrib * pInfo);
extern int F_LampGate_Pair(TBYTE btNo);
extern int F_LampGate_RemoveLight(TBYTE btNo);
//	return: D_TRUE, Ready
extern L2S F_LampGate_IsReady(void);



#endif

