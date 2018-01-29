
#include <stdio.h>
#include "ieee80211_mgmt.h"
#include "systemconf.h"
#include "comm_apps_res.h"
#include "ssv_lib.h"
#include "userSmart.h"
#include "airkiss.h"
#include "smartComm.h"
#include "hash_alg.h"
#include "skbuff.h"
#include "bsp/bsp.h"

extern IEEE80211STATUS gwifistatus;

//airkiss global value
airkiss_context_t akcontexExample;
const airkiss_config_t akconfExample = {
    (airkiss_memset_fn)&memset,
    (airkiss_memcpy_fn)&memcpy,
    (airkiss_memcmp_fn)&memcmp,
    0
};

void userSmartCallBackFrame(U8 *data, U32 num) ATTRIBUTE_SECTION_SRAM;

/*---------------------------------------------------------------------------*/
PROCESS(userSmartDemo_process, "user smart process");
/*---------------------------------------------------------------------------*/
void showAllApInfo()
{
    U8 i = 0;
    for (i=0; i<getAvailableIndex(); i++) {
        printf ("[%d] name: %s  ch: %d ",i,  &(ap_list[i]).name[0], (ap_list[i]).channel);
         if(ap_list[i].security_type == 0)
        	    printf ("security_type = open");
         if(ap_list[i].security_type == 1)
        	    printf ("security_type = wep");
         if(ap_list[i].security_type == 2)
        	    printf ("security_type = wpa");
         if(ap_list[i].security_type == 3)
        	    printf ("security_type = wpa2");
         if(ap_list[i].security_type == 4)
        	    printf ("security_type = wpawpa2");
         if(ap_list[i].security_subType == 0)
        	    printf ("/NONE");
         if(ap_list[i].security_subType == 1)
        	    printf ("/TKIP");
         if(ap_list[i].security_subType == 2)
        	    printf ("/AES");
         if(ap_list[i].security_subType == 3)
        	    printf ("/TKIP_CCMP");
        
        printf (" rssi: %d -dBm\n", ap_list[i].rssi);
    }
}
/*---------------------------------------------------------------------------*/
void airKissInitExample()
{
    U8 *ver;
    ver = (U8 *)airkiss_version();
    printf("##%s##\n",ver);
    airkiss_init(&akcontexExample, &akconfExample);
}
/*---------------------------------------------------------------------------*/
void airKissResultExample()
{
    airkiss_result_t result;
    
    airkiss_get_result(&akcontexExample, &result);
    strncpy((char *)gwifistatus.connAP.ssid, result.ssid, (unsigned int)result.ssid_length);
    gwifistatus.connAP.ssid[result.ssid_length] = '\0';
    gwifistatus.connAP.ssid_len = result.ssid_length;
    strncpy((char *)gwifistatus.connAP.key, result.pwd, (unsigned int)result.pwd_length);
    gwifistatus.connAP.key[result.pwd_length] = '\0';
    gwifistatus.connAP.key_len= result.pwd_length;
    pbkdf2_sha1((unsigned char *)gwifistatus.connAP.key, gwifistatus.connAP.key_len, (unsigned char *)gwifistatus.connAP.ssid, gwifistatus.connAP.ssid_len, 4096, (unsigned char *)gwifistatus.connAP.pmk, 32);
    printf("###############SSID: %s LEN: %d###############\n", gwifistatus.connAP.ssid, gwifistatus.connAP.ssid_len);
    printf("###############PASS: %s LEN: %d###############\n", gwifistatus.connAP.key, gwifistatus.connAP.key_len);
}
/*---------------------------------------------------------------------------*/
U8 checkAndSetMac()
{
    U8 i = 0;

    for (i=0; i<getAvailableIndex(); i++) {
        if (strcmp ((char *)gwifistatus.connAP.ssid, (char *)ap_list[i].name) == 0) {
            gwifistatus.connAP.channel = ap_list[i].channel;
            memcpy(gwifistatus.connAP.mac, ap_list[i].mac, MAC_LEN);
            gwifistatus.connAP.security_type = ap_list[i].security_type;
            gwifistatus.connAP.security_subType = ap_list[i].security_subType;
            printf("MAC %02x:%02x:%02x:%02x:%02x:%02x, channel\n", 
                                                                gwifistatus.connAP.mac[0],
                                                                gwifistatus.connAP.mac[1],
                                                                gwifistatus.connAP.mac[2],
                                                                gwifistatus.connAP.mac[3],
                                                                gwifistatus.connAP.mac[4],
                                                                gwifistatus.connAP.mac[5],
                                                                gwifistatus.connAP.channel);
            return TRUE;
        }
    }
    return FALSE;
}
/*---------------------------------------------------------------------------*/
void userSmartCallBackFrame(U8 *data, U32 num)
{
    airkiss_status_t ret = 0;
    
    ret = airkiss_recv(&akcontexExample, data, (unsigned short)num);
    if(ret == AIRKISS_STATUS_CHANNEL_LOCKED) {
        printf("##########AIRKISS_STATUS_CHANNEL_LOCKED: %d##########\n", ret);
        printf("lock ch: %d\n", app_res.chNum);
        app_res.chLock = TRUE;
    } else if(ret == AIRKISS_STATUS_COMPLETE) {
        printf("##########AIRKISS_STATUS_COMPLETE: %d##########\n", ret);
        app_res.passLock= TRUE;
        airKissResultExample();
        DisableCallBackFrame();
    }
}
/*---------------------------------------------------------------------------*/
U8 initUserSmart()
{
    U8 status = TRUE;
    U8 type = 0;
    /* enable user smart link */
    EnableUserSmartMode();

    /* set filter type */
    type = RECE_ONLY_BEACON__MANAG | RECE_ONLY_BROADCAST_DATA;
    SetUserReceFilter(type);
    
    /* enable user rece filter */
    if(EnableUserReceFilter() == FALSE) {
        printf("need to set user rece filter\n");
        status = FALSE;
    }

    /* set call back function and rece data */
    SetCallBackFunction(userSmartCallBackFrame);

    if(EnableCallBackFrame() == FALSE) {
        printf("need to set call back function\n");
        status = FALSE;
    }
    
    return status;
}
/*---------------------------------------------------------------------------*/
U8 exitUserSmart()
{
    U8 status = TRUE;

    /* disable user smart link */
    if(DisableUserSmartMode() == FALSE) {
        printf("disable user smart mode fail\n");
    }

    return status;
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(userSmartDemo_process, ev, data)
{
    static struct etimer timer;
    
    PROCESS_BEGIN();

    printf("userSmartDemo_process begin\n");
    
    if(app_res.res_use == 0) {
        memset(&app_res, 0, sizeof(COMM_APPS_RES));
        //gconfig_set_enable_smart(USER, 1);
        
        /* init user smart */
        if(initUserSmart() == FALSE)
            goto exit;
        
        /* init airkiss */
        airKissInitExample();

        /* set delay time 200 ms */
        gwifistatus.smart_tmerSec = 200;
        app_res.chNum = 1;
        
        while(1) {
            if(app_res.chLock == FALSE) {
                userSmartSetChannel(app_res.chNum);
            }
            etimer_set(&timer, gwifistatus.smart_tmerSec * CLOCK_MINI_SECOND);
            PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
            if(app_res.chLock == TRUE) {
                gwifistatus.connAP.channel = app_res.chNum;
            } else {
                app_res.chNum = (app_res.chNum % MAX_CHANNEL) + 1 ;
            }

            if(app_res.passLock == TRUE) {
                break;
            }
        }

    exit:
        /* disable call back frame rece */
        DisableCallBackFrame();
        
        gconfig_set_enable_smart(NOSMART, 1);

        
    /* scan channel */
    AutoScanAP(&userSmartDemo_process);
    /* wait scan response */
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
    showAllApInfo();
        /* disable user rece filter  */
        DisableUserReceFilter();
        if(checkAndSetMac() == TRUE) {
            /* exit user smart */
            if(exitUserSmart() == TRUE) {
                /* connect wifi AP */
                bss_mgmt_connect (NULL);
            } else {
                printf("wifi parameters not set correctly\n");
            }
        } else {
            printf("check ap mac fail, please auto sacn again\n");
        }
    } else {
        printf("comm res used, please check and exit process\n");
    }
    printf("userSmartDemo_process end\n");    
    PROCESS_END();
}

