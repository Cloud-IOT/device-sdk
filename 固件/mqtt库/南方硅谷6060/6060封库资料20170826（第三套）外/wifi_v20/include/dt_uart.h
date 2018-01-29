#ifndef _DT_UART_H
#define _DT_UART_H


#include "dt_define.h"
#include "common_general_interface.h"
#include "dt_mqtt.h"
#include "dt_tcp_serve.h"
#include "udp_listen.h"
#include "contiki.h"
#include "serial_api.h"
#include "atcmd.h"


#define UART_DEBUG_EN



#define SMTLK_FLAG 1
dt_s32 uart_receive_decode(dt_u8 *data,dt_u32 buf_len);

void uart_recv_thread(void* arg);
dt_s32 dt_uart_send_data(dt_u8 *data,dt_u16 data_len);

void uart_send_data_func(void *arg);

dt_s32 dt_tcp_send(dt_u8 *data,dt_u32 data_len);
#endif