#ifndef _DT_MQTT_H
#define _DT_MQTT_H


#include "common_general_interface.h"
#include "dt_define.h"
#include "dt_uart.h"
#include "contiki.h"
#include "ota_api.h"


typedef struct {
	void* socket_info;
	int (*mqtt_send)(void* socket_info, const void* buf, unsigned int count);

	char clientid[50];

	char username[13];
	char password[13];

	uint8_t will_retain;
	uint8_t will_qos;
	uint8_t clean_session;

	uint16_t seq;
	uint16_t alive;
} mqtt_broker_handle_t;




#define MQTT_DUP_FLAG     1<<3
#define MQTT_QOS0_FLAG    0<<1
#define MQTT_QOS1_FLAG    1<<1
#define MQTT_QOS2_FLAG    2<<1

#define MQTT_RETAIN_FLAG  1

#define MQTT_CLEAN_SESSION  1<<1
#define MQTT_WILL_FLAG      1<<2
#define MQTT_WILL_RETAIN    1<<5
#define MQTT_USERNAME_FLAG  1<<7
#define MQTT_PASSWORD_FLAG  1<<6

#define MQTT_MSG_CONNECT       1<<4
#define MQTT_MSG_CONNACK       2<<4
#define MQTT_MSG_PUBLISH       3<<4
#define MQTT_MSG_PUBACK        4<<4
#define MQTT_MSG_PUBREC        5<<4
#define MQTT_MSG_PUBREL        6<<4
#define MQTT_MSG_PUBCOMP       7<<4
#define MQTT_MSG_SUBSCRIBE     8<<4
#define MQTT_MSG_SUBACK        9<<4
#define MQTT_MSG_UNSUBSCRIBE  10<<4
#define MQTT_MSG_UNSUBACK     11<<4
#define MQTT_MSG_PINGREQ      12<<4
#define MQTT_MSG_PINGRESP     13<<4
#define MQTT_MSG_DISCONNECT   14<<4

#define  PUBLISH_TOPIC_LENGTH  23  //user define


#define TURN_SERVER_FLAG 		1
#define GET_DEVICE_INFO_FLAG    2
#define GET_TIME_FLAG           3
#define GET_DOWNLOAD_INFO_FLAG  4

#define RCVBUFSIZE 512

#define MQTTParseMessageType(buffer) ( *buffer & 0xF0 )
#define MQTTParseMessageQos(buffer) ( (*buffer & 0x06) >> 1 )

void mqtt_sub_and_pub(unsigned int socket, unsigned char *data, unsigned short len);


void reset_mqtt_flag(void);

dt_s32 dt_mqtt_send(dt_u8 *data,dt_u32 data_len);
dt_s8  head_info(dt_u8* ret_data);

int dt_mqtt_subscribe(mqtt_broker_handle_t* broker, const char* topic, uint16_t* message_id);
 void mqtt_remote_func(void  *arg);
void  mqtt_set_alive(mqtt_broker_handle_t* broker, uint16_t alive);
 int  dt_mqtt_connect(mqtt_broker_handle_t* broker);

void   DT_mqtt_ping(void);
dt_s32 mqtt_1883_decode(dt_u8 *buf,dt_u32 buf_len,array_info_u* ret_data);
uint16_t  mqtt_parse_pub_topic(const uint8_t* buf, uint8_t* topic);
uint16_t  mqtt_parse_publish_msg(const uint8_t* buf, uint8_t* msg);

void  dt_mqtt_init(mqtt_broker_handle_t* broker, const char* clientid);

#endif