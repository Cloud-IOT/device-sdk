
/*atcmd_mqtt.c的头文件*/
#ifndef _ATCMD_MQTT_H_
#define _ATCMD_MQTT_H_


#ifdef __cplusplus
extern "C"
{
#endif

#include "data_buff.h"

extern buff_queue_t ty_mqtt_tx_queue;
extern buff_queue_t ty_mqtt_rx_queue;

extern int AT_MQTT_INIT(stParam *param);
extern int AT_MQTT_AUTH(stParam *param);
extern int AT_MQTT_CONNECT(stParam *param);

/*设置订阅主题，这样才能收到服务器后面的该主题的publish消息*/
extern int AT_MQTT_SUBSCRIBE(stParam *param);


/*
*设备发送publish报文
*stParam中，
*argv[0] - topic
*argv[1] - 发送的报文内容
*argv[2] - 发送的报文长度，注意由于是字符串，故在使用时，需转化为整型
*/
extern int AT_MQTT_PUBLISH(stParam *param);


#ifdef __cplusplus
}
#endif

#endif


