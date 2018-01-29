#ifndef IPERF_CMD_H
#define IPERF_CMD_H

#include "uip.h"
#include "socket_driver.h"

typedef struct _iperfparam
{
	U8	bClientServer;
	uip_ipaddr_t ipRemote;
	int	nTime;
	int	nInterval;
	U8	bRunning;
	int	nNum;
} iperfparam;

#endif

