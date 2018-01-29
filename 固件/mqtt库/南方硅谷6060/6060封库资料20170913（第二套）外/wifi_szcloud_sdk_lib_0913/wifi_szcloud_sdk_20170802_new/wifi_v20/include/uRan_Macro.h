#ifndef ____uRan_Macro_H____
#define	____uRan_Macro_H____


typedef unsigned char   TBYTE;
typedef unsigned short  TWORD;
//typedef	unsigned long		TDWORD;
typedef unsigned int    TDWORD;

typedef unsigned char   Tu8;
typedef unsigned short  Tu16;
typedef unsigned int    Tu32;
typedef   signed char   Ts8;
typedef   signed short  Ts16;
typedef   signed int    Ts32;
typedef     long long			Ts64;
typedef unsigned long long		Tu64;


typedef Tu32            Tu0;
typedef Ts32            Ts0;

typedef Tu0             L2S;
typedef Ts0             L3S;


#define	D_TRUE				1
#define	D_FALSE				0

#define	D_NULL				NULL


#define	D_Release_Lv0				(0)
#define	D_Debug_Lv0					(D_Release_Lv0 + 1)
#define	D_Debug_Lv1					(D_Debug_Lv0 + 1)
#define	D_Debug_Lv2					(D_Debug_Lv1 + 1)

//	用于标记C文件时间戳版本的最大字符数
#define	D_FileTimeStamp_MaxSize			48

#define	__D_InLine__				__inline

#define	D_szFile					(__FILE__)
#define	D_szDate					(__DATE__)
#define	D_szTime					(__TIME__)
#define	D_szFunc					(__func__)
#define	D_szCC_Name					(__CC_ARM)

#define	D_dwLine					(__LINE__)

//	获取LR寄存器的值
//	如果在调用该宏前有调用到其他函数, 则获得的值不是该宏所在的函数的返回值, 而是上次调用函数的返回值
#define	M_ARM_GetLR(odwLR)	\
__asm	\
{	\
	mov odwLR, lr;	\
}

#define	M_Get_RetPC(odwLR)	M_ARM_GetLR(odwLR)


#define	M_ArrayCount(iArray)	(sizeof(iArray) / sizeof((iArray)[0]))


#define	MF_GetCurrentTick(odwTick)	(odwTick) = drv_get_current_time()

#define	MF_GetDurationTick(odwDurationTick,idwPrewTick)	\
{	\
	DWORD dwCurrentTick;	\
	MF_GetCurrentTick(dwCurrentTick);	\
	(odwDurationTick) = drv_get_duration_tick(dwCurrentTick, idwPrewTick);	\
}

#define	MF_GetDuration_mS(odwDuration_mS,idwPrewTick)	\
	(odwDuration_mS) = drv_get_duration_ms(idwPrewTick)

#if 1
	#define	MF_Dbg_Func_MaskStr1 "[uRan1]"
	#define	D_OS_CrLf	"\r\n"

	#if defined(BUILD_LK) || defined(BUILD_UBOOT)
		#define	mutex_lock(...)
		#define	mutex_unlock(...)
	#endif
#define	MF_EN()
#define	MF_EN1(a)
	#define	MF_Dbg_Print_F(...)
#define	MF_Dbg_Print_LF()
#define	MF_Dbg_Print_sz(sz)
#define	MF_Dbg_Print_4X(a,b,c,d)
#define	MF_Dbg_Print_4D(a,b,c,d)
#define	MF_DbgPrint_F(...)
#define	MF_DbgPrint_LF()
#define	MF_DbgPrint_sz(sz)
#define	MF_DbgPrint_4X(a,b,c,d)
#define	MF_DbgPrint_4D(a,b,c,d)
	#define	MF_DbgPrint_Hex(_buf,_Len)

	#ifdef	____D_uRan_Macro_Dbg_En____
	#define	MF_Dbg_Func printf

	#define	MF_Print(...) \
	MF_Dbg_Func(MF_Dbg_Func_MaskStr1 __VA_ARGS__);\
	MF_Dbg_Func(D_OS_CrLf)
	#else
	#define	MF_Print(...)
	#endif

#endif


#if 0
#define	M_Func_Begin()		\
{	\
	F_DbgPrint("[????] --> %s()\r\n", D_szFunc);	\
}	\
{

#else
#define	M_Func_Begin()		\
{

#endif

#define	M_Func_End()		\
}


#endif

