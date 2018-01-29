
#ifndef _SMARTCOMM_H_
#define _SMARTCOMM_H_


#include "sys/process.h"

#define SMART_TYPE 1

#if (SMART_TYPE == 0)
//#define SMART_DISABLE
#define SMART_ICOMM
#define SMART_WECHAT
#define SMART_USER
#define ENABLE_SMART_LINK NOSMART
#elif (SMART_TYPE == 1)
//#define SMART_ICOMM
#define SMART_ICOMM
#define SMART_WECHAT
#define SMART_USER
#define ENABLE_SMART_LINK ICOMM
#elif (SMART_TYPE == 2)
//#define SMART_WECHAT
#define SMART_ICOMM
#define SMART_WECHAT
#define SMART_USER
#define ENABLE_SMART_LINK WECHAT
#elif (SMART_TYPE == 3)
//#define SMART_USER
#define SMART_ICOMM
#define SMART_WECHAT
#define SMART_USER
#define ENABLE_SMART_LINK USER
#elif (SMART_TYPE == 4)
#define SMART_ICOMM
#define SMART_WECHAT
#define SMART_USER
#define ENABLE_SMART_LINK USER
#else
#define SMART_DISABLE
#define ENABLE_SMART_LINK NOSMART
#endif

typedef enum t_SMARK_LINK_TYPE
{
    NOSMART	= 0,
    ICOMM,
    WECHAT,
    USER,
    JDSMART,
} SMARK_LINK_TYPE;

#define IP_LEN 4
#define MAC_LEN 6
#define PHONE_PORT 8209
#define DEVICE_PORT 8302
#define SEND_MAX_BUF 64

enum DATA_STATUS
{
    NOTYPE = -1,
    AP_MODE = 0,
    PHONE_MODE = 1,
    BOTH = 2,
} DATA_STATUS;

enum CMD_NUM
{
    START = 0,
    INFO,
    SSID_PASS_DATA, 
    AP_LIST_CHECK, 
    DONE, 
    CHECK = 255,
} CMD_NUM;

typedef struct _smartComm {
    U8 sendToPhoneEnable;
    U8 sendToPhoneDataEnable;
    U8 phoneIP[IP_LEN];
    U8 phoneMAC[MAC_LEN];
    U8 buf[SEND_MAX_BUF];
    U8 bufLen;
    struct process *callback;
} smartComm;

void SmartSetChannel (U8 ch);
U8 clear_mac_bssid();
U8 get_now_channel();
U8 get_smart_timer();
int set_smart_timer(U8 sec);
int check_beacon(U8 *enable);
int smart_set_ssid_pass(U8 *pssid, U8 ssidlen, U8 *pkey, U8 keylen);
void set_slink_ready_ack(struct process *p);
void enableSmartHwFilter();
void disableSmartHwFilter();

void setSmartCmdStatus(U8 cmd, U8 type);
U8 getSmartCmdStatus(U8 type) ATTRIBUTE_SECTION_SRAM;

void disableSendToPhone();
U8 getSendToPhoneStatus();

void enableSendToPhoneData();
void disableSendToPhoneData();
U8 getSendToPhoneDataStatus() ATTRIBUTE_SECTION_SRAM;

void smartSetIfconfigAndConnect();
U8 checkAndConnAP();

inline U8 isSmartConfigDataFromPhone() ATTRIBUTE_SECTION_SRAM;
inline U8 isSmartConfigDataFromAP() ATTRIBUTE_SECTION_SRAM;

void sendMessageToPhone();
void setPhoneData(U8 *data, U8 len);
void setPhoneMAC(U8 *mac, U8 len);
void setSmartPhoneIP(U8 *ip, U8 len);

#endif

