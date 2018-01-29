#ifndef ____UA_Func_H____
#define	____UA_Func_H____

#include	"Cabrio-conf.h"
#include	"systemconf.h"
#include	"atcmd.h"

#include	"uRan_Macro_Contiki.h"

#define		D_btPrjName_Len			32

#define	D_SerialNUmber_Length		16
#define	D_IMEI_Length		15

#ifdef	____D_WDT_SUPPORT____
//----------------------------------------
#define	D_WDT_Time					5000	//	unit: mS

//----------------------------------------
extern void	F_WDT_EnableManualReboot(void);

#endif

#if (FUNC_OTA_ON == 1)
#define	D_OTA_Password_Len		32		//	密文字符长度
#define	D_szOTA_DefaultPassword		"9876543210"		//	密文字符

#endif


#define	D_TCPDataBuffer_Size		MAX_SEND_BUFFER
//----------------------------------------
extern IEEE80211STATUS gwifistatus;
extern char R_btaSerialNUmber[D_SerialNUmber_Length];
extern char R_szIMEI[D_IMEI_Length + 1];
extern int At_Reboot (stParam *param);

#if	(defined(____D_UA_LED_Indicator____) || defined(____D_Module_IO_Indicator____))
//----------------------------------------
typedef enum _EConnectStatus{
	E_ConnectStatus_Disconnect,
	E_ConnectStatus_ATConnecting,
	E_ConnectStatus_SmartLinkConnecting,
	E_ConnectStatus_Connected,
	E_ConnectStatus_SrvLogin_Ing,
	E_ConnectStatus_SrvLogined,
}EConnectStatus;

extern EConnectStatus	R_btConnectStatus;
extern int R_iSmartLinking;
#endif



//----------------------------------------
//	函数返回用
typedef enum _ERetErr{
	E_RetErr_ParaUnknowErr = -3,				//	参数无法解析
	E_RetErr_ParaErr = -2,					//	参数错误
	E_RetErr_Err = -1,
	E_RetErr_Sucess = 0,
	E_RetErr_Other,
}ERetErr;

//----------------------------------------

#define	D_Socket_Invalid	-1

typedef enum _EURSocketIndex{
	E_URSocketIndex_Invalid = -1,
	E_URSocketIndex_UACloud,			//	广域网
	E_URSocketIndex_UACloud_Local,		//	局域网

	E_URSocketIndex_Max,
	E_URSocketIndex_UACloud_SrvAndLocal,	//	特殊用法, 使用宏来判断是否有效
											//	用于指示发送时发送到 服务端和本地
	E_URSocketIndex_ATCmd2Dev,				//	特殊用法, ATcmd转接
	//E_URSocketIndex_ATCmd2Srv,				//	特殊用法, ATcmd转接
	E_URSocketIndex_UACloud_UDP,

	E_URSocketIndex_Dbg_HP = 0,			//	高优先
	E_URSocketIndex_Dbg,

	E_URSocketIndex_Cust


}EURSocketIndex;

#define	M_URSocketIdx_IsInvalidMin(__idx)	\
	((EURSocketIndex)(__idx) <= E_URSocketIndex_Invalid \
	|| (EURSocketIndex)(__idx) >= E_URSocketIndex_Max)

#define	M_URSocketIdx_IsInvalid(__idx)	\
	((EURSocketIndex)(__idx) <= E_URSocketIndex_Invalid \
	|| ((EURSocketIndex)(__idx) >= E_URSocketIndex_Max \
	&& (EURSocketIndex)(__idx) != E_URSocketIndex_ATCmd2Dev))

#define	M_URSocketIdx_IsInvalid_Ext(__idx)	\
	((EURSocketIndex)(__idx) <= E_URSocketIndex_Invalid \
	|| ((EURSocketIndex)(__idx) >= E_URSocketIndex_Max \
	&& ((EURSocketIndex)(__idx) != E_URSocketIndex_UACloud_SrvAndLocal \
	&& (EURSocketIndex)(__idx) != E_URSocketIndex_ATCmd2Dev)))

typedef struct _STCPDataBuffer
{
	#ifdef	____D_UACloud_Support_LocalConnect____
	EURSocketIndex	eSocket;
	#endif

	TWORD	wCount;
	TWORD	wSize;					//	buf大小
	TDWORD	dwSum;					//	收发总字节
	TDWORD	dwTimes;				//	收发次数
	TBYTE 	Buf[D_TCPDataBuffer_Size];
	#ifdef	DEBUG_VERSION
	clock_time_t	tTime;				//	tick, for debug
	#endif
} STCPDataBuffer;



typedef struct _SUAMsg32{
	TBYTE	btType;
	TBYTE	btDat0;
	TBYTE	btDat1;
	TBYTE	btDat2;
}SUAMsg32;


typedef struct _SUMsg32{
	TBYTE	btDat0;
	TBYTE	btDat1;
	TBYTE	btType0;
	TBYTE	btType1;
}SUMsg32;


//	该IC系统结构中, UUMsg32如果是指针, 则.msg.btType1肯定为非0
typedef union _UUMsg32{
	SUMsg32	msg;
	TDWORD dwData;
	int iData;
	void * p;
}UUMsg32;

typedef union _UID128{
	TBYTE btID[16];
	TDWORD dwID[4];
}UID128;

typedef union _UID256{
	TBYTE btID[32];
	TDWORD dwID[8];
}UID256;



typedef struct _SVersion
{
	TDWORD	dwMask;
	char	szBuildTime[32];
	char	szPrjName[D_btPrjName_Len];
	TBYTE	btHWVer;
	TBYTE	btSWVer;
	TWORD	wSDKVer;
	char	szRCVer[40];
	TWORD	wProtocolVer;				//	通讯协议版本
	char    sw_version[16];				// major.minor, add by qinjiangwei 2016/11/4, 软件版本标准格式major代笔主版本号，minor表示次版本号，大功能升级时,升级主版本号
} SVersion;
extern const SVersion S_Ver;

extern int R_bDebugModeInReleaseVer;
extern int R_bDebugModeInReleaseVer_LocalSrv;


typedef enum _EFMTest_OptID{
	E_FMTest_Opt_Dbg = 0,		//
	E_FMTest_Opt_Dbg_LocalSrv,		//
	E_FMTest_Opt_Max,		//
}EFMTest_OptID;


//	bit0, 测试结果; bit2:1, 测试项
#define	D_btFMTest_Item_Max		2		//	2, 只有IO和RF; 4, IO/RF, 还有预留的T1/T2
//#define	D_btFMTest_Item_IO		(0<<1)
//#define	D_btFMTest_Item_RF		(1<<1)
//#define	D_btFMTest_Item_Menu	(2<<1)
//#define	D_btFMTest_Item_Other	(3<<1)
#define	D_btFMTest_MaxVal		(((D_btFMTest_Item_Max - 1)<<1) | 0x1)
#define	D_btFMTest_Result_AllNoTest		0x00
#if	(D_btFMTest_Item_Max == 4)
#define	D_btFMTest_Result_AllOK			0xff
#else
#define	D_btFMTest_Result_AllOK			0x0f
#endif
typedef struct _SFMTestResult
{
	TDWORD	dwMask;				//	D_FMTestResult_Mask
	TBYTE	btResult	;		//	4项测试, 每项2bit, IO, RF, Memu, Other:
								//		0X'b, 没有测试, 10'b, 失败, 11'b, 成功.
	TBYTE	btRev0;
	TBYTE	btRev1;
	TBYTE	btRev2;
	char	szPrjName[D_btPrjName_Len];
	TDWORD	dwChk;
} SFMTestResult;

extern void F_FMTest_Save(void);
extern TBYTE F_FMTest_GetResult(void);
extern void F_FMTest_SetResult(TBYTE btResult);
extern void	F_FMTest_SetOpt(EFMTest_OptID eID, int bVal);

//----------------------------------------
PROCESS_NAME(F_Proc_Delay);

//----------------------------------------
//	XmS Delay
//	M_Sleepms 允许其他线程运行
#define	M_Sleepms(_ms)	process_start(&(F_Proc_Delay), (char *)(_ms));
//	F_Delayms 不允许其他线程运行
extern void	F_Delayms(TDWORD ms);



//----------------------------------------
#ifdef	____D_UA_WIFI2UART_SUPPORT____
//----------------------------------------
//----------------------------------------
#define	D_ATCmd_Length_Max		255
//----------------------------------------
extern char R_caATCmdBuffer[D_ATCmd_Length_Max + 1];
extern char R_caATCmdPrintfBuffer[D_ATCmd_Length_Max + 1];

//----------------------------------------
extern int F_Wifi2Uart_IsCmdMode(void);
extern void F_Wifi2Uart_Init(void);
extern void F_Wifi2Uart_Start(void);




#endif

//----------------------------------------
#ifdef	____D_UACLOUD_SUPPORT____
#include	"UACloud_Inc.h"
#endif

//----------------------------------------
#ifdef	____D_UA_LAMPGATE_SUPPORT____
#include	"UALampGate_Inc.h"
#endif

//----------------------------------------
#ifdef	____D_CLOUDMODULE_SUPPORT____
#include	"UACloudModule_Inc.h"
#include	"UASmartlink_Ext_Inc.h"
#endif

extern void F_btVal2Hex(TBYTE btVal, char * pcAscii);
extern void F_btVal2Hex_LC(TBYTE btVal, char * pcAscii);
extern int F_Hex2btVal(char * pcAscii, TBYTE *pbtVal);
extern void F_PrintHex(TBYTE * buf, TWORD wLen);

extern int F_GetLocalIP_V4(uip_ip4addr_t * pIP);
//	判断IP是否本主机的IP
extern int F_IsLocalIP_V4(uip_ip4addr_t * pIP);

//----------------------------------------
#ifdef	____D_UA_WIFI2UART_SUPPORT____
#define	MF_ATPrintf(...) \
{ \
	if (F_Wifi2Uart_IsCmdMode()) \
	{ \
		sprintf(R_caATCmdBuffer, __VA_ARGS__); \
		SerialWrite(R_caATCmdBuffer, strlen(R_caATCmdBuffer)); \
		printf(R_caATCmdBuffer); \
	} \
	else \
	{ \
		printf(__VA_ARGS__); \
	} \
}
#else
#define	MF_ATPrintf(...) 		printf(__VA_ARGS__)

//#define	MF_ATPrintf 		printf
#endif






#ifdef	____D_UA_TIME_CLOCK_SUPPORT____
#include	"UA_TIME_clock_Inc.h"
#endif

//----------------------------------------
//		Custom Area Save Addr
#define	D_CustomArea_Addr			CFG_CUSTOMER_START_ADDR
#define	D_FMT_Save_AddrOffset			(0x0000)
#define	D_LampGate_Save_AddrOffset		(0x0080)
#define	D_TimerData_Save_AddrOffset		(0x0100)


#define	D_FMTestResult_Addr			(CFG_CUSTOMER_START_ADDR + D_FMT_Save_AddrOffset) ////0x0307c000
//#define	D_LampGate_CfgSave_Addr		(CFG_CUSTOMER_START_ADDR + D_LampGate_Save_AddrOffset) //(CFG_CUSTOMER_START_ADDR + 0x10) //0x0307c000
#define	D_TimerData_CfgSave_Addr	(CFG_CUSTOMER_START_ADDR + D_TimerData_Save_AddrOffset) //(CFG_CUSTOMER_START_ADDR + 0x10) //0x0307c000

//	__sizeof_data 必须小于32K
#define	M_CustAreaEnum_AddID(__id,__sizeof_data) \
	__id##_Begin, \
	__id = ((((__id##_Begin - 1) + (((__id##_Begin - 1) & 0x7fff)<<16)) & 0x7fff0000) | ((__sizeof_data) & 0x7fff))


typedef enum _ECustAreaDataID{
	E_CustAreaDataID_Cust = -1,		//	手动保存用
	E_CustAreaDataID_Start = 0,

	E_CustAreaDataID_Start_P0 = 0 + 0x00000000,			//	Area0

	M_CustAreaEnum_AddID(E_CustAreaDataID_FMT, sizeof(SFMTestResult)),


	#if (defined(____D_UA_TIME_CLOCK_SUPPORT____) && defined(____D_Project_UA402_WIFI_GN____))
	M_CustAreaEnum_AddID(E_CustAreaDataID_Timer, sizeof(SActionTimer) * D_ActionTimer_Amount),
	#endif

	#if	(CFG_CUSTOMER_SIZE > 0x1000)
	E_CustAreaDataID_Start_P1 = 0 + 0x10000000,			//	Area1
	//	存储到4 ~ 8k空间
	#endif

	#ifdef	____D_UA_LAMPGATE_SUPPORT____
	M_CustAreaEnum_AddID(E_CustAreaDataID_LampGate, sizeof(SLampGate_CfgSave)),
	#endif

	#ifdef	____D_CLOUDMODULE_SUPPORT____
	M_CustAreaEnum_AddID(E_CustAreaDataID_CloudModuleCfg, sizeof(SCloudModule_CfgSave)),
	#endif

	#if (defined(____D_UA_TIME_CLOCK_SUPPORT____) && !defined(____D_Project_UA402_WIFI_GN____))
	M_CustAreaEnum_AddID(E_CustAreaDataID_Timer, sizeof(SActionTimer) * D_ActionTimer_Amount),
	#endif


	#if	(CFG_CUSTOMER_SIZE > 0x2000)
	E_CustAreaDataID_Start_P2 = 0 + 0x20000000,			//	Area2
	//	存储到4 ~ 8k空间
	#endif



	#if	(CFG_CUSTOMER_SIZE > 0x3000)
	E_CustAreaDataID_Start_P3 = 0 + 0x30000000,			//	Area3
	//	存储到4 ~ 8k空间
	#endif



}ECustAreaDataID;


extern TBYTE	R_CustomArea_Buff[0x1000];	//
extern ERetErr	F_CustArea_Save(ECustAreaDataID eID, TBYTE * pData, TWORD wSize, TDWORD dwOffset);
extern ERetErr	F_CustArea_Load(ECustAreaDataID eID, TBYTE * pData, TWORD wSize, TDWORD dwOffset);
extern ERetErr	F_CustArea_Erea(ECustAreaDataID eID, TBYTE btPage);
#endif


