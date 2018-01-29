
#include "ssv_lib.h"
#include "smartConf.h"
#include "smartComm.h"
#include "ieee80211_mgmt.h"
#include "ssv6200_rx.h"
#include "arc4.h"
#include "sys_status_api.h"


#if	defined(____D_UA_LED_Indicator____) || defined(____D_Module_IO_Indicator____)
#include	"UA_Func.h"
#endif

//constant data
U8 magicNum[11] = "iot";//strt cmd will be check magic number
U8 rc4Key[4] = "Key";//rc4 key
U8 poly = 0x1D;//crc8 poly

static struct etimer timer;
//struct data
static smart_control recePhone;
struct arc4_ctx smart_rc4;

static inline void  setSmartConfData(U8 *data, U32 value, U8 type) ATTRIBUTE_SECTION_SRAM;
static inline void checkBaseLen(U8 value, U8 type) ATTRIBUTE_SECTION_SRAM;
static inline void checkSeqSetPacket(U8 cmdValue, U8 type) ATTRIBUTE_SECTION_SRAM;
static inline U8 checkCrcWithSaveData(U8 data, U8 cmdValue, U8 type) ATTRIBUTE_SECTION_SRAM;
static inline U8 decryption(U8 value, U8 num) ATTRIBUTE_SECTION_SRAM;
static inline U8 decryptionSeq(U8 value) ATTRIBUTE_SECTION_SRAM;

/*---------------------------------------------------------------------------*/
PROCESS(smart_conf_process, "Smart Conf Process");
/*---------------------------------------------------------------------------*/
static U8
compData(U8 *a, U8 *b, U8 size)
{
    U8 i = 0;

    for(i = 0; i < size; i++)
        if(*(a + i) != *(b + i))
            return FALSE;

    return TRUE;
}

static U8
crc8_msb(U8 poly, U8* data, U8 size)
{
    U8 crc = 0;
    U8 bit = 0;

    while (size--) {
        crc ^= *data++;
        for (bit = 0; bit < 8; bit++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ poly;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}

static inline U8
decryption(U8 value, U8 num)
{
    return (value ^ recePhone.sonkey[num]);
}

static inline U8
decryptionSeq(U8 value)
{
    return (value ^ recePhone.sonkey[0]);
}

static inline U8
setCmdLen(U8 type)
{
    U8 cmdNum = getSmartCmdStatus(type);
    if(cmdNum == START) {
        recePhone.CmdNumLen[type] = strlen((char *)magicNum) + CRC8_DATA;
    } else if(cmdNum == INFO) {
        recePhone.CmdNumLen[type] =  SSID_DATA + PASS_DATA + CRC8_DATA + IP_DATA;
    } else if(cmdNum == SSID_PASS_DATA){
        recePhone.CmdNumLen[type] = recePhone.ssidLen[type] + recePhone.passLen[type] + CRC8_DATA;
    } else {
        icomprintf(GROUP_SLINK,LOG_ERROR, "### setCmdLen error: %d type: %d ###\n", cmdNum, type);
        return FALSE;
    }
    //icomprintf(GROUP_SLINK,LOG_DEBUG, "### recePhone.CmdNumLen: %d ###\n", recePhone.CmdNumLen[type]);

    return TRUE;
}

static inline U8
checkCrcWithSaveData(U8 data,U8 cmdValue, U8 type)
{
    U8 cmdNum = getSmartCmdStatus(type);

    if(recePhone.backData[type]  != 0xffff) {
        recePhone.backSeq[type]  = recePhone.CmdNumLen[type] + 1;
    } else {
        recePhone.backData[type] = cmdValue;
    }

    icomprintf(GROUP_SLINK,LOG_DEBUG, "type: %d recePhone.backData: %d\n", type, recePhone.backData[type]);
    if(recePhone.backSeq[type] == (recePhone.CmdNumLen[type] - CRC8_DATA)) {//seq num equal crc8 data
        recePhone.crcData = crc8_msb(poly,recePhone.packet[type], (recePhone.CmdNumLen[type] - CRC8_DATA));
        if(recePhone.crcData == decryption((U8)recePhone.backData[type], recePhone.backSeq[type]) && cmdNum == START) {
            if(TRUE == compData(recePhone.packet[type], magicNum, strlen((char *)magicNum))) {
                setSmartCmdStatus(INFO, type);//change start cmd to ssid_data cmd
                recePhone.backSeq[type] = 0;
                recePhone.backData[type] = 0xffff;
                memset(recePhone.packet[type], 0, CMD_DATA_MAX_LEN);
                etimer_reset(&timer);
                icomprintf(GROUP_SLINK,LOG_INFO, "[%d]have magic data\n", type);
                if(recePhone.stage1Timer == FALSE) {
                    set_smart_timer(20);
                    process_post_synch(&smart_conf_process, PROCESS_EVENT_MSG, NULL);
                    recePhone.stage1Timer =TRUE;
                }
            }
        } else if(recePhone.crcData == decryption((U8)recePhone.backData[type], recePhone.backSeq[type]) && cmdNum == INFO) {
            setSmartCmdStatus(SSID_PASS_DATA, type);//change start cmd to ssid_data cmd
            recePhone.backSeq[type] = 0;
            recePhone.backData[type] = 0xffff;
            recePhone.ssidLen[type] = recePhone.packet[type][0];
            recePhone.passLen[type] = recePhone.packet[type][SSID_DATA];
            memcpy(recePhone.phoneIP[type], &recePhone.packet[type][SSID_DATA + 1], IP_DATA);
            icomprintf(GROUP_SLINK,LOG_INFO, "[%d] phoneIP: %d.%d.%d.%d\n",type, recePhone.phoneIP[type][0],  recePhone.phoneIP[type][1],  recePhone.phoneIP[type][2],  recePhone.phoneIP[type][3]);
            if(type == PHONE_MODE) {
                memcpy(recePhone.phoneMAC[PHONE_MODE], data +10, MAC_DATA);
            } else {
                memcpy(recePhone.phoneMAC[AP_MODE], data +16, MAC_DATA);
            }
            icomprintf(GROUP_SLINK,LOG_INFO, "[%d] phoneMAC: %x:%x:%x:%x:%x:%x\n", type, recePhone.phoneMAC[type][0],  recePhone.phoneMAC[type][1],  recePhone.phoneMAC[type][2],  recePhone.phoneMAC[type][3],  recePhone.phoneMAC[type][4],  recePhone.phoneMAC[type][5]);
            memset(recePhone.packet[type], 0, CMD_DATA_MAX_LEN);
            if(recePhone.stage2Timer == FALSE) {
                set_smart_timer(40);
                process_post_synch(&smart_conf_process, PROCESS_EVENT_MSG, NULL);
                recePhone.stage2Timer =TRUE;
            }
        } else if(recePhone.crcData == decryption((U8)recePhone.backData[type], recePhone.backSeq[type]) && cmdNum == SSID_PASS_DATA){
            //icomprintf(GROUP_SLINK,LOG_DEBUG, "data cmd end packet: %s\n", recePhone.packet);
            setSmartCmdStatus(AP_LIST_CHECK, PHONE_MODE);//change ssid_data cmd to check base len
            setSmartCmdStatus(AP_LIST_CHECK, AP_MODE);//change ssid_data cmd to check base len
            recePhone.backData[type] = 0xffff;
            recePhone.baseLenBufCount[type] = 0;
            smart_set_ssid_pass((char *)recePhone.packet[type], recePhone.ssidLen[type],
                                       (char *)(recePhone.packet[type] + recePhone.ssidLen[type]), recePhone.passLen[type]);
            memset(recePhone.packet[PHONE_MODE], 0, CMD_DATA_MAX_LEN);
            memset(recePhone.packet[AP_MODE], 0, CMD_DATA_MAX_LEN);
            setSmartPhoneIP(&(recePhone.phoneIP[type][0]), IP_DATA);
            setPhoneMAC(&(recePhone.phoneMAC[type][0]), MAC_DATA);
            U8 local_mac[6];
            uip_ipaddr_t local_ipaddr;
            get_local_mac(local_mac, 6);
            get_local_ipaddr(&local_ipaddr);
            memcpy(recePhone.buf, &local_ipaddr, IP_DATA);
            memcpy(recePhone.buf + IP_DATA, local_mac, MAC_DATA);
            setPhoneData(&(recePhone.buf[0]), IP_DATA + MAC_DATA);
            return TRUE;
        } else {
            //printf("do nothing: %d\n", cmdValue);
        }
    }
    return FALSE;
}

static inline void
checkSeqSetPacket(U8 cmdValue, U8 type)
{
    U8 cmdNum = getSmartCmdStatus(type);
    recePhone.seqData[type] = decryptionSeq(cmdValue);
    recePhone.NumSeq[type] = (recePhone.seqData[type] >> cmdNum);//seq shift cmd num bit

    icomprintf(GROUP_SLINK,LOG_DEBUG, "recePhone.seqData:%d cmdValue:%d\n", recePhone.seqData[type], cmdValue);
    //icomprintf(GROUP_SLINK,LOG_DEBUG, "[%d]seqData: %d NumSeq :%d CmdNumLen: %d\n", type,recePhone.seqData[type], recePhone.NumSeq[type], recePhone.CmdNumLen[type]);
    if(((recePhone.seqData[type] & cmdNum) == cmdNum) && (recePhone.NumSeq[type] <= recePhone.CmdNumLen[type]) ) {
        //printf("[%d]NumSeq: %d backSeq :%d backData%d\n", type,recePhone.NumSeq[type], recePhone.backSeq[type], recePhone.backData[type]);
        if((recePhone.NumSeq[type] - recePhone.backSeq[type]) == 1 && recePhone.backData[type] != 0xffff) {
            recePhone.decrypValue = decryption((U8)recePhone.backData[type], recePhone.backSeq[type]);
            recePhone.packet[type][recePhone.backSeq[type]] = recePhone.decrypValue;
            icomprintf(GROUP_SLINK,LOG_DEBUG, "[%d]packet: %s\n", type, recePhone.packet[type]);
        } else {
            //printf("may be lost smartConf data: %d\n", cmdValue);
        }
        recePhone.backSeq[type] = recePhone.NumSeq[type];
    } else {
        recePhone.backSeq[type]  = recePhone.CmdNumLen[type] + 1;
       // printf("not smartConf data: %d\n", cmdValue);
    }
    recePhone.backData[type] = 0xffff;

}

static inline void
checkBaseLen(U8 value, U8 type)
{
    U8 i = 0, count = 0, lastBufVal = 0;
    //printf("[%d]recePhone.baseLenBufCount: %d value: %d \n", type ,recePhone.baseLenBufCount[type], value);
    if(recePhone.checkBaseLenBuf[type][0] >= value) {
        memset(recePhone.checkBaseLenBuf[type], 0, 5);
        recePhone.baseLenBufCount[type] = 0;
    }
    count = recePhone.baseLenBufCount[type];
    lastBufVal = recePhone.checkBaseLenBuf[type][count -1];
    if(count > 0 && (lastBufVal== value)) {
        return;
    } else if(count > 0 && ((value - lastBufVal) != 1)) {
        memset(recePhone.checkBaseLenBuf[type], 0, 5);
        recePhone.baseLenBufCount[type] = 0;
        return;
    }
    recePhone.checkBaseLenBuf[type][count] = value;
    recePhone.baseLenBufCount[type] = (count + 1) % MAX_BASE_LEN_BUF;
#if 1
    icomprintf(GROUP_SLINK,LOG_DEBUG, "status[%d] - ", type);
    for( i = 0; i < MAX_BASE_LEN_BUF; i++ ) {
        icomprintf(GROUP_SLINK,LOG_DEBUG, "%d ", recePhone.checkBaseLenBuf[type][i]);
    }
    icomprintf(GROUP_SLINK,LOG_DEBUG, "\n");
#endif
    for(i = 0; i < 3; i++) {
        if(recePhone.checkBaseLenBuf[type][i+1] - recePhone.checkBaseLenBuf[type][i] != 1) {
            if(count >= 4)
                memset(recePhone.checkBaseLenBuf[type], 0, 5);
            return;
        }
    }
    recePhone.baseLenBufCount[type] = MAX_BASE_LEN_BUF + 1;
    setSmartCmdStatus(START, type);
    if(recePhone.stopScan == FALSE) {
        stopscanchannel();
        recePhone.stopScan = TRUE;
        check_beacon(FALSE);
        process_start(&smart_conf_process, NULL);
        icomprintf(GROUP_SLINK,LOG_INFO, "set channel: %d\n", get_now_channel());
    }
}

static inline void
setSmartConfData(U8 *data, U32 value, U8 type)
{
    U32 size = 0;
    U8 status = 0;

    if(recePhone.baseLenBufCount[type] <= MAX_BASE_LEN_BUF) {
    	checkBaseLen(value, type);
    } else {
        if(value < recePhone.checkBaseLenBuf[type][0] )
            return;

        size  =  value - recePhone.checkBaseLenBuf[type][0] ;//send data offset 1 byte
        status = setCmdLen(type);
        //printf("value: %d size: %d\n", value, size);
        if(FALSE == status) {
            icomprintf(GROUP_SLINK,LOG_ERROR, "getCmdLen can not get normal cmd number\n");
        }

        if(size >= SEQ_BASE_CMD) {// seq data
            checkSeqSetPacket(size - SEQ_BASE_CMD, type);
        } else {// ssid & password data
            if(TRUE == checkCrcWithSaveData(data, size, type)) {
                 //SET_REASON_TRAP0(0x0000);
            } else {
                //printf("setSmart do nothing: %d\n", cmdValue);
            }
        }
    }
}
/*---------------------------------------------------------------------------*/
void clearSmartLinkBuf()
{
    if(getSmartCmdStatus(PHONE_MODE) == CHECK) {
        memset(recePhone.checkBaseLenBuf[PHONE_MODE], 0, 5);
        recePhone.CmdNumLen[PHONE_MODE] = 0;
        recePhone.baseLenBufCount[PHONE_MODE]= 0;
    } else if(getSmartCmdStatus(AP_MODE) == CHECK) {
        memset(recePhone.checkBaseLenBuf[AP_MODE], 0, 5);
        recePhone.CmdNumLen[AP_MODE] = 0;
        recePhone.baseLenBufCount[AP_MODE]= 0;
    }
    clearRxBuffer();
}

void clearAllSmartLinkBuf()
{
    memset(recePhone.checkBaseLenBuf[PHONE_MODE], 0, 5);
    recePhone.CmdNumLen[PHONE_MODE] = 0;
    recePhone.baseLenBufCount[PHONE_MODE]= 0;
    memset(recePhone.checkBaseLenBuf[AP_MODE], 0, 5);
    recePhone.CmdNumLen[AP_MODE] = 0;
    recePhone.baseLenBufCount[AP_MODE]= 0;
    clearRxBuffer();
}

void resetSmartLink()
{
    clearAllSmartLinkBuf();
    setSmartCmdStatus(CHECK, PHONE_MODE);
    setSmartCmdStatus(CHECK, AP_MODE);
    set_smart_timer(10);
    recePhone.stopScan = FALSE;
    recePhone.stage1Timer = FALSE;
    recePhone.stage2Timer = FALSE;
    startscanchannel(200 * CLOCK_MINI_SECOND);
}

void initSmartLink()
{
    U8 tmpSonkey[CMD_DATA_MAX_LEN];
    memset(tmpSonkey, 0, sizeof(tmpSonkey));
    clear_mac_bssid();
    enableSmartHwFilter();
    set_smart_timer(10);
    memset(&recePhone, 0, sizeof(recePhone));
    recePhone.backData[PHONE_MODE] = 0xffff;
    recePhone.backData[AP_MODE] = 0xffff;
    recePhone.stopScan = FALSE;
    recePhone.stage1Timer = FALSE;
    recePhone.stage2Timer = FALSE;
    setSmartCmdStatus(CHECK, PHONE_MODE);
    setSmartCmdStatus(CHECK, AP_MODE);
    arc4_setkey(&smart_rc4, rc4Key, strlen((char *)rc4Key));
    arc4_xor(&smart_rc4, tmpSonkey, recePhone.sonkey, CMD_DATA_MAX_LEN);
}

void rx_process_smartConf(U8 *data, U32 len)
{
    U8 cmdNum = 0;
    U8 type = NOTYPE;

    if(TRUE == isSmartConfigDataFromPhone()) {
        type = PHONE_MODE;
    } else if(TRUE == isSmartConfigDataFromAP()) {
        type = AP_MODE;
    } else
        return;

    cmdNum = getSmartCmdStatus(type);
    if(cmdNum != DONE) {
        if(cmdNum != AP_LIST_CHECK) {
            setSmartConfData(data, len, type);
        } else {
            setSmartCmdStatus(AP_LIST_CHECK, type);
            process_post_synch(&smart_conf_process, PROCESS_EVENT_CONTINUE, NULL);
        }
    } else {
        //dump_data ("other data", data, len);
    }
}



void hx_update_ap_info();
extern U8 sys_slink_mode;

PROCESS_THREAD(smart_conf_process, ev, data)
{

    PROCESS_BEGIN();

    startrecordAP();

	#if	defined(____D_UA_LED_Indicator____) || defined(____D_Module_IO_Indicator____)
	R_iSmartLinking = 1;
	#endif

    while(1) {
        icomprintf(GROUP_SLINK,LOG_INFO, "smartlink adjust timer: %d sec\n", get_smart_timer());
        etimer_set(&timer, get_smart_timer()* CLOCK_SECOND);
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER || ev == PROCESS_EVENT_CONTINUE || ev == PROCESS_EVENT_MSG);
        if(ev !=  PROCESS_EVENT_MSG)
            break;
    }
    if(ev == PROCESS_EVENT_TIMER)
    {
        icomprintf(GROUP_SLINK,LOG_INFO, "RESET\n");
        resetSmartLink();
        goto exit;
    }

    disableSmartHwFilter();
    etimer_set(&timer, CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
    if(checkAndConnAP() == 0) {
        icomprintf(GROUP_SLINK,LOG_INFO, "BREAK\n");
		sys_slink_mode = 0;			// 连接OK后退出
		hx_update_ap_info();		// 保存ssid/password
		
    } else {
        enableSmartHwFilter();
        icomprintf(GROUP_SLINK,LOG_INFO, "not found ap, reset again\n");
        resetSmartLink();
    }

exit:
	#if	defined(____D_UA_LED_Indicator____) || defined(____D_Module_IO_Indicator____)
	R_iSmartLinking = 2;
	#endif
    stoprecordAP();

    PROCESS_END();
}

