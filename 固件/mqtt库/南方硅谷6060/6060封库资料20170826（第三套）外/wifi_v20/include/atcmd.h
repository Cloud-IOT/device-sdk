#ifndef _ATCMD_H_
#define _ATCMD_H_

#include "contiki.h"
//#include "Cabrio-conf.h"
#include <stdio.h> /* For printf() */
#include "console.h"

////Customer AT Command
#define MAX_ARGUMENT 24
typedef struct _stParam
{
	char*	argv[MAX_ARGUMENT];
	int	argc;
} stParam;

typedef int (*PF_handle) (stParam *param);
typedef struct _at_cmd_info
{
	char*		atCmd;
	PF_handle	pfHandle;
    U8 maxargu;
} at_cmd_info;

int At_ParserInd (EVENTMSG_ID msgid, unsigned char *buff, int len) ;
int At_Parser (char *buff, int len);
int At_SSLConnect(stParam *param);
int At_SSLSend(stParam *param);
int At_SSLDisconnect(stParam *param);
extern int At_AP (stParam *param);
extern int At_EnableSmartReboot(stParam *param);

#endif
