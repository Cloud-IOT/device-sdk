#include <string.h>
#include <stdio.h>
#include <time.h>

#ifndef WIN32
#include "systemconf.h"
#include "serial_api.h"
#include "gpio_api.h"
#include "drv_uart.h"
#include "socket_driver.h"
#include "comm_apps_res.h"
#include "ssv_lib.h"
#include "sys_status_api.h"
#include "atcmd.h"
#include "Cabrio-conf.h"
#include "uip.h"
#include "atcmd_icomm.h"
#include "soft_ap.h"
#include "drv_flash.h"
#include "flash_api.h"
#include "soft_ap.h"
#include "soft_ap_init.h"
#include "ieee80211_mgmt.h"

#endif
#include "atcmd.h"

#include "data_buff.h"
#include "tuya_http.h"
#include "c_types.h"
#include "dt_mqtt.h"
#include "szcloud_uart.h"
#include "uart_cmd.h"
#include "wdog_api.h"
#include "dt_mqtt.h"
#include "szcloud_uart.h"

#include "dt_define.h"
#include "common_general_interface.h"
#include "dt_tcp_serve.h"

#include "udp_listen.h"
#include "dtlib.h"


#ifdef __SZCLOUD_SUPPORT__


PROCESS(szcloud_process, "szcloud_process");
//PROCESS(tcp_connect_process, "Tcp Connect Process");

#if 0

extern char link_router_flag;
U8 mqtt_success_flag = 0;

extern user_flash_data_t user_flash_data;
extern save_param_struct save_param;
extern mqtt_broker_handle_t broker;
TAG_NET_STATUS gNetStatus;
TAG_NET_STATUS gNetStatus1;




PROCESS(dt_uart_process, "Dt_uart_process");


PROCESS(dt_mqtt_client_process,"MQTT Client Process");


#endif
save_param_struct save_param;
user_flash_data_t user_flash_data;


int Dt_EnableSmartReboot(dt_u8 data)
{
    S32 rlt = 0;
    
    //printf ("[%s] : +++\n",__func__);
    if(data == 0) {
        gconfig_set_enable_smart(NOSMART, 0);
    } else if(data == 1){
        gconfig_set_enable_smart(ICOMM, 0);
    } else if(data == 2){
        gconfig_set_enable_smart(WECHAT, 0);
    } else if(data == 3){
        gconfig_set_enable_smart(USER, 0);
    } else if(data == 4){
        gconfig_set_enable_smart(JDSMART, 0);
    } else {
        printf("please enter range 0 to 4\n");
    }
    
    rlt = gconfig_save_config();
    if(rlt!=0){
        printf("<Error>gconfig_save_config failed!!\n");
    }
    rlt = remove_sysconfig();
	reset_global_conf();
    if(rlt!=0){
        printf("<Error>systemconf remove failed!!\n");
    }
    api_wdog_reboot(); 
    
    return ERROR_SUCCESS;
}
/**********************************************************************************************************/

#if 0
PROCESS_THREAD(dt_uart_process, ev, data)  
{  
  static struct etimer et;  
  PROCESS_BEGIN();  
  
  while(1) {  
    etimer_set(&et, 200*CLOCK_MINI_SECOND);                    
    PROCESS_WAIT_EVENT_UNTIL( ev == PROCESS_EVENT_TIMER);  

	uart_send_data_func(NULL);
  }  
  PROCESS_END();  
}  




void dt_DataUartRead(void *dara)
{
	dt_u32 i=0;
    S32 input;
    
    while(1) {
        input = drv_uart_rx(SSV6XXX_UART1);//SerialRead();
        if(input == -1) {
            break;
        } else {
			
			save_param.uart_cache_buf_struct.uart_recv_data_time_count=0;

			save_param.uart_cache_buf_struct.uart_cache_buf[save_param.uart_cache_buf_struct.uart_data_save_data_add++]=(U8)input;
			if(UART_CACHE_SIZE==save_param.uart_cache_buf_struct.uart_data_save_data_add)
				{
					save_param.uart_cache_buf_struct.uart_data_save_data_add=0;
				}
			save_param.uart_cache_buf_struct.uart_buf_len+=1;
			save_param.uart_cache_buf_struct.uart_buf_not_empty_flag=1;
			
        }
    }
}




#endif

/*********************************************user param init***************************************************/
void type_init(void)
{
	dt_u8 i=0;
	dt_s8 *type="2299";
	for(i=0;i<4;i++)
		{
			save_param.type_str[i]=type[i];
		}
	save_param.type_str[i]=0;
	save_param.type_value=2299;
}


void wifi_version_init(void)
{
	if((user_flash_data.wifi_version_str[0]==0x00)||(user_flash_data.wifi_version_str[0]==0xff))
		{
			dt_s8 *version_str="130";
			dt_u8 i;
			for(i=0;i<4;i++)
				{
					user_flash_data.wifi_version_str[i]=version_str[i];
				}
		}
}

void server_ip_init(void)
{
	dt_u32 i=0;
	dt_u8 test_ip[4]={120,24,55,58};
	dt_u8 formal_ip[4]={121,40,201,245};
	#ifdef 			TEST_ENV
	if((user_flash_data.server_info.server_ip_flag!=SERVER_IP_HAVE_CHANGED))
		{
			
			for(i=0;i<4;i++)
				{
					user_flash_data.server_info.server_ip[i]=test_ip[i];
				}
			sprintf(user_flash_data.server_info.server_ip_str,"%d%s%d%s%d%s%d",user_flash_data.server_info.server_ip[0],".",
					user_flash_data.server_info.server_ip[1],".",user_flash_data.server_info.server_ip[2],".",user_flash_data.server_info.server_ip[3]);
		}
	#else
	if((user_flash_data.server_info.server_ip_flag!=SERVER_IP_HAVE_CHANGED))
		{
		
			for(i=0;i<4;i++)
				{
					user_flash_data.server_info.server_ip[i]=formal_ip[i];
				}
			sprintf(user_flash_data.server_info.server_ip_str,"%d%s%d%s%d%s%d",user_flash_data.server_info.server_ip[0],".",
					user_flash_data.server_info.server_ip[1],".",user_flash_data.server_info.server_ip[2],".",user_flash_data.server_info.server_ip[3]);
		}
	#endif
}



void get_mac_func(void)
{
	get_local_mac(save_param.mac,6);
	char_string_new(save_param.mac,6,save_param.mac_str);
}
#if 0
void print_all_param_func(void)
{
	dt_u8 i=0;
	dt_debug(USER_SYS_DEBUG,"user_flash_data.wifi_version_str=%s",user_flash_data.wifi_version_str);
	dt_debug(USER_SYS_DEBUG,"user_flash_data.server_info.server_ip_flag=%d",user_flash_data.server_info.server_ip_flag);
	dt_debug(USER_SYS_DEBUG,"user_flash_data.server_info.server_ip_str=%s",user_flash_data.server_info.server_ip_str);
	dt_debug(USER_SYS_DEBUG,"save_param.type_value=%d",save_param.type_value);
	dt_debug(USER_SYS_DEBUG,"save_param.type_str=%s",save_param.type_str);
	dt_debug(USER_SYS_DEBUG,"save_param.mac_str=%s",save_param.mac_str);
	printf("user_flash_data.server_info.server_ip[i]=");
	for(i=0;i<4;i++)
		{
			printf("%d",user_flash_data.server_info.server_ip[i]);
			printf("%c",'.');
		}
}

void param_init(void)
{
	wifi_version_init();
	type_init();
	server_ip_init();
	get_mac_func();
	dt_write_to_flash();
	print_all_param_func();
}

#endif
/*****************************************************net work updata*********************************************/

void network_stat_update(void)
{
	char net_work_data[10]={0xee,0xee,0x00,0x0a,0x00,0x50,0x08,0x00,0x00,0x00};
	printf("send network to mcu");
	if(save_param.connect_route_flag)
		{
			net_work_data[7]=0;
		}
	else
		{
			net_work_data[7]=1;
		}
	if(save_param.mqtt_connect_flag.mqtt_sub_success_flag)
		{
			net_work_data[8]=0;
		}
	else
		{
			net_work_data[8]=1;
		}
	net_work_data[9]=get_CRC(net_work_data,10);
//	dt_uart_send_data(net_work_data,10);
}
/***********************************************************************************************************/


#if 0

void szcloud_parse_uart_rx_queue(void)
{
	buf_t *b;
	dt_u32 i=0;
	while ((b = remove_buf(&uart_rx_queue)))
	{
		printf("uart recv data,recv_len=%d\n",b->len);
		save_param.uart_cache_buf_struct.uart_recv_data_time_count=0;
		save_param.uart_cache_buf_struct.uart_buf_not_empty_flag=1;
		for(i=0;i<b->len;i++)
			{
				save_param.uart_cache_buf_struct.uart_cache_buf[save_param.uart_cache_buf_struct.uart_data_save_data_add++]=b->pbuff[i];
				if(UART_CACHE_SIZE==save_param.uart_cache_buf_struct.uart_data_save_data_add)
				{
					save_param.uart_cache_buf_struct.uart_data_save_data_add=0;
				}
				save_param.uart_cache_buf_struct.uart_buf_len+=1;
			}
		free_buffer(b);
	}
}

/********************************************************mqtt config********************************************************/
#endif



/*******************************************************************************************************************/

#if 0
/*******************************************************udp server ********************************************************/

PROCESS_THREAD(udp_server_process, ev, data)
{
	PROCESS_BEGIN();
	SOCKETMSG msg;
	int recvlen;
	uip_ipaddr_t peeraddr;
	U16 peerport;
	u8_t k;
    uint8_t i=0;
	
	
	while(1) 
	{
		PROCESS_WAIT_EVENT();

		if( ev == PROCESS_EVENT_EXIT) 
		{
			break;
		} 
		else if(ev == PROCESS_EVENT_MSG) 
		{
			msg = *(SOCKETMSG *)data;
			
			if(msg.status == SOCKET_NEWDATA)
				{
					if(UIP_CONNS <= msg.socket && msg.socket < UIP_CONNS + UIP_UDP_CONNS)
						{
							recvlen = udprecvfrom(msg.socket, save_param.udp_server_info.udp_server_recv_buf, UDP_SERVER_BUF_MAX, &peeraddr, &peerport);
							save_param.udp_server_info.udp_server_recv_buf[recvlen]=0;
							//printf("udp recv data=%s",&save_param.udp_server_info.udp_server_recv_buf[13]);

							udp_server_decode(save_param.udp_server_info.udp_server_recv_buf,recvlen,&save_param.udp_server_info.udp_server_send_buf);
			
							//printf("save_param.udp_server_info.udp_server_send_buf.dt_array=%s",&save_param.udp_server_info.udp_server_send_buf.dt_array[13]);
	
							udpsendto(msg.socket,save_param.udp_server_info.udp_server_send_buf.dt_array, save_param.udp_server_info.udp_server_send_buf.length,&peeraddr,peerport);
						}
					else
						{
							printf("why get hear!!");
						}
				}
			else
				{
					dt_debug(USER_SYS_DEBUG,"why_get hear!!");
				}
		}	
		else
			{
				dt_debug(USER_SYS_DEBUG,"why_get_hear");
			}
	}
	PROCESS_END();
}







/*******************************************************************************************************************/








/**************************************************udp broadcast!!******************************************************/
#endif



/*****************************************************************************************************************/

/*

PROCESS_THREAD(tcp_connect_process, ev, data)
{
	PROCESS_BEGIN();
	SOCKETMSG msg;
	int recvlen;
	uip_ipaddr_t peeraddr;
	U16 peerport;
	u8_t k;
	static dt_u8 i=0;
	static array_info_u send_pack;

    char version_number_str[32];
	while(1) 
	{
		PROCESS_WAIT_EVENT();

		if( ev == PROCESS_EVENT_EXIT) 
		{
			break;
		} 
		else if(ev == PROCESS_EVENT_MSG) 
		{
			msg = *(SOCKETMSG *)data;

			if(msg.status == SOCKET_CONNECTED)
			{
				printf("socked:%d connected\n", msg.socket);
			}

			else if(msg.status == SOCKET_CLOSED)
			{
				printf("socket:%d,closed ",msg.socket);
			}

			else if(msg.status == SOCKET_SENDACK)
			{
				printf("socked:%d send data ack\n", msg.socket);

			}

			else if(msg.status == SOCKET_NEWDATA)
			{
				if(0 <= msg.socket && msg.socket < UIP_CONNS)
				{
					recvlen = tcprecv(msg.socket, save_param.tcp_server_info.tcp_server_recv_buf, TCP_SERVER_BUF_MAX);
					save_param.tcp_server_info.tcp_server_recv_buf[recvlen] = 0;
					for(i=0;i<TCP_CLIENT_MAX;i++)
						{
							if(save_param.tcp_server_info.tcp_client_fds[i]<0)
								{
									save_param.tcp_server_info.tcp_client_fds[i]=msg.socket;
									break;
								}
						}
					printf("tcp  recv data len =%d",recvlen);
			//		tcp_server_protocol(save_param.tcp_server_info.tcp_server_recv_buf,recvlen,&send_pack);
					if(SEND_TO_MCU==send_pack.data_flag)
						{
				//			dt_uart_send_data(send_pack.dt_array,send_pack.length);
						}
				}
				
				else
		        {
					printf("Illegal socket:%d\n", msg.socket);
				}
			}

			else if(msg.status == SOCKET_NEWCONNECTION)
			{
				printf("SOCKET_NEWCONNECTION!!");
			}
			else
            {
				printf("unknow message type\n");
            }
        }	
    }	

	PROCESS_END();
}
*/




/*int Dt_TCPListen (void)
{
	char  *pport = 0;
	static dt_u8 i=0;


	for(i=0;i<TCP_CLIENT_MAX;i++)
		{
			save_param.tcp_server_info.tcp_client_fds[i]=-1;
		}

	if(tcplisten(TCP_SERVER_PORT, &tcp_connect_process) == -1)
	{
		printf("listen tcp fail\n");
		return ERROR_TCP_CONNECTION;
	}
	else
	{
		printf("Dt_TCPListen\n");
		return ERROR_SUCCESS;
	}

}

*/



/*******************************************************************************************************************/

#define TIME_PERIOD (5000)
#define TEN_SECOND  (2*TIME_PERIOD)
#define FIVE_SECOND  TIME_PERIOD


wifi_station_info_t wifi_station_info;

user_callback_t tcp_user_callb;
user_callback_t mqtt_user_callb;


void tcp_recv_callback(dt_u8* data,dt_u16 len)
{
	dt_u32 i;
	printf("tcp recv data len =%d\r\n",len);
	printf("tcp recv data data =");
	for(i=0;i<len;i++)
		{
			printf("%2x",data[i]);
		}
	dt_send_data_to_app_clound(data,len,0);
}


void  tcp_disconnect_callback(void)
{
	printf("tcp user  user  disconnect callback!!!!\r\n");
}

void tcp_send_ack_callback(void)
{
	printf("tcp user  user  send callback!!!!\r\n");
}


void mqtt_recv_callback(dt_u8* data,dt_u16 len)
{
	dt_u32 i;
	printf("mqtt recv data len =%d\r\n",len);
	printf("mqtt recv data data =");
	for(i=0;i<len;i++)
	{
		printf("%2x",data[i]);
	}
	dt_send_data_to_app_clound(data,len,0);
}

void mqtt_disconnect_callback(void)
{
	printf("mqtt user  user  disconnect callback!!!!\r\n");
}

void mqtt_send_ack_callback(void)
{
	printf("mqtt user  user  send ack callback!!!!\r\n");
}









PROCESS_THREAD(szcloud_process, ev, data)
 {

 	static struct etimer timer;
	static dt_u8 start_dt_service_flag=0;
	static dt_u8 once_first_broadcast_count=0;
	static dt_u8 send_network_stat_to__MCU_count=0;
	static dt_u8 mqtt_heartbeat_count=0;
	static int ret=0;
	
	dt_u32 i=0;
	 PROCESS_BEGIN();

	static unsigned char server_ip[]={120,24,55,58};

	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nn!!!\n");
 	printf("ssv6060_v1.0_d_third_protocol!!\n");
	printf("ssv6060_v1.0_d_third_protocol!!\n");
	printf("ssv6060_v1.0_d_third_protocol!!\n");
	printf("ssv6060_v1.0_d_third_protocol!!\n");

	tcp_user_callb.recv_callback=tcp_recv_callback;
	tcp_user_callb.user_disconnect_callback=tcp_disconnect_callback;
	tcp_user_callb.user_send_callback=tcp_send_ack_callback;

	mqtt_user_callb.recv_callback=mqtt_recv_callback;
	mqtt_user_callb.user_disconnect_callback=mqtt_disconnect_callback;
	mqtt_user_callb.user_send_callback=mqtt_send_ack_callback;

	
//	dt_read_from_flash();

//	printf("user_flash_data.server_info.server_ip[i]=");
//	for(i=0;i<4;i++)
//		{
//			printf("%d.",user_flash_data.server_info.server_ip[i]);
//		}
	
	get_mac_func();
	type_init();
	

//	func_do_with_uart_queue = szcloud_parse_uart_rx_queue;

//	process_start(&dt_uart_process, NULL);
	
	 while (1)
	 {
		if( (1==save_param.connect_route_flag) &&(0==start_dt_service_flag))
		{
			for(i=0;i<4;i++)
				{
					wifi_station_info.ip[i]=save_param.ip[i];
				}
			 for(i=0;i<6;i++)
			 	{
			 		wifi_station_info.mac[i]=save_param.mac[i];
			 	}
			 for(i=0;i<4;i++)
			 	{
			 		wifi_station_info.server_ip[i]=server_ip[i];
			 	}
			wifi_station_info.type_value=save_param.type_value;
			wifi_station_info.lib_debug_level=JUST_WARNING_ERROR;
			wifi_station_info.max_buf_size=DT_LIB_MAX_BUF_SIZE;
			
			start_dt_service_flag=1;

			ret=dt_check_wheather_connect_cloud();
			printf("connect cloud flag =%d\r\n",ret);
			
			dt_system_param_init(&wifi_station_info);
			dt_start_udp_server();
			dt_start_tcp_server(&tcp_user_callb);
			dt_start_mqtt_server(&mqtt_user_callb);
			
			
		}

		ret=dt_check_wheather_connect_cloud();
		printf("connect cloud flag =%d\r\n",ret);
		
		 etimer_set(&timer, TIME_PERIOD * CLOCK_MINI_SECOND);
		 PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
		 
	 }


 
	 PROCESS_END();
 }
 #endif
