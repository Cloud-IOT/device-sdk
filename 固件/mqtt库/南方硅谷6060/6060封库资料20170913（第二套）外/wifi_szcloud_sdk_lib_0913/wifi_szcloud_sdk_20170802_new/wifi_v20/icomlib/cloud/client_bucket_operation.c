
#include "oss_client.h"
#include "oss_auth.h"
#include "cloud_api.h"

int client_create_bucket(oss_client_t *client, const char *bucket_name, char *data)
{
    if(client == NULL || bucket_name == NULL ||data == NULL ) {
        printf("%s input para  have null, please check\n",__func__);
        return NULL;
    }
    
    unsigned int bucket_name_len = strlen(bucket_name);
    char *resource = (char *)malloc(sizeof(char)*bucket_name_len + 16);
    char header_host[256];
    char header_date[48];
    char header_auth[128];
    char now[65];
    unsigned int sign_len = 0;
    int http_len = 0;
    char cr_lf[2] = {ASCII_CR, ASCII_LF};
    char *cmd = "PUT ";
    char *http_version = " HTTP/1.1";
    char *header_accept = "Accept: */*";
    oss_map_t *default_headers = oss_map_new(16);
    
    sprintf(resource, "/%s", bucket_name);
    sprintf(header_host,"Host: %s", client->endpoint);
    strncpy(data, cmd, strlen(cmd));
    http_len += strlen(cmd);
    strncpy(data  + http_len, resource, strlen(resource));
    http_len += strlen(resource);
    strncpy(data + http_len, http_version, strlen(http_version));
    http_len += strlen(http_version);
    memcpy(data + http_len, cr_lf, sizeof(cr_lf));
    http_len += sizeof(cr_lf);
    strncpy(data +http_len, header_host, strlen(header_host));
    http_len += strlen(header_host);
    memcpy(data  + http_len, cr_lf, sizeof(cr_lf));
    http_len += sizeof(cr_lf);
    strncpy(data + http_len, header_accept, strlen(header_accept));
    http_len += strlen(header_accept);
    memcpy(data  + http_len, cr_lf, sizeof(cr_lf));
    http_len += sizeof(cr_lf);
    get_gmt_time_info(now);
    sprintf(header_date, "Date: %s", now);
    strncpy(data +http_len, header_date, strlen(header_date));
    http_len += strlen(header_date);
    memcpy(data  + http_len, cr_lf, sizeof(cr_lf));
    http_len += sizeof(cr_lf);
    oss_map_put(default_headers, OSS_DATE, now);
    
    char *sign = (char *)generate_authentication(client->access_key, OSS_HTTP_PUT,
    default_headers, NULL, resource, &sign_len);
    if(strcmp(client->endpoint, "s.jcloud.com") == 0) {
        printf("===========s.jcloud.com=============\n");
        sprintf(header_auth, "Authorization: jingdong %s:%s", client->access_id, sign);
    } else if(strcmp(client->endpoint, "oss.aliyuncs.com") == 0) {
        printf("===========oss.aliyuncs.com=============\n");
        sprintf(header_auth, "Authorization: OSS %s:%s", client->access_id, sign);
    } else {
        printf("unknown cloud name, please check\n");
    }
    strncpy(data + http_len, header_auth, strlen(header_auth));
    http_len += strlen(header_auth);
    memcpy(data  + http_len, cr_lf, sizeof(cr_lf));
    http_len += sizeof(cr_lf);
    memcpy(data  + http_len, cr_lf, sizeof(cr_lf));
    http_len += sizeof(cr_lf);
    data[http_len] = '\0';
    
    oss_map_delete(default_headers);
    
    if(sign != NULL) {
        free(sign);
        sign = NULL;
    }
    
    if(resource != NULL) {
        free(resource);
        resource = NULL;
    }
    return http_len;
}

int client_delete_bucket(oss_client_t *client, const char *bucket_name, char *data)
{
    if(client == NULL || bucket_name == NULL ||data == NULL ) {
        printf("%s input para  have null, please check\n",__func__);
        return NULL;
    }
    unsigned int bucket_name_len = strlen(bucket_name);
    char *resource = (char *)malloc(sizeof(char) *bucket_name_len + 16);
    char header_host[256]  = {0};
    char header_date[48]  = {0};
    char header_auth[128] = {0};
    char now[65];
    unsigned int sign_len = 0;
    int http_len = 0;
    char cr_lf[2] = {ASCII_CR, ASCII_LF};
    char *cmd = "DELETE ";
    char *http_version = " HTTP/1.1";
    char *header_accept = "Accept: */*";

    oss_map_t *default_headers = oss_map_new(16);
    sprintf(resource, "/%s", bucket_name);
    sprintf(header_host,"Host: %s", client->endpoint);
    strncpy(data, cmd, strlen(cmd));
    http_len += strlen(cmd);
    strncpy(data  + http_len, resource, strlen(resource));
    http_len += strlen(resource);
    strncpy(data + http_len, http_version, strlen(http_version));
    http_len += strlen(http_version);
    memcpy(data + http_len, cr_lf, sizeof(cr_lf));
    http_len += sizeof(cr_lf);
    strncpy(data +http_len, header_host, strlen(header_host));
    http_len += strlen(header_host);
    memcpy(data + http_len, cr_lf, sizeof(cr_lf));
    http_len += sizeof(cr_lf);
    strncpy(data + http_len, header_accept, strlen(header_accept));
    http_len += strlen(header_accept);
    memcpy(data + http_len, cr_lf, sizeof(cr_lf));
    http_len += sizeof(cr_lf);
    get_gmt_time_info(now);
    sprintf(header_date, "Date: %s", now);
    strncpy(data +http_len, header_date, strlen(header_date));
    http_len += strlen(header_date);
    memcpy(data + http_len, cr_lf, sizeof(cr_lf));
    http_len += sizeof(cr_lf);
    oss_map_put(default_headers, OSS_DATE, now);

    char *sign = (char *)generate_authentication(client->access_key, OSS_HTTP_DELETE,
    default_headers, NULL, resource, &sign_len);

    if(strcmp(client->endpoint, "s.jcloud.com") == 0) {
        printf("===========s.jcloud.com=============\n");
        sprintf(header_auth, "Authorization: jingdong %s:%s", client->access_id, sign);
    } else if(strcmp(client->endpoint, "oss.aliyuncs.com") == 0) {
        printf("===========oss.aliyuncs.com=============\n");
        sprintf(header_auth, "Authorization: OSS %s:%s", client->access_id, sign);
    } else {
        printf("unknown cloud name, please check\n");
    }
    strncpy(data + http_len, header_auth, strlen(header_auth));
    http_len += strlen(header_auth);
    memcpy(data + http_len, cr_lf, sizeof(cr_lf));
    http_len += sizeof(cr_lf);
    memcpy(data + http_len, cr_lf, sizeof(cr_lf));
    http_len += sizeof(cr_lf);
    data[http_len] = '\0';
    
    oss_map_delete(default_headers);

    if(sign != NULL) {
        free(sign);
        sign = NULL;
    }

    if(resource != NULL) {
        free(resource);
        resource = NULL;
    }
    return http_len;
}

