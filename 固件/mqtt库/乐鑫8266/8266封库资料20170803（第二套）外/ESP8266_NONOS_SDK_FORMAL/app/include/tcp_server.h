
#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include "espconn.h"



/********************************************************/
/*功能：近程控制初始化*/
/*输入参数：product_type_value ---- 产品类型整数，该参数我司提供*/
/*返回：无*/
/*注:调用该函数之前，必须连上路由器*/
/********************************************************/
void ICACHE_FLASH_ATTR dt_near_control_init(uint16_t product_type_value);


/********************************************************/
/*功能：近程发送数据*/
/*输入参数：char *uart_data ------------- 串口数据（字符串格式）*/
/*返回：无*/
/*注:调用该函数之前，必须连上路由器，并且有互联网 （即dt_mqtt_status=1）*/
/********************************************************/
void ICACHE_FLASH_ATTR dt_near_send_data(char *uart_data);

#endif
