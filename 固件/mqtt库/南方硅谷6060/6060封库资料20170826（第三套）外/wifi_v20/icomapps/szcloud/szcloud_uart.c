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
//#include "dt_mqtt.h"
#include "szcloud_uart.h"

#ifdef __SZCLOUD_SUPPORT__

#define SZ_DEMO_GPIO_NUM		GPIO_2

static Boolean sz_switch_status;
static void sz_switch_action(int act) ATTRIBUTE_SECTION_SRAM;
buf_t *sz_uart_build_reply_packet(sz_uart_packet_t *req, buf_t *b, sz_switch_t op) ATTRIBUTE_SECTION_SRAM;
sz_uart_packet_t * __sz_get_packet(U8 *buff, int len, int *error) ATTRIBUTE_SECTION_SRAM;
static unsigned char sz_calc_chksum(sz_uart_packet_t *pack) ATTRIBUTE_SECTION_SRAM;

static char sz_char_bin2hex(unsigned char v)
{
	if (v >= 0 && v <= 9)
	{
		return '0' + v;
	}
	else if (v >= 10 && v <= 15)
	{
		return 'A' + v - 10;
	}
	else
	{
		return '0';
	}
}

static Boolean sz_bin2hex(char *in_buf, int in_len, char *out_buf, int out_len)
{
	int i;
	int j;
	char *p;
	char *q;

	p = in_buf;
	q = out_buf;
	for (i = 0, j = 0; i < in_len; i++, p++, j+= 2)
	{
		*q++ = sz_char_bin2hex(*p >> 4 & 0xf);
		*q++ = sz_char_bin2hex((*p) & 0xf);

		if (j >= out_len)
		{
			return FALSE;
		}
	}

	return TRUE;
}

static U8 sz_b2h(U8 ch)
{
	if (ch >= 0 && ch <= 9)
	{
		return '0' + ch;
	}
	else if (ch >= 10 && ch <= 15)
	{
		return 'A' + ch - 10;
	}
	else
	{
		return '0';
	}
}

static int sz_h2b(int v)
{
	if (v >= '0' && v <= '9')
	{
		return v - '0';
	}
	else if (v >= 'a' && v <= 'f')
	{
		return 10 + v - 'a';
	}
	else if (v >= 'A' && v <= 'F')
	{
		return 10 + v - 'A';
	}
	else 
	{
		return -1;
	}
}

static int sz_hex2bin(int hi, int lo)
{
	int base = 16;
	
	return sz_h2b(hi) * base + sz_h2b(lo);
}

static unsigned char sz_calc_chksum(sz_uart_packet_t *pack)
{
	int v = 0;
	int i;
	U8 chksum;
	U8 *p;

	p = (U8 *)pack;

	for (i = 0, v = 0; i < pack->len - 1; i++, p++)
	{
		v += *p;
	}

	chksum = v & 0xFF;
	chksum = ~chksum;
	chksum += 1;

	return chksum;	
}

/*
 * 从buf中解析一个数据包
 */
sz_uart_packet_t * sz_get_packet(buf_t *b, int *error)
{
	int i;
	U8 *p;
	Boolean found = FALSE;
	sz_uart_packet_t *pack;
	unsigned int val;

	if (b->len < SZ_MIN_LENGTH)
	{
		*error = SZ_ERR_RANGE;
		return NULL;
	}

	for (i = 0, p = (U8 *)(b->pbuff); i < b->len; i++, p++)
	{
		if (*p == SZ_START_CHAR)
		{
			found = TRUE;
			break;
		}
	}

	if (!found)
	{
		*error = SZ_ERR_OP;
		return NULL;
	}

	if (b->len - i < SZ_MIN_LENGTH)
	{
		*error = SZ_ERR_RANGE;
		return NULL;
	}

	pack = (sz_uart_packet_t *)p;

	val = sz_calc_chksum(pack);

	if (val != p[pack->len - 1]) 
	{
		printf("cal = %x, org = %x\n", val, p[pack->len - 1]);
		*error = SZ_ERR_CHKSUM;
		return NULL;
	}

	return pack;
}

sz_uart_packet_t * __sz_get_packet(U8 *buff, int len, int *error)
{
	int i;
	U8 *p;
	Boolean found = FALSE;
	sz_uart_packet_t *pack;
	unsigned int val;

	if (len < SZ_MIN_LENGTH)
	{
		*error = SZ_ERR_RANGE;
		return NULL;
	}

	for (i = 0, p = buff; i < len; i++, p++)
	{
		if (*p == SZ_START_CHAR)
		{
			found = TRUE;
			break;
		}
	}

	if (!found)
	{
		*error = SZ_ERR_OP;
		return NULL;
	}

	if (len - i < SZ_MIN_LENGTH)
	{
		*error = SZ_ERR_RANGE;
		return NULL;
	}

	pack = (sz_uart_packet_t *)p;

	val = sz_calc_chksum(pack);

	if (val != p[pack->len - 1]) 
	{
		printf("cal = %x, org = %x\n", val, p[pack->len - 1]);
		*error = SZ_ERR_CHKSUM;
		return NULL;
	}

	return pack;
}


// TODO. Has bug. return the local variable.
static sz_uart_packet_t * sz_get_pack_from_raw(char *in)
{
	int len;
	int c1, c2;
	sz_uart_packet_t *pack;
	int error;
	int i;
	buf_t *b;

	if (!(b = alloc_buffer()))
	{
		return NULL;
	}
	
	len = strlen(in);
	
	for (i = 0; i < len;)
	{
		c1 = in[i];
		c2 = in[i + 1];
		i += 2;

		add_char(b, sz_hex2bin(c1, c2));
	}

	pack = sz_get_packet(b, &error);
	if (!pack)
	{
		printf("raw pack error = %d", error);
		return NULL;
	}

	return pack;
}

#ifndef WIN32
void sz_init_switch_gpio()
{
	GPIO_CONF conf;

	conf.driving_en = 0;
	conf.dirt = OUTPUT;
	conf.pull_en = 0;

	pinMode(SZ_DEMO_GPIO_NUM, conf);
	sz_switch_action(SZ_OFF);
}
#endif

static void sz_switch_action(int action)
{
	if (action == SZ_ON)
	{
		sz_switch_status = TRUE;
	}
	else
	{
		sz_switch_status = FALSE;
	}
	
//	printf("act=%d\n", action);
#ifndef WIN32
	digitalWrite(SZ_DEMO_GPIO_NUM, sz_switch_status);
#endif	
}

void sz_key_switch_action()
{
	sz_switch_status = !sz_switch_status;
#ifndef WIN32
	digitalWrite(SZ_DEMO_GPIO_NUM, sz_switch_status);
#endif
}


/*
 * 构造查询版本的返回包
 */
buf_t *sz_uart_build_reply_packet(sz_uart_packet_t *req, buf_t *b, sz_switch_t op)
{
	U8 *p_data;
	U16 v;
	
	if (req->len > b->size)
	{
		printf("error:uartpack len=%d\r\n", req->len);
		return NULL;
	}

	
	v = SZ_WORD_SWAP(req->msg_type);		// 大端转小端
	SZ_SET_BACK_DIR(v);
	req->msg_type = SZ_WORD_SWAP(v);	// 小端转大端
	p_data = req->data;
	*p_data++ = op;
	*p_data = sz_calc_chksum(req);

	memcpy(b->pbuff, (void *)req, req->len);
	b->len = req->len;

//	print_buffer(b);

	return b;
}

Boolean __sz_do_with_uart_packet(U8 *in, int len, buf_t *b)
{
	sz_uart_packet_t *pack;
	int type;
	Boolean ret = FALSE;
	int error;
	
	pack = __sz_get_packet(in, len, &error);
	if (!pack)
	{
		printf("no pack, erro=%d!\n", error);
		return FALSE;
	}

	type = MSG_TYPE(SZ_WORD_SWAP(pack->msg_type));
//	printf("msg type=%d\n", type);
	switch (type)
	{
		case SZ_SYSTEM_MSG:
		{
			break;
		}

		case SZ_CONTROL_MSG:
		{
			char *data = pack->data;
			int cmd = MSG_CMD(SZ_WORD_SWAP(pack->msg_type));

			if (cmd == SZ_SWITCH_CMD)
			{
				if (*data == SZ_ON || *data == SZ_OFF)
				{
					sz_switch_action(*data);
					ret = (sz_uart_build_reply_packet(pack, b, *data) != NULL);
				}
				else
				{
					printf("unknown op:%d\n", *data);
				}
			}
			else if (cmd == SZ_GET_STATUS)
			{
				int op;
				if (sz_switch_status)
				{
					op = SZ_ON;
				}
				else
				{
					op = SZ_OFF;
				}
				
				ret = (sz_uart_build_reply_packet(pack, b, op) != NULL);
			}
			else
			{
				printf("unknown cmd:%d\n", cmd);
			}
			
			break;
		}

		case SZ_UPLINK_MSG:
		{

			break;
		}

		case SZ_PRODUCT_MSG:
		{

			break;
		}

		default:
			break;
	}

	return ret;
}


/*
 * 传入为hex类型
 */
Boolean sz_do_with_uart_packet(char *in, buf_t *b)
{
	sz_uart_packet_t *pack;
	int type;
	Boolean ret = FALSE;
	
	pack = sz_get_pack_from_raw(in);
	if (!pack)
	{
		printf("no pack!\n");
		return FALSE;
	}

	type = MSG_TYPE(SZ_WORD_SWAP(pack->msg_type));
	printf("msg type=%d\n", type);
	switch (type)
	{
		case SZ_SYSTEM_MSG:
		{
			break;
		}

		case SZ_CONTROL_MSG:
		{
			char *data = pack->data;

			if (*data == SZ_ON || *data == SZ_OFF)
			{
				sz_switch_action(*data);
				ret = (sz_uart_build_reply_packet(pack, b, *data) != NULL);
			}
			else
			{
				printf("unknown op:%d\n", *data);
			}
			
			break;
		}

		case SZ_UPLINK_MSG:
		{

			break;
		}

		case SZ_PRODUCT_MSG:
		{

			break;
		}

		default:
			break;
	}

	free_buffer((buf_t *)pack);
	return ret;
}

void sz_uart_test()
{
	char action_str[] = "AA1210000000000000000000100100000122";
	buf_t *b;

	b = alloc_buffer();
	
	sz_do_with_uart_packet(action_str, b);

	free_buffer(b);
}


#endif
