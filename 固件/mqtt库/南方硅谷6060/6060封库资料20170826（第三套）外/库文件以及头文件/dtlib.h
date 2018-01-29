#ifndef _DT_LIB_H_
#define _DT_LIB_H_




typedef void (*user_recv_callback_t)(unsigned char *data, unsigned short len);
typedef void (*user_send_callback_t)(void);
typedef void (*user_disconnect_callback_t)(void);
typedef void (*user_connect_callbacke_t)(void* socket_fd);


typedef struct 
{
	user_recv_callback_t recv_callback;
	user_send_callback_t user_send_callback;
	user_disconnect_callback_t user_disconnect_callback;
	user_connect_callbacke_t user_connect_callbacke;
}user_callback_t;



typedef enum
{
	JUST_WARNING_ERROR,
	UDP_DEBUG_ENABLE=0X01,
	TCP_DEBUG_ENABLE=0X02,
	MQTT_DEBUG_ENABLE=0X04,
	ENABLE_ALL=0X07
}lib_debug_level_t;




typedef struct 
{
	unsigned char  mac[6];
	unsigned char  ip[4];
	unsigned short type_value;
	unsigned char  server_ip[4];
	unsigned short max_buf_size;
	lib_debug_level_t  lib_debug_level;
}wifi_station_info_t;


#define ERROR_SOCKET_CREATE     (-2)
#define ERROR_PASS_PARAM        (-1)
#define SUCCESS                 (1)



/**********************************custom  param***************************************/


#define DT_LIB_MAX_BUF_SIZE  512  
/****************************************************************************************/



int dt_system_param_init(wifi_station_info_t *wifi_station_info);

int dt_start_udp_server(void);

int  dt_start_tcp_server(user_callback_t *user_callback);

int dt_start_mqtt_server(user_callback_t *user_callback);

int dt_check_wheather_connect_cloud(void);

int dt_send_data_to_app_clound(unsigned char  *data,unsigned short data_len,unsigned char flag);


#endif
