
#include "ssv_types.h"

#ifndef CLOUD_API_H
#define CLOUD_API_H

#define MIN_BUCKET_LEN 3
#define MIN_OBJECT_LEN 3
#define MAX_BUCKET_LEN 63
#define MAX_OBJECT_LEN 63
#define HTTP_VERSION_ELN 8
#define HTTP_STATUS_ELN 3
#define ASCII_CR 0XD
#define ASCII_LF 0XA
#define ASCII_SPACE 0X20
#define ASCII_CR_LEN 1
#define ASCII_LF_LEN 1
#define ASCII_SPACE_LEN 1
#define MAX_CLOUD_BUFFER (1024)

typedef enum 
{
    NO_ACTION = 0,
    CREATE_BUCKET,
    DELETE_BUCKET,
    OBJECT_UPDATE,
    OBJECT_DOWNLOAD,
} ALIYUN_CMD;

typedef struct
{
    char version[HTTP_VERSION_ELN + 1];
    char status[HTTP_STATUS_ELN + 1];
    char *reason;
    short content_len;
} http_response_header;

typedef struct
{
    U8 cmd;
    U16 tmp_offset;
    U16 tmp_len;
    U16 update_len;
    char *send_buf;
    char *rece_buf;
    char *tmp_buf;
    char *update_buf;
} cloud_request;

U8 get_gmt_time_info(char *data);
void object_update(char *buc_name, int buc_len, char *obj_name, int obj_len, char *buf, int buf_len);
void object_download(char *buc_name, int buc_len, char *obj_name, int obj_len);
void create_bucket(char *name, int len);
void delete_bucket(char *name, int len);

#endif
