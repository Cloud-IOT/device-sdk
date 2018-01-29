

####	TRUE, FALSE
##	TRUE: DEBUG_VERSION; FALSE: RELEASE_VERSION
##	建议用 RELEASE_VERSION 来区分release版本
DEBUG_ENABLE = TRUE


#-----------------------------------------------
#-----------------------------------------------
####	项目
PROJECT_NAME = $(MYPROJECT) 
# HXWL
CUSTOMER = $(MYCUSTOMER) 
# MCU类型
MCUTYPE = $(MCU) #JX #FZ 

#	don't modify
PROJECT_PROFIX = UA


#-----------------------------------------------
#-----------------------------------------------
####	功能定义
####	TRUE, FALSE
#	WIFI模块Demo专用
UA_WIFIMODULE_DEMO = FALSE
#	连接UA服务器
#UACLOUD_SUPPORT = FALSE
#	模块带Cloud SRV 功能
#UA_CLOUDMODULE_SUPPORT = FALSE
#	透传
#UA_WIFI2UART_SUPPORT = FALSE
#	UA420 灯控网关
#UA_LAMPGATE_SUPPORT = FALSE
#	UA 插座
#UA_POWERJACK_DEMO_SUPPORT = FALSE
#  按键
UA_KEY_DEMO_SUPPORT = FALSE
#	使能看门狗功能		默认DBG模式会关掉WDT, RELEASE模式会打开, 但最后可以根据项目来定
UA_WDT_SUPPORT = FALSE
#	模块时钟系统
#UA_TIME_CLOCK_SUPPORT = FALSE
#	使用DCDC来降低功耗	无特殊要求, 默认使用LDO
UA_POWERSAVE_USE_DCDC_SUPPORT = FALSE
#	OTA
UA_OTA_SUPPORT = FALSE
#	连接闪烁指示灯
UA_LED_Indicator_SUPPORT = FALSE
#	状态指示引脚
#	UA_IO_Indicator_Only_Link_SUPPORT 打开, 就只支持nLink.
#	UA_IO_Indicator_Only_Link_SUPPORT 关闭, 就支持nLink + nReady.
UA_IO_Indicator_SUPPORT = FALSE
UA_IO_Indicator_Only_Link_SUPPORT = FALSE

WIFI_WEATHER = FALSE

UA_ESP_8266_SMART_LINK = FALSE

UA_TIMER_TASK_SUPPORT = FALSE		# 定时器开关功能


#-----------------------------------------------
#-----------------------------------------------
####	固定的宏
COM_DEFS = ____D_OS_ConTiki____

ifdef DEBUG_ENABLE
  ifeq ($(strip $(DEBUG_ENABLE)),TRUE)
    COM_DEFS += DEBUG_VERSION
    COM_DEFS += ____D_uRan_Macro_Dbg_En____ ____D_Dev_IOT____
    COM_DEFS += ____D_uRan_Macro_Dbg_WithTime____
	VERSION_NAME = Debug
	UA_WDT_SUPPORT := FALSE
  else
    COM_DEFS += RELEASE_VERSION
    VERSION_NAME = Release
    UA_WDT_SUPPORT := TRUE
  endif
endif


####COM_DEFS +=
#COM_DEFS += ____D_UA_LED_Indicator____
####
####	v1134 not use
#COM_DEFS += ____D_ATCmd_Fill____
#COM_DEFS += ____D_ATCmd_SendNetData____
####
#COM_DEFS += ____D_ATCmd_SendNetHexData____
COM_DEFS += ____D_ATCmd_UARTSendData____

#COM_DEFS += ____D_OBD_Uart2Wifi____

###
#-----------------------------------------------
#-----------------------------------------------
####	项目属性
ifdef PROJECT_NAME
#-----------------------------------------------



ifeq ($(strip $(PROJECT_NAME)), UA402_SZCLOUD)
UA_OTA_SUPPORT := TRUE
COM_DEFS += __SZCLOUD_SUPPORT__
UA_LED_Indicator_SUPPORT := FALSE
UA_IO_Indicator_SUPPORT := TRUE
UA_WDT_SUPPORT := FALSE
UA_KEY_DEMO_SUPPORT := TRUE
UA_SZCLOUD_SUPPORT := TRUE
endif

ifeq ($(strip $(PROJECT_NAME)), UA402D_SZCLOUD)
UA_OTA_SUPPORT := TRUE
COM_DEFS += __SZCLOUD_SUPPORT__
UA_LED_Indicator_SUPPORT := FALSE
UA_IO_Indicator_SUPPORT := TRUE
UA_WDT_SUPPORT := FALSE
UA_KEY_DEMO_SUPPORT := TRUE
UA_SZCLOUD_SUPPORT := TRUE
UA_POWERSAVE_USE_DCDC_SUPPORT := TRUE
endif

ifneq ($(strip $(CUSTOMER)),)
COM_DEFS += __CUSTOMER_$(strip $(CUSTOMER))__=$(CUSTOMER)
COM_DEFS += __CUSTOMER__=$(CUSTOMER)
endif

ifneq ($(strip $(MCUTYPE)),)
COM_DEFS += __GIZ_MCU_$(strip $(MCUTYPE))__
endif

#-----------------------------------------------
##	____D_Project_UA402_STYLE____ 意思是基于UA402项目的一系列产品
##	____D_Project_UA402_WIFI_GN____ 意思是UA402项目下的一个客户产品

  ifneq ($(findstring UA402,$(strip $(PROJECT_NAME))),)
  ##	根据 PROJECT_NAME 自动生成宏 ____D_Project_(PROJECT_STYLE)____
  ##	如: ____D_Project_UA402_STYLE____
PROJECT_STYLE = UA402_STYLE
COM_DEFS += ____D_Project_$(strip $(PROJECT_STYLE))____
  endif

#-----------------------------------------------
  ifneq ($(findstring UA420,$(strip $(PROJECT_NAME))),)
PROJECT_STYLE = UA420_STYLE
COM_DEFS += ____D_Project_$(strip $(PROJECT_STYLE))____

COM_DEFS += ____D_UACloud_Protocol_CmdSerialID____
COM_DEFS += ____D_UACloud_Support_LocalConnect____
COM_DEFS += ____D_UACloud_Support_LocalConnect_Plus____

COM_DEFS += ____D_LampGate_Support_Protocol_V2____
  endif

#-----------------------------------------------
  ifneq ($(findstring UA436,$(strip $(PROJECT_NAME))),)
PROJECT_STYLE = UA436_STYLE
COM_DEFS += ____D_Project_$(strip $(PROJECT_STYLE))____
  endif

#-----------------------------------------------
  ifneq ($(strip $(PROJECT_NAME)),)
  ##	根据 PROJECT_NAME 自动生成宏 ____D_Project_(PROJECT_NAME)____
  ##	如: ____D_Project_UA402_WIFI_GN____
COM_DEFS += ____D_Project_$(strip $(PROJECT_NAME))____
  endif


endif


ifeq ($(strip $(CUSTOMER)),SBKJ)
WIFI_WEATHER = TRUE
endif

ifeq ($(strip $(CUSTOMER)),WYYL)
UA_KEY_DEMO_SUPPORT := TRUE
COM_DEFS += __BEEP_SUPPORT__
endif

ifeq ($(strip $(CUSTOMER)),XMRL)
COM_DEFS += __WEBSERVER_SUPPORT__
UA_WEBSERVER_SUPPORT := TRUE
UA_KEY_DEMO_SUPPORT := TRUE
endif


#-----------------------------------------------
#-----------------------------------------------
####	功能宏
ifdef UACLOUD_SUPPORT
  ifeq ($(strip $(UACLOUD_SUPPORT)),TRUE)
    COM_DEFS += ____D_UACLOUD_SUPPORT____
    ifdef UA_CLOUDMODULE_SUPPORT
      ifeq ($(strip $(UA_CLOUDMODULE_SUPPORT)),TRUE)
      	COM_DEFS    += ____D_CLOUDMODULE_SUPPORT____
      endif
    endif


  ifneq ($(findstring UA402,$(strip $(PROJECT_NAME))),)
#	UA402 UACLOUD使用此宏测试
    COM_DEFS += __HTTP_SUPPORT__
    COM_DEFS += ____D_Not_Use_HTTP_UACLOUD____
  endif

	COM_DEFS += __CJSON_SUPPORT__

  endif
endif


#-----------------------------------------------
ifdef UA_WIFI2UART_SUPPORT
  ifeq ($(strip $(UA_WIFI2UART_SUPPORT)),TRUE)
  	COM_DEFS    += ____D_Disable_iComm_WifiUart____
    COM_DEFS    += ____D_UA_WIFI2UART_SUPPORT____
  endif
endif


ifeq ($(strip $(WIFI_WEATHER)),TRUE)
COM_DEFS += __CJSON_SUPPORT__
COM_DEFS += __WIFI_WEATHER__

endif


#-----------------------------------------------
ifdef UA_LAMPGATE_SUPPORT
  ifeq ($(strip $(UA_LAMPGATE_SUPPORT)),TRUE)
    ifneq ($(strip $(UACLOUD_SUPPORT)),TRUE)
      $(error "UACLOUD_SUPPORT" Must Be Set "TRUE"!)
    endif
    ifeq ($(strip $(UA_WIFI2UART_SUPPORT)),TRUE)
      $(error "UA_WIFI2UART_SUPPORT" Must Be Set "FALSE"!)
    endif

    COM_DEFS    += ____D_Disable_iComm_WifiUart____
    COM_DEFS    += ____D_UA_LAMPGATE_SUPPORT____
  endif
endif


#-----------------------------------------------
ifdef UA_POWERJACK_DEMO_SUPPORT
  ifeq ($(strip $(UA_POWERJACK_DEMO_SUPPORT)),TRUE)
    ifneq ($(strip $(UACLOUD_SUPPORT)),TRUE)
      $(error "UACLOUD_SUPPORT" Must Be Set "TRUE"!)
    endif

    COM_DEFS    += ____D_UA_POWERJACK_DEMO_SUPPORT____
  endif
endif


#-----------------------------------------------
ifdef UA_WIFIMODULE_DEMO
  ifeq ($(strip $(UA_WIFIMODULE_DEMO)),TRUE)
    ifeq ($(strip $(UACLOUD_SUPPORT)),TRUE)
      $(error "UACLOUD_SUPPORT" Must Be Set "FALSE"!)
    endif
    ifeq ($(strip $(UA_LAMPGATE_SUPPORT)),TRUE)
      $(error "UA_LAMPGATE_SUPPORT" Must Be Set "FALSE"!)
    endif
    COM_DEFS    += ____D_UAWifiModule_Demo____
  endif
endif


#-----------------------------------------------
ifdef UA_KEY_DEMO_SUPPORT
  ifeq ($(strip $(UA_KEY_DEMO_SUPPORT)),TRUE)
  	COM_DEFS    += ____D_UA_KEY_DEMO_SUPPORT____
  endif
endif


#-----------------------------------------------
ifdef UA_WDT_SUPPORT
  ifeq ($(strip $(UA_WDT_SUPPORT)),TRUE)
  	COM_DEFS    += ____D_WDT_SUPPORT____
  endif
endif


#-----------------------------------------------
ifdef UA_TIME_CLOCK_SUPPORT
  ifeq ($(strip $(UA_TIME_CLOCK_SUPPORT)),TRUE)
  	COM_DEFS    += ____D_UA_TIME_CLOCK_SUPPORT____
  endif
endif


#-----------------------------------------------
ifdef UA_POWERSAVE_USE_DCDC_SUPPORT
  ifeq ($(strip $(UA_POWERSAVE_USE_DCDC_SUPPORT)),TRUE)
  	COM_DEFS    += ____UA_POWERSAVE_USE_DCDC_SUPPORT____
  endif
endif


#-----------------------------------------------
ifdef UA_LED_Indicator_SUPPORT
  ifeq ($(strip $(UA_LED_Indicator_SUPPORT)),TRUE)
  	COM_DEFS    += ____D_UA_LED_Indicator____
  endif
endif


#-----------------------------------------------
ifdef UA_IO_Indicator_SUPPORT
  ifeq ($(strip $(UA_IO_Indicator_SUPPORT)),TRUE)
  	COM_DEFS    += ____D_Module_IO_Indicator____

  	ifdef UA_IO_Indicator_Only_Link_SUPPORT
	  ifeq ($(strip $(UA_IO_Indicator_Only_Link_SUPPORT)),TRUE)
	  	 COM_DEFS    += ____D_Module_IO_Indicator_Only_Link____
	  endif
    endif

  endif
endif

ifdef UA_ESP_8266_SMART_LINK
  ifeq ($(strip $(UA_ESP_8266_SMART_LINK)),TRUE)
  	COM_DEFS    += __ESP_8266_SMTLNK_SUPPORT__
  endif
endif

ifdef UA_HF_SMART_LINK
  ifeq ($(strip $(UA_HF_SMART_LINK)),TRUE)
  	COM_DEFS    += __HF_SMTLNK_SUPPORT__
  endif
endif

ifdef UA_TIMER_TASK_SUPPORT
  ifeq ($(strip $(UA_TIMER_TASK_SUPPORT)),TRUE)
  	COM_DEFS    += __SK_TIMER_SUPPORT__
  endif

endif


#-----------------------------------------------
#-----------------------------------------------
D_CFLAG_CUSTOM  = $(foreach def, $(COM_DEFS),-D$(def))




