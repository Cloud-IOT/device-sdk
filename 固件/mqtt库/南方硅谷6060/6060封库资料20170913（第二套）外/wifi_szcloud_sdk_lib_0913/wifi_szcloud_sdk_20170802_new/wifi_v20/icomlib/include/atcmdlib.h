#ifndef _ATCMDLIB_H_
#define _ATCMDLIB_H_

#include <drv_uart.h>

#define ATCMD_UARDID SSV6XXX_UART0
#define ATCMD_UARDBAUDRATE BAUD_115200

typedef struct t_ATCMD_CTX
{
    U8                init;
    U8                irqunm;
    SSV6XXX_UART_ID_E uartid;
    BAUD_RATE         baudrate;
    EVENTMSG_DATA     data;
} ATCMD_CTX ;

int atcmd_init(SSV6XXX_UART_ID_E id, BAUD_RATE rate);
int atcmdprintf(const char *fmt, ...);
void backecho(U8 enable);

#endif /* _ATCMDLIB_H_ */
