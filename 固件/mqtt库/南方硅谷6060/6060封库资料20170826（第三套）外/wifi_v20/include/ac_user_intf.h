#ifndef AC_USER_INTF_H
#define AC_USER_INTF_H

#include "ServerCmd.h"

#ifndef U8 
#define U8 unsigned char 
#endif

#ifndef U16
#define U16 unsigned short
#endif

/*************************************************
* Function: AC_DealEvent
* Description:============================================
             |  1B  |1B|1B|1B| 1B | 1B |  nB  | 1B |  1B |
             ============================================
            | 0x5A | len |id|resv|code|payload|sum|0x5B |
            ============================================
 */
#ifdef WIN32
#pragma pack(1)
typedef struct 
{
	U8 header;			// 包头
	U16 len;			// 包长 NETWORK 
	U8 id;
	U8 resv;
	U8 code;
	U8 data[1];
	// checksum
} ac_uart_packet_t;
#pragma pack()
#else
typedef struct 
{
	U8 header;			// 包头
	U16 len;			// 包长 NETWORK 
	U8 id;
	U8 resv;
	U8 code;
	U8 data[1];
	// checksum
} __attribute__ ((packed)) ac_uart_packet_t;
#endif

typedef enum 
{
	AC_ERR_OP = 0X1,		/* 操作错误 */
	AC_ERR_CHKSUM,			/* 校验和错误 */
	AC_ERR_RANGE,			/* 数据范围错误*/
	AC_ERR_NO_SUPPORT		/* 命令不支持 */
} ac_uart_error_t;


#define AC_MIN_LENGTH	9
#define AC_MAX_LENGTH	255
#define AC_START_CHAR 	0x5A
#define AC_END_CHAR 	0X5B

#define AC_ON 	0x1
#define AC_OFF 0x0


Boolean ac_send_user_register();
void ac_connect_callback();
void ac_parse_uart_data(U8 *pdata, int len);

#endif
