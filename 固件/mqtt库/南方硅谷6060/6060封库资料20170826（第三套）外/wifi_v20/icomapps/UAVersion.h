
#define	D_VID 			0x0055
#define	D_CID 			0x6060		//	Chip ID


#if defined(____D_Project_UA402_STYLE____)
#include	"UAVersion_UA402.h"
#elif defined (____D_Project_UA420_STYLE____)
#include	"UAVersion_UA420.h"
#elif defined (____D_Project_UA436_STYLE____)
#include	"UAVersion_UA436.h"
#else
#define	D_szPrj_Name	"UA_iCommWifi Def"
#define	D_btPrj_HWVer	0x10
#define	D_btPrj_SWVer	0x10
#endif

#if (FUNC_OTA_ON == 1)
	#ifdef	D_szProjectName
		#define	D_szOTA_FWFileName	D_szProjectName ".bin"
	#endif
#endif

#ifdef	____D_UACLOUD_SUPPORT____
#define	D_wPrj_ProtocolVer	0xA001
#else
//	ŒﬁÕ¯¬Á–≠“È
#define	D_wPrj_ProtocolVer	0x0000
#endif


