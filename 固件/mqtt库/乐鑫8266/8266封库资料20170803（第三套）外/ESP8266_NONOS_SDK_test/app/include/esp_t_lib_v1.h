#ifndef _ESP_T_LIB_V1_H
#define _ESP_T_LIB_V1_H

#include "c_types.h"

#include "mem.h"
#include "ets_sys.h"
#include "osapi.h"

#include "user_interface.h"
#include "smartconfig.h"
#include <stdio.h>
#include "eagle_soc.h"

#include "espconn.h"



#define TEST_ENV
//#define FORMAL_ENV


typedef unsigned int  		dt_u32;
typedef unsigned short   	dt_u16;
typedef unsigned char       dt_u8;
typedef int                 dt_s32;
typedef short               dt_s16;
typedef char                dt_s8;
typedef void                dt_v;


typedef void (*user_recv_callback_t)(dt_u8 *data, dt_u16 len);
typedef void (*user_send_callback_t)(void);
typedef void (*user_disconnect_callback_t)(void);


typedef struct 
{
	user_recv_callback_t recv_callback;
	user_send_callback_t user_send_callback;
	user_disconnect_callback_t user_disconnect_callback;
}user_callback_t;

typedef struct 
{
	dt_u8  mac[6];
	dt_u8  ip[4];
	dt_u16 type_value;
}wifi_station_info_t;


/*func name:    dt_start_udp_server
**function:     start udp service
**param:        param1:wifi_station_info_t type pointer,The MAC address is an array of 6 bytes, 
**                     the IP address is an array of 4 bytes, the typevalue is assigned by the SHENZHIYUN, and each product corresponds to a product type
**return:       none
*/
/*函数名        :dt_start_udp_server
**功能          :打开UDP服务
**参数          :wifi_station_info_t 类型的指针，mac地址为6字节数组，IP地址为4字节数组，typevalue是由深智云分配的，每一个产品对应一个产品类型
**返回值        :无
*/
void ICACHE_FLASH_ATTR dt_start_udp_server(wifi_station_info_t *wifi_staticon_infom);







/*func name:    dt_start_tcp_server
**function:     start tcp service
**param:        param1:user_callback_t type pointer,user can register recvcallback/disconnect callback/send callback,if you don't want to register,
**                     You can pass a NULL,However, if you do not register recv_callback, you will not receive data
**              param2:wifi_station_info_t type pointer,The MAC address is an array of 6 bytes, 
**                     the IP address is an array of 4 bytes, the typevalue is assigned by the deep wisdom cloud, and each product corresponds to a product type
**return:       1-Success,else-fail 
*/
/*函数名        :dt_start_tcp_server
**功能          :打开TCP服务
**参数          :1.user_callback_t 类型的指针，用户可以注册接收回调函数、断开连接回调函数、发送回调函数，如果你不想注册，你可以传递一个NULL，但是，如果不注册
**                  recv_callback,将会收不到tcp发过来的数据
**               2.wifi_station_info_t 类型的指针，mac地址为6字节数组，IP地址为4字节数组，typevalue是由深智云分配的，每一个产品对应一个产品类型
**return         1-成功  非1-失败
*/
dt_s32 ICACHE_FLASH_ATTR dt_start_tcp_server(user_callback_t *user_callback,wifi_station_info_t *wifi_station_infom);




/*func name:    dt_start_mqtt_server
**function:     start mqtt remote service MQTT is a remote data sending and receiving service, which is forwarded by the server
**param:        param1:user_callback_t type pointer,user can register recvcallback/disconnect callback/send callback,if you don't want to register,
**                     You can pass a NULL,However, if you do not register recv_callback, you will not receive data
**              param2:wifi_station_info_t type pointer,The MAC address is an array of 6 bytes, 
**                     the IP address is an array of 4 bytes, the typevalue is assigned by the deep wisdom cloud, and each product corresponds to a product type
**return:      1-Success,else-fail 

*/
/*函数名        :dt_start_mqtt_server
**功能          :打开MQTT服务，MQTT是远程数据收发服务，通过服务器进行转发
**参数          :1.user_callback_t 类型的指针，用户可以注册接收回调函数、断开连接回调函数、发送回调函数，如果你不想注册，你可以传递一个NULL，但是，如果不注册
**                  recv_callback,将会收不到MQTT发过来的数据
**               2.wifi_station_info_t 类型的指针，mac地址为6字节数组，IP地址为4字节数组，typevalue是由深智云分配的，每一个产品对应一个产品类型
**返回          -1-失败，1-成功
*/
dt_s32 ICACHE_FLASH_ATTR dt_start_mqtt_server(user_callback_t *user_callback,wifi_station_info_t *wifi_station_infom);





/*func name:     check_wheather_connect_cloud
**function       Check whether the WiFi module is connected to the external network，If the connection is broken, there is a reconnection mechanism, 
**               do not need to repeat the call dt_start_mqtt_server
**param          none
**return         1-Success,else-fail    
*/

/*函数名:        check_wheather_connect_cloud
**功能           检查WiFi模块有没有连上外网(即mqtt连接)，如果中途断掉连接，有重连机制，不需要重复调用dt_start_mqtt_server
**
**参数           无
**返回值         1-成功  非1-失败
*/
dt_s32 ICACHE_FLASH_ATTR check_wheather_connect_cloud(void);





/*func name:     dt_send_data_to_app_clound
**function       Check whether the WiFi module is connected to the external network，If the connection is broken, there is a reconnection mechanism, 
**               do not need to repeat the call dt_start_mqtt_server
**param          1.Data array pointer
**               2.Length of data to send
**               3.Need to send the destination: the default is 0, and sent to the APP and the background, when set to 1:
				   only the near field transmission, APP in the same LAN network can receive data, remote data can not receive,When set to 2, only remote transmission.
**return         1-Success (here refers to the success of the function call, the data sent to the success of the callback function to determine),else-fail    
*/

/*函数名:        dt_send_data_to_app_clound
**功能           发送数据给APP和后台，远程控制时，只有后台服务器能进行数据交互
**
**参数           1.需要发送的数据数组指针
**               2.需要发送的数据长度
**               3.需要发送的目的地:默认为0，同时发送给APP和后台，当设置为1时:只进行近场发送，APP在同一局域网网内能收到数据，远程收不到数据，
				   当设置为2时，只进行远程发送。
**返回值         1-成功(这里指函数调用成功，数据发送成功得去发送回调函数里面判断)  非1-失败
*/
dt_s32 ICACHE_FLASH_ATTR dt_send_data_to_app_clound(dt_u8 *data,dt_u16 data_len,dt_u8 flag);



#endif

