#ifndef _KEY_H_
#define _KEY_H_

#include "UAConfig.h"

#ifdef	____D_UA_KEY_DEMO_SUPPORT____

//#define LED_original_value  0            //初始状态
//#define LED_gpio  GPIO_15                 //  led灯


//#define RELAY_gpio  GPIO_8              //继电器
//#define RELAY_original_value  0        //初始状态

#endif


#ifdef	____D_UA_KEY_DEMO_SUPPORT____
//=========================按键模块配置=============================================================
#define  key_original_value  1		   //按键初始状态高电平1 按下低电平 0 视硬件的情况而定

#define  DOUBLE_PRESS  0			   //是否有双击功能     有1   无 0

#ifdef	____D_Project_UA420_STYLE____
#define  KEY_GPIO   GPIO_15
#elif defined(____D_Project_UA402_CloudModule____)
#define  KEY_GPIO   GPIO_8
#elif defined(__CUSTOMER_WYYL__)
#define KEY_GPIO   GPIO_3
#define	KEY_PRESS_LEVEL	1			// 按下高电平
#else
//  ____D_Project_UA402_STYLE____
#define KEY_GPIO   GPIO_8
#define KEY_PRESS_LEVEL 0		// 按下低电平
#endif
//#define  KEY_IQR_ON       //是否有中断触发    define有     //#define没有


#define KEY_LONG_DOWN_SMART_LINK 3 		/*长按多少秒以上进入配网模式，单位为秒*/
#define KEY_LONG_DOWN_SOFTAP_LINK 10	/*长按多少秒以上进入softap配网模式，单位为秒*/

#define KEY_DOWN_LONG_TIME			(3000)	// 3000 ms
#define KEY_DOWN_LONG_LONG_TIME		(9000)	// 6000 ms


//==========================定时器跳转时间==================================
#define	key_Time_ms		50			   //50ms一次执行
#define  Shake_Time 1				   //消抖时间      50ms     按键才消抖，松手不消抖。
#define  Double_Press_Time 5		   //双击间隔时间  250ms    若有双击在此调整单击识别时间
#define  Double_Press_Timeout  10	   //双击起始超时时间    500ms

#ifdef	____D_UA_LAMPGATE_SUPPORT____
#define  Press_Long_Time   (5000 / key_Time_ms )
													//长按时间      5 000ms
#define  Press_Long_long_Time  (10000 / key_Time_ms )
														//超长按时间    10 000ms
#elif defined(____D_Project_UA402_CloudModule____)
#define  Press_Long_Time   (3000 / key_Time_ms )
													//长按时间      5 000ms
#define  Press_Long_long_Time  (10000 / key_Time_ms )
														//超长按时间    10 000ms
#else
#define  Press_Long_Time   200	   //长按时间      10 000ms
#define  Press_Long_long_Time  220 //超长按时间    22 000ms
#endif

//===========================================================================
//定义几种情景状态
enum
{
	BUTTON_PRESS_NONE = 0,
	BUTTON_PRESS_DOWN,
	BUTTON_PRESS_UP,
	BUTTON_DOUBLE_PRESS_DOWN,
	BUTTON_PRESS_DOWN_LONG1,
	BUTTON_PRESS_DOWN_LONG2
};

 //定义几种按键类型
enum
{
	APP_BUTTON_PRESS_NONE1 = 0,
	APP_BUTTON_PRESS_NONE2,

	APP_BUTTON_ACTION_CLICK,

	APP_BUTTON_ACTION_WAIT_DOUBLE_CLICK,
	APP_BUTTON_ACTION_DOUBLE_CLICK_Down,
	APP_BUTTON_ACTION_DOUBLE_CLICK_NO_UP,
	APP_BUTTON_ACTION_DOUBLE_CLICK,

	APP_BUTTON_ACTION_LONG_PRESS,
	APP_BUTTON_ACTION_LONG_PRESS_NO_UP,

	APP_BUTTON_ACTION_LONG_LONG_PRESS,
	APP_BUTTON_ACTION_LONG_LONG_PRESS_NO_UP
};

#endif



//----------------------------------------
extern void F_KEY_Init(void);

#endif
