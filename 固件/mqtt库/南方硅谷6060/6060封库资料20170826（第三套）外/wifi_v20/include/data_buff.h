#ifndef __DATA_BUFF_H__
#define __DATA_BUFF_H__
#include "ServerCmd.h"

#ifdef __cplusplus
extern "C"
{
#endif


enum 
{
	BLK_SIZE_32 = 0,
	BLK_SIZE_64 = 1,
	BLK_SIZE_128 = 2,
	BLK_SIZE_256 = 3,
	BLK_SIZE_512 = 4,
	BLK_SIZE_1024 = 5,
	BLK_SIZE_2048 = 6,
	BLK_NUM
};

#if defined(__HEKR_CLOUD_SUPPORT__)	// 氦氪云支持
//#define BUF_SIZE 512
//#define NR_SIZE 4
#elif defined(__HX_IOT_SUPPORT__) || defined(__SZCLOUD_SUPPORT__)		//  红心物联
#define NR_BLK_SIZE_32		(0)
#define NR_BLK_SIZE_64		(0)
#define NR_BLK_SIZE_128		(0)
#define NR_BLK_SIZE_256		(8)
#define NR_BLK_SIZE_512		(2)
#define NR_BLK_SIZE_1024	(2)
#define NR_BLK_SIZE_2048	(3)
#define BUF_SIZE ((1 << BLK_SIZE_256) * 32)
#elif defined(__TUYA_CLOUD_SUPPORT__)
#define NR_BLK_SIZE_32		(0)
#define NR_BLK_SIZE_64		(0)
#define NR_BLK_SIZE_128		(0)
#define NR_BLK_SIZE_256		(6)
#define NR_BLK_SIZE_512		(2)
#define NR_BLK_SIZE_1024	(1)
#define NR_BLK_SIZE_2048	(2)

#define BUF_SIZE ((1 << BLK_SIZE_256) * 32)
#elif defined(__AC_CLOUD_SUPPORT__) || defined(__JD_CLOUD_SUPPORT__)
#define NR_BLK_SIZE_32		(0)
#define NR_BLK_SIZE_64		(0)
#define NR_BLK_SIZE_128		(0)
#define NR_BLK_SIZE_256		(2)
#define NR_BLK_SIZE_512		(0)
#define NR_BLK_SIZE_1024	(0)
#define NR_BLK_SIZE_2048	(0)

#define BUF_SIZE ((1 << BLK_SIZE_256) * 32)

#elif defined(__GIZ_CLOUD_SUPPORT__)
#define NR_BLK_SIZE_32		(16)	/*与机智云中对接时，有很多小数据*/
#define NR_BLK_SIZE_64		(16)
#define NR_BLK_SIZE_128		(8)
#define NR_BLK_SIZE_256		(8)
#define NR_BLK_SIZE_512		(2)
#define NR_BLK_SIZE_1024	(1)
#define NR_BLK_SIZE_2048	(2)		/*主要用于与服务器的http交互*/

#define BUF_SIZE ((1 << BLK_SIZE_256) * 32)
#define BUF_64_SIZE ((1 << BLK_SIZE_64) * 32)
#define BUF_128_SIZE ((1 << BLK_SIZE_128) * 32)
#define BUF_512_SIZE ((1 << BLK_SIZE_512) * 32)
#define BUF_1024_SIZE ((1 << BLK_SIZE_1024) * 32)

#elif defined(__GIZ_UART_SUPPORT__)
#define NR_BLK_SIZE_32		(0)
#define NR_BLK_SIZE_64		(0)
#define NR_BLK_SIZE_128		(0)
#define NR_BLK_SIZE_256		(8)
#define NR_BLK_SIZE_512		(2)
#define NR_BLK_SIZE_1024	(1)
#define NR_BLK_SIZE_2048	(2)

#define BUF_SIZE ((1 << BLK_SIZE_256) * 32)
#else
//#define BUF_SIZE 256
//#define NR_SIZE 6
#error "please define project!"
#endif



#ifdef WIN32
#define ATTRIBUTE_SECTION_SRAM			/* blank */
#endif

typedef struct buf_t_struct 
{
	struct buf_t_struct *next;	// 队列指针
	struct buf_t_struct *prev;
	char *pbuff;			// pbuff 和 size 仅仅初始化一次
	short size;				// pbuff 总长度
	short len;				// 有效长度
	short count;			// 重发次数
	int magic0;					// magic 
} buf_t;


typedef struct
{
//	int index;
	buf_t *head;
	buf_t *tail;
	char name[15];
	Boolean busy;
} buff_queue_t;

typedef struct 
{
	int sock;
	buf_t rx_buff;
	buf_t tx_buff;		// added by qinjiangwei 2017/7/22
	short tx_size;		// 当前发送的大小
	Boolean tx_busy;	// 已经发送,但是没有收到ACK,则认为是BUSY.
	short tx_count;
	buff_queue_t rx_queue;
	buff_queue_t tx_queue;
	
#ifdef __HEKR_CLOUD_SUPPORT__	
	buff_queue_t tx_sent_queue;			// 氦氪云支持
#endif
} net_line_t;

struct httpd_fs_file 
{
  char *data;
  int len;
};

// key=value
#define HTTPD_KV_LEN 32
#define HTTPD_MAX_PARAM 8
#define HTTPD_PATH_LEN 64
typedef struct httpd_kv_struct
{
	char key[HTTPD_KV_LEN];
	char value[HTTPD_KV_LEN];
} httpd_kv_t;


typedef struct httpd_get_request_struct
{
	char filepath[HTTPD_PATH_LEN];
	httpd_kv_t kv_array[HTTPD_MAX_PARAM];
} httpd_get_req_t;

typedef struct _sock_server_ 
{
	int sock;
	buff_queue_t rx_queue;
	buff_queue_t tx_queue;
	char filename[HTTPD_PATH_LEN];
	struct httpd_fs_file file;
	char *scriptptr;
	int scriptlen;

	char *left_bracket;
	int  bracket_len;

	httpd_get_req_t http_req;
} sock_server_t;

typedef void (*send_cb)(Boolean);
typedef void (*tcp_send_cb)(int socket, char *buf, int len, Boolean resend);
typedef Boolean (*fn_compare)(int, buf_t *b);
typedef void (*map_fn_t)(buf_t *b);

void tcp_send_callback(Boolean bOk);

void init_buffer(buf_t *b) ATTRIBUTE_SECTION_SRAM;
Boolean input_char(int input) ATTRIBUTE_SECTION_SRAM;
Boolean add_rx_buffer() ATTRIBUTE_SECTION_SRAM;
buf_t *get_buf(buff_queue_t *q) ATTRIBUTE_SECTION_SRAM;
Boolean add_buf(buff_queue_t *pb, buf_t *b) ATTRIBUTE_SECTION_SRAM;
Boolean add_char(buf_t *buff, char ch) ATTRIBUTE_SECTION_SRAM;
Boolean add_string(buf_t *buff, char *str) ATTRIBUTE_SECTION_SRAM;
buf_t *remove_buf(buff_queue_t *pb) ATTRIBUTE_SECTION_SRAM;
Boolean append_buf(buf_t *to, buf_t *from) ATTRIBUTE_SECTION_SRAM;
Boolean init_buffer2(buf_t *b, char *ptr, int len) ATTRIBUTE_SECTION_SRAM;
buf_t *alloc_buffer() ATTRIBUTE_SECTION_SRAM;
void free_buffer(buf_t *b) ATTRIBUTE_SECTION_SRAM;
buf_t *new_alloc_buffer(int size) ATTRIBUTE_SECTION_SRAM;
void new_free_buffer(buf_t *b) ATTRIBUTE_SECTION_SRAM;

void init_buff_queue(buff_queue_t *pb, char *name);
void print_queue(buff_queue_t *q);
Boolean check_timeout(buff_queue_t *q, int seconds);
void remove_timeout_buf(buff_queue_t *q, int seconds);
int get_buf_by_func(buff_queue_t *q, int v, fn_compare f);
Boolean append_buf(buf_t *to, buf_t *from);
void print_buffer(buf_t *b);
void test_data_buff();
void tx_timeout_callback();
int buf_get_total_len(buff_queue_t *q);
Boolean buf_decrease_to_len(buff_queue_t *q, int final_len);
Boolean queue_is_full(buff_queue_t *pb);
void free_queue_buff(buff_queue_t *pb);
int append_buf2(buf_t *to, buf_t *from);
Boolean copy_buf(buf_t * to, buf_t * from);
void init_mem_pool(kal_uint16 *size_array);
void init_256_mem_pool();
void init_2048_mem_pool();
buf_t * get_buff_from_mem_pool(char *p);
void print_mem_pool_array();
Boolean check_buffer_valid(buf_t *b, const char *func) ATTRIBUTE_SECTION_SRAM;

buf_t *new_alloc_index_buffer(int size_index);
/*重新初始化buf,主要是清空其下的pbuff*/
extern Boolean reset_buffer(buf_t *b);

/*取尾节点buf*/
extern buf_t *get_buf_tail(buff_queue_t *q);

/*显示buf信息*/
extern void data_buf_show_info(void);

Boolean buf_acc(buff_queue_t *q, buf_t *dst_buf);

void print_mem(char *b, int len);

#ifdef __cplusplus
}
#endif
#endif
