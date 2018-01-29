#ifndef _COMMON_GENERAL_INTERFACE_H
#define _COMMON_GENERAL_INTERFACE_H
#include "dt_define.h"


#define USER_SAVE_PARFAM 10

dt_u8 dt_check_data(dt_u8 *buf,dt_u8 length);
array_info_u* Mosaic_array(dt_u8 *before,dt_u8 before_len,dt_u8 *after,dt_u8 after_len);

dt_u8 *ip_array_to_str(dt_u8 *ip);
dt_u16 judge_array_count_new(char *array);
dt_u8 * type10_to_str(dt_u16 type);
dt_u8* ip32_to_ip_array(dt_u32 ip);
dt_s8 char_string_new(const dt_u8 *src_data,dt_u16 src_len,dt_s8* dest_data);


dt_u8 get_CRC(const dt_u8 *buf,unsigned int buf_len);

dt_u8 check_crc_data(dt_u8 *buf,unsigned int buf_len);

void dt_write_to_flash(void);

void dt_read_from_flash(void);

dt_s8 mac_str_to_mac_hex_array(const dt_u8 *mac_str,dt_u8* mac_hex_array);




void dt_sysreset(void);
dt_s8 printf_hex_table(const dt_u8 *src_data,dt_u16 src_len,dt_s8* debug_iden,debug_type flag);





#endif
