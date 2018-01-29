#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "sys_status_api.h"
#include <uiplib.h>
#include "ssl_api.h"
#include "atcmd.h"
#include "atcmdlib.h"

#if 0

ssl_t *gatsslctx = NULL;
extern void SSL_printinbuf(ssl_t *sslctx);
char sslrecv[500];

PROCESS(sslcallbackproc, "ssl callback process");

PROCESS_THREAD(sslcallbackproc, ev, data)
{
	SSLMSG *msg;
    int rc;

    PROCESS_BEGIN();

    while(1)
    {
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_MSG || ev == PROCESS_EVENT_EXIT);
		if( ev == PROCESS_EVENT_EXIT) 
		{
			break;
		} 
		else if(ev == PROCESS_EVENT_MSG) 
        {
            msg = data;
            if(msg->status == SSL_CONNECTED)
            {
                //Show AT+SSLCONNECT OK
                atcmdprintf("AT+SSLCONNECT=OK\n");
            }
            else if(msg->status == SSL_CLOSED)
            {
                //Show AT+SSLDISCONNECT OK
                process_exit(&sslcallbackproc);
                atcmdprintf("AT+SSLDISCONNECT=OK\n");
                gatsslctx = NULL;
            }
            else if(msg->status == SSL_NEWDATA)
            {
                while(1)
                {
                    rc = SSL_read(gatsslctx, sslrecv, sizeof(sslrecv) - 1);
                    if(rc > 0)
                    {
                        sslrecv[rc] = 0;
//                        printf("recvlen:%d\n", rc);
                        atcmdprintf("AT+SSLRECV:%d\n%s", rc, sslrecv);
                    }
                    else
                        break;
                }
            }
            else if(msg->status == SSL_SENDACK)
            {
                atcmdprintf("AT+SSLSEND=OK\n");
            }
        }
    }

    PROCESS_END();
}

/*---------------------------------------------------------------------------*/
int At_SSLConnect(stParam *param)
{
	char *pIp = 0, *pport = 0;
    U16 port;
    uip_ip4addr_t remote_ip_addr;

	if (get_wifi_connected() != 1)
		return ERROR_WIFI_CONNECTION;

	if (gatsslctx != NULL)
	{
		atcmdprintf("SSL connection exist\n");
		return ERROR_TCP_CONNECTION;
	}
	
	if (param->argc < 2) {
		return ERROR_INVALID_PARAMETER;
	}

	pIp = param->argv[0];
	pport = param->argv[1];
	
	port = atoi (pport);
	if (port == 0) {
		return ERROR_INVALID_PARAMETER;
	}

	if( uiplib_ipaddrconv(pIp, &remote_ip_addr)  == 0 )	
	{
		return ERROR_INVALID_PARAMETER;
	}
	else
	{
//		printf("IP format : %s\n", pIp);
        process_start(&sslcallbackproc, NULL);
        SSL_init();
        gatsslctx = SSL_connect(&sslcallbackproc, NULL, remote_ip_addr, port);
	}

	return ERROR_SUCCESS_NO_RSP;
}

/*---------------------------------------------------------------------------*/
int At_SSLSend(stParam *param)
{
	if (gatsslctx == NULL || gatsslctx->hsState != SSL_HS_DONE)
	{
		atcmdprintf("AT+SSLSEND=FAIL\n");
		return ERROR_TCP_CONNECTION;
	}

    SSL_write(gatsslctx, param->argv[0], strlen(param->argv[0]));
    
	return ERROR_SUCCESS_NO_RSP;
}

/*---------------------------------------------------------------------------*/
int At_SSLDisconnect(stParam *param)
{
	if (gatsslctx == NULL)
	{
		atcmdprintf("AT+SSLDISCONNECT=FAIL\n");
		return ERROR_TCP_CONNECTION;
	}
    process_exit(&sslcallbackproc);
    SSL_disconnect(gatsslctx);
    atcmdprintf("AT+SSLDISCONNECT=OK\n");
    gatsslctx = NULL;
	return ERROR_SUCCESS_NO_RSP;
}
#endif
