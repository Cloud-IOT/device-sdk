#ifndef __SSL_API_H__
#define __SSL_API_H__

#include "matrixsslApi.h"
#include "tcpip.h"

typedef enum t_SSLSTATE{
	SSL_CONNECTED,		//SSL connection is connected.
	SSL_CLOSED,			//SSL connection is closed.
	SSL_SENDACK,        //SSL connection get TCP ack.
	SSL_NEWDATA,		//There is new data coming.
}SSLSTATE;

typedef struct t_SSLMSG
{
	ssl_t 	*sslctx;
	SSLSTATE status;
	int     recvlen;
}SSLMSG;

//Show SSL connection information
void SSL_PrintHSDetails(ssl_t *sslctx);

//Init ssl library
int SSL_init(void);

//put socket into ssl context
void SSL_setsocket(ssl_t *sslctx, NETSOCKET socket);

//Do the SSL handshake
ssl_t *SSL_connect(struct process *callbackproc, sslKeys_t *keys, uip_ipaddr_t peeraddr, U16 port);

//Send data into https connection
int SSL_write(ssl_t *sslctx, U8 *data, int datalen);

//load cert or private key or root CAs
sslKeys_t *SSL_loadkeysbybuf(char *cert, int certlen, char *key, int keylen, char *rootCA, int rootCAlen);
sslKeys_t *SSL_loadkeys(char *certname, char *keyname, char *rootCAname);

//free keys.
int SSL_freekeys(sslKeys_t	*keys);

//Read data from https connection
int SSL_read(ssl_t *sslctx, char *buf, int buflen);

//Close the SSL connection & free SSL context memory
void SSL_disconnect(ssl_t *sslctx);

//Deinit ssl library
void SSL_deinit(void);

#endif
