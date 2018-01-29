#ifndef __MQTT_H__
#define __MQTT_H__

#include "espconn.h"

#define DT_TEST_SERVER

extern uint8_t dt_ping_lose_count;    //mqtt心跳丢包个数
extern char forbid_search_flag;

enum mqtt_status
{
	MQTT_FAIL=0,	        //mqtt失败
	MQTT_SUCCESS,	        //mqtt主题订阅成功
	MQTT_CONNECT_FAIL,      //mqtt连接请求失败
	MQTT_SUBSCRIBE_FAIL,    //mqtt订阅请求失败
}dt_mqtt_status;



struct mqtt_subscribe{
	uint8_t get_subscribe_data_flag;	//订阅的主题收到数据标识符
	uint8_t uart_len;					//订阅的主题收到的数据解析后的串口数据长度
	uint8_t uart_data_array[255];		//订阅的主题收到的数据解析后的串口数据内容
};
struct mqtt_subscribe dt_mqtt_subscribe;



/********************************************************/
/*功能：模块自动连接深智云服务器*/
/*输入参数：product_type_value ---- 产品类型整数，该参数我司提供*/
/*返回：无*/
/*注:调用该函数之前，必须连上路由器，并且有互联网 （即dt_mqtt_status=1）*/
/********************************************************/
void ICACHE_FLASH_ATTR dt_remote_control_init(uint16_t product_type_value);


/********************************************************/
/*功能：远程发送数据*/
/*输入参数：char *uart_data ------------- 串口数据（字符串格式）*/
/*返回：无*/
/*注:调用该函数之前，必须连上路由器，并且有互联网 （即dt_mqtt_status=1）*/
/********************************************************/
void ICACHE_FLASH_ATTR dt_remote_send_data(char *uart_data);


#endif
