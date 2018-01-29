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


#define DT_LIB_MAX_BUF_SIZE  255  
/****************************************************************************************/


/*
**should be first ccalled after device connected to the router
*/
int dt_system_param_init(wifi_station_info_t *wifi_station_info);

/*func name:    dt_start_udp_server
**function:     start udp service
**param:        none
**return:       none
*/
int dt_start_udp_server(void);


/*func name:    dt_start_tcp_server
**function:     start tcp service
**param:        param1:user_callback_t type pointer,user can register recvcallback/disconnect callback/send callback,if you don't want to register,
**                     You can pass a NULL,However, if you do not register recv_callback, you will not receive data
**             
**return:       1-Success,else-fail 
*/

int  dt_start_tcp_server(user_callback_t *user_callback);

/*func name:    dt_start_mqtt_server
**function:     start mqtt remote service MQTT is a remote data sending and receiving service, which is forwarded by the server
**param:        param1:user_callback_t type pointer,user can register recvcallback/disconnect callback/send callback,if you don't want to register,
**                     You can pass a NULL,However, if you do not register recv_callback, you will not receive data
**             
**return:      1-Success,else-fail 

*/

int dt_start_mqtt_server(user_callback_t *user_callback);

/*func name:     check_wheather_connect_cloud
**function       Check whether the WiFi module is connected to the external network£¬If the connection is broken, there is a reconnection mechanism, 
**               do not need to repeat the call dt_start_mqtt_server
**param          none
**return         1-Success,else-fail   
*/

int dt_check_wheather_connect_cloud(void);


/*func name:     dt_send_data_to_app_clound
**function       Check whether the WiFi module is connected to the external network£¬If the connection is broken, there is a reconnection mechanism, 
**               do not need to repeat the call dt_start_mqtt_server
**param          1.Data array pointer
**               2.Length of data to send
**               3.Need to send the destination: the default is 0, and sent to the APP and the background, when set to 1:
				   only the near field transmission, APP in the same LAN network can receive data, remote data can not receive,When set to 2, only remote transmission.
**return         1-Success (here refers to the success of the function call, the data sent to the success of the callback function to determine),else-fail    
*/

int dt_send_data_to_app_clound(unsigned char  *data,unsigned short data_len,unsigned char flag);


#endif
