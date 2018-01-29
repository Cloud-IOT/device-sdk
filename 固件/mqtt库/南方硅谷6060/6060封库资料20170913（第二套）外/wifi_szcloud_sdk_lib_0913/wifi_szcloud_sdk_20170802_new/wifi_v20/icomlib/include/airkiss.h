/*
 * airkiss.h
 *
 *  Created on: 2015-1-26
 *      Author: peterfan
 */

#ifndef AIRKISS_H_
#define AIRKISS_H_

#include "ssv_types.h"
#include "smartComm.h"

#if (defined(SMART_WECHAT) ||  defined(SMART_USER))

#ifdef __cplusplus
extern "C" {
#endif

#ifndef AIRKISS_ENABLE_CRYPT
#define AIRKISS_ENABLE_CRYPT 0
#endif

#define AIRKISS_SCAN_PERIOD 150 * CLOCK_MINI_SECOND

typedef void* (*airkiss_memset_fn) (void* ptr, int value, unsigned int num);
typedef void* (*airkiss_memcpy_fn) (void* dst, const void* src, unsigned int num);
typedef int (*airkiss_memcmp_fn) (const void* ptr1, const void* ptr2, unsigned int num);
typedef int (*airkiss_printf_fn) (const char* format, ...);

typedef struct
{
    airkiss_memset_fn memset;
    airkiss_memcpy_fn memcpy;
    airkiss_memcmp_fn memcmp;
    airkiss_printf_fn printf;
} airkiss_config_t;

typedef struct
{
    int dummyap[26];
    int dummy[32];
} airkiss_context_t;

typedef struct
{
	char* pwd;
	char* ssid;	
	unsigned char pwd_length;
	unsigned char ssid_length;
	unsigned char random;
	unsigned char reserved;
} airkiss_result_t;

typedef enum
{
    AIRKISS_STATUS_CONTINUE = 0,
    AIRKISS_STATUS_CHANNEL_LOCKED = 1,
    AIRKISS_STATUS_COMPLETE = 2
} airkiss_status_t;

#if AIRKISS_ENABLE_CRYPT
int airkiss_set_key(airkiss_context_t* context, const unsigned char* key, unsigned int length);
#endif

const char* airkiss_version(void);

int airkiss_init(airkiss_context_t* context, const airkiss_config_t* config);

int inline airkiss_recv(airkiss_context_t* context, const void* frame, unsigned short length) ATTRIBUTE_SECTION_SRAM;

int airkiss_get_result(airkiss_context_t* context, airkiss_result_t* result);

int airkiss_change_channel(airkiss_context_t* context);
typedef enum
{
	/* 提供的?据????度不足 */
	AIRKISS_LAN_ERR_OVERFLOW = -5,

	/* ?前版本不支持的指令?型 */
	AIRKISS_LAN_ERR_CMD = -4,

	/* 打包?据出? */
	AIRKISS_LAN_ERR_PAKE = -3,

	/* 函?????出? */
	AIRKISS_LAN_ERR_PARA = -2,

	/* ?文?据?? */
	AIRKISS_LAN_ERR_PKG = -1,

	/* ?文格式正确，但是不需要???理的?据包 */
	AIRKISS_LAN_CONTINUE = 0,

	/* 接收到?????求?据包 */
	AIRKISS_LAN_SSDP_REQ = 1,

	/* ?据包打包完成 */
	AIRKISS_LAN_PAKE_READY = 2


} airkiss_lan_ret_t;
void airKissResult();

void clearAirKissBuf();
typedef enum
{
	AIRKISS_LAN_SSDP_REQ_CMD = 0x1,
	AIRKISS_LAN_SSDP_RESP_CMD = 0x1001,
	AIRKISS_LAN_SSDP_NOTIFY_CMD = 0x1002
} airkiss_lan_cmdid_t;

void initAirKiss();


int airkiss_lan_recv(const void* body, unsigned short length, const airkiss_config_t* config);
airkiss_status_t inline airKissRecv(void *frame, short len) ATTRIBUTE_SECTION_SRAM;

int airKissInit();

void airKissGetVersion();
void rx_process_airkiss(U8 *data, U32 len) ATTRIBUTE_SECTION_SRAM;
int airkiss_lan_pack(airkiss_lan_cmdid_t ak_lan_cmdid, void* appid, void* deviceid, void* _datain, unsigned short inlength, void* _dataout, unsigned short* outlength, const airkiss_config_t* config);

#ifdef __cplusplus
}
#endif

#endif

#endif /* AIRKISS_H_ */
