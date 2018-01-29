#ifndef ____uRan_Macro_Contiki_H____
#define	____uRan_Macro_Contiki_H____

#include	"uRan_Macro.h"
#include	"uRCVer.h"


#define	D_DefineProcess(_Proc_Name)	PROCESS(_Proc_Name, #_Proc_Name)


#define	M_Min(a,b)				(((a) < (b)) ? (a) : (b))
#define	M_Max(a,b)				(((a) > (b)) ? (a) : (b))

#define	M_SetBit(_val,_bit)		(_val) |= (_bit)
#define	M_ClrBit(_val,_bit)		(_val) &= ~(_bit)

#define	M_Assert(_exp,_para0,_para1,_para2,_sz)	\
{	\
	MF_Print(#_exp" = 0x%x\r\n" #_para0" = 0x%x, " #_para1" = 0x%x, " #_para2" = 0x%x, " "%s", _exp,_para0,_para1,_para2,_sz); \
	while (!(_exp)); \
}


#define	M_UID128_SetInValid(_id1) \
	((_id1).dwID[0] = 0); \
	((_id1).dwID[1] = 0); \
	((_id1).dwID[2] = 0); \
	((_id1).dwID[3] = 0)

#define	M_UID128_IsInvalid(_id1) \
	((((_id1).dwID[0] == 0) && ((_id1).dwID[1] == 0) \
	&& ((_id1).dwID[2] == 0) && ((_id1).dwID[3] == 0)) \
	|| (((_id1).dwID[0] == 0xFFFFFFFF) && ((_id1).dwID[1] == 0xFFFFFFFF) \
	&& ((_id1).dwID[2] == 0xFFFFFFFF) && ((_id1).dwID[3] == 0xFFFFFFFF)))

#define	M_UID128_Cmp_Eq(_id1, _id2) \
	(((_id1).dwID[0] == (_id2).dwID[0]) && ((_id1).dwID[1] == (_id2).dwID[1]) \
	&& ((_id1).dwID[2] == (_id2).dwID[2]) && ((_id1).dwID[3] == (_id2).dwID[3]))

//	DWORD ÐÍ
#define	M_GetClockTime()	((TDWORD)(clock_time()/100))


#endif


