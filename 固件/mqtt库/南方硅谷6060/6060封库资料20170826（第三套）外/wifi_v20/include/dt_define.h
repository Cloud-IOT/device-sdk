#ifndef _DT_DEFINE_H
#define _DT_DEFINE_H

#include "stdio.h"
#include "string.h"
#include "stdarg.h"
#include "contiki.h"


#define D_MODE


#define APP_DEBUG_EN

//#define UDP_DEBUG_EN

#define TCP_SERVER_DEBUG_EN

#define DEBUG_EN

#define UART_MAX_SIZE 512



#define REMOTE_SERVER_IP  	"120.24.55.58"
#define REMOTE_SERVER_PORT  1883
#define UDP_BROADCAST_PORT  8091
#define UDP_SERVER_PORT     8090
#define TCP_SERVER_PORT     8080








#define USER_DEBUG_COLSE          0X00
#define UDP_SERVER_DEBUG_OPEN     0x01
#define UDP_BROADCAST_DEBUG_OPEN  0X02
#define TCP_DEBUG_OPEN            0X04
#define MQTT_DEBUG_OPEN           0X08
#define UART_DEBUG_OPEN           0X10
#define UPGRADE_DEBUG_OPEN        0X20
#define USER_SYS_DEBUG_OEPN       0X40

#define ALL_USER_DEBUG_OPEN       0xff

#define debug_level 0xff


#define __SHORT_FILE__  (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)



#define dt_debug(a,...) \
	do					\
		{				\
			if(a&debug_level) \
				{ \
					printf("%s,%d:",__SHORT_FILE__,__LINE__); \
					printf(__VA_ARGS__); \
					printf("%c",'\r\n');  \
				}  \
		}while(0)




typedef enum
{
	user_debug_close=USER_DEBUG_COLSE,
	udp_server_debug_open=UDP_SERVER_DEBUG_OPEN,
	udp_broadcast_debug_open=UDP_BROADCAST_DEBUG_OPEN,
	tcp_debug_open=TCP_DEBUG_OPEN,
	mqtt_debug_open=MQTT_DEBUG_OPEN,
	uart_debug_open=UART_DEBUG_OPEN,
	upgrade_debug_open=UPGRADE_DEBUG_OPEN,
	user_sys_debug_open=USER_SYS_DEBUG_OEPN,
}debug_type;


#define UDP_SERVER_DEBUG    UDP_SERVER_DEBUG_OPEN
#define UDP_BROADCAST_DEBUG UDP_BROADCAST_DEBUG
#define TCP_SERVER_DEBUG 			TCP_DEBUG_OPEN
#define MQTT_CLIENT_DEBUG 			MQTT_DEBUG_OPEN
#define UART_DEBUG			UART_DEBUG_OPEN
#define UPGRADE_DEBUG		UPGRADE_DEBUG_OPEN
#define USER_SYS_DEBUG		USER_SYS_DEBUG_OEPN

#define PAIR_DEVICE_PROTOCOL 0x0001
#define DISCOVER_DEVICE_PROTOCOL 0x0005
#define SEARCH_DEVICE_PROTOCOL 0x0008
#define BROADCAST_ONCE_PROTOCOL    0x000a
#define FIRMWARE_UPDATE_RESULT_PROTOCOL 0x000C

#define ON   1
#define OFF  0 



#define IP_STR_LEN 20 
#define ERROR_PARAM  -1;
#define WRONG_RETURN_VALUE -2;
#define TEST_ENV


typedef unsigned int  		dt_u32;
typedef unsigned short   	dt_u16;
typedef unsigned char       dt_u8;

//typedef unsigned int  		uint32;
//typedef unsigned short   	uint16;
//typedef unsigned char       uint8;


typedef signed int                 dt_s32;
typedef signed short               dt_s16;
typedef signed char         dt_s8;

typedef void                dt_v; 


typedef struct
{
	dt_u8 year;
	dt_u8 month;
	dt_u8 day;
	dt_u8 hour;
	dt_u8 min;
	dt_u8 sec;
	dt_u8 week;
}time_struct;



#define CHECK_PARAM(a)    \
		do							\
		{						\
			if(a)			\
			{					\
				printf("WARNING:::%s",#a);		\
				return -1;						\
			}					\
		}while(0)


typedef enum
{
	SEND_TO_MCU,
	TURN_SERVER_FLAG,
	GET_VERSION_INFO_FLAG,
	SEND_TIME_TO_MCU_FLAG,
	GET_DEVICE_INFO,
	FORCE_TO_UPGRADE_FLAG,
	UPGRADE_REQUIR_FLAG,
	UPGRADE_REQUEST_FLAG
}array_info_flag;


typedef struct return_array
{
	#ifdef LARGE_SIZE
	dt_u8 dt_array[512];
	#else
	dt_u8 dt_array[256];
	
	#endif	
	
	dt_u32 length;
}array_info;

typedef struct return_array_u
{
	#ifdef LARGE_SIZE
	dt_u8 dt_array[512];
	#else
	dt_u8 dt_array[256];
	#endif	
	array_info_flag  data_flag;
	dt_u32 length;
}array_info_u;


typedef struct net
{
	dt_u8   connect_flag;
	dt_s32  fd;
	
}net_struct;


typedef struct mqtt
{
	dt_u8 mqtt_client_connect_flag;
	dt_u8 mqtt_part1_connect_flag;
	dt_u8 mqtt_part2_connect_flag;
	dt_u8 mqtt_connect_flag;
	dt_s32 fd;
}mqtt_net_struct;


typedef struct
{
	dt_u8 get_ip_flag;
	dt_u8 get_mac_flag;
	dt_u8 get_type_value_flag;
}sys_para_struct;


typedef struct flag
{
	sys_para_struct sys_para_flag;
	net_struct udp_8090;
	net_struct udp_8091;
	net_struct tcp_8080;
	mqtt_net_struct mqtt_client;
	dt_u8 client;
	dt_u8 reset_system_flag;
	dt_u8 connect_route_flag;
}flag_struct;



typedef enum type
{
	WIFI_MODE=0,
	TWO_G_MODE,
	THREE_G_MODE,
	FORE_G_MODE,
	ZIGBEE_MODE,
}translat_type;


typedef struct device_information
{
	dt_u16 device_type;
	dt_u16 Manufacturer_ID;
	translat_type  trans_type;
	dt_s8  MCU_firewarm_version;
	dt_s8  Hardware_version;
	dt_s8  wifi_firewarm_vertion;
}device_info_struct;


typedef enum ip
{
	SERVER_IP_HAVE_NOT_CHANGED=0,
	SERVER_IP_HAVE_CHANGED,
}server_ip_enum;

typedef struct server
{
	dt_u8	server_ip[4];
	server_ip_enum server_ip_flag;
	dt_u8 server_ip_str[IP_STR_LEN];
}server_info_struct;


#define  UART_CACHE_SIZE  200

typedef struct 
{
	dt_u8 uart_cache_buf[UART_CACHE_SIZE];
	volatile dt_u8 uart_data_save_data_add;
	volatile dt_u8 uart_data_send_data_add;
	dt_u8 uart_buf_not_empty_flag;
	dt_u8 uart_buf_len;
	dt_u8 uart_recv_data_time_count;
}uart_cache_buf_t;


typedef struct
{
	dt_s32 mqtt_fd;
	dt_u8 connect_ACK_flag;
	dt_u8 mqtt_connect_socket_flag;
	dt_u8 mqtt_sub_success_flag;
	dt_u8 mqtt_lose_heart_beat_count;
	dt_u8 mqtt_need_not_send_mqtt_heartbeat;
	dt_u8 mqtt_reconnect_time_count;
}mqtt_connect_flag_t;


#define UDP_SERVER_BUF_MAX  128
#define TCP_SERVER_BUF_MAX  256
#define MQTT_CLIENT_BUF_MAX  256
#define TCP_CLIENT_MAX      5



typedef struct
{
	dt_u8 udp_server_recv_buf[UDP_SERVER_BUF_MAX]; 
	array_info_u udp_server_send_buf;
	dt_s32 udp_server_fd;
}udp_server_info_t;

typedef struct
{
	dt_u8 tcp_server_recv_buf[TCP_SERVER_BUF_MAX];

	dt_s32 tcp_server_fd;
	dt_s32 tcp_client_fds[TCP_CLIENT_MAX];
}tcp_server_info_t;

typedef struct
{
	dt_u8 mqtt_client_recv_buf[MQTT_CLIENT_BUF_MAX];

}mqtt_client_info_t;

typedef struct
{
	dt_u8 url_bin[150];
	dt_u8 host_name[100];
	dt_u8 get_wifi_version_str_from_server[5];
	dt_u8 force_to_upgrade_flag;
	dt_u8 have_new_version_flag;
}upgrade_info_t;


typedef struct save_para
{
	dt_u8 mac[6];
	dt_u8 ip[4];
	dt_u8 ip_str[16];
	dt_u8 mac_str[13];

	dt_u8 transmit_with_server_flag;


/*************************************/
	dt_s32 udp_broadcast_fd;
	dt_u8  udp_broadcast_open_flag;
/************************************/
	
	udp_server_info_t udp_server_info;

/************************************/
	tcp_server_info_t tcp_server_info;
/*************************************/
	mqtt_client_info_t mqtt_client_info;
	
	device_info_struct MCU_device_info;
	dt_u16 type_value;
	dt_u8 type_str[5];
	volatile dt_u8 broatcast_pair_once_resp_flag;
	volatile dt_u8 connect_route_flag;
	uart_cache_buf_t uart_cache_buf_struct;
	
	mqtt_connect_flag_t mqtt_connect_flag;
/*****************upgrade info************************/
	upgrade_info_t upgrade_info;
/********************************************/

	
}save_param_struct;

#define CLIENT_MAX        (5)
#define MAX_DATA_LEN     (200)


typedef struct
{
	dt_u8 socket_connect_flag;
	dt_u8 mqtt_base_connect_flag;
	dt_u8 mqtt_connect_cloud_server_flag;

	dt_u32 connect_count;
	dt_u32 mqtt_reconnect_delay_time_s;   //hf_thread_delay;
	
}mqtt_conn_flag_t;



typedef enum
{
	UPGRADE_FAILED,
	UPGRADE_ING,
	UPGRADE_SUCCESS
}UPGRADE_RESULT_ENUM;


typedef enum
{
	HAVE_NOT_NEW_VERSION,
	HAVE_NEW_VERSION
}UPGRADE_REQUIRE_ENUM;




typedef struct
{
	dt_s8 server_name;
	server_info_struct server_info;
	dt_u16 wifi_version_value;
	dt_s8  wifi_version_str[5];
}user_flash_data_t;






#endif

