/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_main.c
 *
 * Description: entry file of user application
 *
 * Modification history:
 *     2014/1/1, v1.0 create this file.
*******************************************************************************/
#include "ets_sys.h"
#include "osapi.h"
#include "ip_addr.h"
#include "espconn.h"
#include "mem.h"
#include "user_interface.h"
#include "gpio.h"
#include "test.h"
#include "tcp_server.h"
#include "mqtt.h"
#include "smartconfig.h"
#include "esp_t_lib_v1.h"



 struct station_config s_staconf;


os_timer_t mqtt_test_timer;
os_timer_t pair_limit_timer;
os_timer_t test_timer;

char pair_limit_count = 0;


/*void ICACHE_FLASH_ATTR
pair_limit_func(void )
{
	pair_limit_count ++;

	os_printf("pair_limit_count=%d\n",pair_limit_count);
	forbid_search_flag =1 ;

	if(pair_limit_count == 10)
	{
	   forbid_search_flag =0 ;
	   os_timer_disarm(&pair_limit_timer);
	}
} 
*/

 dt_u8* ip32_to_ip_array(dt_u32 ip)
 {
	 static dt_u8 ip_table[5]={0};
	  ip_table[0]=ip>>0;
	  ip_table[1]=ip>>8;
	  ip_table[2]=ip>>16;
	  ip_table[3]=ip>>24;
	  ip_table[4]='\0';
	  return ip_table;
 }


char* char_string_new(dt_u8 *char_array  ,dt_u16 char_array_length)
{
    static char bady_date[255]={0};
    char data_high=0;
    char data_low=0;
	dt_u8 i;
    for(i=0;i<char_array_length;i++)
    {
        data_high=char_array[i]/16;
        if(data_high>9)
        {
            bady_date[2*i]=data_high-10+'A';
        }
        else
        {
            bady_date[2*i]=data_high+'0';
        }
        data_low=char_array[i]%16;

        if(data_low>9)
        {
            bady_date[2*i+1]=data_low-10+'A';
        }
        else
        {
            bady_date[2*i+1]=data_low+'0';
        }
    }
	bady_date[2*i]='\0';
return (char *)bady_date;
}




void ICACHE_FLASH_ATTR tcp_recv_callba( dt_u8 *pdata, unsigned short len)
{
	char *debug_str=NULL;
	debug_str=char_string_new(pdata,len);
	os_printf("tcp recv str=%s",debug_str);
	dt_send_data_to_app_clound(pdata,len,0);
	os_printf("len----------------------------------------------=%d",len);
}

void ICACHE_FLASH_ATTR tcp_send_callba(void)
{
	os_printf("tcp send callback!!!");
}

void ICACHE_FLASH_ATTR tcp_disconnect_callba(void)
{
	os_printf("tcp disconnect_callba!!");
}



void ICACHE_FLASH_ATTR mqtt_send_callba(void)
{
	os_printf("mqtt_send_callback!!!");
}



void ICACHE_FLASH_ATTR mqtt_disconnect_callba(void)
{
	os_printf("mqtt_disconnect_callback!!!");
}



void ICACHE_FLASH_ATTR mqtt_recv_callba(dt_u8 *pdata,dt_u16 len)
{
	char *debug_str=NULL;
	debug_str=char_string_new(pdata,len);
	os_printf("mqtt recv str=%s",debug_str);
	dt_send_data_to_app_clound(pdata,len,0);
	os_printf("len----------------------------------------------=%d",len);
}

 user_callback_t user_tcp_callback=
 	{
 		tcp_recv_callba,
		tcp_send_callba,
		tcp_disconnect_callba
 	};
 user_callback_t user_mqtt_callback=
 	{
 		mqtt_recv_callba,
		mqtt_send_callba,
		mqtt_disconnect_callba
 	};




 void ICACHE_FLASH_ATTR
 user_check_ip(void)
 {
	 struct ip_info ipconfig;
	 dt_u8 *ip_str=NULL;
	 wifi_station_info_t wifi_station_info;
	 uint8_t i;
	//disarm timer first
	 os_timer_disarm(&test_timer);

	
	 wifi_get_ip_info(STATION_IF, &ipconfig);
 	 wifi_get_macaddr(STATION_IF, wifi_station_info.mac);
	
	 if (wifi_station_get_connect_status() == STATION_GOT_IP && ipconfig.ip.addr != 0) 
	{
		 os_printf("got ip !!! \r\n");
		 ip_str=ip32_to_ip_array(ipconfig.ip.addr);
		 for(i=0;i<4;i++)
		 	{
		 		wifi_station_info.ip[i]=ip_str[i];
		 	}
		 wifi_station_info.type_value=3000;
		 dt_start_udp_server(&wifi_station_info);
		 
		 dt_start_tcp_server(&user_tcp_callback,&wifi_station_info);
		 dt_start_mqtt_server(&user_mqtt_callback,&wifi_station_info);
	 } 
	else 
	{
		 if ((wifi_station_get_connect_status() == STATION_WRONG_PASSWORD ||
				 wifi_station_get_connect_status() == STATION_NO_AP_FOUND ||
				 wifi_station_get_connect_status() == STATION_CONNECT_FAIL)) 
		 {
			  os_printf("connect fail !!! \r\n");
		 } 
		 
	   else 
	   {
           //re-arm timer to check ip
            os_timer_setfn(&test_timer, (os_timer_func_t *)user_check_ip, NULL);
            os_timer_arm(&test_timer, 100, 0);
		}
	 }
 }
 
 void ICACHE_FLASH_ATTR
 user_set_station_config(void)
 {
	//set a timer to check whether got ip from router succeed or not.
	 os_timer_disarm(&test_timer);
	 os_timer_setfn(&test_timer, (os_timer_func_t *)user_check_ip, NULL);
	 os_timer_arm(&test_timer, 100, 0);
 }
 
 
 void ICACHE_FLASH_ATTR
 smartconfig_done(sc_status status, void *pdata)
 {
 
	 switch(status) {
		 case SC_STATUS_WAIT:
			 os_printf("SC_STATUS_WAIT\n");
			 break;
		 case SC_STATUS_FIND_CHANNEL:
			 os_printf("SC_STATUS_FIND_CHANNEL\n");
			 break;
		 case SC_STATUS_GETTING_SSID_PSWD:
			 os_printf("SC_STATUS_GETTING_SSID_PSWD\n");
			 sc_type *type = pdata;
			 if (*type == SC_TYPE_ESPTOUCH) {
				 os_printf("SC_TYPE:SC_TYPE_ESPTOUCH\n");
			 } else {
				 os_printf("SC_TYPE:SC_TYPE_AIRKISS\n");
			 }
			 break;
		 case SC_STATUS_LINK:
			 os_printf("SC_STATUS_LINK\n");
			 struct station_config *sta_conf = pdata;
	 
			 wifi_station_set_config(sta_conf);
			 wifi_station_disconnect();
			 wifi_station_connect();
 
		/*	 os_timer_disarm(&pair_limit_timer);
			 os_timer_setfn(&pair_limit_timer, pair_limit_func, NULL);
			 os_timer_arm(&pair_limit_timer, 2000, 1);*/
			 
			 break;
		 case SC_STATUS_LINK_OVER:
			 os_printf("SC_STATUS_LINK_OVER\n");
			 if (pdata != NULL) {
				 uint8 phone_ip[4] = {0};
 
				 os_memcpy(phone_ip, (uint8*)pdata, 4);
				 os_printf("Phone ip: %d.%d.%d.%d\n",phone_ip[0],phone_ip[1],phone_ip[2],phone_ip[3]);
			 }
			 smartconfig_stop();
			 user_set_station_config();
			 break;
	 }
 }

uint32 ICACHE_FLASH_ATTR
user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

void ICACHE_FLASH_ATTR
user_rf_pre_init(void)
{
}	 


 char* char_string(uint8_t	char_array_length,char char_array[255])
 {
	 static char bady_date[255];
	 char data_high=0;
	 char data_low=0;
	 uint8_t i;
	 memset(bady_date, 0, sizeof(bady_date));
	 for(i=0;i<char_array_length;i++)
	 {
		 data_high=char_array[i]/16;
		 if(data_high>9)
		 {
			 bady_date[2*i]=data_high-10+'A';
		 }
		 else
		 {
			 bady_date[2*i]=data_high+'0';
		 }
		 data_low=char_array[i]%16;
 
		 if(data_low>9)
		 {
			 bady_date[2*i+1]=data_low-10+'A';
		 }
		 else
		 {
			 bady_date[2*i+1]=data_low+'0';
		 }
	 }
 return (char *)bady_date;
 }

 void ICACHE_FLASH_ATTR
 mqtt_test_timer_func(void )
 {
	 if(dt_mqtt_subscribe.get_subscribe_data_flag == 1){
	 	
	 	 dt_mqtt_subscribe.get_subscribe_data_flag = 0;
		 os_printf("dt_mqtt_status=%d\n",dt_mqtt_status);
		 os_printf("dt_ping_lose_count=%d\n",dt_ping_lose_count);
		 os_printf("uart_data_array=%s\n",char_string(dt_mqtt_subscribe.uart_len,dt_mqtt_subscribe.uart_data_array));

		 dt_remote_send_data(char_string(dt_mqtt_subscribe.uart_len,dt_mqtt_subscribe.uart_data_array));
		 dt_near_send_data(char_string(dt_mqtt_subscribe.uart_len,dt_mqtt_subscribe.uart_data_array));
	 }
 } 

 void ICACHE_FLASH_ATTR user_init(void)
 {	 
	 wifi_set_opmode(STATION_MODE);
	  
	 wifi_station_get_config_default(&s_staconf);

	
	 if (os_strlen(s_staconf.ssid) != 0) {
		 os_printf("user_scan\n");
		 user_set_station_config();
	 } else {
		 os_printf("smartcfg\n");
		
		 smartconfig_start(smartconfig_done);
	 }
 
 }

