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



typedef unsigned int  		dt_u32;
typedef unsigned short   	dt_u16;
typedef unsigned char       dt_u8;
typedef int                 dt_s32;
typedef short               dt_s16;
typedef char                dt_s8;
typedef void                dt_v; 

typedef void (*callback_func)(void *arg);


typedef struct 
{
	dt_u8  mac[6];
	dt_u8  ip[4];
	dt_u16 type_value;
}wifi_station_info_t;


void ICACHE_FLASH_ATTR dt_start_udp_server(wifi_station_info_t *wifi_staticon_infom);
dt_u32 ICACHE_FLASH_ATTR dt_start_tcp_server(wifi_station_info_t *wifi_station_infom);




#endif

