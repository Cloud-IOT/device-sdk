
#include <stdio.h>
#include "cloud_api.h"
#include "oss_client.h"
#include "uip.h"
#include "socket_driver.h"
#include "net/uip_arp.h"
#include "net/resolv.h"
#include "sys_status_api.h"
#include "time.h"

PROCESS(Cloud_connect_process, "aliyunCloud Connect Process");
PROCESS(Cloud_nslookup_process, "aliyunCloud nslookup Process");

static int gclientAliyun = -1;

#if 1
static const char *access_id = "l57duxj0fWyWoF82";
static const char *access_key = "VhHcaHYtss1mPylreWczeck5gGa5MG";
static const char *hostname = "oss.aliyuncs.com";
#else
static const char *access_id = "MobcDzadpmlRgcdd";
static const char *access_key = "nClZVneKvdJXiBE2vsh5hc1viTZfynCKBHd1mhxv";
static const char *hostname = "s.jcloud.com";
#endif
static const char *content_length = "Content-Length: ";

static http_response_header http_rsp;
static cloud_request cloud_req;

int printf(const char *fmt, ...);

void free_cloud_buf()
{
    if(cloud_req.send_buf != NULL) {
        free(cloud_req.send_buf);
        cloud_req.send_buf = NULL;
        //printf("alloc cloud send buf free\n");
    }
    
    if(cloud_req.rece_buf != NULL) {
        free(cloud_req.rece_buf);
        cloud_req.rece_buf = NULL;
        //printf("alloc cloud rece buf free\n");
    }
    
    if(cloud_req.tmp_buf != NULL) {
        free(cloud_req.tmp_buf);
        cloud_req.rece_buf = NULL;
        //printf("alloc cloud tmp buf free\n");
    }

    if(cloud_req.update_buf != NULL) {
        free(cloud_req.update_buf);
        cloud_req.update_buf = NULL;
        //printf("alloc cloud update buf free\n");
    }
}

U8 alloc_cloud_buf()
{
    if(cloud_req.send_buf == NULL) {
        cloud_req.send_buf = (char *)malloc(MAX_CLOUD_BUFFER);
        memset(cloud_req.send_buf, 0, MAX_CLOUD_BUFFER);
        //printf("alloc cloud send buf ready\n");
    }
    
    if(cloud_req.rece_buf == NULL) {
        cloud_req.rece_buf = (char *)malloc(MAX_CLOUD_BUFFER);
        memset(cloud_req.rece_buf, 0, MAX_CLOUD_BUFFER);
        //printf("alloc cloud rece buf ready\n");
    }
    
    if(cloud_req.tmp_buf == NULL) {
        cloud_req.tmp_buf = (char *)malloc(MAX_CLOUD_BUFFER);
        memset(cloud_req.tmp_buf, 0, MAX_CLOUD_BUFFER);
        //printf("alloc cloud tmp buf ready\n");
    }

    if(cloud_req.update_buf == NULL) {
        cloud_req.update_buf = (char *)malloc(MAX_CLOUD_BUFFER);
        memset(cloud_req.update_buf, 0, MAX_CLOUD_BUFFER);
        //printf("alloc cloud update buf ready\n");
    }
    
    if(cloud_req.send_buf == NULL || cloud_req.rece_buf == NULL 
        || cloud_req.tmp_buf == NULL || cloud_req.update_buf== NULL) {
        printf("cloud alloc fail: send_buf: %x rece_buf: %x tmp_buf: %x update_buf: %x\n", 
                                    cloud_req.send_buf, cloud_req.rece_buf, 
                                    cloud_req.tmp_buf, cloud_req.update_buf);
        free_cloud_buf();
        return -1;
    }
    return 0;
}

void start_cloud_socket()
{
    process_start(&Cloud_nslookup_process, NULL);
    process_start(&Cloud_connect_process, NULL);
    set_nslookup(hostname);
}

U8 get_gmt_time_info(char *data)
{
    const char * weekday[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
    const char * month[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    unsigned int rawtime;
    struct aliyuntm tm;
    rawtime = get_ntp_time();
    aliyun_time(&rawtime, &tm);

    sprintf(data, "%s, %02d %s %d %02d:%02d:%02d GMT",weekday[tm.tm_wday], tm.tm.tm_mday, month[tm.tm.tm_mon -1],tm.tm.tm_year,tm.tm_hour,tm.tm_min,tm.tm_sec);
    
    return strlen(data);
}

void dump_aliyun_down_data()
{
    int i = 0;
    printf("===dump_aliyun_down_data===\n");
    for(i = 1; i <= http_rsp.content_len; i++) {
        printf("%02x ", cloud_req.tmp_buf[i -1]);
        if(i % 16 == 0)
            printf("\n");
    }
    printf("\n");
}

U8 get_content_buf(char *data, int len)
{
    int i = 0, j = 0;
    for(i = 1; i < (len -3); i++) {
        if((*(data + i) == ASCII_CR) && (*(data + i + 1) == ASCII_LF)
            && (*(data + i + 2) == ASCII_CR) && (*(data + i + 3) == ASCII_LF)) {
            data += (i + ASCII_CR_LEN * 2 + ASCII_LF_LEN * 2);
            len-= (i + ASCII_CR_LEN * 2 + ASCII_LF_LEN * 2);
            memcpy(cloud_req.tmp_buf, data, len);
            cloud_req.tmp_offset += len;
            return len;
        }
    }
    return 0;
}

U8 get_http_content_len(char *data, int len, int *offset_len)
{
    int i = 0, total_len = len, j = 0, content_len = 0;

    for(i = 1; i < (total_len -1); i++) {
        if((*(data + i) == ASCII_CR) && (*(data + i + 1) == ASCII_LF)) {
            if(i > strlen(content_length)) {
                content_len = i -strlen(content_length);
                if(memcmp(content_length,data, content_len) == 0) {
                    *offset_len = (len - total_len + strlen(content_length));
                    return content_len;
                }
                total_len -= i +ASCII_CR_LEN+ASCII_LF_LEN;
                data +=  i +ASCII_CR_LEN+ASCII_LF_LEN;
                i = 1;
            }
        }
    }
    return 0;
}

U8 get_http_reason_len(char *data, int len)
{
    int i = 0;
    for(i = 1; i < (len -1); i++) {
        if((*(data + i) == ASCII_CR) && (*(data + i + 1) == ASCII_LF))
            return i;
    }
    return 0;
}

void parser_http_data(char *data, int len)
{
    int reason_len = 0;
    int content_len = 0;
    int offset_len = 0;
    char *offset = data;

    strncpy(http_rsp.version, data, HTTP_VERSION_ELN);
    http_rsp.version[HTTP_VERSION_ELN] = '\0';
    data += HTTP_VERSION_ELN + ASCII_SPACE_LEN;
    len -= HTTP_VERSION_ELN + ASCII_SPACE_LEN;
    strncpy(http_rsp.status, data, HTTP_STATUS_ELN);
    http_rsp.status[HTTP_STATUS_ELN] = '\0';
    data += HTTP_STATUS_ELN + ASCII_SPACE_LEN;
    len -= HTTP_STATUS_ELN + ASCII_SPACE_LEN;
    reason_len = get_http_reason_len(data, len);
    if(reason_len > 0) {
        http_rsp.reason= (char *)malloc(reason_len + 1);
        strncpy(http_rsp.reason, data, reason_len);
        http_rsp.reason[reason_len] = '\0';
    } else {
        printf("http response get reason fail\n");
        return;
    }
    data += reason_len + ASCII_CR_LEN + ASCII_LF_LEN;
    len -= reason_len + ASCII_CR_LEN + ASCII_LF_LEN;
    content_len = get_http_content_len(data, len, &offset_len);
     if(content_len > 0) {
        data += offset_len;
        len -= offset_len;
        char content_num[content_len + 1];
        memcpy(content_num, data, content_len);
        content_num[content_len] = '\0';
        //printf("content_len: %d content_num:%s\n", content_len, content_num);
        http_rsp.content_len = strtoul(content_num, NULL, 10);
        get_content_buf(data, len);
    } else {
        //printf("http response get content length fail\n");
        return;
    }
}

void object_download(char *buc_name, int buc_len, char *obj_name, int obj_len)
{
    if(cloud_req.cmd != NO_ACTION) {
        printf("please wait http response, then you can send next http request\n");
        return;
    }
    
    if((buc_len < MIN_BUCKET_LEN) || (buc_len > MAX_BUCKET_LEN) ||
        (obj_len < MIN_OBJECT_LEN) || (obj_len > MAX_OBJECT_LEN) ) {
        printf("Aliyun cloud can not set %d len name\n", buc_len);
        return;
    }
    char bucket_name[MAX_BUCKET_LEN];
    char object_name[MAX_OBJECT_LEN];
    strncpy(bucket_name, buc_name, buc_len);
    bucket_name[buc_len] = '\0';
    strncpy(object_name, obj_name, obj_len);
    object_name[obj_len] = '\0';
    if(alloc_cloud_buf() == 0) {
        cloud_req.cmd = OBJECT_DOWNLOAD;
        printf("== bucket_name:%s object_name:%s ==\n",bucket_name, object_name);
        oss_client_t *client = client_initialize_with_endpoint(access_id, access_key, hostname);
        oss_get_object_request_t *request = get_object_request_initialize(bucket_name, object_name);
        client_get_object_to_buffer_2nd(client, request, cloud_req.send_buf);
        get_object_request_finalize(request);
        client_finalize(client);
        start_cloud_socket();
    }
}

void object_update(char *buc_name, int buc_len, char *obj_name, int obj_len, char *buf, int buf_len)
{
    if(cloud_req.cmd != NO_ACTION) {
        printf("please wait http response, then you can send next http request\n");
        return;
    }
    
    if((buc_len < MIN_BUCKET_LEN) || (buc_len > MAX_BUCKET_LEN) ||
        (obj_len < MIN_OBJECT_LEN) || (obj_len > MAX_OBJECT_LEN) ) {
        printf("Aliyun cloud can not set %d len name\n", buc_len);
        return;
    }
    char bucket_name[MAX_BUCKET_LEN];
    char object_name[MAX_BUCKET_LEN];
    strncpy(bucket_name, buc_name, buc_len);
    bucket_name[buc_len] = '\0';
    strncpy(object_name, obj_name, obj_len);
    object_name[obj_len] = '\0';
    if(alloc_cloud_buf() == 0) {
        cloud_req.cmd = OBJECT_UPDATE;
        printf("== bucket_name:%s object_name:%s ==\n",bucket_name, object_name);
        memcpy(cloud_req.update_buf, buf, buf_len);
        cloud_req.update_len = buf_len;
        oss_client_t *client = client_initialize_with_endpoint(access_id, access_key, hostname);
        oss_object_metadata_t *metadata = object_metadata_initialize(); 
        metadata->set_content_length(metadata, buf_len);
        metadata->set_content_type(metadata, "application/octet-stream");
        metadata->set_cache_control(metadata, "no-cache");
        metadata->set_content_encoding(metadata, "utf-8");
        metadata->set_content_disposition(metadata, "attachment;");
        metadata->set_expiration_time(metadata, "Thu, 13 Sep 2012 21:08:42 GMT");
        oss_put_object_result_t *result =
        	client_put_object_from_buffer(client, bucket_name, object_name, cloud_req.update_len, metadata, cloud_req.send_buf);
        put_object_result_finalize(result);
        client_finalize(client);
        object_metadata_finalize(metadata);
        start_cloud_socket();
    }
}

void delete_bucket(char *name, int len)
{
    if(cloud_req.cmd != NO_ACTION) {
        printf("please wait http response, then you can send next http request\n");
        return;
    }
    
    if((len < MIN_BUCKET_LEN) || (len > MAX_BUCKET_LEN)) {
        printf("Aliyun cloud can not set %d len name\n", len);
        return;
    }
    char bucket_name[MAX_BUCKET_LEN];
    strncpy(bucket_name, name, len);
    bucket_name[len] = '\0';
    if(alloc_cloud_buf() == 0) {
        cloud_req.cmd = DELETE_BUCKET;
        printf("== bucket_name:%s ==\n",bucket_name);
        oss_client_t *client = client_initialize_with_endpoint(access_id, access_key, hostname);
        client_delete_bucket(client, bucket_name, cloud_req.send_buf);
        client_finalize(client);
        start_cloud_socket();
    }
}

void create_bucket(char *name, int len)
{
    if(cloud_req.cmd != NO_ACTION) {
        printf("please wait http response, then you can send next http request\n");
        return;
    }
    
    if((len < MIN_BUCKET_LEN) || (len > MAX_BUCKET_LEN)) {
        printf("Aliyun cloud can not set %d len name\n", len);
        return;
    }
    char bucket_name[MAX_BUCKET_LEN];
    strncpy(bucket_name, name, len);
    bucket_name[len] = '\0';
    if(alloc_cloud_buf() == 0) {
        cloud_req.cmd = CREATE_BUCKET;
        printf("== bucket_name:%s ==\n",bucket_name);
        oss_client_t *client = client_initialize_with_endpoint(access_id, access_key, hostname);
        client_create_bucket(client, bucket_name, cloud_req.send_buf);
        client_finalize(client);
        start_cloud_socket();
    }
}

PROCESS_THREAD(Cloud_nslookup_process, ev, data)
{
    PROCESS_BEGIN();
    PROCESS_WAIT_EVENT_UNTIL(ev == resolv_event_found);
    {
        uip_ipaddr_t *addrptr;
        uip_ip4addr_t	server_ip_addr;
        addrptr = &server_ip_addr;

        char *pHostName = (char*)data;
        if(ev==resolv_event_found)
        {
            resolv_lookup(pHostName, &addrptr);
            uip_ipaddr_copy(&server_ip_addr, addrptr);
            printf("dns parse:%d.%d.%d.%d\n", addrptr->u8[0] , addrptr->u8[1] , addrptr->u8[2] , addrptr->u8[3] );
            gclientAliyun = tcpconnect( &server_ip_addr , 80, &Cloud_connect_process);
        }
    }
    PROCESS_END();
}

PROCESS_THREAD(Cloud_connect_process, ev, data)
{
    PROCESS_BEGIN();
    SOCKETMSG msg;
    int recvlen;
    uip_ipaddr_t peeraddr;
    U16 peerport;

    while(1) {
        PROCESS_WAIT_EVENT();

        if( ev == PROCESS_EVENT_EXIT) {
            break;
        } else if(ev == PROCESS_EVENT_MSG) {
            msg = *(SOCKETMSG *)data;
            //The TCP socket is connected, This socket can send data after now.
            if(msg.status == SOCKET_CONNECTED) {
                if(msg.socket == gclientAliyun) {
                    tcpsend(gclientAliyun, cloud_req.send_buf, strlen (cloud_req.send_buf));
                }
            //TCP connection is closed. Clear the socket number.
            } else if(msg.status == SOCKET_CLOSED) {
                if(gclientAliyun == msg.socket) {
                    printf("=====close socket=====\n");
                    //free_cloud_buf();
                    cloud_req.cmd = NO_ACTION;
                    gclientAliyun = -1;
                }
            //Get ack, the data trasmission is done. We can send data after now.
            } else if(msg.status == SOCKET_SENDACK) {
            //printf("socked:%d send data ack\n", msg.socket);
            //There is new data coming. Receive data now.
            } else if(msg.status == SOCKET_NEWDATA) {
                if(0 <= msg.socket && msg.socket < UIP_CONNS) {
                    recvlen = tcprecv(msg.socket, cloud_req.rece_buf, MAX_CLOUD_BUFFER);
                    cloud_req.rece_buf[recvlen] = 0;
                    if(cloud_req.cmd == OBJECT_DOWNLOAD && http_rsp.content_len > 0) {
                        memcpy(cloud_req.tmp_buf +cloud_req.tmp_offset, cloud_req.rece_buf, recvlen);
                        cloud_req.tmp_offset += recvlen;
                        if(cloud_req.tmp_offset >= http_rsp.content_len) {
                            tcpclose(gclientAliyun);
                            dump_aliyun_down_data();
                            http_rsp.content_len = cloud_req.tmp_offset = 0;
                            tcpclose(gclientAliyun);
                        }
                    } else {
                        parser_http_data(cloud_req.rece_buf, recvlen);
                        printf("http_rsp.version: %s\n", http_rsp.version);
                        printf("http_rsp.status: %s\n", http_rsp.status);
                        printf("http_rsp.reason: %s\n", http_rsp.reason);
                        free(http_rsp.reason);
                        if(cloud_req.tmp_offset >= http_rsp.content_len && (cloud_req.cmd == OBJECT_DOWNLOAD)) {
                            tcpclose(gclientAliyun);
                            dump_aliyun_down_data();
                            http_rsp.content_len = cloud_req.tmp_offset = 0;
                            cloud_req.cmd = NO_ACTION;
                        }

                        if(strcmp(http_rsp.status, "100") == 0) {
                            if(cloud_req.cmd == OBJECT_UPDATE) {
                                tcpsend(gclientAliyun, cloud_req.update_buf, cloud_req.update_len);
                            }
                        } else {                            
                            if(cloud_req.cmd == CREATE_BUCKET ||cloud_req.cmd == DELETE_BUCKET  ||cloud_req.cmd == OBJECT_UPDATE) {
                                tcpclose(gclientAliyun);
                            }
                        }
                    }
                } else {
                	printf("Illegal socket:%d\n", msg.socket);
                }
            } else {
                printf("unknow message type\n");
            }
        }	
    }	

    PROCESS_END();
}
