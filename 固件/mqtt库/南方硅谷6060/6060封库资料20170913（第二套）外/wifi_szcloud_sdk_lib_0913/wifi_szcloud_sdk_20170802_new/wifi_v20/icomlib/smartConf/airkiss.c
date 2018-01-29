
#include "ssv_lib.h"
#include "airkiss.h"
#include "smartComm.h"
#include "ieee80211_mgmt.h"
#include "ssv6200_rx.h"
#include "console.h"
#include "atcmdlib.h"

airkiss_context_t akcontex;
const airkiss_config_t akconf = {
    (airkiss_memset_fn)&memset,
    (airkiss_memcpy_fn)&memcpy,
    (airkiss_memcmp_fn)&memcmp,
    0
};
/*---------------------------------------------------------------------------*/
PROCESS(airkiss_conf_process, "AirKiss Conf Process");
/*---------------------------------------------------------------------------*/

void airKissResult(U8 *data)
{
    airkiss_result_t result;
    U8 sendVaule = 0;
    U8 phoneMAC[6];
    airkiss_get_result(&akcontex, &result);

    smart_set_ssid_pass(result.ssid, result.ssid_length, result.pwd, result.pwd_length);
    if(TRUE == isSmartConfigDataFromPhone()) {
        memcpy(phoneMAC, data + 10, 6);
    } else if(TRUE == isSmartConfigDataFromAP()) {
        memcpy(phoneMAC, data + 16, 6);
    }
    setPhoneMAC(phoneMAC, 6);
    sendVaule = result.random;
    setPhoneData(&sendVaule, 1);
}

airkiss_status_t inline airKissRecv(void *frame, short len)
{
    return airkiss_recv(&akcontex, frame, (unsigned short)len);
}

void airKissPrintVersion()
{
    U8 *ver;
    ver = (U8 *)airkiss_version();
    atcmdprintf("##%s##\n",ver);
}

int airKissInit()
{
    int ret = 0;
    
    airKissPrintVersion();
    ret = airkiss_init(&akcontex, &akconf);
    
    return ret;
}

void SetAirkissStart()
{
    setSmartCmdStatus(START, BOTH);
    check_beacon(FALSE);
    process_start(&airkiss_conf_process, NULL);
    printf("set channel: %d\n", get_now_channel());
}
/*---------------------------------------------------------------------------*/
void clearAirKissBuf()
{
    if(getSmartCmdStatus(BOTH) == CHECK) {
        airkiss_init(&akcontex, &akconf);
        clearRxBuffer();
    }
}

void resetAirkiss()
{
    clearAirKissBuf();
    setSmartCmdStatus(CHECK, BOTH);
    startscanchannel(200 * CLOCK_MINI_SECOND);
}

void initAirKiss()
{
    int ret = 0;
    
    clear_mac_bssid();
    enableSmartHwFilter();
    
    ret = airKissInit();
    if(ret < 0) {
        printf("airKissInit fail: %d\n", ret);
    }
    setSmartCmdStatus(CHECK, BOTH);
}

void rx_process_airkiss(U8 *data, U32 len)
{
    airkiss_status_t ret = 0;
    U8 cmdNum = getSmartCmdStatus(BOTH);
    if(((isSmartConfigDataFromAP() == TRUE) && (cmdNum != DONE)) || 
    ((isSmartConfigDataFromPhone() == TRUE) && (cmdNum != DONE))) {
        if(cmdNum != AP_LIST_CHECK) {
            ret = airKissRecv((void *)data, len);
            if(ret == AIRKISS_STATUS_CHANNEL_LOCKED)
            {
                printf("##########AIRKISS_STATUS_CHANNEL_LOCKED: %d##########\n", ret);
                stopscanchannel();
                SetAirkissStart();
            } else if(ret == AIRKISS_STATUS_COMPLETE) {
                printf("##########AIRKISS_STATUS_COMPLETE: %d##########\n", ret);
                airKissResult(data);
                setSmartCmdStatus(AP_LIST_CHECK, BOTH);
                process_post_synch(&airkiss_conf_process, PROCESS_EVENT_CONTINUE, NULL);
            }
        }
    }
}

PROCESS_THREAD(airkiss_conf_process, ev, data)
{
    static struct etimer timer;
    
    PROCESS_BEGIN();

    startrecordAP();
    set_smart_timer(40);
    etimer_set(&timer, get_smart_timer()* CLOCK_SECOND);	
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER || ev == PROCESS_EVENT_CONTINUE);
    if(ev == PROCESS_EVENT_TIMER) {
        printf("RESET\n");
        resetAirkiss();
        goto exit;
    }

    disableSmartHwFilter();
    etimer_set(&timer, CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
    if(0 == checkAndConnAP()) {
        printf("BREAK\n");
    } else {
        printf("not found ap, reset again\n");
        enableSmartHwFilter();
        resetAirkiss();
    }
     
exit:
    stoprecordAP();
    
    PROCESS_END();	
}
/*---------------------------------------------------------------------------*/
