#ifndef __DT_TCP_SERVE_H_
#define __DT_TCP_SERVE_H_


#include "dt_define.h"
#include "common_general_interface.h"
#include "dt_mqtt.h"
#include "contiki.h"

void dt_upgrade_require_result(UPGRADE_REQUIRE_ENUM upgrade_flag);
void  socket_tcp_server( void * arg);

dt_s32 tcp_server_protocol(dt_u8 *buf,dt_u32 length,array_info_u *ret_pack);
void dt_upgrade_result(UPGRADE_RESULT_ENUM upgrade_flag);
#endif