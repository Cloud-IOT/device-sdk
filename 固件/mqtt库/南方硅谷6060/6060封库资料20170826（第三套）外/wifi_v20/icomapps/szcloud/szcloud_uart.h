#ifndef _SZCLOUD_UART_CMD_H_
#define _SZCLOUD_UART_CMD_H_

#include "ServerCmd.h"
#include "data_buff.h"

#ifndef WIN32
#include "UA_Func.h"
#endif



#ifdef __cplusplus
extern "C"
{
#endif

#ifndef U8 
#define U8 unsigned char 
#endif

#ifndef U16
#define U16 unsigned short
#endif

#ifndef U32
#define U32 unsigned int
#endif


#define SZ_MIN_LENGTH	17
#define SZ_MAX_LENGTH	255
#define SZ_START_CHAR 	0XAA

/* 	引导头	长度	包描述	设备信息							功能码	保留				消息体    		1个字节	校验码
	0xAA	0x12	0x1000	0x0000 	0x0000 	0x0000 	0x0000		0x1001	0x0000				01				0xXX
*/

#ifdef WIN32
#pragma pack(1)
typedef struct 
{
	U8 header;		// 包头
	U8 len;			// 包长
	U16 seq;		// 包类型
	U16 info[4];
	U16 msg_type;
	U16 reserved;
	U8 data[1];
	// checksum
} sz_uart_packet_t;
#pragma pack()
#else
typedef struct 
{
	U8 header;		// 包头
	U8 len;			// 包长
	U16 seq;		// 包类型
	U16 info[4];
	U16 msg_type;
	U16 reserved;
	U8 data[1];
	// checksum
} __attribute__ ((packed)) sz_uart_packet_t;
#endif

typedef enum 
{
	SZ_ERR_OP = 0X1,		/* 操作错误 */
	SZ_ERR_CHKSUM,			/* 校验和错误 */
	SZ_ERR_RANGE,			/* 数据范围错误*/
	SZ_ERR_NO_SUPPORT		/* 命令不支持 */
} sz_uart_error_t;

typedef enum 
{
	SZ_SYSTEM_MSG,
	SZ_CONTROL_MSG,
	SZ_UPLINK_MSG,
	SZ_PRODUCT_MSG
} sz_msg_type_t;

typedef enum 
{
	SZ_ON = 1,
	SZ_OFF = 2
} sz_switch_t;

enum 
{
	SZ_SWITCH_CMD = 0X1,
	SZ_GET_STATUS = 0X2
};


#define SZ_WORD_SWAP(X) (((X << 8) | (X >> 8)) & 0xFFFF)

#define PRO_VER(x) 	((x >> 12) & 0xf)
#define SEQ(x)		(x & 0x0fff)

#define MSG_TYPE(x)	((x >> 12) & 0xf)
#define MSG_CMD(x) 	((x) & 0x3FF)
#define MSG_DIR(x)	((x >> 11) & 0x1)
#define SZ_SET_BACK_DIR(x)	 (x |= (1 << 11))

void sz_init_switch_gpio();
Boolean sz_do_with_uart_packet(char *in, buf_t *b);
Boolean __sz_do_with_uart_packet(U8 *in, int len, buf_t *b) ATTRIBUTE_SECTION_SRAM;

#endif
