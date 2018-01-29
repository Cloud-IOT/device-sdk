
#define	D_btPrj_HWVer	0x10


	#if defined(____D_UA_WIFI2UART_SUPPORT____)
#define	D_szPrj_Name	"UA_Wifi2Uart"
#define	D_btPrj_SWVer	0x01
	#elif (defined(____D_UACLOUD_SUPPORT____) && defined(____D_UA_POWERJACK_DEMO_SUPPORT____))
		#ifdef	____D_Project_UA402_WIFI_GN____
			#if defined(__HTTP_SUPPORT__)//#ifdef	__HTTP_SUPPORT__//add by lijing
#define	D_szPrj_Name	"UA402_WIFI_GN PowerJack HT"
#define	D_btPrj_SWVer	0x03
			#elif defined(____D_LED_4PWM____)//#else
#define	D_szPrj_Name	"UA402_WIFI_GN PowerJack 4PWM"
#define	D_btPrj_SWVer	0x02
            #else
#define	D_szPrj_Name	"UA402_WIFI_GN PowerJack TCP"
#define	D_btPrj_SWVer	0x01
			#endif
		#else
#define	D_szPrj_Name	"UA_PowerJack_Demo"
#define	D_btPrj_SWVer	0x01
		#endif
	#elif defined(____D_Project_UA402_CloudModule____)
#define D_szProjectName		"UA402_CloudModule"

#define	D_szPrj_Name	"UA_CloudModule Demo"
#define	D_btPrj_SWVer	0x07

#define	D_PID			0x4024

		#ifdef	____UA_POWERSAVE_USE_DCDC_SUPPORT____
		#undef	D_btPrj_HWVer
		#define	D_btPrj_HWVer	0x20
		#endif

	#elif defined(____D_UACLOUD_SUPPORT____)
#define	D_szPrj_Name	"UA_Cloud"
#define	D_btPrj_SWVer	0x01
	#else
		#ifdef	____D_UAWifiModule_Demo____
#define	D_szPrj_Name	"UA_iCommWifi_Demo"
#define	D_btPrj_SWVer	0x01
		#elif defined(____D_Project_UA402_HOMI_CUSTOMER____)
			#if (FUNC_OTA_ON == 1)
#define	D_szPrj_Name_PostFix	"8O"
#undef	D_btPrj_HWVer
#define	D_btPrj_HWVer	0x11
			#else
#define	D_szPrj_Name_PostFix
			#endif
#define	D_szPrj_Name	"UA402_HOMI" D_szPrj_Name_PostFix
#define	D_btPrj_SWVer	0x02
		#else
#define	D_szPrj_Name	"UA_iCommWifi"
#define	D_btPrj_SWVer	0x05
		#endif
	#endif




