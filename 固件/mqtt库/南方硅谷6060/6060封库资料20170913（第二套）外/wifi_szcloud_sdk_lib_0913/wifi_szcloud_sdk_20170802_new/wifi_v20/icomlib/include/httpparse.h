#ifndef __HTTPPARSE_H__
#define __HTTPPARSE_H__

#include "socket_driver.h"

#ifdef __TUYA_CLOUD_SUPPORT__
#define HTTPCMD_MAX (512 + 512) /*256*/
#else
#define HTTPCMD_MAX (512) /*256*/
#endif

#ifdef __WIFI_WEATHER__
#define HTTPRSP_MAX (512 + 512 + 2048 + 512)
#elif defined(__TUYA_CLOUD_SUPPORT__)
#define HTTPRSP_MAX (512 + 512 + 512)			// SHOULD be 1.5k by qinjiangwei 2017/6/22
#else
#define HTTPRSP_MAX (512)
#endif

enum {
    HTTPREQ_SUCC        = 0,
    HTTPREQ_STILL_RUN,
    HTTPREQ_CMD_ERROR,
    HTTPREQ_CONN_ERROR,
    HTTPREQ_RSP_DATA,
    HTTPREQ_RSP_TIMEOUT,
    HTTPREQ_RSP_ERROR,
};

enum
{
    HTTP_IDLE        = 0,
    HTTP_CONNECTING,
    HTTP_SENDDATA,
    HTTP_WAITRESPONSE,
};

typedef struct t_HTTPMSG
{
    U8 msgtype;
    U8 *rsp;
    U16 rsplen;
}HTTPMSG;

typedef struct t_HTTP_REQ
{
    NETSOCKET httpsock;
    U8 httpstatus;
    void (*callbackfn)(void *);
    U8 hostname[16 + 128];			// qinjiangwei 2017/1/13
    uip_ip4addr_t ip_addr;
    U16 port;
    U8 httpcmd[HTTPCMD_MAX];
    U16 cmdlen;
    U8 httprsp[HTTPRSP_MAX];
    U16 rsplen;
} HTTP_REQ;

extern HTTP_REQ httpreqdata;

void httpparse_init();
void httpparse_deinit();
int httprequest_send(U8 *httpcmd, U16 cmdlen, void (*fn)(void *));
int http_send_cmd(const char *hostname, int port, const char *req_str, void (*fn)(void *));
//void httprsp_parse(U8 *httprsp, U16 rsplen);

#endif /* __HTTPPARSE_H__ */
