#ifndef __DTSTON_PACKET_H__
#define __DTSTON_PACKET_H__

#include<string.h>
#include<stdio.h>

typedef unsigned int  		dt_u32;
typedef unsigned short   	dt_u16;
typedef unsigned char       dt_u8;

typedef int                 dt_s32;
typedef short               dt_s16;
typedef unsigned char       dt_s8;

typedef void				VOID, *PVOID;
typedef unsigned char		UINT8,BYTE, UCHAR, *PUCHAR;
typedef char				INT8;
typedef short				INT16;
typedef unsigned short		UINT16;
typedef unsigned long		UINT32;
typedef unsigned char       uint8_t;
typedef short 				int16_t;
typedef unsigned short 		uint16_t;
typedef unsigned int 		uint32_t;

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

#define RCVBUFSIZE 1024
#define  PUBLISH_TOPIC_LENGTH  23
#define MQTTParseMessageType(buffer) ( *buffer & 0xF0 )
#define MQTTParseMessageQos(buffer) ( (*buffer & 0x06) >> 1 )
#define NULL ((void *)0)


typedef struct {
	void* socket_info;
	int (*dt_mqtt_send)(void* socket_info, const void* buf, unsigned int count);
	int (*dt_mqtt_recv)(void* socket_info,void* buf, unsigned int count);

	char clientid[50];

	char username[13];
	char password[13];

	char mac_addr_str[32];
	char product_type_str[4];

	uint8_t will_retain;
	uint8_t will_qos;
	uint8_t clean_session;

	uint16_t seq;
	uint16_t alive;
} mqtt_broker_handle_t;

typedef struct return_array_u
{
	unsigned char dt_array[512];//接收到的数据内容(字节)
	int length;//接收到的数据长度
	unsigned char data_flag;
}array_info_u;

typedef struct save_para
{
	dt_u8 mac[6];
	dt_s8 mac_str[13];
	dt_u16 type_value;
	dt_s8 type_str[5];
}save_param_struct;

extern array_info_u* dt_packet_body_data;
extern mqtt_broker_handle_t broker;
extern save_param_struct save_param;


extern int dt_mqtt_init(mqtt_broker_handle_t* broker,void *fd);//初始化mqtt参数
extern int  dt_mqtt_subscribe(mqtt_broker_handle_t* broker);//订阅深智云
extern char dt_mqtt_subscribe_recv_func(mqtt_broker_handle_t* broker);//WiFi收到深智云mqtt的数据
extern void  dt_mqtt_data_send(unsigned char *data,int len);//mqtt发送数据到深智云

extern int  dt_mqtt_ping(mqtt_broker_handle_t* broker);//mqtt心跳   30s发一次


#endif
