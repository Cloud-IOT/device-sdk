//#include "uip.h"
//#include "socket_driver.h"
#include "uiplib.h"
#include "irq.h"
#include "flash_api.h"
#include "wdog_api.h"
#include "socket_app.h"
#include "net/uip_arp.h"
#include "atcmd.h"
#include "iperf_cmd.h"
#include "systemconf.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int AT_IPERF_CMD(stParam *param)
{
    return 0;
}


#if 0

#define IPERF_BUFFER_SIZE 1500

extern TAG_CABRIO_CONFIGURATION	gCabrioConf;

iperfparam iPerfParam = { 0 };
int giperfsock = -1;

char iperf_cmd[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x13, 0x89, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xfe, 0x0c};
static char *iperf_buffer = NULL;
static char *iperf_string = NULL;
int iperf_size = 0;
int start_time = 0;
int now_time = 0;
	
/*---------------------------------------------------------------------------*/
PROCESS(iperf_handle_process, "iperf handle Process");
PROCESS(iperf_client_process, "iperf client Process");
/*---------------------------------------------------------------------------*/


S32 allocate_iperf_buffer()
{
    S32 rlt = -1;
    if(NULL==iperf_buffer)
    {
        iperf_buffer = malloc(IPERF_BUFFER_SIZE);
        memset(iperf_buffer, 0x0, IPERF_BUFFER_SIZE);
        rlt = 0;
    }

    return rlt;
}

S32 release_iperf_buffer()
{
    S32 rlt = -1;
    if(iperf_buffer != NULL)
    {
        free(iperf_buffer);
        iperf_buffer = NULL;
        rlt  = 0;
    }
    return rlt;
}

S32 allocate_iperf_string()
{	
    S32 rlt = -1;
    int i;
    if(NULL==iperf_string)
    {
        iperf_string = malloc(iPerfParam.nNum);
        memset(iperf_string, 0x0, iPerfParam.nNum);
	  for(i = 0;i<iPerfParam.nNum;i++)
	  {
	      *(iperf_string+i) = i%10 + 0x30; 
	  }
        rlt = 0;
    }

    return rlt;
}

S32 release_iperf_string()
{	
    S32 rlt = -1;
    if(iperf_string != NULL)
    {
        free(iperf_string);
        iperf_string = NULL;
        rlt  = 0;
    }
    return rlt;
}

int DefaultStuct()
{
	iperf_size = 0;
	memset(&iPerfParam, 0, sizeof(iPerfParam));
	iPerfParam.nTime = 10;
	iPerfParam.nNum = 1460;
	return 0;
}

int parseIperf2Param(char* bufCmd, stParam* pParam)
{
	int buflen, i;
	const char delimiters = ' ';

	buflen = strlen (bufCmd);
	if (strlen (bufCmd) == 0) 
		return -1;

	pParam->argc = 1;
	pParam->argv[pParam->argc - 1] = &bufCmd[0];
	i = 0;
	while(pParam->argc < 10)
	{
		for(; i < buflen ; i++)
		{
			if(bufCmd[i] == delimiters)
			{
				bufCmd[i] = 0;
				break;
			}
		}
        
		if(bufCmd[i + 1] != 0)
		{
			pParam->argc += 1;
			pParam->argv[pParam->argc - 1] = &bufCmd[i + 1];
		}
		else
			break;
	}
    
	return 0;
}

int Param2Struct(stParam *param)
{
	int i = 0;
	for (i = 0; i < param->argc; i++)
	{
		{
			char *op = param->argv[i];
			switch (*(op+1))
			{
			case 's':	//server
				{
					iPerfParam.bClientServer = 0;
				}
				break;
			case 'c':	//client
				{
					iPerfParam.bClientServer = 1;
					if( uiplib_ipaddrconv(param->argv[i+1], &iPerfParam.ipRemote)  == 0 )	
					{
						printf("ip format error\n");
						return 1;
					}		
					else
						printf("remote ip:%d.%d.%d.%d\n", iPerfParam.ipRemote.u8[0], iPerfParam.ipRemote.u8[1], iPerfParam.ipRemote.u8[2], iPerfParam.ipRemote.u8[3] );
				}			
				i++;
				break;
			case 'i':	//interval
				{
					iPerfParam.nInterval = atoi(param->argv[i+1]);
				}				
				i++;
				break;
			case 't':	//time
				{
					iPerfParam.nTime = atoi(param->argv[i+1]);
				}			
				i++;
				break;
			case 'n':	//time
				{
					iPerfParam.nNum = atoi(param->argv[i+1]);
				}			
				i++;
				break;				

			//not implement
			case 'u':	//udp
			case 'b':	//bandwidth for udp
			default:
				printf("unsupport param\n");
				//wprintf(L"unsupport param %c\n", *(wp + 1));
				break;
			}
		}

	}
	return 0;	
}

int iperfserver()
{
	allocate_iperf_buffer();
	process_start(&iperf_handle_process, NULL);
	if(tcplisten(5001, &iperf_handle_process) == -1)
	{
		printf("listen tcp fail\n");
		return ERROR_TCP_CONNECTION;
	}
	else
	{
		/*
		------------------------------------------------------------
		Server listening on TCP port 5001
		TCP window size: 64.0 KByte (default)
		------------------------------------------------------------	
		*/
		printf("--------------------------------------------\n");
		printf("Server listening on TCP port 5001\n");
		printf("TCP windows size: 1460 Byte (default)\n");
		printf("--------------------------------------------\n");
		return ERROR_SUCCESS;
	}
	
}

int iperfclient()
{
	/*
	------------------------------------------------------------
	Client connecting to 192.168.31.174, TCP port 5001
	TCP window size: 46.1 KByte (default)
	------------------------------------------------------------
	*/
	printf("------------------------------------------------------------\n");
	printf("Client connecting to %d.%d.%d.%d, TCP port 5001\n", iPerfParam.ipRemote.u8[0], iPerfParam.ipRemote.u8[1], iPerfParam.ipRemote.u8[2], iPerfParam.ipRemote.u8[3] );
	printf("TCP window size: 1460 Byte (default)\n");
	printf("------------------------------------------------------------\n");
	allocate_iperf_string();
	process_start(&iperf_client_process, NULL);
	return 1;
}

int iperfclose()
{
	if(giperfsock != -1)
	{
		tcpclose(giperfsock) ;
		giperfsock = -1;
	}
}

int AT_IPERF_CMD(stParam *pParam)
{
	stParam iperfParam = {0};
	stParam* param;
	param = &iperfParam;
	//pParam transfer iperfParam
	if(pParam->argc == 1)
		parseIperf2Param(pParam->argv[0], param);
	
	if(param->argc == 0)
	{
		if(iPerfParam.bRunning == 1)
		{
			printf("stop iPerf Server\n");
			process_post (&iperf_handle_process, PROCESS_EVENT_EXIT, NULL);
		}
		process_post (&iperf_client_process, PROCESS_EVENT_EXIT, NULL);
		return 0;
	}
	if(iPerfParam.bRunning == 1)	// iperf is running
	{
		printf("iPerf Server is Running, please enter iperf to stop process\n");
		return 0;
	}
	DefaultStuct();
	Param2Struct(param);
	
	//DUMP param
/*
	printf("DUT is %s\n", iPerfParam.bClientServer==0?"server":"client");
	printf("DUT interval %d sec\n", iPerfParam.nInterval);
	printf("DUT time %d sec\n", iPerfParam.nTime);
*/
	if(iPerfParam.bClientServer == 0)	//Server
		iperfserver();
	else
		iperfclient();

	return 0;
}

PROCESS_THREAD(iperf_handle_process, ev, data)
{
	iPerfParam.bRunning = 1;
//	allocate_iperf_buffer();
	
	PROCESS_BEGIN();
	
	SOCKETMSG msg;
	int recvlen;
	uip_ipaddr_t peeraddr;
	U16 peerport;

	while(1) 
	{
		PROCESS_WAIT_EVENT();

		if( ev == PROCESS_EVENT_EXIT) 
		{
			break;
		} 
		else if(ev == PROCESS_EVENT_MSG) 
		{
			msg = *(SOCKETMSG *)data;
			//The TCP socket is connected, This socket can send data after now.
			if(msg.status == SOCKET_CONNECTED)
			{
				printf("socked:%d connected\n", msg.socket);
			}
			//TCP connection is closed. Clear the socket number.
			else if(msg.status == SOCKET_CLOSED)
			{
				/*
				[  4]  0.0-10.0 sec   361 MBytes   303 Mbits/sec
				*/	
				now_time = clock_time();
				int nTime = (now_time - start_time) / 1000000;
				printf("[%3d] %d - %d sec\t %d Bytes\t %d bits/sec\n", msg.socket, 0, nTime, iperf_size, iperf_size*8/nTime);
				if(giperfsock == msg.socket)
					giperfsock = -1;
			}
			//Get ack, the data trasmission is done. We can send data after now.
			else if(msg.status == SOCKET_SENDACK)
			{
				printf("socked:%d send data ack\n", msg.socket);
			}
			//There is new data coming. Receive data now.
			else if(msg.status == SOCKET_NEWDATA)
			{
				if(0 <= msg.socket && msg.socket < UIP_CONNS)
				{
					recvlen = tcprecv(msg.socket, iperf_buffer, IPERF_BUFFER_SIZE);
					iperf_buffer[recvlen] = 0;  
	  				iperf_size+=recvlen;
				}
				else
				{
					printf("Illegal socket:%d\n", msg.socket);
				}
			}
			//A new connection is created. Get the socket number and attach the calback process if needed.
			else if(msg.status == SOCKET_NEWCONNECTION)
			{
//				if(gserversock == -1)
//				{
//					gserversock = msg.socket;
//					printf("new connected to listen port(%d), socket:%d\n", msg.lport, msg.socket);
					/*
					[  4] local 192.168.0.110 port 5001 connected with 192.168.0.110 port 18104
					[ ID] Interval       Transfer     Bandwidth
					*/
					start_time =  clock_time();
					getpeerIPv4addr(msg.socket, &peeraddr);
					printf("\n");
					printf("[%3d] local %d.%d.%d.%d port %d connected with %d.%d.%d.%d\n", msg.socket, gCabrioConf.local_ip_addr.u8[0] , gCabrioConf.local_ip_addr.u8[1], gCabrioConf.local_ip_addr.u8[2], gCabrioConf.local_ip_addr.u8[3] , 5001, peeraddr.u8[0], peeraddr.u8[1], peeraddr.u8[2], peeraddr.u8[3], 12345);
					printf("[ ID] Interval\t Transfer\t Bandwidth\n");
//				}
//				else
//				{
					//Only allow one client connection for this application.
					//tcpclose(msg.socket);
//				}
			}
			else
			{
				printf("unknow message type\n");
			}
		}	
	}	
	release_iperf_buffer();
	iPerfParam.bRunning = 0;
	PROCESS_END();	
}

PROCESS_THREAD(iperf_client_process, ev, data)
{
	static struct etimer iperftimeout;
	SOCKETMSG msg;
	int recvlen;
	//allocate_iperf_string();

	PROCESS_BEGIN();
	
		//tcp connection
		giperfsock = tcpconnect( &iPerfParam.ipRemote, 5001, &iperf_client_process);
		
		//wait for TCP connected or uip_timeout.
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_MSG);
		msg = *(SOCKETMSG *)data;
		if(msg.status != SOCKET_CONNECTED)
		{
			printf("TCP connect fail! Post message type:%d\n", msg.status);
			goto dissconnect;
		}
		else
		{
			giperfsock = msg.socket;
			//[  3] local 192.168.40.223 port 54060 connected with 192.168.31.174 port 5001
			printf("[%3d] local %d.%d.%d.%d port %d connected with %d.%d.%d.%d port %d\n",giperfsock, gCabrioConf.local_ip_addr.u8[0] , gCabrioConf.local_ip_addr.u8[1], gCabrioConf.local_ip_addr.u8[2], gCabrioConf.local_ip_addr.u8[3], 54050 
																					, iPerfParam.ipRemote.u8[0], iPerfParam.ipRemote.u8[1], iPerfParam.ipRemote.u8[2], iPerfParam.ipRemote.u8[3],  5001);
		}
	
		//Send http request to server		
		tcpsend(giperfsock, iperf_cmd, 24);	
		//Wait for data is transmitted or uip_timeout.

 		start_time = clock_time()/1000;
		now_time = clock_time()/1000;
		
		//printf("start at %d\n", now_time);		
		while(1)
		{
			now_time = clock_time()/1000;
			if( (now_time-start_time) >= iPerfParam.nTime*1000 )
				break;	
			
			tcpsend(giperfsock, iperf_string, iPerfParam.nNum);		
			iperf_size += iPerfParam.nNum;
			PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_MSG||ev == PROCESS_EVENT_TIMER || ev == PROCESS_EVENT_EXIT);			
			if(ev == PROCESS_EVENT_MSG)
			{
				msg = *(SOCKETMSG *)data;
				if(msg.status == SOCKET_SENDACK)
				{
					;
				}
				else if(msg.status == SOCKET_CLOSED)
				{
					printf("TCP send fail! Post message type:%d\n", msg.status);
					goto dissconnect;
				}
			}
			else if(ev == PROCESS_EVENT_TIMER)
			{
				goto dissconnect;		
			}
			else if(ev == PROCESS_EVENT_EXIT)
			{
				printf("user stop iperf\n");
				goto dissconnect;	
			}
			
			
		}
		//printf("end at %d\n", now_time);
		//[ ID] Interval       Transfer     Bandwidth
		//[  3]  0.0-10.0 sec   381 MBytes   320 Mbits/sec
		int nTime = (now_time -start_time)/1000;
		printf("[ ID] Interval\t Transfer\t Bandwidth\n");
		printf("[%3d] %d - %d sec\t %d Bytes\t %d bits/sec\n", giperfsock, 0, nTime , iperf_size, iperf_size*8/nTime);
		

dissconnect:
		//dissconnect
		tcpclose(giperfsock);
		
		//Wait for one second to close the connection.
		etimer_set(&iperftimeout, 1 * CLOCK_SECOND);
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
    release_iperf_string();
    PROCESS_END();

}
/*---------------------------------------------------------------------------*/
#endif

