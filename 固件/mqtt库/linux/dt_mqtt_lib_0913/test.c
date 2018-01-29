#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <linux/tcp.h>

#include <signal.h>
#include <sys/time.h>

#include "dtston_packet.h"


int socket_id;

int mqtt_send(void* sockfd, const void* buf,unsigned int count)
{
	int fd = *((int*)sockfd);
	return send(fd, buf, count, 0);
}

int mqtt_recv(void* sockfd, void* buf,unsigned int count)
{
	int fd = *((int*)sockfd);
	return recv(fd, buf, count, 0);
}

int socket_init()
{
	int flag = 1;
	// Create the socket
	if((socket_id = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		return -1;

	// Disable Nagle Algorithm
	if (setsockopt(socket_id, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag)) < 0)
		return -2;

	struct sockaddr_in socket_address;
	// Create the stuff we need to connect
	socket_address.sin_family = AF_INET;
	socket_address.sin_port = htons(1883);
	socket_address.sin_addr.s_addr = inet_addr("120.24.55.58");

	// Connect the socket
	if((connect(socket_id, (struct sockaddr*)&socket_address, sizeof(socket_address))) < 0)
		return -1;

	printf("sockfd = %d\n", socket_id);
	return 1;
}

void recv_func()
{
	fd_set readfds;
	struct timeval timeout;
    FD_ZERO(&readfds);
    if(socket_id>=0)
        FD_SET(socket_id,&readfds);

    timeout.tv_sec = 3;
    timeout.tv_usec = 0;
    select(socket_id+1, &readfds, NULL, NULL, &timeout);

    if (FD_ISSET(socket_id, &readfds))
    {
    	switch(dt_mqtt_subscribe_recv_func(&broker))
    	{
    		case 1://收到订阅的数据
    		printf("packet_body_data->dt_array=%s\n", dt_packet_body_data->dt_array);
			printf("packet_body_data->length->=%d\n", dt_packet_body_data->length);

			dt_mqtt_data_send(dt_packet_body_data->dt_array,dt_packet_body_data->length);

			memset(dt_packet_body_data->dt_array, 0x0, sizeof(dt_packet_body_data->dt_array));
			dt_packet_body_data->length=0;
			break;
    		case 2://收到心跳的数据
    		break;
    		default:
    		break;
    	}



    }
}

void set_time(void)
{
   struct itimerval itv;
   itv.it_interval.tv_sec = 30;//锟皆讹拷装锟截ｏ拷之锟斤拷每10锟斤拷锟斤拷应一锟斤拷
   itv.it_interval.tv_usec = 0;
   itv.it_value.tv_sec = 15;//锟斤拷一锟轿讹拷时锟斤拷时锟斤拷
   itv.it_value.tv_usec = 0;
   setitimer(ITIMER_REAL,&itv,NULL);
}

void send_mqtt_ping()
{
    dt_mqtt_ping(&broker);
}

int main(int argc, char** argv)
{
	//创建mqtt的socket
	socket_init();

	//初始化深智云参数(必须)
	save_param.type_value = 999;//产品类型，十进制
	char mac_id[12]="ACCF234F19B0";//WiFi模块的MAC地址
	memcpy(save_param.mac_str,mac_id,12);

	//初始化mqtt参数，socket_id是mqtt的套接字
	//mqtt连接上了返回1
	int n = dt_mqtt_init(&broker,&socket_id);
    printf("n = %d\n", n);
	printf("444\n");
	if (n == 1) {
		printf("555\n");
		//mqtt订阅上深智云，返回2
		n = dt_mqtt_subscribe(&broker);
		printf("n = %d\n", n);
		if (n == 2) {
			printf("777\n");
			//周期发送mqtt心跳
            signal(SIGALRM, send_mqtt_ping);
			set_time();

			while (1) {
				//非阻塞的等待接收数据
				recv_func();
			}

		} else {
			printf("n = %d\n", n);
			return -2;
		}
	} else {
		printf("n = %d\n", n);
		return -1;
	}

	return 0;
}
