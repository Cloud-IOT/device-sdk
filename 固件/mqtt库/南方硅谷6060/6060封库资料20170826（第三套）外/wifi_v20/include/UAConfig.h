
#ifndef	____UAConfig_H____
#define	____UAConfig_H____

//#include	"uRan_Macro.h"

#define	D_iRFTblSize	0x15

/*
typedef struct _SMPTbl
{
	TBYTE	btaMask[4];
	TBYTE	btList;
	TBYTE	btaRev0[3];

	TBYTE	btSizeMAC;
	TBYTE	btaRev1[3];
	TBYTE	btaMAC[8];

	TBYTE	btSizeRT;
	TBYTE	btaRevRT[2];
	TBYTE	btIndexRT[1];
	TBYTE	btaRT[D_iRFTblSize];

	TBYTE	btSizeHT;
	TBYTE	btaRevHT[2];
	TBYTE	btIndexHT[1];
	TBYTE	btaHT[D_iRFTblSize];

	TBYTE	btSizeLT;
	TBYTE	btaRevLT[2];
	TBYTE	btIndexLT[1];
	TBYTE	btaLT[D_iRFTblSize];

	TBYTE	btBond;
} SMPTbl;
*/



#define	D_szDefaultAutoConnect		0		//	0, disable auto connect, 1, enable
#define	D_szDefaultSmartLinkMode	0		//	0, none; 1, smartlink; 2, airkiss; ...

//	max 32bytes
#define	D_szDefaultAP_SSID			"UA_"
//	max 64bytes
#define	D_szDefaultAP_PWD			"12345678"
#define	D_szDefaultAP_EncrytMode	0		//	0: open, 1:WPA2-tkip 2:WPA2:CCMP



#define PRODUCT_GPIO_PIN GPIO_19				// Éú²ú²âÊÔÒý½Å


#endif

