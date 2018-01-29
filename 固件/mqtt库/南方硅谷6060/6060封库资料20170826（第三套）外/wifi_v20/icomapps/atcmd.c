
//standard libary include
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ssv_config.h"

//contiki include
#include "contiki.h"
#include "contiki-net.h"
#include "contiki-lib.h"
#include "cfs/cfs.h"
#include "cfs/cfs-coffee.h"

#include "sys/process.h"
#include "sys/etimer.h"
#include "sys/clock.h"
//#include "sys/log.h"

#include "uip.h"
#include "tcpip.h"
#include "dhcpc.h"
#include "uip_arp.h"
#include "mac.h"

//icomm low level include (????????????)
#include <drv_uart.h>
#include "drv_phy.h"
#include <drv_pmu.h>

#include <gpio_api.h>
#include "pwm_api.h"
#include "swpwm.h"

#include "sys_status_api.h"
#include "console.h"
#include "dbg-atcmd-api.h"
#include "systemconf.h"
#include "rate_control.h"

//icomm high level include
#include "socket_app.h"
#include "socket_driver.h"
#include "flash_api.h"
#include "wdog_api.h"
#include "soft_ap_init.h"
#include "soft_ap.h"
#include "ota_api.h"
#include "smartComm.h"
#include "test_entry.h"

#include "Cabrio-conf.h"
#include "atcmd.h"
#include "atcmd_icomm.h"  //these are all icomm-semi only debug command!!
#include "dbg-printf.h"
#include "atcmdlib.h"
#include "send_raw_packet.h"
#include "cloud_api.h"
#include "ieee80211_mgmt.h"
#include "ServerCmd.h"
//#define CUSTOM_HYNEHOMI

#ifdef CUSTOM_HYNEHOMI
#include "uart_cmd.h"
#endif
//#ifdef	____D_WDT_SUPPORT____
#include	"UA_Func.h"
//#endif

extern void dbg_dump_binary(U32 dump_addr,S32 length);
extern void cfs_dump_info();
int icomm_cfs_dump_file_info(const char *name);
extern void add_ip_to_table(const char *host, kal_uint8 ip[]);


static uint8_t bTXRun = 0;
static uint8_t ghttprun = 0;
static char *buffer_in_dynamic = NULL;
//static char *buffer_out_dynamic = NULL;
char *buffer_out_dynamic = NULL;


U32 testcnt = 0;
U32 successcnt = 0;
#ifndef __SZCLOUD_SUPPORT__
int gclientsock = -1;
int gserversock = -1;
int gudpsock = -1;
#endif

extern TAG_NET_STATUS gNetStatus;
extern IEEE80211STATUS gwifistatus;

#define RATE_NUM 39
static char *dataRateMapping[RATE_NUM] =
{
"11b 1M",
"11b 2M",
"11b 5.5M",
"11b 11M",
"11b 2M SP",
"11b 5.5M SP",
"11b 11M SP",
"NON-HT 6M",
"NON-HT 9M",
"NON-HT 12M",
"NON-HT 18M",
"NON-HT 24M",
"NON-HT 36M",
"NON-HT 48M",
"NON-HT 54M",
"HT-MM MCS0",
"HT-MM MCS1",
"HT-MM MCS2",
"HT-MM MCS3",
"HT-MM MCS4",
"HT-MM MCS5",
"HT-MM MCS6",
"HT-MM MCS7",
"HT-MM SGI MCS0",
"HT-MM SGI MCS1",
"HT-MM SGI MCS2",
"HT-MM SGI MCS3",
"HT-MM SGI MCS4",
"HT-MM SGI MCS5",
"HT-MM SGI MCS6",
"HT-MM SGI MCS7",
"HT-GF MCS0",
"HT-GF MCS1",
"HT-GF MCS2",
"HT-GF MCS3",
"HT-GF MCS4",
"HT-GF MCS5",
"HT-GF MCS6",
"HT-GF MCS7"
};


//dns
static char hostname[128] = {0};
extern at_cmd_info atcmd_info_tbl[];

/*---------------------------------------------------------------------------*/
//PROCESS(Cabrio_ate_process, "Cabrio_ate_process");
/*---------------------------------------------------------------------------*/
PROCESS_NAME(Cabrio_ate_process);   //This process is used for uart receiver!!
/*---------------------------------------------------------------------------*/

#ifndef __SZCLOUD_SUPPORT__
PROCESS(tcp_connect_process, "Tcp Connect Process");
#endif

PROCESS(poll_tx_process, "Poll TX Process");
PROCESS(nslookup_process, "NSLookup Process");
PROCESS(http_request_process, "http requset Process");
PROCESS(rf_event_process, "rf event Process");

/*---------------------------------------------------------------------------*/
PROCESS_NAME(bss_reconnect_process2);
#if (ENABLE_ICOMM_DEMO==1)
PROCESS_NAME(userSmartDemo_process);
PROCESS_NAME(tcpServerDemo_process);
PROCESS_NAME(wifiUartDemo_process);
PROCESS_NAME(sta_connect_demoProcess);
#endif

/*---------------------------------------------------------------------------*/

S32 allocate_buffer_in()
{
    S32 rlt = -1;
    if(NULL==buffer_in_dynamic)
    {
        buffer_in_dynamic = malloc(MAX_SEND_BUFFER);
        memset(buffer_in_dynamic, 0x0, MAX_SEND_BUFFER);
        rlt = 0;
    }

    return rlt;
}

S32 release_buffer_in()     //not necessary to call this  yet!!
{
    S32 rlt = -1;
    if(buffer_in_dynamic != NULL)
    {
        free(buffer_in_dynamic);
        buffer_in_dynamic = NULL;
        rlt  = 0;
    }
    return rlt;
}


S32 allocate_buffer_out()
{
    S32 rlt = -1;
    if(NULL==buffer_out_dynamic)
    {
        buffer_out_dynamic = malloc(MAX_SEND_BUFFER);
        memset(buffer_out_dynamic, 0x0, MAX_SEND_BUFFER);
        rlt = 0;
    }

    return rlt;
}

S32 release_buffer_out()        //not necessary to call this  yet!!
{
    S32 rlt = -1;
    if(buffer_out_dynamic != NULL)
    {
        free(buffer_out_dynamic);
        buffer_out_dynamic = NULL;
        rlt  = 0;
    }
    return rlt;
}


int parseBuff2Param(char* bufCmd, stParam* pParam, U8 maxargu)
{
	int buflen, i;
	const char delimiters = ',';

    buflen = strlen (bufCmd);
	if (strlen (bufCmd) == 0)
		return ERROR_INVALID_PARAMETER;

    if(maxargu == 0)
        maxargu = MAX_ARGUMENT;

    pParam->argc = 1;
    pParam->argv[pParam->argc - 1] = &bufCmd[0];
    i = 0;
    while(pParam->argc < maxargu)
    {
        for(; i < buflen ; i++)
        {
            if(bufCmd[i] == delimiters)
            {
                bufCmd[i] = 0;
                break;
            }
        }

        if(bufCmd[i + 1] != 0)
        {
            pParam->argc += 1;
            pParam->argv[pParam->argc - 1] = &bufCmd[i + 1];
        }
        else
            break;
    }

	return 0;
}
int At_EnableSmartReboot(stParam *param)
{
    S32 rlt = 0;

    printf ("[%s] : +++\n",__func__);
    if(*(param->argv[0]) == '0') {
        gconfig_set_enable_smart(NOSMART, 0);
    } else if(*(param->argv[0]) == '1'){
        gconfig_set_enable_smart(ICOMM, 0);
    } else if(*(param->argv[0]) == '2'){
        gconfig_set_enable_smart(WECHAT, 0);
    } else if(*(param->argv[0]) == '3'){
        gconfig_set_enable_smart(USER, 0);
    } else if(*(param->argv[0]) == '4'){
        gconfig_set_enable_smart(JDSMART, 0);
    } else {
        atcmdprintf("please enter range 0 to 4\n");
    }

    rlt = gconfig_save_config();
    if(rlt!=0){
        atcmdprintf("<Error>gconfig_save_config failed!!\n");
    }
    rlt = remove_sysconfig();
    if(rlt!=0){
        atcmdprintf("<Error>systemconf remove failed!!\n");
    }

	#ifdef	____D_WDT_SUPPORT____
	F_WDT_EnableManualReboot();
	#endif
    api_wdog_reboot();

    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_EnableSmartMode(stParam *param)
{
    S32 rlt = 0;

    atcmdprintf("[%s] : +++\n",__func__);

    if(*(param->argv[0]) == '0') {
        gconfig_set_enable_smart(NOSMART, 1);
    } else if(*(param->argv[0]) == '1'){
        gconfig_set_enable_smart(ICOMM, 1);
    } else if(*(param->argv[0]) == '2'){
        gconfig_set_enable_smart(WECHAT, 1);
    } else if(*(param->argv[0]) == '3'){
        gconfig_set_enable_smart(USER, 1);
    } else if(*(param->argv[0]) == '4'){
        gconfig_set_enable_smart(JDSMART, 1);
    } else {
        atcmdprintf("please enter range 0 to 4\n");
    }

    rlt = gconfig_save_config();
    if(rlt!=0){
        atcmdprintf("<Error>gconfig_save_config failed!!\n");
    }

    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
char* showDataRate(int idx)
{
	if(idx >=0 && idx <= 38 )
		return dataRateMapping[idx];
	else
		return "Unknown";
}
/*---------------------------------------------------------------------------*/
const char *securityString(U8 ver)
{
    switch(ver) {
    case NONE:
        return "OPEN";
        break;
    case WEP:
        return "WEP";
        break;
    case WPA:
        return "WPA";
        break;
    case WPA2:
        return "WPA2";
        break;
    case WPAWPA2:
        return "WPAWPA2";
        break;
    default:
        return "unknown security type";
    }
    return NULL;
}
/*---------------------------------------------------------------------------*/
const char *securitySubString(U8 ver)
{
    switch(ver) {
    case NOSECTYPE:
        return "NONE";
        break;
    case TKIP:
        return "TKIP";
        break;
    case CCMP:
        return "AES";
        break;
    case TKIP_CCMP:
        return "TKIP_CCMP";
        break;
    default:
        return "unknown security type";
    }
    return NULL;
}
/*---------------------------------------------------------------------------*/
int At_Reboot (stParam *param)
{
	#ifdef	____D_WDT_SUPPORT____
	F_WDT_EnableManualReboot();
	#endif

    api_wdog_reboot();

	return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_WatchDOG_Start(stParam *param)
{
    atcmdprintf("<%s> continue to kick watch dog in 5000ms or will be rebooted\n",__func__);

    api_wdog_period_5000ms();
    //api_wdog_period_10000ms();
    //api_wdog_period_cutommed(????);

    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_WatchDOG_Stop(stParam *param)
{
    atcmdprintf("<%s> stop watch dog\n",__func__);

    api_wdog_stop();

    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int
At_ProcessQueKill(stParam *param)
{
    char *buff = param->argv[0];

    if(*buff == 'a') {
        process_para.event = PROCESS_ALL_QUE_KILL;
        process_para.num = 0;
    } else {
        process_para.event = PROCESS_SIGNAL_QUE_KILL;
        *(buff + 1) = '\0';
        process_para.num = atoi(buff);
    }
    atcmdprintf("process event: %d num: %d\n", process_para.event, process_para.num);
    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int
At_ProcessKill(stParam *param)
{
    char *buff = param->argv[0];

    if(*buff == 'a') {
        process_para.event = PROCESS_ALL_KILL;
        process_para.num = 0;
    } else {
        process_para.event = PROCESS_SIGNAL_KILL;
        *(buff + 1) = '\0';
        process_para.num = atoi(buff);
    }
    atcmdprintf("process event: %d num: %d\n", process_para.event, process_para.num);
    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int
At_ProcessAllQueDump(stParam *param)
{
    process_para.event = PROCESS_ALL_QUE_DUMP;
    process_para.num = 0;
    atcmdprintf("process event: %d num: %d\n", process_para.event, process_para.num);
    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int
At_ProcessAllDump(stParam *param)
{
    process_para.event = PROCESS_ALL_DUMP;
    process_para.num = 0;
    atcmdprintf("process event: %d num: %d\n", process_para.event, process_para.num);
    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_ShowNtpTime(stParam *param)
{
    unsigned long time;
    time = get_ntp_time();
    atcmdprintf("get time: %d sec\n", time);
    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int
At_ShowAllAP (stParam *param)
{
    int i;
    U8 apcnt;
    TAG_AP_INFO *aplist;

    apcnt = get_ap_lsit_total_num();
    if (apcnt > 0) {
        aplist = malloc(apcnt * sizeof(TAG_AP_INFO));
        if(aplist)
        {
            get_ap_list(aplist, &apcnt);
            for (i=0; i<apcnt; i++) {
                atcmdprintf ("[%d]%s\n",i, aplist[i].name);
                atcmdprintf ("        ch: %2d", aplist[i].channel);
                atcmdprintf (", rssi: -%3d dBm", aplist[i].rssi);
                atcmdprintf (", rssiLevel: %d, ", aplist[i].rssi_level);
                atcmdprintf (", mac:  %x:%x:%x:%x:%x:%x", aplist[i].mac[0],aplist[i].mac[1],aplist[i].mac[2],
                                                     aplist[i].mac[3],aplist[i].mac[4],aplist[i].mac[5]);
                atcmdprintf (", security_type = %s", securityString(aplist[i].security_type));
                atcmdprintf ("/%s\r\n", securitySubString(aplist[i].security_subType));
            }
            free(aplist);
        }
    }
    else {
        atcmdprintf ("please scan ap then you can show all ap info\r\n");
    }
    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int
At_ShowConnectAP (stParam *param)
{
    TAG_AP_INFO apinfo;

    if (get_wifi_connected() == 1) {
        get_connected_ap_info(&apinfo);
        atcmdprintf ("%s", apinfo.name);
        atcmdprintf (", ch: %d", apinfo.channel);
        atcmdprintf (", rssi: -%d dBm", apinfo.rssi);
        atcmdprintf (", rssiLevel: %d, ", apinfo.rssi_level);
        atcmdprintf (", mac: %x:%x:%x:%x:%x:%x", apinfo.mac[0],apinfo.mac[1],apinfo.mac[2],
                                         apinfo.mac[3],apinfo.mac[4],apinfo.mac[5]);
        atcmdprintf (", security_type = %s", securityString(apinfo.security_type));
        atcmdprintf ("/%s ", securitySubString(apinfo.security_subType));
        atcmdprintf (", %s\r\n", showDataRate(get_current_rate()) );
    } else {
        atcmdprintf ("please connect ap\r\n");
    }

    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int
At_SetGpio (stParam *param)
{
	char *pid = 0, *pmode = 0;
	GPIO_CONF conf;

	//printf ("At_SetGpio +++\n");
	if (param->argc < 2) {
		return ERROR_INVALID_PARAMETER;
	}

	pid = param->argv[0];
	pmode = param->argv[1];

	if (atoi(pid) > GPIO_20 || atoi(pmode) > OUTPUT) {
		return ERROR_INVALID_PARAMETER;
	}

	conf.dirt = atoi(pmode);
	conf.driving_en = 0;
	conf.pull_en = 0;

	pinMode(atoi(pid), conf);

	return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int
At_WriteGpio (stParam *param)
{
	char *pid = 0, *pdata = 0;

	if (param->argc < 2) {
		return ERROR_INVALID_PARAMETER;
	}

	pid = param->argv[0];
	pdata = param->argv[1];

	if (atoi(pid) > GPIO_20) {
		return ERROR_INVALID_PARAMETER;
	}

	digitalWrite(atoi(pid), atoi(pdata));

	return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int
At_ReadGpio (stParam *param)
{
	char *pid = 0;
	U32 data;

	if (param->argc < 1) {
		return ERROR_INVALID_PARAMETER;
	}

	pid = param->argv[0];

	if (atoi(pid) > GPIO_20) {
		return ERROR_INVALID_PARAMETER;
	}

	data = digitalRead(atoi(pid));

	atcmdprintf (ATIND_GPIO, data);

	return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_SetPWMspecial (stParam *param)
{
	char *bEnable = 0, *cnt = 0;

	if (param->argc < 2) {
		return ERROR_INVALID_PARAMETER;
	}

	bEnable = param->argv[0];
	cnt = param->argv[1];

	if(atoi(bEnable) == 0)
	{
#if CHIP_SEL == CHIP_NEW
        disableHwPWM(PWM_A);
#else
        disableHwPWM();
#endif
	}
	else
	{
		if (cnt == NULL)
		{
			return ERROR_INVALID_PARAMETER;
		}
		enableHwPWMspecial(atoi(cnt));
	}

	return ERROR_SUCCESS;}
/*---------------------------------------------------------------------------*/
int
At_SetPWM (stParam *param)
{
	char *bEnable = 0, *nCycle = 0, *pid = 0;

	if (param->argc < 3) {
		return ERROR_INVALID_PARAMETER;
	}

	pid = param->argv[0];
	bEnable = param->argv[1];
	nCycle = param->argv[2];

	//soft
	if(atoi(pid) == GPIO_8)
	{
    	if(atoi(bEnable) == 0)
    	{
#if CHIP_SEL == CHIP_NEW
    		disableHwPWM(PWM_A);
#else
            disableHwPWM();
#endif
    	}
    	else
    	{
    		if (atoi(nCycle) < 0 || atoi(nCycle) > 10)
    		{
    			return ERROR_INVALID_PARAMETER;
    		}
#if CHIP_SEL == CHIP_NEW
            enableHwPWM(PWM_A, atoi(nCycle));
#else
    		enableHwPWM(atoi(nCycle));
#endif
    	}
	}
	else
	{
		if(atoi(bEnable) == 0)
		{
			disableSwPWM(atoi(pid));
		}
		else
		{
			if (atoi(nCycle) < 0 || atoi(nCycle) > 100)
			{
				return ERROR_INVALID_PARAMETER;
			}
			enableSwPWM(atoi(pid), atoi(nCycle));
		}
	}

	return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int AT_SetDebugInfo(stParam *param)
{
    char *group_id = 0,*log_level = 0;

    char *bEnable = 0;

    if (param->argc < 3) {
        return ERROR_INVALID_PARAMETER;
    }

    group_id = param->argv[0];
    bEnable = param->argv[1];
    log_level = param->argv[2];

    if((atoi(group_id) > GROUP_MAX)||(atoi(log_level) > MAX_LOG_TYPE_NUM))
    {
        return ERROR_INVALID_PARAMETER;
    }

    set_log_single_group_level(atoi(group_id),atoi(bEnable),atoi(log_level));

    return ERROR_SUCCESS;
}

/*---------------------------------------------------------------------------*/
/* Receive RX data from UART */
int
At_ParserRecv (unsigned char * buff, int len)
{
	EVENTMSG_DATA	msg;

	/* Handle received data with null end*/
	*(buff+len) = 0;

	msg.msgid = MSG_ATE_RXRECV;
	msg.data_len = len;
	memcpy ((unsigned char *)msg.data,  buff, len);

	process_post (&Cabrio_ate_process, PROCESS_EVENT_CONTINUE, &msg);

	return ERROR_SUCCESS;
}
#if CURRENT_TEST
int
At_PowerOn (stParam *param)
{
	bTXRun = 0;
	Radio_Command(RX_DISABLE, "1");
	printf (ATRSP_OK);
	return ERROR_SUCCESS;
}
int
At_RX (stParam *param)
{
	bTXRun = 0;
	Radio_Command(RX_DISABLE, "0");
	printf (ATRSP_OK);
	return ERROR_SUCCESS;
}
int
At_TX (stParam *param)
{
	process_start(&poll_tx_process, NULL);
	bTXRun = 1;
	process_post (&poll_tx_process, PROCESS_EVENT_CONTINUE, NULL);
	printf (ATRSP_OK);
	return ERROR_SUCCESS;
}
int
At_PowerSaving (stParam *param)
{
	char *nSec = 0;

	bTXRun = 0;

	if (param->argc < 1)
	{
		//printf (ATRSP_ERROR, ERROR_INVALID_PARAMETER);
		return ERROR_INVALID_PARAMETER;
	}

	nSec = param->argv[0];

	if(nSec == 0)
		return ERROR_INVALID_PARAMETER;
	atcmdprintf ("Sleep %d sec\n", atoi(nSec));
	//sleep
	drv_pmu_setwake_cnt(atoi(nSec)*100000);
	//drv_pmu_setwake_cnt2(atoi(nSec));
	drv_pmu_init();
	drv_pmu_sleep();
	drv_pmu_chk();
	if(drv_pmu_check_interupt_event() == 1)
		atcmdprintf("pmu check interupt wakeup!\n");

	atcmdprintf (ATRSP_OK);
	return ERROR_SUCCESS;
}
#endif
/*---------------------------------------------------------------------------*/
int At_TCPChange_Timeout(stParam *param)
{
	u16_t time_val;

    if (param->argc < 1) {
        return ERROR_INVALID_PARAMETER;
    }

	time_val = strtoul(param->argv[0], NULL, 10);
	if(time_val > 0 && time_val <= 1000) {
		atcmdprintf("time_val: %d\n", time_val);
		set_tcp_timeout(time_val);
	} else {
		atcmdprintf("over tcp time out range\n");
	}
	return ERROR_SUCCESS;
}

#ifdef CUSTOM_HYNEHOMI
static Boolean heny_tcp_connect = FALSE;
static Boolean heny_udp_connect = FALSE;
static int heny_udp_sock;

#endif
/*---------------------------------------------------------------------------*/
int At_TCPConnect (stParam *param)
{
	char *pIp = 0, *pport = 0;

	if (  (get_wifi_connected() != 1) && (get_softap_sta_connected() != 1) )
		return ERROR_WIFI_CONNECTION;

#ifndef __SZCLOUD_SUPPORT__
	if (gclientsock >= 0)
	{
		atcmdprintf("TCP already Connect\n");
		return ERROR_TCP_CONNECTION;
	}
#endif

    if (param->argc < 2) {
        return ERROR_INVALID_PARAMETER;
    }

	pIp = param->argv[0];
	pport = param->argv[1];

	gNetStatus.port = atoi (pport);
	if (gNetStatus.port == 0) {
		return ERROR_INVALID_PARAMETER;
	}

	if( uiplib_ipaddrconv(pIp, &gNetStatus.remote_ip_addr)  == 0)
	{
		U8 *ptr;

		atcmdprintf("DNS format : %s\n", pIp);

#ifdef CUSTOM_HYNEHOMI
		ptr = get_ip_from_cache(pIp);
		if (!ptr)
		{
			heny_tcp_connect = TRUE;
			process_start(&nslookup_process, NULL);
			set_nslookup(pIp);

			return ERROR_SUCCESS_NO_RSP;
		}
		else
		{
			memcpy(&gNetStatus.remote_ip_addr, ptr, sizeof(uip_ip4addr_t));
		}
#endif
	}
	//else
	{
		atcmdprintf("IP format : %s\n", pIp);
#ifndef __SZCLOUD_SUPPORT__		
        gclientsock = tcpconnect(&gNetStatus.remote_ip_addr, gNetStatus.port, &tcp_connect_process);
		atcmdprintf("create socket:%d\n", gclientsock);
#endif		
	}

	return ERROR_SUCCESS_NO_RSP;
}
/*---------------------------------------------------------------------------*/
int At_TCPSend (stParam *param)
{
	char *pdata = 0;
	int sock;

    if (param->argc < 2) {
        return ERROR_INVALID_PARAMETER;
    }

	sock = atoi(param->argv[0]);
	pdata = param->argv[1];

	if (sock < 0)
	{
		atcmdprintf("socket error:%d\n", sock);
		return ERROR_TCP_CONNECTION;
	}

    allocate_buffer_out();

    memcpy(buffer_out_dynamic, pdata, strlen (pdata));
	if(tcpsend(sock, buffer_out_dynamic, strlen (pdata)) == -1)
	{
		return ERROR_TCP_CONNECTION;
	}
	else
	{
		return ERROR_SUCCESS_NO_RSP;
	}
}
int At_TCPDisconnect (stParam *param)
{
	int sock;

    if (param->argc < 1) {
        return ERROR_INVALID_PARAMETER;
    }

	sock = atoi(param->argv[0]);

	if(tcpclose(sock) == -1)
	{
		return ERROR_TCP_CONNECTION;
	}
	else
	{
		return ERROR_SUCCESS_NO_RSP;
	}
}
int At_TCPSendCurr (stParam *param)
{
#ifndef __SZCLOUD_SUPPORT__
	if(gclientsock < 0)
		return ERROR_TCP_CONNECTION;
#endif

    if (param->argc < 1) {
        return ERROR_INVALID_PARAMETER;
    }

	allocate_buffer_out();
	memcpy(buffer_out_dynamic, param->argv[0], strlen (param->argv[0]));

#ifndef __SZCLOUD_SUPPORT__	
	if(tcpsend(gclientsock, buffer_out_dynamic, strlen (param->argv[0])) == -1)
	{
		return ERROR_TCP_CONNECTION;
	}
	else
#endif			
	{
		return ERROR_SUCCESS_NO_RSP;
	}

}

int At_TCPDisconnectCurr(stParam *param)
{
#ifndef __SZCLOUD_SUPPORT__
 	if(gclientsock < 0)
		return ERROR_TCP_CONNECTION;

	if(tcpclose(gclientsock) == -1)
	{
		return ERROR_TCP_CONNECTION;
	}
	else
#endif		
	{
		return ERROR_SUCCESS_NO_RSP;
	}
}
/*---------------------------------------------------------------------------*/
int At_TCPListen (stParam *param)
{
	char  *pport = 0;

    if (param->argc < 1) {
        return ERROR_INVALID_PARAMETER;
    }

	pport = param->argv[0];

	gNetStatus.listenport = strtoul(pport,NULL,10);

	if (gNetStatus.listenport == 0) {
		return ERROR_INVALID_PARAMETER;
	}

#ifndef __SZCLOUD_SUPPORT__	
	if(tcplisten(gNetStatus.listenport, &tcp_connect_process) == -1)
	{
		atcmdprintf("listen tcp fail\n");
		return ERROR_TCP_CONNECTION;
	}
	else
	{
		return ERROR_SUCCESS;
	}
#endif

}
/*---------------------------------------------------------------------------*/
int At_TCPUnlisten (stParam *param)
{
	char  *pport = 0;

	if (get_wifi_connected() != 1)
		return ERROR_WIFI_CONNECTION;

    if (param->argc < 1) {
        return ERROR_INVALID_PARAMETER;
    }

	pport = param->argv[0];

	gNetStatus.listenport = atoi (pport);
	if (gNetStatus.listenport == 0) {
		return ERROR_INVALID_PARAMETER;
	}

	tcpunlisten(gNetStatus.listenport);

	return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_UDPCreate (stParam *param)
{
	char  *pport = 0;

	if ( get_wifi_connected() != 1)
		return ERROR_WIFI_CONNECTION;

    if (param->argc < 1) {
        return ERROR_INVALID_PARAMETER;
    }

	pport = param->argv[0];

	gNetStatus.udpport = atoi (pport);
	if (gNetStatus.udpport == 0) {
		return ERROR_INVALID_PARAMETER;
	}

#ifndef __SZCLOUD_SUPPORT__	
	gudpsock = udpcreate(gNetStatus.udpport, &tcp_connect_process);

	if(gudpsock == -1)
	{
		atcmdprintf("create udp socket fail\n");
		return ERROR_UDP_CONNECTION;
	}
	else
	{
		atcmdprintf("create socket:%d\n", gudpsock);
		gNetStatus.udp_connect = 1;
		return ERROR_SUCCESS;
	}
#endif

}
/*---------------------------------------------------------------------------*/
int At_UDPSend (stParam *param)
{
	int sock;
	char *pIp = 0, *pport = 0, *psock = 0, *pdata = 0;

    if (param->argc < 4) {
        return ERROR_INVALID_PARAMETER;
    }

	psock = param->argv[0];
	pIp = param->argv[1];
	pport = param->argv[2];
	pdata = param->argv[3];

	gNetStatus.port = atoi (pport);
	if (gNetStatus.port == 0) {
		return ERROR_INVALID_PARAMETER;
	}

	sock = atoi(psock);

	//	if (ERROR_SUCCESS != ip2int(pIp, &gNetStatus.remote_ip_addr))
	if ( uiplib_ipaddrconv(pIp, &gNetStatus.remote_ip_addr)  == 0)
	{
#ifdef CUSTOM_HYNEHOMI
		U8 *ptr;

		ptr = get_ip_from_cache(pIp);
		if (!ptr)
		{
			heny_udp_connect = TRUE;

			allocate_buffer_out();
    		memcpy(buffer_out_dynamic, pdata, strlen (pdata));
			heny_udp_sock = sock;

			process_start(&nslookup_process, NULL);
			set_nslookup(pIp);
			return ERROR_SUCCESS_NO_RSP;
		}
		else
		{
			memcpy(&gNetStatus.remote_ip_addr, ptr, sizeof(uip_ip4addr_t));
		}
#else
		return ERROR_INVALID_PARAMETER;
#endif
	}

	if(udpsendto(sock, pdata, strlen(pdata), &gNetStatus.remote_ip_addr, gNetStatus.port) == -1)
	{
		return ERROR_UDP_CONNECTION;
	}
	else
	{
		return ERROR_SUCCESS;
	}
}
/*---------------------------------------------------------------------------*/
int At_UDPClose (stParam *param)
{
	int sock;

    if (param->argc < 1) {
        return ERROR_INVALID_PARAMETER;
    }

	sock = atoi(param->argv[0]);

	if(udpclose(sock) == -1)
	{
		return ERROR_UDP_CONNECTION;
	}
	else
	{
#ifndef __SZCLOUD_SUPPORT__	
		gudpsock = -1;
#endif
		gNetStatus.udp_connect = 0;
		return ERROR_SUCCESS;
	}

}
/*---------------------------------------------------------------------------*/
int At_HTTPTest (stParam *param)
{
	char *pIp = 0, *pport = 0, *pbuff = 0;

	if (get_wifi_connected() != 1)
		return ERROR_WIFI_CONNECTION;

    if (param->argc < 3) {
        return ERROR_INVALID_PARAMETER;
    }

	pIp = param->argv[0];
	pport = param->argv[1];
	pbuff = param->argv[2];

	gNetStatus.port = atoi (pport);
	if (gNetStatus.port == 0) {
		return ERROR_INVALID_PARAMETER;
	}

    allocate_buffer_out();

	if( uiplib_ipaddrconv(pIp, &gNetStatus.remote_ip_addr)  == 0 )
	{
		atcmdprintf("DNS format : %s\n", pIp);
		set_nslookup(pIp);
	}
	else
	{
		atcmdprintf("IP format : %s\n", pIp);
	}

    memset(buffer_out_dynamic, 0, 256);
    strcat(buffer_out_dynamic, "GET /cgi-bin/test.pl?param1=\"");
//	strcat(buffer_out, "GET /cgi-bin/test.cgi?param1=\"");
    strcat(buffer_out_dynamic, pbuff);
	strcat(buffer_out_dynamic, "\" HTTP/1.0\r\nHost:\r\n\r\n");

	ghttprun = 1;
	process_start(&http_request_process, NULL);

	return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_HTTPTestStop(stParam *param)
{
	ghttprun = 0;
	return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_NSLookup(stParam *param)
{
	memset(hostname, 0, 128);
	memcpy(hostname, param->argv[0], strlen(param->argv[0]));

	process_start(&nslookup_process, NULL);
	set_nslookup(hostname);
	return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_SET_AP_SSID (stParam *param)
{
    icomprintf(GROUP_ATCMD,LOG_INFO,"[%s] : +++\n",__func__);
    char *p_ssid = NULL;

    p_ssid = param->argv[0];

    gconfig_set_softap_ssid(p_ssid);

    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_SET_AUTO_CONNECT (stParam *param)
{
    icomprintf(GROUP_ATCMD,LOG_INFO,"[%s] : +++\n",__func__);
    U32 auto_connect = 0;
    S32 rlt =0;

    if (param->argc < 1) {
        return ERROR_INVALID_PARAMETER;
    }

    auto_connect = atoi(param->argv[0]);

    if( (auto_connect !=0) &&  (auto_connect !=1))
    {
        rlt = ERROR_INVALID_PARAMETER;
        goto error_exit;
    }

    gconfig_set_auto_connect(auto_connect);

error_exit:
    return rlt;
}
/*---------------------------------------------------------------------------*/
S32 At_SAVE_CONFIG (stParam *param)
{
    S32 rlt = 0;
//    atcmdprintf("[%s] : +++\n",__func__);

    rlt = gconfig_save_config();

    if(rlt!=0){
        atcmdprintf("<Error>gconfig_save_config failed!!\n");
    }

    return rlt;
}
/*---------------------------------------------------------------------------*/
int At_Clean_Lists (stParam *param)
{
 //   atcmdprintf("[At_Clean_Lists] : +++\n");
    clear_ap_lists();

    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_AP (stParam *param)
{
//    atcmdprintf("[At_AP] : +++\n");
    softap_start();

    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_DBG_LEASETIME (stParam *param)
{
    char *token;
    U32 lease_time = 0;

    token = param->argv[0];
    lease_time = strtoul(token, NULL, 10);
    softap_dhcp_leasetime(lease_time);

    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_SET_APCONFIG (stParam *param)
{
    S32 rlt = ERROR_INVALID_PARAMETER;
    U8 a_gw_digit[3];

    U8 max_sta_num=0;
    U8 encryt_mode=0;
    U8 keylen = 0;
    U8 key[64] ="";
    U8 channel = 1;

    icomprintf(GROUP_ATCMD, LOG_ERROR,"[At_AP_EX] : +++\n");

    if (param->argc < 5) {
        return ERROR_INVALID_PARAMETER;
    }

    a_gw_digit[0] = strtoul(param->argv[0], NULL, 10);
    a_gw_digit[1] = strtoul(param->argv[1], NULL, 10);
    a_gw_digit[2] = strtoul(param->argv[2], NULL, 10);
    max_sta_num = strtoul(param->argv[3], NULL, 10);
    encryt_mode = strtoul(param->argv[4], NULL, 10);
    channel = strtoul(param->argv[7], NULL, 10);

    if(encryt_mode!= 0)
    {
        keylen = strtoul(param->argv[5], NULL, 10);
        if(keylen != strlen(param->argv[6])){
            rlt = -1;
            goto error_exit;
        }
        memset(key,0x0,sizeof(key));
        memcpy(key,param->argv[6],keylen);
        channel = strtoul(param->argv[7], NULL, 10);
    }

    //for example :  softap_init_ex(192,168,0);     //it means to set gateway as 192.168.0.1
    //for example :  softap_init_ex(192,168,10);    //it means to set gateway as 192.168.10.1
    //softap_init_ex(a_gw_digit[0],a_gw_digit[1],a_gw_digit[2]);
    icomprintf(GROUP_ATCMD, LOG_ERROR,"a_gw_digit[0]=%d,a_gw_digit[1]=%d,a_gw_digit[2]=%d\n",a_gw_digit[0],a_gw_digit[1],a_gw_digit[2]);
    icomprintf(GROUP_ATCMD, LOG_ERROR,"(max_sta_num,encryt_mode,keylen,key,channel)=(%d,%d,%d,%s,%d)\n",max_sta_num,encryt_mode,keylen,key,channel);

    softap_init_ex2(a_gw_digit[0],a_gw_digit[1],a_gw_digit[2],max_sta_num,encryt_mode,keylen,key,channel,100); 
    rlt = ERROR_SUCCESS;
    return rlt;
error_exit:
    atcmdprintf("<Error>At_SET_APCONFIG rlt = %d\n", rlt);
    return rlt;
}
int At_GET_APCONFIG (stParam *param)
{
    U8 ssid[33], ssidlen = 32, key[65], keylen = 64, encryt = 0;



    if(get_apmode_config(ssid, &ssidlen, &encryt, key, &keylen) == 0)
    {
        ssid[ssidlen] = 0;
        key[keylen] = 0;
    }
    else
	  return ERROR_INVALID_PARAMETER;

    atcmdprintf("AT+GET_APCONFIG=%s,%d,%d,%s,%d\n", ssid, ssidlen, encryt, key, keylen);


    return ERROR_SUCCESS;
}
int At_GET_APMODE (stParam *param)
{
    U8 mode = get_wifi_mode();
    atcmdprintf("AT+GET_APMODE=%s\n", mode == 1?"AP":"STA");
    return 0;
}
/*---------------------------------------------------------------------------*/
int At_AP_EXIT (stParam *param)
{
//    atcmdprintf("[At_AP_EXIT] : +++\n");
    softap_exit();
    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_TestFlashErase(stParam *param)
{
    //step1:split cmd into ip first!!
    S32 rlt = 0;
    U32 au32val[2];

    if (param->argc < 2) {
        return ERROR_INVALID_PARAMETER;
    }

    au32val[0] = strtoul(param->argv[0], NULL, 16);
    au32val[1] =  strtoul(param->argv[1], NULL, 16);

    //step2:ping!!
    //ping_start(&ip_addr);
    icomprintf(GROUP_ATCMD, LOG_ERROR,"(addr,length)=(0x%08x,0x%08x)\n",au32val[0],au32val[1]);
    if( ((au32val[0] & 0xfff) != 0x0) || ((au32val[1] & 0xfff) != 0x0))
    {
        icomprintf(GROUP_ATCMD, LOG_ERROR,"addr and length must all be 4KB aligned\n");
        rlt = ERROR_INVALID_PARAMETER;
        goto error_exit;
    }

    if( au32val[0] < 12*KB)
    {
        icomprintf(GROUP_ATCMD, LOG_ERROR,"addr must >= 12KB\n");
        rlt = ERROR_INVALID_PARAMETER;
        goto error_exit;
    }

//spi_flash_test(au32val[0],au32val[1]);
    spi_flash_sector_erase(au32val[0]);

error_exit:
    return rlt;
}
/*---------------------------------------------------------------------------*/
int At_TestFlashWrite(stParam *param){
    //step1:split cmd into ip first!!
    S32 rlt = 0;
    U32 au32val[2];

	if (param->argc < 2)
		return ERROR_INVALID_PARAMETER;

    au32val[0] = strtoul(param->argv[0], NULL, 16);
    au32val[1] =  strtoul(param->argv[1], NULL, 16);

    //step2:ping!!
    //ping_start(&ip_addr);
    icomprintf(GROUP_ATCMD, LOG_ERROR,"(addr,length)=(0x%08x,0x%08x)\n",au32val[0],au32val[1]);
#if 0
    if( (au32val[0] & 0xfff != 0x0) || (au32val[1] & 0xfff != 0x0))
    {
        printf("addr and length must all be 4KB aligned\n");
        rlt = ERROR_INVALID_PARAMETER;
        goto error_exit;
    }
#endif

    if( au32val[0] < 12*KB)
    {
        icomprintf(GROUP_ATCMD, LOG_ERROR,"addr must >= 12KB\n");
        rlt = ERROR_INVALID_PARAMETER;
        goto error_exit;
    }

    spi_flash_test(au32val[0],au32val[1]);

//    spi_flash_test2();

error_exit:
    return rlt;
}
/*---------------------------------------------------------------------------*/
static int ls_test(void)
{
    struct cfs_dir dir;
    struct cfs_dirent dirent;

    /* Coffee provides a root directory only. */
    if(cfs_opendir(&dir, "") != 0)
    {
        printf("failed to open the root directory\n");
        return 0;
    }

    /* List all files and their file sizes. */
    printf("-------------------------------------------------\n");
    printf("Available files:\n");
    while(cfs_readdir(&dir, &dirent) == 0)
    {
        printf("%s (%d bytes)\n", dirent.name, (unsigned long)dirent.size);
    }

    cfs_closedir(&dir);

    return 1;
}
/*---------------------------------------------------------------------------*/
int At_TestCfsInfo(stParam *param){
    //step1:split cmd into ip first!!
    S32 rlt = 0;
    char *file_name = NULL;

    if (param->argc < 1) {
        return ERROR_INVALID_PARAMETER;
    }

    file_name = param->argv[0];

    //step2:actions!!
    printf("<%s>(file_name=(%s)\n",__func__,file_name);

    icomm_cfs_dump_file_info(file_name);

    return rlt;
}
/*---------------------------------------------------------------------------*/
int At_TestCfsLs(stParam *param){
    S32 rlt = 0;

    cfs_dump_info();
    ls_test();

    return rlt;
}
/*---------------------------------------------------------------------------*/
int At_TestCfsWrite(stParam *param){
    //step1:split cmd into ip first!!
    S32 rlt = 0;
    S32 wrlt = -1;
    S32 loop_i = 0;
    char *file_name = NULL;
    U32 file_size;
    int wfd=-1;
    U32* test_buf = NULL;
    U32 pattern = 0;

    if (param->argc < 2) {
        return ERROR_INVALID_PARAMETER;
    }

    file_name = param->argv[0];
    file_size =  strtoul(param->argv[1], NULL, 16);

    //step2:actions!!
    file_size = ((file_size>>2)<<2);
    printf("<%s>(file_name,file_size)=(%s,0x%x)\n",__func__,file_name,file_size);
    wfd = cfs_open(file_name,CFS_WRITE);
    test_buf = malloc(file_size);

    //set up test buffer
    for( loop_i=0; loop_i<(file_size>>2); loop_i++){
        test_buf[loop_i] = pattern++;
    }

    wrlt = cfs_write( wfd, test_buf, file_size);
    if(-1==wrlt)
    {
        printf("<%s> cfs_write failed file_size=%d!!\n",__func__,file_size);
        rlt = -1;
        goto error_exit;
    }

    cfs_close(wfd);
    wfd = -1;

    free(test_buf);


error_exit:
    if(wfd!=-1){
        cfs_close(wfd);
    }
    return rlt;
}
/*---------------------------------------------------------------------------*/
int At_TestCfsRead(stParam *param)
{
    //step1:split cmd into ip first!!
    S32 rlt = 0;
    S32 loop_i = 0;
    char *file_name = NULL;
    U32 file_size;
    U32 read_size=16;
    int rfd=-1;
    U32* test_buf = NULL;
    U8 *p_buf = NULL;

    if (param->argc < 2) {
        return ERROR_INVALID_PARAMETER;
    }

    file_name = param->argv[0];
    file_size =  strtoul(param->argv[1], NULL, 16);

    //step2:actions!!
    file_size = ((file_size>>4)<<4);
    printf("<%s>(file_name,file_size)=(%s,0x%x)\n",__func__,file_name,file_size);
    rfd = cfs_open(file_name,CFS_READ);

    test_buf = malloc(read_size);
    p_buf = (U8*)test_buf;
    //set up test buffer

    for( loop_i=0; loop_i< file_size; )
    {
        cfs_read( rfd, test_buf, read_size);
        printf("<0x%04x>: %02x %02x %02x %02x-%02x %02x %02x %02x-%02x %02x %02x %02x-%02x %02x %02x %02x\r\n", loop_i, p_buf[0],p_buf[1],p_buf[2],p_buf[3],p_buf[4],p_buf[5],p_buf[6],p_buf[7],p_buf[8],p_buf[9],p_buf[10],p_buf[11],p_buf[12],p_buf[13],p_buf[14],p_buf[15]);
        loop_i += read_size;
    }

    cfs_close(rfd);

    free(test_buf);

    return rlt;
}


/*---------------------------------------------------------------------------*/
int At_TestCfsRemove(stParam *param){
    //step1:split cmd into ip first!!
    S32 rlt = 0;
    char *file_name = NULL;

    if (param->argc < 1) {
        return ERROR_INVALID_PARAMETER;
    }

    file_name = param->argv[0];

    //step2:actions!!
    printf("file_name:%s\n", file_name);
    cfs_remove(file_name);

    return rlt;
}


/*---------------------------------------------------------------------------*/
int At_BenchFlashRead(stParam *param)
{
#define BENCH_BLOCK_SIZE (4096)
    //step1:split cmd into ip first!!
    S32 rlt = 0;
    void *buf_dyn = NULL;
    S32 read_addr;
    S32 read_length;
    BOOL dump_flag=0;
    S32 rsize = 0;   //current read size

    if (param->argc < 3) {
        return ERROR_INVALID_PARAMETER;
    }

    read_addr = strtoul(param->argv[0], NULL, 16);
    read_length = strtoul(param->argv[1], NULL, 16);
    dump_flag = strtoul(param->argv[2], NULL, 16);

    //step2:ping!!
    //ping_start(&ip_addr);
    printf("(addr,length,dump_flag)=(0x%x,0x%x,%d)\n",read_addr,read_length,dump_flag);
    //dbg_dump_binary((au32val[0] |SRAM_BOOT_FLASH_BASE_ADDR) ,au32val[1]);

    buf_dyn = malloc(BENCH_BLOCK_SIZE);

    while( read_length > 0 )
    {
        printf("length = 0x%x,rsize=0x%d\n", read_length,rsize);

        if(read_length >= BENCH_BLOCK_SIZE)
        {
            rsize = BENCH_BLOCK_SIZE;
        } else {
            rsize = read_length;
        }

        spi_flash_read( read_addr, buf_dyn, rsize);
        if(1==dump_flag)
        {
            dbg_dump_binary((U32)buf_dyn,rsize);
        }

        read_addr += rsize;
        read_length -= rsize;


    }

    if(buf_dyn != NULL)
    {
        free(buf_dyn);
    }

    return rlt;
}

/*---------------------------------------------------------------------------*/
int At_BenchFlashWrite(stParam *param)
{
#define BENCH_BLOCK_SIZE (4096)
    //step1:split cmd into ip first!!
    S32 rlt = 0;
    S32 loop_i = 0;
    void *buf_dyn = NULL;
    S32 write_addr;
    S32 write_length;
    S32 loop_no=0;

    S32 tmp_write_addr;
    S32 tmp_write_length;
    S32 wsize = 0;   //current write size

    if (param->argc < 3) {
        return ERROR_INVALID_PARAMETER;
    }

    write_addr = strtoul(param->argv[0], NULL, 16);
    write_length = strtoul(param->argv[1], NULL, 16);
    loop_no = strtoul(param->argv[2], NULL, 16);

    //step2:ping!!
    //ping_start(&ip_addr);
    printf("(write_addr,write_length,loop_no)=(0x%x,0x%x,%d)\n", write_addr,write_length,loop_no);
    //dbg_dump_binary((au32val[0] |SRAM_BOOT_FLASH_BASE_ADDR) ,au32val[1]);

    buf_dyn = malloc(BENCH_BLOCK_SIZE);

    for(loop_i=0;loop_i<loop_no;loop_i++)
    {
        tmp_write_addr  = write_addr;
        tmp_write_length = write_length;

        while( tmp_write_length > 0 )
        {
            printf("<loop_i=%d>tmp_write_length = 0x%x,wsize=0x%d\n",loop_i, tmp_write_length, wsize);

            if(tmp_write_length >= BENCH_BLOCK_SIZE)
            {
                wsize = BENCH_BLOCK_SIZE;
            } else {
                wsize = tmp_write_length;
            }

            //spi_flash_write( tmp_write_addr, buf_dyn, wsize);
            spi_flash_test(tmp_write_addr,wsize);

            tmp_write_addr += wsize;
            tmp_write_length -= wsize;

        }

    }


    if(buf_dyn != NULL)
    {
        free(buf_dyn);
    }

    return rlt;
}
/*---------------------------------------------------------------------------*/
int At_TestFlashRead(stParam *param)
{
    //step1:split cmd into ip first!!
    S32 rlt = 0;
    U32 au32val[2];

    if (param->argc < 2) {
        return ERROR_INVALID_PARAMETER;
    }

    au32val[0] = strtoul(param->argv[0], NULL, 16);
    au32val[1] = strtoul(param->argv[1], NULL, 16);

    printf("(addr,length)=(0x%08x,0x%08x)\n",au32val[0],au32val[1]);
    dbg_dump_binary((au32val[0] |SRAM_BOOT_FLASH_BASE_ADDR) ,au32val[1]);

    return rlt;
}
/*---------------------------------------------------------------------------*/
int At_RadioTest(stParam *param)
{
    if (param->argc < 1) {
        return ERROR_INVALID_PARAMETER;
    }

    Parse_Radio_Command(param->argv[0]);

    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_RFCallback(stParam *param)
{
    S32 rlt = 0;

    process_start(&rf_event_process, NULL);
    rlt = SetRFCallback(&rf_event_process);

    return rlt;
}
/*---------------------------------------------------------------------------*/
int At_BackEcho(stParam *param)
{
    S32 rlt = 0;
    if (param->argc < 1)
        return ERROR_INVALID_PARAMETER;

    if(atoi(param->argv[0]) == ENABLE) {
        backecho(ENABLE);
    } else if(atoi(param->argv[0]) == DISABLE) {
        backecho(DISABLE);
    } else {
        printf("over range\n");
        rlt = ERROR_INVALID_PARAMETER;
    }
    return rlt;
}
/*---------------------------------------------------------------------------*/
#ifdef ENABLE_DOWNLOAD_FORM_CLOUD_SERVER
int At_OTASetServer (stParam *param)
{
	char *pIp = 0, *pport = 0;

	if (get_wifi_connected() != 1)
		return ERROR_WIFI_CONNECTION;

    if (param->argc < 2) {
        return ERROR_INVALID_PARAMETER;
    }

	pIp = param->argv[0];
	pport = param->argv[1];

	if (atoi(pport) == 0)
	{
		return ERROR_INVALID_PARAMETER;
	}

	ota_set_server(1, pIp, atoi(pport));

	return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_OTASetParam (stParam *param)
{
	char *pFilename = 0, *pPathname = 0;

	pFilename = param->argv[0];
	pPathname = param->argv[1];

	if(pFilename == 0)
		return 1;
	if(pPathname == NULL)
		ota_set_parameter(pFilename, "");
	else
		ota_set_parameter(pFilename, pPathname);
	return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_OTAStart (stParam *param)
{
	ota_update(NULL);
	return ERROR_SUCCESS;
}
#endif
#if (ENABLE_ICOMM_DEMO==1)
/*---------------------------------------------------------------------------*/
int AT_WIFIUART_DEMO(stParam *param)
{
	atcmdprintf("Call AT_WIFIUART_DEMO\n");
	if(strncmp(param->argv[0] ,"enable", sizeof("enable")) == 0) {
		process_start(&wifiUartDemo_process, NULL);
	} else if(strncmp(param->argv[0],"disable", sizeof("disable")) == 0) {
		process_post_synch(&wifiUartDemo_process, PROCESS_EVENT_EXIT, NULL);
	}
	else
	{
		/*none*/
	}

	return 0;
}

int AT_TCPSERVER_DEMO(stParam *param)
{
	atcmdprintf("Call AT_TCPSERVER_DEMO\n");
	if(strncmp(param->argv[0] ,"enable", sizeof("enable")) == 0) {
		process_start(&tcpServerDemo_process, NULL);
	} else if(strncmp(param->argv[0],"disable", sizeof("disable")) == 0) {
		process_post_synch(&tcpServerDemo_process, PROCESS_EVENT_EXIT, NULL);
	} else {
		atcmdprintf("tcp server demo unknown param, please check\n");
	}
	return 0;
}
int AT_USERSMART_DEMO(stParam *param)
{
	atcmdprintf("Call AT_USERSMART_DEMO\n");
	if(strncmp(param->argv[0] ,"enable", sizeof("enable")) == 0) {
		process_start(&userSmartDemo_process, NULL);
	} else if(strncmp(param->argv[0],"disable", sizeof("disable")) == 0) {
		process_post_synch(&userSmartDemo_process, PROCESS_EVENT_EXIT, NULL);
	} else {
		atcmdprintf("user smart demo unknown param, please check\n");
	}
	return 0;
}
int AT_STACONNECT_DEMO(stParam *param)
{
	atcmdprintf("Call AT_STACONNECT_DEMO\n");
	if(strncmp(param->argv[0] ,"enable", sizeof("enable")) == 0) {
		process_start(&sta_connect_demoProcess, NULL);
	} else if(strncmp(param->argv[0],"disable", sizeof("disable")) == 0) {
		process_post_synch(&sta_connect_demoProcess, PROCESS_EVENT_EXIT, NULL);
	} else {
		atcmdprintf("sta connect demo unknown param, please check\n");
	}
	return 0;
}
#endif
#if (FUNC_CLOUD_ON==1)
/*---------------------------------------------------------------------------*/
int At_BUKET_CREATE(stParam *param)
{
    if (param->argc < 1) {
        return ERROR_INVALID_PARAMETER;
    }

    create_bucket(param->argv[0], strlen(param->argv[0]));
    return ERROR_SUCCESS;
}
int At_BUKET_DELETE(stParam *param)
{
    if (param->argc < 1) {
        return ERROR_INVALID_PARAMETER;
    }

    delete_bucket(param->argv[0], strlen(param->argv[0]));
    return ERROR_SUCCESS;
}
int At_OBJECT_DOWNLOAD(stParam *param)
{
    if (param->argc < 2) {
        return ERROR_INVALID_PARAMETER;
    }
    char *buc_name = 0, *obj_name = 0;
    buc_name = param->argv[0];
    obj_name = param->argv[1];
    object_download(buc_name, strlen(param->argv[0]),
                                      obj_name, strlen(param->argv[1]));
    return ERROR_SUCCESS;
}
int At_OBJECT_UPDATE(stParam *param)
{
    if (param->argc < 2) {
        return ERROR_INVALID_PARAMETER;
    }
    char update_buf[4] = {1,2,3,4};

    object_update(param->argv[0], strlen(param->argv[0]),
                                      param->argv[1], strlen(param->argv[1]),
                                      update_buf, sizeof(update_buf));
    return ERROR_SUCCESS;
}
#endif
#if (FUNC_SEND_RAW_DATA_ON==1)
/*---------------------------------------------------------------------------*/
int AT_SendRawInit(stParam *param)
{
    init_raw_packet_info();
    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int AT_SendRawPacket(stParam *param)
{
    char *token = 0;
    U8 tmpbuf[256];
    const char delimiters[] = " ,";
    U8 num = 0, i = 0;

    if (param->argc < 2) {
        return ERROR_INVALID_PARAMETER;
    }

    num = atoi(param->argv[0]);
    if(num  < 0 || num  >= SEND_RAW_MAX_NUM)
        return ERROR_INVALID_PARAMETER;

    token = param->argv[1];
    for(;;) {
        token = strtok(NULL, delimiters);
        if(token == NULL) {
            break;
        }
        tmpbuf[i] = strtoul(token, NULL, 16);
        i++;
    }

    send_raw_packet(num, tmpbuf, i);
    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int AT_SetRawRemoteMac(stParam *param)
{
    const char delimiters[] = ":";
    char *pMac = 0;
    S8 loop_i = 0;
    U8 mac[6];
    unsigned long int toul_val;

    if (param->argc < 1) {
        return ERROR_INVALID_PARAMETER;
    }

    pMac = param->argv[0];

    if(pMac != NULL) {
        for(loop_i=0;loop_i<5;loop_i++) {
            toul_val =  strtoul(pMac, NULL, 16);
            mac[loop_i] = toul_val;
            if(pMac==NULL){
                return ERROR_INVALID_PARAMETER;
            }
            pMac = strtok (NULL, delimiters);
        }
        toul_val =  strtoul(pMac, NULL, 16);
        mac[loop_i] = toul_val;
        set_raw_packet_remote_mac(mac, 6);
    } else {
        return ERROR_INVALID_PARAMETER;
    }
    return ERROR_SUCCESS;
}
#endif
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(http_request_process, ev, data)
{
    static struct etimer httptimeout;
	static NETSOCKET httpsock;
	int recvlen;
	SOCKETMSG msg;
   // static struct etimer send_timer;

      allocate_buffer_in();
      allocate_buffer_out();

	PROCESS_BEGIN();

	while(ghttprun)
	{
		//tcp connection
		if(get_wifi_connected() == 0)
	{
			icomprintf(GROUP_ATCMD,LOG_INFO,"******WIFI disconnected, sleep one second\n");
			etimer_set(&httptimeout, 1 * CLOCK_SECOND);
			PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
			continue;
	}

    testcnt++;
	httpsock = tcpconnect( &gNetStatus.remote_ip_addr, gNetStatus.port, &http_request_process);

	//wait for TCP connected or uip_timeout.
	PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_MSG);
	msg = *(SOCKETMSG *)data;
	if(msg.status != SOCKET_CONNECTED)
	{
		icomprintf(GROUP_ATCMD,LOG_ERROR,"TCP connect fail! Post message type:%d\n", msg.status);
		goto dissconnect;
	}

	//Send http request to server
	tcpsend(httpsock, buffer_out_dynamic, strlen (buffer_out_dynamic));

	//Wait for data is transmitted or uip_timeout.
	while(1)
	{
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_MSG);
		msg = *(SOCKETMSG *)data;
		if(msg.status == SOCKET_SENDACK)
		{
			break;
		}
		else if(msg.status == SOCKET_CLOSED)
		{
			icomprintf(GROUP_ATCMD,LOG_ERROR,"TCP send fail! Post message type:%d\n", msg.status);
				goto dissconnect;
			}
		}

		//Wait for response or timeout
		etimer_set(&httptimeout, 15 * CLOCK_SECOND);
		while(1)
		{
			PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_MSG || ev == PROCESS_EVENT_TIMER);
			if(ev == PROCESS_EVENT_TIMER)
			{
				icomprintf(GROUP_ATCMD,LOG_ERROR,"Recv data timeout\n");
				goto dissconnect;
			}
			else if(ev == PROCESS_EVENT_MSG)
			{
				msg = *(SOCKETMSG *)data;
				if(msg.status == SOCKET_NEWDATA)
				{
					successcnt++;
					recvlen = tcprecv(httpsock, buffer_in_dynamic, MAX_SEND_BUFFER);
					buffer_in_dynamic[recvlen] = 0;
					icomprintf(GROUP_ATCMD,LOG_INFO,"recv:%s\n", buffer_in_dynamic);
					break;
				}
				else if(msg.status == SOCKET_CLOSED)
	            {
					icomprintf(GROUP_ATCMD,LOG_ERROR,"Recv data fail! Post message type:%d\n", msg.status);
					goto dissconnect;
				}
			}
	}

dissconnect:
		//dissconnect
		tcpclose(httpsock);
		icomprintf(GROUP_ATCMD,LOG_ERROR,"Testcnt:%d failcnt:%d\n", testcnt, (testcnt-successcnt));
		//Wait for one second to close the connection.
		etimer_set(&httptimeout, 1 * CLOCK_SECOND);
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
	}

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
void update_uart_config()
{
	gwifistatus.atuartid = ATCMD_UARDID;
	gwifistatus.atuartrate = ATCMD_UARDBAUDRATE;
}

#ifndef __SZCLOUD_SUPPORT__	
PROCESS_THREAD(tcp_connect_process, ev, data)
{
	PROCESS_BEGIN();
	SOCKETMSG msg;
	int recvlen;
	uip_ipaddr_t peeraddr;
	U16 peerport;

    allocate_buffer_in();

	while(1)
	{
		PROCESS_WAIT_EVENT();

		if( ev == PROCESS_EVENT_EXIT)
		{
			break;
		}
		else if(ev == PROCESS_EVENT_MSG)
		{
			msg = *(SOCKETMSG *)data;
			//The TCP socket is connected, This socket can send data after now.
			if(msg.status == SOCKET_CONNECTED)
			{
				atcmdprintf("socked:%d connected\n", msg.socket);
#ifndef __SZCLOUD_SUPPORT__					
				if(msg.socket == gclientsock)
				{
					gNetStatus.tcp_connect = 1;
					At_RespOK(ATCMD_TCPCONNECT);
				}
#endif				
			}
			//TCP connection is closed. Clear the socket number.
			else if(msg.status == SOCKET_CLOSED)
			{
				atcmdprintf("socked:%d closed\n", msg.socket);
				At_RespOK(ATCMD_TCPDISCONNECT);
#ifndef __SZCLOUD_SUPPORT__					
				if(gclientsock == msg.socket)
				{
					gclientsock = -1;
					gNetStatus.tcp_connect = 0;
				}
				
				if(gserversock == msg.socket)
				{
					gserversock = -1;
					gNetStatus.tcp_connect = 0;
				}
#endif				
			}
			//Get ack, the data trasmission is done. We can send data after now.
			else if(msg.status == SOCKET_SENDACK)
			{
				atcmdprintf("socked:%d send data ack\n", msg.socket);
				At_RespOK(ATCMD_TCPSEND);
			}
			//There is new data coming. Receive data now.
			else if(msg.status == SOCKET_NEWDATA)
			{
				if(0 <= msg.socket && msg.socket < UIP_CONNS)
				{
					recvlen = tcprecv(msg.socket, buffer_in_dynamic, MAX_SEND_BUFFER);
					buffer_in_dynamic[recvlen] = 0;
      #if 1
					atcmdprintf("TCP socked:%d recvdata:%s\n", msg.socket, buffer_in_dynamic);
      #endif
      //icomprintf(GROUP_ATCMD,LOG_INFO,"TCP socked-%d: %c-%c-%c\n", msg.socket, buffer_in_dynamic[0],buffer_in_dynamic[1],buffer_in_dynamic[2]);
      //printf("%d:%c%c\n", msg.socket, buffer_in_dynamic[0],buffer_in_dynamic[1]);

				}
				else if(UIP_CONNS <= msg.socket && msg.socket < UIP_CONNS + UIP_UDP_CONNS)
				{
					recvlen = udprecvfrom(msg.socket, buffer_in_dynamic, MAX_SEND_BUFFER, &peeraddr, &peerport);
					buffer_in_dynamic[recvlen] = 0;
					atcmdprintf("UDP socked:%d recvdata:%s from %d.%d.%d.%d:%d\n", msg.socket, buffer_in_dynamic, peeraddr.u8[0], peeraddr.u8[1], peeraddr.u8[2], peeraddr.u8[3], peerport);
				}
				else
		        {
					atcmdprintf("Illegal socket:%d\n", msg.socket);
				}
			}
			//A new connection is created. Get the socket number and attach the calback process if needed.
			else if(msg.status == SOCKET_NEWCONNECTION)
			{
				if(gserversock == -1)
				{
					gserversock = msg.socket;
					atcmdprintf("new connected to listen port(%d), socket:%d\n", msg.lport, msg.socket);
					gNetStatus.tcp_connect = 1;
				}
				else
				{
					//Only allow one client connection for this application.
					//tcpclose(msg.socket);
				}
			}
			else
            {
				atcmdprintf("unknow message type\n");
            }
        }
    }

	PROCESS_END();
}
#endif

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(poll_tx_process, ev, data)
{
	PROCESS_BEGIN();
	while(1)
	{
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
		while(bTXRun)
		{
			Radio_Command(SEND_FRAME, "");
		}
	}
	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(nslookup_process, ev, data)
{
	PROCESS_BEGIN();
	PROCESS_WAIT_EVENT_UNTIL(ev == resolv_event_found);
	{
		uip_ipaddr_t addr;
		uip_ipaddr_t *addrptr;
		addrptr = &addr;

		char *pHostName = (char*)data;
		if(ev==resolv_event_found)
		{
			resolv_lookup(pHostName, &addrptr);
			uip_ipaddr_copy(&addr, addrptr);
#ifdef __GIZ_CLOUD_SUPPORT__	/*»úÖÇÔÆµÄdns½âÎö*/
			add_ip_to_table(pHostName, addrptr->u8);
#endif
		#ifdef CUSTOM_HYNEHOMI
			memcpy(&gNetStatus.remote_ip_addr, addrptr, sizeof(uip_ip4addr_t));

			if (heny_tcp_connect)
			{
				heny_tcp_connect = FALSE;

				atcmdprintf("IP format : %d.%d.%d.%d\r\n", uip_ipaddr_to_quad(addrptr));
#ifndef __SZCLOUD_SUPPORT__				
		        gclientsock = tcpconnect(&gNetStatus.remote_ip_addr, gNetStatus.port, &tcp_connect_process);
				atcmdprintf("create socket:%d\r\n", gclientsock);
#endif				
			}
			else if (heny_udp_connect)
			{
				heny_udp_connect = FALSE;

				if (udpsendto(heny_udp_sock, buffer_out_dynamic,
										strlen(buffer_out_dynamic),
										&gNetStatus.remote_ip_addr,
										gNetStatus.port) == -1)
				{
					//printf("udp send error!\r\n");
					atcmdprintf (ATRSP_ERROR, ERROR_UDP_CONNECTION);
				}
				else
				{
					At_RespOK(ATCMD_UDPSEND);
				}
			}
			else
		#endif

			atcmdprintf("AT+NSLOOKUP=%d.%d.%d.%d\n", addr.u8[0] , addr.u8[1] , addr.u8[2] , addr.u8[3]);
		}
	}
	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
at_cmd_info atcmdicomm_info_tbl[] =
{
    {ATCMD_REBOOT ,            At_Reboot,             0},
    {ATCMD_WDOG_START,         At_WatchDOG_Start,     0},
    {ATCMD_WDOG_STOP,          At_WatchDOG_Stop,      0},
    {ATCMD_VERSION,            At_GetVersion,         0},
    {ATCMD_MINFO,              At_GetManufactureInfo, 0},
    {ATCMD_SETWIFICONFIG,      At_SetWifiConfig,      0},
    {ATCMD_GETCONFIG,          At_GetConfigure,       0},
    {ATCMD_SETIFCONFIG,        At_SetIfConfigure,     0},
    {ATCMD_NETSTATUS,          At_GetNetStatus,       0},
    {ATCMD_IPCONF,             At_GetIPconf,          0},
    {ATCMD_HELP,               At_Help,               0},
    {ATCMD_APSTATUS,           At_GetApStatus,        0},
    {ATCMD_SOFTAP_STATUS,      At_GetSoftapStatus,    0},
    {ATCMD_WIFISTATUS,         At_GetWifiStatus,      0},
    {ATCMD_MRX,                At_GetMRX,             0},
    {ATCMD_FLASH_WRITE,        At_TestFlashWrite,     0},
    {ATCMD_CFS_LS,             At_TestCfsLs,          0},
    {ATCMD_CFS_INFO,           At_TestCfsInfo,        0},
    {ATCMD_CFS_WRITE,          At_TestCfsWrite,       0},
    {ATCMD_CFS_READ,           At_TestCfsRead,        0},
    {ATCMD_CFS_REMOVE,         At_TestCfsRemove,      0},
    {ATCMD_FLASH_READ,         At_TestFlashRead,      0},
    {ATCMD_BENCH_FLASH_READ,   At_BenchFlashRead,     0},
    {ATCMD_BENCH_FLASH_WRITE,  At_BenchFlashWrite,    0},
    {ATCMD_MEM_READ,           At_TestMemRead,        0},
    {ATCMD_MEM_INFO,           At_TestMemInfo,        0},
    {ATCMD_MBOX,               At_MboxDump,           0},
    {ATCMD_PBUF,               At_PbufDump,           0},
    {ATCMD_WIFI_RESET,         At_WifiReset,          0},
    {ATCMD_NTP_TIME,           At_ShowNtpTime,        0},
    {ATCMD_FLASH_ERASE,        At_TestFlashErase,     0},
    {ATCMD_PING,               At_Ping,               0},
    {ATCMD_NETSCAN,            At_NetScan,            0},
    {ATCMD_CLEAN_LISTS,        At_Clean_Lists,        0},
    {ATCMD_NETSCAN_CUSTOM,     At_NetScan_Custom,     0},
    {ATCMD_CONNECT,            At_Connect,            0},
    {ATCMD_RECONNECT,          At_ReConnect,          0},
    {ATCMD_DISCONNECT,         At_Disconnect,         0},
    {ATCMD_SET_DEBUGINFO,      AT_SetDebugInfo,       0},
    {ATCMD_BACK_ECHO,          At_BackEcho,            0},
    {ATCMD_POWERSAVE,          At_POWERSAVE,          0},
    {ATCMD_SET_GPIO,           At_SetGpio,            0},
    {ATCMD_WRITE_GPIO,         At_WriteGpio,          0},
    {ATCMD_READ_GPIO,          At_ReadGpio,           0},
    {ATCMD_SET_PWM,            At_SetPWM,             0},
    {ATCMD_SET_PWMSPEC,        At_SetPWMspecial,      0},
#if UIP_CONF_ICMP_CNT
    {ATCMD_SHOWPINGCNT,        At_ShowPingCount,      0},
    {ATCMD_CLEARPINGCNT,       At_ClearPingCount,     0},
#endif
    {ATCMD_WRITE_REG,          At_WriteReg32,         0},
    {ATCMD_READ_REG,           At_ReadReg32,          0},
    {ATCMD_SET_CHANNEL,        At_SetChannel,         0},
    {ATCMD_RF,                 At_ShowRfCommand,      0},
    {ATCMD_PHY_COUNT,          At_GetRfStatus,        0},
    {ATCMD_REMOVE_CONF,        AT_RemoveCfsConf,      0},
    {ATCMD_TCPCHANGETIMEOUT,   At_TCPChange_Timeout,  0},
    {ATCMD_TCPCONNECT,         At_TCPConnect,         0},
    {ATCMD_TCPSEND_CURR,       At_TCPSendCurr,        1},
    {ATCMD_TCPDISCONNECT_CURR, At_TCPDisconnectCurr,  0},
    {ATCMD_TCPSEND,            At_TCPSend,            2},
    {ATCMD_TCPDISCONNECT,      At_TCPDisconnect,      0},
#if 0    
    {ATCMD_SSLCONNECT,         At_SSLConnect,         0},
    {ATCMD_SSLSEND,            At_SSLSend,            1},
    {ATCMD_SSLDISCONNECT,      At_SSLDisconnect,      0},
#endif

    {ATCMD_UDPCREATE,          At_UDPCreate,          0},
    {ATCMD_UDPSEND,            At_UDPSend,            4},
    {ATCMD_UDPCLOSE,           At_UDPClose,           0},
    {ATCMD_TCPLISTEN,          At_TCPListen,          0},
    {ATCMD_TCPUNLISTEN,        At_TCPUnlisten,        0},
    {ATCMD_HTTPTESTSTART,      At_HTTPTest,           3},
    {ATCMD_HTTPTESTSTOP,       At_HTTPTestStop,       0},
    {ATCMD_GET_LOCALMAC,       At_GetLocalMac,        0},
    {ATCMD_PROCESS_DUMP,       At_ProcessAllDump,     0},
    {ATCMD_PROCESS_KILL,       At_ProcessKill,        0},
    {ATCMD_PROCESS_QUE_DUMP,   At_ProcessAllQueDump,  0},
    {ATCMD_PROCESS_QUE_KILL,   At_ProcessQueKill,     0},
    {ATCMD_SHOWCONNECTAP,      At_ShowConnectAP,      0},
    {ATCMD_SHOWALLAP,          At_ShowAllAP,          0},
    {ATCMD_BMODE_COUNT,        At_bmode_count,        0},
    {ATCMD_BMODE_RESET,        At_bmode_reset,        0},
    {ATCMD_GMODE_COUNT,        At_gmode_count,        0},
    {ATCMD_GMODE_RESET,        At_gmode_reset,        0},
    {ATCMD_FILTER_COUNT,       At_filter_count,       0},
    {ATCMD_SET_RATE,           At_set_rateindex,      0},
    {ATCMD_FILTER_RESET,       At_filter_reset,       0},
    {ATCMD_NSLOOKUP,           At_NSLookup,           0},
#ifdef ENABLE_DOWNLOAD_FORM_CLOUD_SERVER	//ota
    {ATCMD_OTASETSERVER,       At_OTASetServer,       0},
    {ATCMD_OTASETPARAM,        At_OTASetParam,        0},
    {ATCMD_OTASTART,           At_OTAStart,           0},
#endif
#if (ENABLE_ICOMM_DEMO==1)
    {ATCMD_WIFIUART_DEMO,      AT_WIFIUART_DEMO,      0},
    {ATCMD_TCPSERVER_DEMO,     AT_TCPSERVER_DEMO,     0},
    {ATCMD_USERSMART_DEMO,     AT_USERSMART_DEMO,     0},
    {ATCMD_STACONNECT_DEMO,    AT_STACONNECT_DEMO,    0},
#endif
#if (FUNC_CLOUD_ON==1)
    {ATCMD_BUKET_CREATE,      At_BUKET_CREATE,      0},
    {ATCMD_BUKET_DELETE,     At_BUKET_DELETE,     0},
    {ATCMD_OBJECT_DOWNLOAD,     At_OBJECT_DOWNLOAD,     0},
    {ATCMD_OBJECT_UPDATE,    At_OBJECT_UPDATE,    0},
#endif
#if (FUNC_SEND_RAW_DATA_ON==1)
    {ATCMD_SEND_RAW_INIT,    AT_SendRawInit,      0},
    {ATCMD_SEND_RAW_PACKET,    AT_SendRawPacket,      0},
    {ATCMD_SET_RAW_REMOTE_MAC, AT_SetRawRemoteMac,    0},
#endif
    {ATCMD_ENABLE_SMARTMODE,   At_EnableSmartMode,    0},
    {ATCMD_ENABLE_SMARTREBOOT, At_EnableSmartReboot,  0},
    {ATCMD_SET_RFCALLBACK,     At_NetScan,            0},
    {ATCMD_AP_EXIT,            At_AP_EXIT,            0},       //warning: due to current flow, ATCMD_AP_EXIT should always be in front of ATCMD_AP
    {ATCMD_AP,                 At_AP,                 0},
    {ATCMD_DBG_LEASETIME,      At_DBG_LEASETIME,      0},
    {ATCMD_SET_APCONFIG,       At_SET_APCONFIG,       0},
    {ATCMD_GET_APCONFIG,       At_GET_APCONFIG,       0},
    {ATCMD_GET_APMODE,         At_GET_APMODE,         0},
    {ATCMD_SET_AP_SSID,        At_SET_AP_SSID,        0},
    {ATCMD_SET_AUTO_CONNECT,   At_SET_AUTO_CONNECT,   0},
    {ATCMD_SAVE_CONFIG,        At_SAVE_CONFIG,        0},
//    {ATCMD_RADIO_CMD,          At_RadioTest,          1},
    {ATCMD_SET_RFCALLBACK,     At_RFCallback,         0},
#if 0
    {ATCMD_POWERON,            At_PowerOn,            0},
    {ATCMD_RX,                 At_RX,                 0},
    {ATCMD_TX,                 At_TX,                 0},
    {ATCMD_POWERSAVING,        At_PowerSaving,        0},
#endif
#if 1	//RF ATCMD
    {ATCMD_RADIO_CHANNEL,            At_RadioChannel,            1},
    {ATCMD_RADIO_RF_START,            At_RadioRFStart,            1},
    {ATCMD_RADIO_RF_RATE,            At_RadioRFRate,            1},
    {ATCMD_RADIO_RF_BGAIN,            At_RadioRFBGain,            1},
    {ATCMD_RADIO_RF_GNGAIN,            At_RadioRFGNGain,            1},
    {ATCMD_RADIO_RF_IQPHASE,            At_RadioRFIQPhase,            1},
    {ATCMD_RADIO_RF_IQAMP,            At_RadioRFIQAmp,            1},
    {ATCMD_RADIO_RF_STOP,            At_RadioRFStop,            0},
    {ATCMD_RADIO_RF_RESET,            At_RadioRFReset,            0},
    {ATCMD_RADIO_RF_COUNT,            At_RadioRFCount,            1},
    {ATCMD_RADIO_RF_FREQOFFSET,            At_RadioRFFreqOffset,            2},
    {ATCMD_RADIO_RF_TEMPCS_HT,            At_RadioRFTempcsHT,            2},
    {ATCMD_RADIO_RF_TEMPCS_RT,            At_RadioRFTempcsRT,            2},
    {ATCMD_RADIO_RF_TEMPCS_LT,            At_RadioRFTempcsLT,            2},
    {ATCMD_RADIO_RF_READ_TEMPCS,            At_RadioRFReadTempcs,            0},
    {ATCMD_RADIO_RF_ENABLE_TCSR,            At_RadioRFEnableTCSR,            1},
    {ATCMD_RADIO_RF_DUMP,            At_RadioRFDump,            0},
    {ATCMD_RADIO_RF_SET_HT,            At_RadioRFSetHT,            21},
    {ATCMD_RADIO_RF_SET_RT,            At_RadioRFSetRT,            21},
    {ATCMD_RADIO_RF_SET_LT,            At_RadioRFSetLT,            21},
    {ATCMD_RADIO_RF_LDO,            At_RadioRFLDO,            2},
    {ATCMD_RADIO_RF_PA1,            At_RadioRFPA1,            1},
    {ATCMD_RADIO_RF_PA2,            At_RadioRFPA2,            1},
    {ATCMD_RADIO_RF_PA3,            At_RadioRFPA3,            1},
    {ATCMD_RADIO_RF_XTAL,            At_RadioRFXTAL,            1},
    {ATCMD_RADIO_RF_FLASH_XTAL,            At_RadioRFFlashXTAL,            3},
    {ATCMD_RADIO_TEMP_BOUNDARY,            At_RadioRFTempBoundary,            2},
    {ATCMD_RADIO_RF_SINGLE,            At_RadioRFSingle,            1},
    {ATCMD_RADIO_WRITE_MAC,            At_RadioWriteMAC,            1},
    {ATCMD_RADIO_READ_MAC,            At_RadioReadMAC,            0},
    {ATCMD_RADIO_WRITE_SERIAL,            At_RadioWriteSerial,            1},
    {ATCMD_RADIO_READ_SERIAL,            At_RadioReadSerial,            0},
    {ATCMD_RADIO_RF_REG_DUMP,            At_RadioRFRegisterDump,            0},
    {ATCMD_RADIO_PHY_REG_DUMP,            At_RadioPHYRegisterDump,            0},
    {ATCMD_RADIO_SELF_CHECK,            At_RadioSelfCheck,            0},
#endif
};

void hx_set_conn_status(Boolean run);

int At_ParserInd (EVENTMSG_ID msgid, unsigned char *buff, int len)
{
	int i = 0;

	switch (msgid) {
		case MSG_ATE_RECV:
			atcmdprintf(ATIND_RECV, buff);
			break;
		case MSG_ATE_SCAN:
			atcmdprintf("+SCAN:");
			for (i=0; i<get_ap_lsit_total_num(); i++) {
				atcmdprintf("%s, ", &(ap_list[i]).name[0]);
			}
			atcmdprintf("\r\n");
			At_RespOK(ATCMD_NETSCAN);
			break;
			
		case MSG_ATE_DISCONNECT:
			//printf (ATIND_DISCONNECT, "SUCCESS");
			bss_mgmt_disconnect();			// added by tim @20170224
			At_RespOK(ATCMD_DISCONNECT);
#ifndef __CUSTOMER_XMRL__			
			hx_set_conn_status(FALSE);
#endif			
			break;
			
		case MSG_ATE_ERROR:
			atcmdprintf(ATRSP_ERROR_STR, buff);
			break;
			
		case MSG_EV_GET_IP:
			At_RespOK(ATCMD_CONNECT);
#ifndef __CUSTOMER_XMRL__				
			hx_set_conn_status(FALSE);
#endif			
			break;
            default:
                break;
	}

	//printf ("[Ate_ParserInd] : %d---\n",msgid);
	return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_Parser (char *buff, int len)
{
	int i = 0;
	int	nRet = ERROR_UNKNOWN_COMMAND;
    char cmd[32], operat = 0;
    stParam param;

    memset(&param, 0, sizeof(stParam));
    if( (1==len) && (buff[0]=='\r' || buff[0]=='\n')){
        nRet = ERROR_SUCCESS;
        goto exit;
    }

	if (0 == len) {
		printf (ATRSP_ERROR, ERROR_INVALID_PARAMETER);
		return ERROR_INVALID_PARAMETER;
	}

	buff[len-1] = 0x0;  //chomp!! replace \r\n with null string

    for(i = 0; i < len; i++)
    {
        if(buff[i] == 0 || buff[i] == '=' || buff[i] == ' ')
        {
            memcpy(cmd, buff, i);
            operat = buff[i];
            cmd[i] = 0;
            break;
        }
    }

    for(i = 0; i < sizeof(atcmdicomm_info_tbl)/sizeof(at_cmd_info); i++)
    {
        if(strncmp(atcmdicomm_info_tbl[i].atCmd, cmd, strlen(atcmdicomm_info_tbl[i].atCmd)) == 0)
        {
            if(operat != 0)
                parseBuff2Param(buff + strlen(atcmdicomm_info_tbl[i].atCmd) + 1, &param, atcmdicomm_info_tbl[i].maxargu);

            nRet = atcmdicomm_info_tbl[i].pfHandle(&param);
            goto exit_rsp;
        }
    }

    //Handle Customer AT Command
    i=0;
	while(1)
	{
		if(atcmd_info_tbl[i].pfHandle == NULL || atcmd_info_tbl[i].atCmd == NULL || strlen(atcmd_info_tbl[i].atCmd) < 0)
		{
			nRet = ERROR_UNKNOWN_COMMAND;
			break;
		}

		//local variable
        if(strncmp(atcmd_info_tbl[i].atCmd, cmd, strlen(atcmd_info_tbl[i].atCmd)) == 0)
        {
            if(operat != 0)
                parseBuff2Param(buff + strlen(atcmd_info_tbl[i].atCmd) + 1, &param, 0);

            nRet = atcmd_info_tbl[i].pfHandle(&param);
            goto exit_rsp;
        }

		i++;
	}

exit_rsp:
	if (ERROR_SUCCESS > nRet)
		atcmdprintf (ATRSP_ERROR, nRet);
    else if(ERROR_SUCCESS == nRet)
        At_RespOK(cmd);

exit:
	return nRet;
}

PROCESS_THREAD(rf_event_process, ev, data)
{
	PROCESS_BEGIN();
	while(1)
	{
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_RF_RESULT_OK || ev == PROCESS_RF_RESULT_DUMP || ev == PROCESS_RF_RESULT_ERROR || ev == PROCESS_EVENT_EXIT);
		if(ev == PROCESS_RF_RESULT_OK)
		{
			int* pResult = (int *)data;
			printf("OK Result = %d\n", *pResult);

		}
		else if(ev == PROCESS_RF_RESULT_DUMP)
		{
			int* pResult = (int *)data;
			printf("Channel = %d\n", *pResult);
			printf("Rate= %d\n", *(pResult+1));
			printf("B Gain = %d\n", *(pResult+2));
			printf("G/N Gain = %d\n", *(pResult+3));
			printf("Freq p = %d\n", *(pResult+4));
			printf("Freq n = %d\n", *(pResult+5));
			printf("ldo n = %d\n", *(pResult+6));
			printf("ldo m = %d\n", *(pResult+7));
			printf("pa1 = %d\n", *(pResult+8));
			printf("pa2 = %d\n", *(pResult+9));
			printf("pa3 = %d\n", *(pResult+10));
			printf("temp lt= %d\n", *(pResult+11));
			printf("temp ht= %d\n", *(pResult+12));
		}
		else if(ev == PROCESS_RF_RESULT_ERROR)
		{
				printf("ERROR\n");
		}
		else if(ev == PROCESS_EVENT_EXIT)
			break;
		}
	PROCESS_END();
}
