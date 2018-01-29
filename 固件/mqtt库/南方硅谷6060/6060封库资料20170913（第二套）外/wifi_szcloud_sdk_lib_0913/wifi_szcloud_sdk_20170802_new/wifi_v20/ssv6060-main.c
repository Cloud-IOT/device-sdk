/*
 * Copyright (c) 2014, SouthSilicon Valley Corp.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         A very simple Contiki application showing how Contiki programs look
 * \author
 *         Adam Dunkels <adam@sics.se>
 */
#include "contiki.h"
#include <string.h>
#include <stdio.h> /* For printf() */
#include "gpio_api.h"
#include "atcmd.h"


#include	"UA_Func.h"

#include "dbg-printf.h"
#include "iperf_cmd.h"
//#include "customer_socket.h"
#include "soft_ap.h"
#include "soft_ap_init.h"
#include <atcmdlib.h>

#ifdef __GIZ_CLOUD_SUPPORT__
#include "giz_extern.h"
#endif

int AT_Customer(stParam *param);
int AT_Customer2(stParam *param);
int AT_HIDE_MSG(stParam *param);
int AT_IPERF_CMD(stParam *param);
//mqtt at command
int AT_MQTT_INIT(stParam *param);
int AT_MQTT_CONNECT(stParam *param);
int AT_MQTT_SSL_CONNECT(stParam *param);
int AT_MQTT_DISCONNECT(stParam *param);
int AT_MQTT_PUBLISH(stParam *param);
int AT_MQTT_SUBSCRIBE(stParam *param);
int AT_MQTT_UNSUBSCRIBE(stParam *param);
int AT_MQTT_AUTH(stParam *param);


extern int F_ATCmd_GetIMEI(stParam *param);
extern int F_ATCmd_Get_uVer(stParam *param);
extern int F_ATCmd_IOTest(stParam *param);
extern int F_ATCmd_FMTest_TCPSend(stParam *param);
extern int F_ATCmd_FMTest_ReadResult(stParam *param);
extern int F_ATCmd_FMTest_Stop(stParam *param);
extern int F_ATCmd_FMTest_SetOpt(stParam *param);
extern int F_ATCmd_PowerSave(stParam *param);

#ifdef	DEBUG_VERSION
////	Only for u debug use
extern int F_ATCmd_Flash_E(stParam *param);
extern int F_ATCmd_Flash_R(stParam *param);
extern int F_ATCmd_Flash_W(stParam *param);

extern int F_ATCmd_DbgVal(stParam *param);

	#ifdef	____D_UA_TIME_CLOCK_SUPPORT____
extern int F_ATCmd_ClockTimer_Add(stParam *param);
extern int F_ATCmd_ClockTimer_Del(stParam *param);
extern int F_ATCmd_ClockTimer_Init(stParam *param);
extern int F_ATCmd_ClockTimer_Print(stParam *param);
	#endif

extern int F_ATCmd_UACloud(stParam *param);

#endif

#ifdef	____D_ATCmd_UARTSendData____
extern int F_ATCmd_UARTSend(stParam *param);
#endif

#ifdef ____D_LED_4PWM____
extern int F_ATCmd_SET_4PWM(stParam *param);//add by ljing
#endif


void http_get_city_name();
void http_get_weather_from_yahoo();
void At_get_city();
void At_get_weather();
void GetHttpContent();
void PostHttpReq();
void tuya_test_intf();
void print_mem_pool_array();
void beep_test();
void ac_test_register();
void sk_print_timer_list();

//#ifdef	____D_UACLOUD_SUPPORT____
//PROCESS_NAME(F_Proc_UACloudAction);
//#endif

#ifdef	____D_UA_WIFI2UART_SUPPORT____
PROCESS_NAME(F_Proc_Wifi2Uart_Ctrl);
#endif

#ifdef	____D_Module_IO_Indicator____
PROCESS_NAME(F_Proc_Module_IO_Indicator);
#endif

void at_enter_smtlnk();
void test_protocol();
at_cmd_info atcmd_info_tbl[] =
{
/*
 *	no use this. --u
 *
	{"AT+CUSTOMER_CMD",	&AT_Customer},
	{"AT+CUSTOMER_CMD2",	&AT_Customer2},
 */
	{"AT+HIDE_MSG",&AT_HIDE_MSG},
	{"iperf",&AT_IPERF_CMD},
	//mqtt
#ifndef __AC_CLOUD_SUPPORT__	
	{"AT+MQTT_INIT",	&AT_MQTT_INIT},
	{"AT+MQTT_CONNECT",	&AT_MQTT_CONNECT},
    {"AT+MQTT_SSL_CONNECT",	&AT_MQTT_SSL_CONNECT},
	{"AT+MQTT_DISCONNECT",	&AT_MQTT_DISCONNECT},
	{"AT+MQTT_PUBLISH",	&AT_MQTT_PUBLISH},
	{"AT+MQTT_SUBSCRIBE",	&AT_MQTT_SUBSCRIBE},
	{"AT+MQTT_UNSUBSCRIBE",	&AT_MQTT_UNSUBSCRIBE},
	{"AT+MQTT_AUTH",	&AT_MQTT_AUTH},
#endif

	{"AT+IMEI",				&F_ATCmd_GetIMEI},
	{"AT+uVer",				&F_ATCmd_Get_uVer},

	{"AT+PowerSave",		&F_ATCmd_PowerSave},	//	AT+PowerSave=[0,1]	关开省电模式

////	内部用
	{"AT^IOTest",			&F_ATCmd_IOTest},

	{"AT^FMTest_TCPSend", 	&F_ATCmd_FMTest_TCPSend},
	{"AT^FMTest_Stop", 		&F_ATCmd_FMTest_Stop},
	{"AT^FMTest_GetResult", &F_ATCmd_FMTest_ReadResult},
	{"AT^FMTest_SetOpt", 	&F_ATCmd_FMTest_SetOpt},	//=Debug,1


	#ifdef	DEBUG_VERSION
	////	Only for u debug use
	{"AT^FlashE", 			&F_ATCmd_Flash_E},		//	4K HEX: AT^FlashE=addr
	{"AT^FlashR", 			&F_ATCmd_Flash_R},		//	4K HEX: AT^FlashR=addr,size
	{"AT^FlashW", 			&F_ATCmd_Flash_W},		//	4K HEX: AT^FlashW=addr,size,btVal
	{"AT^DbgVal", 			&F_ATCmd_DbgVal},		//	<! | ?>
													//	<index | ValName>, <? | val>

		#ifdef	____D_UA_TIME_CLOCK_SUPPORT____
	{"AT^TimerAdd", 		&F_ATCmd_ClockTimer_Add},		//	TimerAdd=<mode>,<Onoff>		mode = [1,4], Onoff=0|1
	{"AT^TimerDel", 		&F_ATCmd_ClockTimer_Del},		//	TimerDel[=id]		, 不带参数, 全部删除.
	{"AT^TimerInit", 		&F_ATCmd_ClockTimer_Init},		//	TimerInit=[unixtime],2016-09-14 19:04:31
	{"AT^TimerPrint", 		&F_ATCmd_ClockTimer_Print},
		#endif

		#ifdef	____D_ATCmd_UARTSendData____
	{"AT^UartSend", 		&F_ATCmd_UARTSend},		//	UartSend=ff0d0a0009

		#endif

#ifdef	____D_UACLOUD_SUPPORT____
	{"AT^UACloud", 			&F_ATCmd_UACloud},		//	UACloud=<dest>,<cmd>,<val>			<dest>: 0,1
#endif

	#endif
////
 #ifdef ____D_LED_4PWM____

	{"AT^SET_4PWM", &F_ATCmd_SET_4PWM},// add by lijing

#endif

#ifdef __HTTP_SUPPORT__
	{"AT+MYTEST1", PostHttpReq},	//qinjiangwei for test.
	{"AT+MYTEST", GetHttpContent},	//qinjiangwei for test.
#endif

#ifdef __TUYA_CLOUD_SUPPORT__
	{"AT+TUYA", at_enter_smtlnk},	//qinjiangwei for test.

	{"AT+MEM", print_mem_pool_array},	
#endif

#ifdef __WIFI_WEATHER__
	{"AT+Weather", At_get_weather},
#endif
#ifdef __BEEP_SUPPORT__
	{"AT+BEEP", beep_test},
#endif

#if defined(__AC_CLOUD_SUPPORT__)
	{"AT+ACTEST", ac_test_register},
#endif

#if defined(__GIZ_CLOUD_SUPPORT__)
	{"AT+GZTEST", &giz_at_cmd_hadle},	/*机智云测试指令*/
#endif

#ifdef __SK_TIMER_SUPPORT__
	{"AT+SKTIMER", &sk_print_timer_list},
#endif
    //
    {"",    NULL}
};

//extern void test1();
/*---------------------------------------------------------------------------*/
PROCESS(main_process, "main process");
/*---------------------------------------------------------------------------*/
PROCESS_NAME(tcpServerDemo_process);
PROCESS_NAME(wifiUartDemo_process);

////------------------------------------
PROCESS_NAME(F_Proc_MyInit);
PROCESS_NAME(F_Proc_MyInit_OK);
//PROCESS_NAME(F_Proc_uRanTest);
//PROCESS_NAME(F_Proc_uRanTest_Timer1);
//PROCESS_NAME(F_Proc_uRanTest_Timer2);
PROCESS_NAME(tcpServerDemo_process);
#ifdef	____D_UA_LED_Indicator____
PROCESS_NAME(F_Proc_LED_Indicator);
#endif
#ifdef __HTTP_SUPPORT__
PROCESS_NAME(http_poll_thread);
#endif


#ifdef  ____D_UA_KEY_DEMO_SUPPORT____
PROCESS_NAME(F_Proc_KEY);
//PROCESS_NAME(F_Proc_KEY_INIT);
#endif

#ifdef  ____D_UA_TIME_CLOCK_SUPPORT____
PROCESS_NAME(F_Proc_UA_TIME_CLOCK);
#endif


#if defined(____D_OBD_Uart2Wifi____) || defined(__HX_IOT_SUPPORT__) || defined(__HEKR_CLOUD_SUPPORT__) || defined(__TUYA_CLOUD_SUPPORT__)
PROCESS_NAME(wifiUartDemo_process);
PROCESS_NAME(hx_timer_checker);
#endif

#ifdef	____D_WDT_SUPPORT____
PROCESS_NAME(F_Proc_WDT);
#endif

#ifdef __TUYA_CLOUD_SUPPORT__
PROCESS_NAME(tuya_startup_process);
#endif

#ifdef __BEEP_SUPPORT__
PROCESS_NAME(beep_process);
#endif

#ifdef __SZCLOUD_SUPPORT__
PROCESS_NAME(szcloud_process);
#endif

#ifdef __AC_CLOUD_SUPPORT__
PROCESS_NAME(ac_startup_process);
#endif

#ifdef __GIZ_CLOUD_SUPPORT__
PROCESS_NAME(giz_cloud_main_process);
#endif

//----------------------------------------------
AUTOSTART_PROCESSES(&main_process
	, &F_Proc_MyInit
	#ifdef	____D_UA_LED_Indicator____
	, &F_Proc_LED_Indicator
	#endif
	//, &F_Proc_uRanTest
	//, &F_Proc_uRanTest_Timer1
	//, &F_Proc_uRanTest_Timer2
	#ifdef	____D_UACLOUD_SUPPORT____

		#if 0 //def __HTTP_SUPPORT__
	, &http_poll_thread
		#else
	, &F_Proc_UACloudAction
		#endif
	#endif

	#ifdef	____D_UA_WIFI2UART_SUPPORT____
	, &F_Proc_Wifi2Uart_Ctrl
	#endif

	#ifdef	____D_UA_LAMPGATE_SUPPORT____
	, &F_Proc_UALampGate
	#endif

	#ifdef	____D_UAWifiModule_Demo____
	,&tcpServerDemo_process
	#endif
	
#ifdef  ____D_UA_KEY_DEMO_SUPPORT____
	//,&F_Proc_KEY_INIT
	,&F_Proc_KEY
#endif

	#if (defined(____D_OBD_Uart2Wifi____) || defined(__HX_IOT_SUPPORT__) || defined(__HEKR_CLOUD_SUPPORT__) || defined(__TUYA_CLOUD_SUPPORT__) || defined(__SZCLOUD_SUPPORT__))
	,&wifiUartDemo_process
	#endif

	#if defined(__CUSTOMER_HOMI_IOT__) || defined(__HEKR_CLOUD_SUPPORT__) || defined(__HX_IOT_SUPPORT__)
	,&hx_timer_checker
	#endif

    #ifdef  ____D_UA_TIME_CLOCK_SUPPORT____
    ,&F_Proc_UA_TIME_CLOCK
    #endif

    #ifdef  ____D_Module_IO_Indicator____
    ,&F_Proc_Module_IO_Indicator
    #endif

    #ifdef  ____D_CLOUDMODULE_SUPPORT____
    ,&F_Proc_UACloudModule_UartHdl
    #endif

	////	This process should be the last one.
	#ifdef	____D_WDT_SUPPORT____
	,&F_Proc_WDT
	#endif

#ifdef __TUYA_CLOUD_SUPPORT__
	,&tuya_startup_process
#endif

#ifdef __BEEP_SUPPORT__
//	, &beep_process
#endif

#ifdef __SZCLOUD_SUPPORT__
	,&szcloud_process
#endif

#ifdef __AC_CLOUD_SUPPORT__
	,&ac_startup_process
#endif

#ifdef __GIZ_CLOUD_SUPPORT__
	,&giz_cloud_main_process
#endif

	, &F_Proc_MyInit_OK
);
/*---------------------------------------------------------------------------*/
/*
 *	no use this. --u
 *
 void TurnOffAllLED()
 {
	 GPIO_CONF conf;

	 conf.dirt = OUTPUT;
	 conf.driving_en = 0;
	 conf.pull_en = 0;

	 pinMode(GPIO_1, conf);
	 digitalWrite(GPIO_1, 0);
	 pinMode(GPIO_3, conf);
	 digitalWrite(GPIO_3, 0);
	 pinMode(GPIO_8, conf);
	 digitalWrite(GPIO_8, 0);

	 return;
 }
 */

/*---------------------------------------------------------------------------*/
/*
 *	no use this. --u
 *
 int SetLED (uint8_t nEnable)
 {
 	GPIO_CONF conf;

	conf.dirt = OUTPUT;
	conf.driving_en = 0;
	conf.pull_en = 0;

	pinMode(GPIO_1, conf);
 	if(nEnable == 0)
 	{
 		digitalWrite(GPIO_1, 0);
 	}
 	else
 	{
 		digitalWrite(GPIO_1, 1);
 	}
 	return ERROR_SUCCESS;
 }

int AT_Customer(stParam *param)
{
	printf("Call AT_Customer\n");
	return 0;
}
int AT_Customer2(stParam *param)
{
	int i = 0;
	printf("Call AT_Customer2\n");
	for(i=0; i<param->argc; i++)
	{
		printf("Param%d:%s\n", i+1, param->argv[i]);
	}
	return 0;
}
 */

int AT_HIDE_MSG(stParam *param)
{
	if(strcmp(param->argv[0] ,"1") == 0)
	{
		set_log_single_group_level(GROUP_ENCPT,LOG_DISABLE,LOG_INFO);
		set_log_single_group_level(GROUP_MGM,LOG_DISABLE,LOG_INFO);
		set_log_single_group_level(GROUP_NET,LOG_DISABLE,LOG_INFO);
		set_log_single_group_level(GROUP_TCPIP,LOG_DISABLE,LOG_INFO);
		set_log_single_group_level(GROUP_ATCMD,LOG_ENABLE,LOG_WARN);

		set_log_all_group_disable();
	}
	else if(strcmp(param->argv[0] ,"0") == 0)
	{
		set_log_single_group_level(GROUP_ENCPT,LOG_ENABLE,LOG_INFO);
		set_log_single_group_level(GROUP_MGM,LOG_ENABLE,LOG_INFO);
		set_log_single_group_level(GROUP_NET,LOG_ENABLE,LOG_INFO);
		set_log_single_group_level(GROUP_TCPIP,LOG_ENABLE,LOG_INFO);
		set_log_single_group_level(GROUP_ATCMD,LOG_ENABLE,LOG_INFO);

	}
	return 0;
}

#define CUSTOMER_NAME(x) (#x)
#define CUSTOMER_NAME_HELP(x) (CUSTOMER_NAME(x))

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(main_process, ev, data)
{
    //    EVENTMSG_DATA *pMesg = NULL;
    PROCESS_BEGIN();

	printf("Customer:%s,Version:%s@%s,HW:%s\r\n", CUSTOMER_NAME_HELP(__CUSTOMER__), S_Ver.sw_version, S_Ver.szBuildTime,
#ifdef ____UA_POWERSAVE_USE_DCDC_SUPPORT____
	"dc"
#else
	"ldo"
#endif
);
#ifdef	____D_OBD_Uart2Wifi____
{
	softap_init_ex2(192, 168, 0, 4, 0, 0, 0, 1, 50);
	softap_start();
}


#endif

    //printf("********hello main_process********\n");
    //test1();
    

    PROCESS_END();
}





