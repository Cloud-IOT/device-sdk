
/*插座定时器的数据结构及函数接口*/
#ifndef __SK_TIMER_H_H__
#define __SK_TIMER_H_H__

#include "ntp_client.h"

#define SK_TIMER_NUM 8 		/*最多支持8个定时器节点*/
#define SK_TIMER_INVALID_ID 0XFF	/*无效节点标志*/

#define SK_INVALID_TIME_ZONE 55


#define SK_ON	0x1
#define SK_OFF	0x0

/*系统中保存的定时器节点*/
typedef struct sk_timer_s
{		
	U8 timer_id;		/*定时器唯一id，区别定时器，由APP生成，设备会保存, 0xFF表示该定时器无效*/
	char timeZone;		/*时区*/
	char timer_action;	/*定时器的动作, 0x00表示关掉开关，0x01 表示开启开关*/
	char week_flag; 	/*0~6 bit 表示周一到周日是否开启周重复，都不置位表示非周重复,0x00表示执行一次定时*/
	U32 time;			/*最近生效的时刻单位为秒*/
} __attribute__ ((packed)) sk_timer_t;

	
extern sk_timer_t gSkTimer[SK_TIMER_NUM];	/*定时器节点组*/

struct tm current_daytime;

/*处理单个定时器节点的外部函数，由应用模块实现及传入*/
typedef void (*sk_handle_timer_fun)(sk_timer_t *ptimer, void *param, int size, int timeCount);

#define SK_TIMER_DEBUG_ON 1

#if SK_TIMER_DEBUG_ON
#define SK_TIMER_DEBUG(fmt,...) printf(fmt, ##__VA_ARGS__)
#else
#define SK_TIMER_DEBUG(fmt,...)
#endif


/*初始化定时器节点*/
extern void init_sk_timer(void);

/*遍历所有的定时器节点，进行fun处理，fun由应用模块实现及传入,返回值为处理的有效节点个数*/
extern int sk_for_each_handle(sk_handle_timer_fun fun, void *param, int size);

/*添加或更新timer节点, 返回值，TRUE - 添加成功，否则，操作失败*/
extern BOOL sk_add_timer(sk_timer_t *ptimer);

/*删除timer节点*/
extern BOOL sk_del_timer(U8 timerId);

/*删除定时器节点, 并重新组织定时器组*/
extern BOOL sk_remove_timer(int timerId);

/*保存配置信息*/
extern BOOL sk_save_conf(void);

/*显示定时节点*/
extern void sk_timer_show(void);

void sk_print_timer_list();

void sk_timer_test();

void sk_timer_check_callback(U32 current_second);

/*从小到大进行排序*/
extern void sk_sort_timer(sk_timer_t *parray);

/*
*	调用函数接口之前,先初始化my_timer_list
*/
extern void sk_timer_task_init(sk_timer_t *parray);

/*
 * 将定时器数据填入到outdata中，size 为outdata大小, 返回值为所占用大小, -1 表示失败
 */
int sk_timer_get_list(U8 *outdata, int size);

/*填充一个timer节点放到param所对应的buf中*/
void sk_timer_fill_one_timer(sk_timer_t *ptimer, char *buf, int size);

#endif

