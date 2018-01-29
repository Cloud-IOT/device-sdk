
/*定义机智云对外提供的相关接口*/
#ifndef _GIZ_EXTERN_H_
#define _GIZ_EXTERN_H_


#ifdef __cplusplus
extern "C"
{
#endif

#define GIZ_MQTT_KEEPALIVE_TIME	   30	/*mqtt保活周期，单位为秒*/


/*获取m2m port*/
extern int giz_get_m2mport(void);
/*获取did,仅返回地址*/
extern char *giz_get_did(void);
/*获取ssid*/
extern char *giz_get_wifi_ssid(void);
/*获取wifi key*/
extern char *giz_get_wifi_key(void);
/*清除wifi配置，含ssid及key*/
extern void giz_clear_wifi_conf(void);
/*清除全局的信息*/
extern void giz_clear_global_conf(void);

/*
*长按键，进入配网模式，需进行这几步
*1. 清除全部配置
*2. 设置配网模式
*3. 再保存配置
*通过上述操作后，设备重启后，会进入指定的配网模式下
*/
extern void giz_only_set_confNet_conf(char confNetMode);







/*获取bActive的参数信息*/
extern BOOL giz_mqtt_get_var_active(void);
/*设置bActive的参数信息*/
extern void giz_mqtt_set_var_active(BOOL set);
/*设置订阅的topic-app2dev*/
extern void giz_mqtt_set_subTopic_app2dev(void);
/*处理mqtt服务器下发public消息时，进行处理，提取消息并置于接收队列中*/
extern void giz_mqtt_event_public(void *pMessage);
/*在处理服务器下发的public消息时，从消息接收队列中，取消息节点进行处理*/
extern void giz_mqtt_client_public(void);
/*订阅消息，与机智云对接时，有多个消息需进行订阅*/
extern void giz_mqtt_subscribe(void);


/*串口初始化*/
extern void giz_uart_init(void);
/*从uart串口队列中取buf,开始进行giz串口处理流程*/
extern void giz_parse_uart_rx_queue(void);


/*保存配置信息*/
extern BOOL giz_read_conf(void);
/*读取配置信息*/
extern BOOL giz_save_conf(void);

/*处理机智云的相关测试指令*/
int giz_at_cmd_hadle(void *param);


/*---需具体设备实现的接口，该设备仅支持开关这唯一功能---*/

/*
*设置设备开关的接口，TRUE - 开，FALSE - 关
*广域网或局域网下发的设置指令操作接口，具体设备需实现该接口
*/
extern void giz_key_switch_action(BOOL bon);

/*
*处理物理按键的操作
*物理的开关按键发生变化后，需调用此接口，以便保存信息，及将开关状态上报至服务器及app
*/
extern void giz_switch_action_api(void);


/*设置开关状态，status-0x01表示开，0x00表示关, 状态发生变化，会上报状态*/
extern void giz_timer_set_on_off_report(char status);


/*
*上报数据，专为外部模块提供的接口,在此接口中，读取设置的数据，如开关，定时器，并上报给app及云
*/
extern void giz_report_status_api(void);

/*
*变长字段上报数据，专为外部模块提供的接口,在此接口中，读取设置的数据，如开关，定时器，并上报给app及云
*type表示上报哪些字段,如方志插座中GIZ_TIMER_REPORT_SWITCH之类定义
*/
extern void giz_report_unfix_status_api(int type);


/*增加发送tcp失败的次数统计*/
extern void giz_add_send_tcp_fail_statistics(void);

/*为保证上下表项状态一致，当上报开关状态的tcp报文发送失败时，会调用此接口再次上报状态*/
extern void giz_timer_notify_report_status(void);


PROCESS_NAME(giz_cloud_mqtt_process);



#ifdef __cplusplus
}
#endif

#endif


