#ifndef __UDP_LISTEN_H__
#define __UDP_LISTEN_H__

#include "contiki.h"
#include "uip.h"
#include "dt_define.h"
#include "common_general_interface.h"
#include "socket_driver.h"
#include "dt_mqtt.h"

#define PAIR_DEVICE_PROTOCOL 0x0001
#define DISCOVER_DEVICE_PROTOCOL 0x0005
#define SEARCH_DEVICE_PROTOCOL 0x0008
#define STATUE_UPDATE_DEVICE_PROTOCOL 0x0009
#define FIRMWARE_UPDATE_RESULT_PROTOCOL 0x000C

#define UDP_MAX_LEN 128




array_info_u*  udp_8090_decode(dt_u8 *buf,dt_u32 length);  

dt_s32 udp_server_decode(dt_u8 *pass_buf,dt_u32 pass_buf_len,array_info_u *ret_packet);

dt_s32 udp_broadcast_send_packet(dt_s32 fd_udp,dt_u16 protocol,dt_u8* send_data,dt_u16 data_len, uip_ipaddr_t *peeraddr,unsigned short peerport);

dt_s32 dt_get_broadcast_packet(dt_u16 protocol,dt_u8 *src_buf,dt_u32 src_buf_len,array_info_u *dest_buf);

dt_s32 dt_udp_broadcast_send(dt_u8 *data,dt_u32 data_len);







#endif
