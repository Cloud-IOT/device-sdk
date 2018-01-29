
#include <stdio.h>
#include "ieee80211_mgmt.h"
#include "systemconf.h"
#include "ssv_lib.h"
#include "hash_alg.h"
#include "dbg-atcmd-api.h"

extern IEEE80211STATUS gwifistatus;

char *demoSSID = "ABCDE";
char *demoPass = "";

PROCESS(sta_connect_demoProcess, "sta_connect_process");

void showAllAp()
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

S8 checkApInList()
{
    int i = 0;
    int ssid_len = strlen(demoSSID);
    int pass_len = strlen(demoPass);
    for (i=0; i<MAX_AP_LIST; i++) {
        if((ssid_len == ap_list[i].name_len) && (ssid_len > 0)) {
            if (strncmp (demoSSID, (char*)(&(ap_list[i]).name), ap_list[i].name_len) == 0) {
                memcpy (&(gwifistatus).connAP.mac, &(ap_list[i]).mac, 6);
                gwifistatus.connAP.channel = ap_list[i].channel;
                gwifistatus.connAP.security_type = ap_list[i].security_type;
                gwifistatus.connAP.security_subType = ap_list[i].security_subType;
                strcpy ((char*)gwifistatus.connAP.ssid, demoSSID);
                gwifistatus.connAP.ssid_len = ssid_len;
                strcpy ((char*)gwifistatus.connAP.key, demoPass);
                gwifistatus.connAP.key_len = pass_len;
                return 0;
            }
        }
    }
    return -1;
}

void setSSIDAndPass()
{
    _DBG_AT_SET_PEERMAC(&(gwifistatus).connAP.mac[0], 0);
    _DBG_AT_SET_BSSID(&(gwifistatus).connAP.mac[0]);
    _DBG_AT_SET_CHANNEL(gwifistatus.connAP.channel);
    if(gwifistatus.connAP.key_len > 0 && gwifistatus.connAP.ssid_len > 0)	
        pbkdf2_sha1(gwifistatus.connAP.key, gwifistatus.connAP.key_len, gwifistatus.connAP.ssid, gwifistatus.connAP.ssid_len, 4096, (unsigned char*)gwifistatus.connAP.pmk, 32);
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sta_connect_demoProcess, ev, data)
{
    S8 status = 0;
    
    PROCESS_BEGIN();
    printf("sta_connect_process begin\n");
    if(gwifistatus.status == IEEE80211_DISCONNECTED) {
        bss_mgmt_scan(&sta_connect_demoProcess);
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
        printf("scan ap ready\n");
        //showAllAp();
        status = checkApInList();
        if(status == 0) {
            setSSIDAndPass();
            bss_mgmt_connect (&sta_connect_demoProcess);
            PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_MSG);
            printf("module connect to ap ready\n");
        } else {
            printf("### ap not in the list, try to connect hide ap: %s ###\n");
            bss_mgmt_connect_active (demoSSID, strlen(demoSSID), demoPass, strlen(demoPass), NULL);
        }
    } else {
        printf("module already connect to ap\n");
    }
    printf("sta_connect_process end\n");
    PROCESS_END();
}
