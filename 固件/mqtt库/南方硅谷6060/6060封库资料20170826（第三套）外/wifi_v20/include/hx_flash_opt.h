#ifndef _HX_FLASH_OPT_H_
#define _HX_FLASH_OPT_H_

#include "data_buff.h"
#include "uart_cmd.h"
#include "hekr_cmd.h"
//#ifdef __TUYA_CLOUD_SUPPORT__
#include "tuya_http_inf.h"
//#endif
#define HX_SSID_LEN	33
#define HX_BSSID_LEN 13
#define HX_AP_SSID_KEYLEN	16

/*
*配置信息在flash中的存放:
----No 1------
*conf - 0.5K
*HX - 1K
*TUYA - 1.5K
*AC_CLOUD - 1K
----No 2------
*GIZ_CLOUD - 1K
*SK_TIMER - 1K

*在读写flash数据时，以4K为单位，故在配置数据组织时，也以4K为单位，所以在上述的配置数据组织时
*已分成2块，同时，需保证读写的起始地址也在4K的边界
*同时，对于第2块的读写，则以GIZ_PARAM_OFFSET为基准，其余的进行偏移即可
*/
#define HX_PARAM_OFFSET (0x200)
#define HX_PARAM_LEN (0x400)	/*1K长度*/

#define TUYA_PARAM_OFFSET (HX_PARAM_OFFSET + HX_PARAM_LEN)
#define TUYA_SCHEMA_TABLE_OFFSET	(TUYA_PARAM_OFFSET + sizeof(tuya_cloud_info_t) + 32)
#define TUYA_SCHEMA_OBJ_OFFSET		(TUYA_SCHEMA_TABLE_OFFSET + sizeof(tuya_idx_entry_t) * TUYA_MAX_OBJ_NUM)
#define TUYA_PARAM_END	(TUYA_SCHEMA_OBJ_OFFSET + sizeof(tuya_obj_int_t) * TUYA_MAX_OBJ_NUM)
#define TUYA_PARAM_LEN 0x600	/*1.5K*/


//#if TUYA_SCHEMA_OBJ_OFFSET > (4 * 1024)
//#error "FLASH OFFSET IS BEYOND 4K"
//#endif

#define AC_CLOUD_PARAM_OFFSET (TUYA_PARAM_OFFSET + TUYA_PARAM_LEN)
#define AC_CLOUD_PARAM_LEN (0x400)	/*1K*/
#define AC_CLOUD_PARAM_END	(AC_CLOUD_PARAM_OFFSET + AC_CLOUD_PARAM_LEN)	/*120为sizeof(ZC_ConfigDB)*/

/*------第2个4K------*/
#define GIZ_PARAM_OFFSET AC_CLOUD_PARAM_END
#define GIZ_PARAM_LEN	(0x400)	/*1K长度*/
#define GIZ_PARAM_END (GIZ_PARAM_OFFSET + GIZ_PARAM_LEN)	/*特别需注意的是，GIZ_PARAM_END不能超过CFG_CUSTOMER_SIZE*/

#define CONF_SECOND_START GIZ_PARAM_OFFSET	/*第2个4k的起始位置,应保证其值为0x1000*/

/*插座定时器保存项*/
#define SK_PARAM_OFFSET GIZ_PARAM_END
#define SK_PARAM_LEN	(0x400)	/*1K长度*/
#define SK_PARAM_END (SK_PARAM_OFFSET + SK_PARAM_LEN)	/*特别需注意的是，SK_PARAM_LEN不能超过CFG_CUSTOMER_SIZE*/




Boolean hx_write_conf();
void hx_update_parameter(int *p_esp_char, int * p_esp_time, int * p_atpt_time, int * p_frame_length,
		hx_uart_t *p_uart, int *p_gpio_mode, Boolean *p_data_flag, hx_sock_t *p_sock_info);
void hx_update_conn_parameter(char *ssid, char *bssid, char *key);
void hx_init_conn_parameter(char *ssid, char *bssid, char *key);
Boolean hx_init_parameter(int *p_esp_char, int * p_esp_time, int * p_atpt_time, int * p_frame_length,
		hx_uart_t *p_uart, int *p_gpio_mode,  Boolean *p_data_flag, hx_sock_t *p_sock_info, Boolean *p_ap_mode,  U8 *p_gpio1_status);

Boolean hx_init_parameter_2();

void hx_update_echo_flag(Boolean *p_flag);
Boolean hx_init_echo_flag(Boolean *p_flag);
Boolean hx_init_smtlnk_flag(char *p_flag);
void hx_update_smtlnk_flag(char *p_flag);

#define HX_SMARTCONF_LINK	1			// smart conf 方式配网
#define HX_SOFTAP_LINK		2			// softap 方式配网
#define HX_AIRKISS_LINK		3			// Airkiss 方式配网

Boolean hx_update_ap_conf(U8 *encrypt_mode, char *ssid, char *key);
Boolean hx_get_ap_conf(U8 *encrypt_mode, char *ssid, int ssid_len, char *key, int key_len);

void hekr_update_conn_parameter(char *token, char *ctrlKey, char *bindKey);
void hekr_init_conn_parameter(char *token, char *ctrlKey, char *bindKey);

char * hx_get_ssid();
char * hx_get_passwd();

Boolean tuya_read_conf_help(int offset, void *ptr, int len);
Boolean tuya_save_conf_help(int offset, void *ptr, int len);

void hx_init_web_sec_info();
char * hx_get_web_username();
char * hx_get_web_password();
Boolean hx_update_web_username(char *name, char *pass);
int hx_get_uart_bdrate();
const char *hx_get_bdrate_string();
Boolean hx_update_uart_bdrate(int rate);

#ifdef __TUYA_CLOUD_SUPPORT__
Boolean tuya_read_conf();
Boolean tuya_save_conf();
#endif

/*获取配网模式*/
extern char hx_get_smtlnk_flag(void);

/*
*start为数据读写的起始位置,为一相对偏移
*
*配置数据是以每4K为一个单位来进行整读、整写的，故当读写的位置如果超过了第1个4k，则需往第2个4k中读写，以此类推
*为实现上述的读写方式，故提供一统一接口，如下的两个函数，具体思路如下
*a. 通过start来获取其在第几个4K中 
*b. 通过在第几个4k中，可获取到其读写的绝对起始位置，同时，还可获取其距第几个4k的偏移差值
*c. 为保证正确，需增加相关的判断
*
*特别注意: 应用模块在定义其配置文件在flash中的存放时，注意其整体保证在在1个4k内，别跨2个4K间
*/
extern BOOL conf_read_help(U32 start, void *ptr, int len);
extern BOOL conf_save_help(U32 start, void *ptr, int len);

#endif

