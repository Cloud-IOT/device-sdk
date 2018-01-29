#ifndef _TUYA_HTTP_INF_H_
#define _TUYA_HTTP_INF_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "c_types.h"
#include "tuya_com_def.h"
#include "data_buff.h"
#include "cJSON.h"
#include "sk_timer.h"


#define TUYA_DATA_VERSION 4					// attention. 

#define TUYA_HTTP_DEBUG_ON 0


#if TUYA_DEBUG_ON
#define TUYA_HTTP_DEBUG(fmt,...) os_printf(fmt, ##__VA_ARGS__)
#else
#define TUYA_HTTP_DEBUG(fmt,...)
#endif

// 0x79:SAVE1  0x7A:SAVE2  0x7B:flag
#define TUYA_PARAM_START_SEC         0x79

#define TUYA_OBJ_NAME_LEN	8
#define TUYA_OBJ_FLAG_RDONLY 1
#define TUYA_OBJ_FLAG_RDWR 2
#define TUYA_OBJ_FLAG_NULL	4

#define TUYA_MAX_OBJ_NUM	16

#define TUYA_BOOL_OBJ_TYPE	1
#define TUYA_INT_OBJ_TYPE 2

#define TUYA_DP_SOCK1	"1"			// 插座1数据点
#define TUYA_DP_SOCK11	"11"			// 插座11数据点
#define TUYA_DP_DATA	"101"		// 透传数据点


#define TUYA_LAN_PROTOCOL_V3_1 				// 协议版本3.1是经过测试

#ifdef TUYA_LAN_PROTOCOL_V3_1
#define TUYA_LAN_PROTOCOL_VER	"3.1"
#define TUYA_MQTT_PROTOCOL_VER	"2.1"

#define TUYA_MQTT_PROTOCOL_V2_1 
#else
#define TUYA_LAN_PROTOCOL_VER	"3.2"
#define TUYA_MQTT_PROTOCOL_VER	"2.2"
#endif

typedef enum 
{
	TUYA_DP_CMD_LAN = 0,
	TUYA_DP_CMD_MQ
} DP_CMD_TYPE_E;

typedef enum 
{
	TUYA_TIMER_WRITE = 0X5,		// 透传下发指令
	TUYA_TIMER_READ = 0X2,		// 读取数据
	TUYA_TIMER_REPORT = 0X6,	// 透传上报指令
} tuya_cmd_t ;

enum 
{
	TUYA_TIMER_RSP = 0X3,					// 命令回复数据
	TUYA_TIMER_STATUS_REPORT = 0x4			// 主动上报数据
};

enum 
{
	TUYA_UPDATE_TIMER,
	TUYA_DELETE_TIMER
};



typedef struct
{
	char devId[32];
	char secKey[32];
	char localKey[32];
	char authKey[48];
	char productKey[32];
	char uuid[32];
	char muid[32];
	char macstr[32];
	
	char oldDevId[32];
	char oldSecKey[32];
	char prtl_ver[16];

	char apiUrl[32];
	char mqttUrl[32];

	uint8 register_flag;// 1:该设备已经注册, devId
	uint8 active_flag;// 该设备已经激活, secKey&localKey
	uint8 init_flag;	// 1:参数初始化, 非1:表示第一次载入参数
	int   data_version;		// 变化一次加1
	
	char token[32];		// APP token , 配网的时候提供

	int is_link;	// 是否配网

	char ota_url[128];
	int  ota_size;
	char ota_md5[32 + 1];
	char ota_ver[16];
	int ota_flag;	// 是否进行ota升级标志，升级完成清除
} tuya_cloud_info_t;

typedef struct
{
	unsigned char total;
	unsigned char cnt;
	char *pos[0];
} HTTP_PARAM_H_S;

#define DEF_URL_LEN 900
typedef struct
{
	HTTP_PARAM_H_S *param_h; // param handle
	char *param_in; // param insert pos
	unsigned short head_size; // head_size == "url?" or "url"
	unsigned short buf_len; // bufer len
	char buf[0]; // "url?key=value" + "kev=value statistics"
} HTTP_URL_H_S;

// aes
typedef struct
{
	//MUTEX_HANDLE mutex;
	unsigned char key[16];
	unsigned char iv[16]; // init vector
} AES_H_S;

typedef struct {
	char name[TUYA_OBJ_NAME_LEN];			// 对象名称
	int offset;								// 在flash中的存储位置
} tuya_idx_entry_t;

typedef struct
{
	uint8_t type;
	uint8_t flag;
} tuya_obj_base_t;				// 基类

typedef struct
{
	uint8_t type;
	uint8_t flag;
	int v;
} tuya_obj_bool_t;			// Boolean 对象

typedef struct
{
	uint8_t type;
	uint8_t flag;
	int v;
	int min;
	int max;
	int step;
} tuya_obj_int_t;		// 整数对象

typedef struct tuya_obj_array_struct
{
	tuya_idx_entry_t tuya_obj_array[TUYA_MAX_OBJ_NUM];
	char tuya_obj_buff[sizeof(tuya_obj_int_t) * TUYA_MAX_OBJ_NUM];
} tuya_obj_array_mem_t;

// 所有的整数都是用大端表示
// lan protocol app head
#ifdef WIN32

#pragma pack(1)
typedef struct lan_app_packet_struct
{
    UINT head; // 0x55aa			
    UINT fr_num;
    UINT fr_type;
    UINT len;
    BYTE data[0];
} lan_app_packet_t ;			// APP发送给网关的数据包

typedef struct lan_tail_packet_struct
{
    UINT crc;
    UINT tail; // 0xaa55
} lan_tail_packet_t;		// 包的结束

// lan protocol gateway head 
typedef struct lan_gw_packet_struct
{
    UINT head; // 0x55aa
    UINT fr_num;
    UINT fr_type;
    UINT len;
    UINT ret_code;
    BYTE data[0];
} lan_gw_packet_t;		// 网关回给APP的数据包

typedef struct __tuya_data_t
{
	U8 cmd;
	U8 version;
	U8 data_cmd;
	U8 status;
	union 
	{
		U8 _action;
		U8 _count;
	} _u;
	sk_timer_t sk_timer;
} tuya_data_t;

#pragma pack()

#else

typedef struct
{
    UINT head; // 0x55aa			
    UINT fr_num;
    UINT fr_type;
    UINT len;
    BYTE data[0];
} __attribute__ ((packed)) lan_app_packet_t ;			// APP发送给网关的数据包

typedef struct 
{
    UINT crc;
    UINT tail; // 0xaa55
} __attribute__ ((packed)) lan_tail_packet_t;		// 包的结束

// lan protocol gateway head 
typedef struct 
{
    UINT head; // 0x55aa
    UINT fr_num;
    UINT fr_type;
    UINT len;
    UINT ret_code;		
    BYTE data[0];
}  __attribute__ ((packed)) lan_gw_packet_t;		// 网关回给APP的数据包


typedef struct __tuya_data_t
{
	uint8_t cmd;
	uint8_t version;
	uint8_t data_cmd;
	uint8_t status;
	union 
	{
		U8 _action;
		U8 _count;
	} _u;
	
	sk_timer_t sk_timer;
} __attribute__ ((packed)) tuya_data_t;


#endif

// 1-硬件准备就绪,2-升级中,3-升级完成,4-升级异常
typedef enum 
{
	TUYA_HW_READY = 1,
	TUYA_SW_UPGRADING,
	TUYA_SW_UPGRADE_DONE,
	TUYA_SW_UPGRADE_ERROR
} TUYA_UPGRADE_STATUS;

typedef enum 
{
	TUYA_WIFI_FIRMWARE = 1,
	TUYA_MCU_FIRMWARE = 9
} TUYA_UPGRADE_TYPE;

extern tuya_cloud_info_t g_tuya_cloud_param;

extern unsigned long now_seconds;			// 1970 年以来的秒数
#define TUYA_MAX_DP_NUM 3
extern char *dp_node_array[TUYA_MAX_DP_NUM + 1];


typedef void (*tuya_func_t)();

void tuya_param_save(void);
void tuya_param_load(void);
void tuya_param_restore(void);
bool tuya_httpclient_create(void);
bool tuya_httpclient_delete(void);
int tuya_httpclient_send(uint8_t *buf, int len);
void tuya_httpclient_device_create(void);
void tuya_httpclient_device_active(void);
void tuya_httpclient_device_reset(void);
void tuya_httpclient_device_exist(void);

void tuya_emit_active_process(Boolean active);

void tuya_send_http_content(int sock);
void tuya_httpclient_recv_cb(char *pdata, unsigned short len);

void tuya_parse_uart_data_bufffer(buf_t *b);
void user_uart_write_data(unsigned char  *pbuff, int len);
char *tuya_get_token();
char *tuya_get_region();

Boolean tuya_add_func(tuya_func_t f);
tuya_func_t tuya_del_func();
Boolean tuya_is_func_queue_empty();
void tuya_notify_http_parse();
void tuya_do_ota();
void tuya_startup_http_process();
void tuya_ota(char *url);

int tuya_check_version(const char *v1, const char *v2);

int ICACHE_FLASH_ATTR Add_Pkcs(char *p, int len) ATTRIBUTE_SECTION_SRAM;
void ICACHE_FLASH_ATTR del_http_url_h(HTTP_URL_H_S *hu_h);
OPERATE_RET ICACHE_FLASH_ATTR httpc_aes_encrypt(const unsigned char *plain, unsigned int len, unsigned char *cipher);
int ICACHE_FLASH_ATTR StrToHex(unsigned char *pbDest, unsigned char *pbSrc, int nLen);
OPERATE_RET ICACHE_FLASH_ATTR httpc_aes_decrypt(unsigned char *plain, const unsigned char *cipher, unsigned int len) ATTRIBUTE_SECTION_SRAM;
OPERATE_RET ICACHE_FLASH_ATTR httpc_gprs_update(char *http_data);
OPERATE_RET ICACHE_FLASH_ATTR httpc_gprs_upgrade_status(char *http_data, TUYA_UPGRADE_TYPE type, TUYA_UPGRADE_STATUS status);
void ICACHE_FLASH_ATTR tuya_httpclient_upgrade_update_status(TUYA_UPGRADE_TYPE type, TUYA_UPGRADE_STATUS status);
void tuya_emit_update_upgrade_status_instruction(TUYA_UPGRADE_TYPE type, TUYA_UPGRADE_STATUS status);
void tuya_emit_update_instruction();
void tuya_emit_upgrade_get_instruction();
Boolean tuya_check_ota_flag();

// tuya http schema interface
Boolean tuya_push_bool_obj(tuya_obj_array_mem_t *pmem, tuya_obj_bool_t *p_obj, char *obj_name);
Boolean tuya_push_integer_obj(tuya_obj_array_mem_t *pmem, tuya_obj_int_t *p_obj, char *obj_name);
Boolean tuya_get_obj(void *ptr, int len, tuya_obj_array_mem_t *pmem, char *obj_name);
void tuya_test_obj_persistent();
Boolean tuya_obj_bool_compare(tuya_obj_bool_t *b1, tuya_obj_bool_t* b2);
Boolean tuya_obj_int_compare(tuya_obj_int_t *t1, tuya_obj_int_t* t2);
Boolean tuya_parse_schema_obj(cJSON *pArray);
Boolean tuya_schema_save(tuya_obj_array_mem_t *pmem);
Boolean tuya_schema_load(tuya_obj_array_mem_t *pmem);
tuya_obj_base_t* tuya_read_obj(tuya_obj_array_mem_t *pmem, char *obj_name) ATTRIBUTE_SECTION_SRAM;
tuya_obj_base_t* tuya_update_obj_value(tuya_obj_array_mem_t *pmem, char *obj_name, int v) ATTRIBUTE_SECTION_SRAM;
Boolean tuya_update_obj_value_from_json(cJSON *pItem) ATTRIBUTE_SECTION_SRAM;
void tuya_read_obj_to_json(cJSON *pItem, char **node_array) ATTRIBUTE_SECTION_SRAM;

// tuya mqtt interface
OPERATE_RET ICACHE_FLASH_ATTR tuya_mqtt_make_reply_str(buf_t *b, cJSON *pRoot) ATTRIBUTE_SECTION_SRAM;
void tuya_mqtt_do_with_cmd(char *pstr) ATTRIBUTE_SECTION_SRAM;
Boolean tuya_mqtt_check_active();
void tuya_mqtt_gen_password(OUT char *pbuff, int len, IN char *key);
void tuya_mqtt_parse_cmd(char *topic, char *cmd_buf) ATTRIBUTE_SECTION_SRAM;
OPERATE_RET tuya_mqtt_check_sign(const char *pstr, buf_t *p_data) ATTRIBUTE_SECTION_SRAM;
OPERATE_RET tuya_mqtt_decode_data(char *p_data, int p_len, const char *key, Boolean base64) ATTRIBUTE_SECTION_SRAM;
ICACHE_FLASH_ATTR  int base64_decode( const char *base64, unsigned char *bindata ) ATTRIBUTE_SECTION_SRAM;
void tuya_mqtt_report_status_change(char **node_array) ATTRIBUTE_SECTION_SRAM;
void AT_MQTT_send_buff(buf_t *b) ATTRIBUTE_SECTION_SRAM;
Boolean tuya_split_rx_buff(net_line_t *pnet) ATTRIBUTE_SECTION_SRAM;

// lan operation interface
void init_net_buff();

#undef ATTRIBUTE_SECTION_SRAM
#define ATTRIBUTE_SECTION_SRAM

net_line_t* get_net_line(int sock) ATTRIBUTE_SECTION_SRAM;
void net_send_tx_queue(int sock, int resend, tcp_send_cb fn) ATTRIBUTE_SECTION_SRAM;
net_line_t* register_net_line(int sock) ATTRIBUTE_SECTION_SRAM;
Boolean unregister_net_line(int sock);
void tcp_send_ok_callback(int sock, tcp_send_cb fn) ATTRIBUTE_SECTION_SRAM;
void net_tcp_send_cb(int socket, char *buf, int len, Boolean resend) ATTRIBUTE_SECTION_SRAM;

Boolean tuya_lan_send_tcp_packet(int sock, buf_t *b) ATTRIBUTE_SECTION_SRAM;
Boolean tuya_lan_do_with_packet(buf_t *head, int sock, Boolean last_pack) ATTRIBUTE_SECTION_SRAM;
Boolean tuya_lan_cons_broadcast_packet(buf_t *p_buf);
Boolean tuya_lan_parse_packet(buf_t *b, int sock);
Boolean __tuya_lan_do_with_packet(lan_app_packet_t *head, int sock, Boolean last_pack) ATTRIBUTE_SECTION_SRAM;
void tuya_start_tcp_server();
void tuya_test_lan_agent();
OPERATE_RET tuya_lan_check_sign(const char *pstr, buf_t *p_data) ATTRIBUTE_SECTION_SRAM;
OPERATE_RET ICACHE_FLASH_ATTR tuya_lan_make_reply_str(buf_t *b, cJSON *pRoot) ATTRIBUTE_SECTION_SRAM;
int tuya_get_frame_type(buf_t *b) ATTRIBUTE_SECTION_SRAM;
int tuya_mqtt_get_cmd_type(char *pstr, buf_t *key,  buf_t *value) ATTRIBUTE_SECTION_SRAM;
Boolean tuya_mqtt_decode_cmd(buf_t *in, buf_t *b) ATTRIBUTE_SECTION_SRAM;

void tuya_lan_report_status_change(char **node);
Boolean tuya_lan_send_status_changed_packet(int sock, char **node_array) ATTRIBUTE_SECTION_SRAM;

void tuya_switch_action(char *name);
void tuya_stop_tcp_server();

void tuya_stop_udp_broadcast();
void tuya_send_udp_broadcast();

void tuya_parse_uart_rx_queue();
Boolean tuya_is_linked() ATTRIBUTE_SECTION_SRAM;
tuya_func_t tuya_get_func();
void tuya_test_heap(int ln);
int __tuya_read_obj_to_json(char *json_str, int size, char **node_array);
Boolean __tuya_update_obj_value_from_json(char *name, int type, int val) ATTRIBUTE_SECTION_SRAM;
Boolean __tuya_lan_do_with_cmd(char *pstr, Boolean last_pack) ATTRIBUTE_SECTION_SRAM;
OPERATE_RET ICACHE_FLASH_ATTR __tuya_mqtt_make_reply_str(buf_t *b, char *json_str) ATTRIBUTE_SECTION_SRAM;
void __tuya_mqtt_do_with_cmd(char *pstr, Boolean last_pack) ATTRIBUTE_SECTION_SRAM;
OPERATE_RET ICACHE_FLASH_ATTR __tuya_lan_make_reply_str(buf_t *b, char *json_str, int seq) ATTRIBUTE_SECTION_SRAM;

#if TUYA_DEBUG_ON
void * tuya_malloc(int size, char *file, int line) ATTRIBUTE_SECTION_SRAM;
void tuya_free(void *p, char *file, int line) ATTRIBUTE_SECTION_SRAM;
#else
void * tuya_malloc(int size) ATTRIBUTE_SECTION_SRAM;
void tuya_free(void *p) ATTRIBUTE_SECTION_SRAM;
#endif
char *TuyaGetHost(char *url);
Boolean tuya_get_sock_status();
int tuya_cons_timer_list(char *data, int size, Boolean quote);

Boolean __tuya_update_obj_value(char *name, int type, int val);

Boolean tuya_check_timeout();

OPERATE_RET tuya_mqtt_decode_data_2(DP_CMD_TYPE_E type, buf_t *in, buf_t *b);

/* 涂鸦定时器操作接口 */
Boolean tuya_do_with_timer_task(char *pdata, int size);

void tuya_update_sw_status();

OPERATE_RET parse_data_with_cmd(IN CONST DP_CMD_TYPE_E cmd, 
								IN CONST BYTE *data,
                                IN CONST INT len,
                                OUT buf_t *b);


OPERATE_RET pack_data_with_cmd(IN CONST DP_CMD_TYPE_E cmd, 
								IN CONST CHAR *src,
                               	IN CONST UINT pro,
                               	IN CONST UINT num,
                               	OUT buf_t * b);

#ifdef __cplusplus
}
#endif

#endif

