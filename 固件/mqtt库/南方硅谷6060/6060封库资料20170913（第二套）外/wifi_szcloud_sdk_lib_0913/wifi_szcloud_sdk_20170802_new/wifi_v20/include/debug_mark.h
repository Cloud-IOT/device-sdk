#ifndef _DEBUG_MARK_H
#define _DEBUG_MARK_H 
/*
 *add by lijing 0826 debug mark
 */
 #ifdef ____D_LED_4PWM____

#define DEBUG_ATCMD_SW    FALSE//TRUE
#define	DEBUG_ATCMD_SW_Printf(...) 		do { \
	if (DEBUG_ATCMD_SW ) { printf(__VA_ARGS__); } \
	} while (0)

    /*
     *add by lijing 2016/08/30
     *该宏用于打印该打印信息所在的文件，所在的函数，所在的行。用于标记程序是否执行到该段。
     */

#define Debug_ATCMD_Mark(x)  DEBUG_ATCMD_SW_Printf ("\r\n\r\n Debug_ATCMD_Mark in func:%s @ in the file:%s @ in the line:%d\r\n\r\n", __FUNCTION__ , __FILE__, __LINE__)

#else
#define DEBUG_ATCMD_SW    FALSE
#define	DEBUG_ATCMD_SW_Printf(...) 		
    
#define Debug_ATCMD_Mark(x) 
#endif
/*end of add*/
#endif
