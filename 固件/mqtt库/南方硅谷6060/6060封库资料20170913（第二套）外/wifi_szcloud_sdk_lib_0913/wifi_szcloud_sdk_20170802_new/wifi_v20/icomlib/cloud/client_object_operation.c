
#include "oss_client.h"
#include "oss_auth.h"
#include "oss_put_object_result.h"
#include "cloud_api.h"

oss_put_object_result_t *
client_put_object_from_buffer(oss_client_t *client,
		const char *bucket_name,
		const char *key,
		int tmp_len,
		oss_object_metadata_t *metadata, char *data)
{
    if(client == NULL || bucket_name == NULL ||key == NULL ||metadata == NULL) {
        printf("%s input para  have null, please check\n",__func__);
        return NULL;
    }
    unsigned int bucket_name_len = strlen(bucket_name);
    unsigned int key_len = strlen(key);
    char *resource = (char *)malloc(sizeof(char) * 
    (bucket_name_len + key_len + 16));
    char content_len[256];
    unsigned int sign_len = 0;
    int http_len = 0;
    char cr_lf[2] = {ASCII_CR, ASCII_LF};
    char now[65];
    char header_host[258]  = {0};
    char header_date[48]  = {0};
    char header_auth[128] = {0};
    char header_content_type[64] = {0};
    char header_cache_control[48] = {0};
    char header_expires[64] = {0};
    char header_content_encoding[64] = {0};
    char header_content_disposition[256] = {0};
    char *cmd = "PUT ";
    char *http_version = " HTTP/1.1";
    char *header_accept = "Accept: */*";
    char *content_type = "Content-Type: application/octet-stream";
    char *cache_control = "Cache-Control: no-cache";
    char *content_encoding = "Content-Encoding: utf-8";
    char *content_disposition = "Content-Disposition: attachment;";
    char *expiration_time = "Expires: Thu, 13 Sep 2012 21:08:42 GMT";
    char*expect = "Expect: 100-continue";

    oss_map_t *default_headers = oss_map_new(16);
    get_gmt_time_info(now);

    sprintf(resource, "/%s/%s", bucket_name, key);
    sprintf(header_host, "Host: %s", client->endpoint);
    sprintf(header_date, "Date: %s", now);
    strncpy(data, cmd, strlen(cmd));
    http_len += strlen(cmd);
    strncpy(data  + http_len, resource, strlen(resource));
    http_len += strlen(resource);
    strncpy(data + http_len, http_version, strlen(http_version));
    http_len += strlen(http_version);
    memcpy(data  + http_len, cr_lf, sizeof(cr_lf));
    http_len += sizeof(cr_lf);
    strncpy(data +http_len, header_host, strlen(header_host));
    http_len += strlen(header_host);
    memcpy(data  + http_len, cr_lf, sizeof(cr_lf));
    http_len += sizeof(cr_lf);
    strncpy(data + http_len, header_accept, strlen(header_accept));
    http_len += strlen(header_accept);
    memcpy(data  + http_len, cr_lf, sizeof(cr_lf));
    http_len += sizeof(cr_lf);
    /* Content-Type is a must */
    strncpy(data + http_len, cache_control, strlen(cache_control));
    http_len += strlen(cache_control);
    memcpy(data  + http_len, cr_lf, sizeof(cr_lf));
    http_len += sizeof(cr_lf);
    strncpy(data + http_len, expiration_time, strlen(expiration_time));
    http_len += strlen(expiration_time);
    memcpy(data  + http_len, cr_lf, sizeof(cr_lf));
    http_len += sizeof(cr_lf);
    strncpy(data + http_len, content_encoding, strlen(content_encoding));
    http_len += strlen(content_encoding);
    memcpy(data  + http_len, cr_lf, sizeof(cr_lf));
    http_len += sizeof(cr_lf);
    strncpy(data + http_len, content_disposition, strlen(content_disposition));
    http_len += strlen(content_disposition);
    memcpy(data  + http_len, cr_lf, sizeof(cr_lf));
    http_len += sizeof(cr_lf);
    strncpy(data + http_len, content_type, strlen(content_type));
    http_len += strlen(content_type);
    memcpy(data  + http_len, cr_lf, sizeof(cr_lf));
    http_len += sizeof(cr_lf);
    strncpy(data + http_len, header_date, strlen(header_date));
    http_len += strlen(header_date);
    memcpy(data  + http_len, cr_lf, sizeof(cr_lf));
    http_len += sizeof(cr_lf);
    sprintf(header_content_type, "Content-Type: %s",
    metadata->get_content_type(metadata));

    oss_map_put(default_headers, OSS_DATE, now);
    oss_map_put(default_headers, OSS_CONTENT_TYPE, metadata->get_content_type(metadata));

    oss_map_t *user_headers = metadata->get_user_metadata(metadata);

    char *sign = (char *)generate_authentication(client->access_key, OSS_HTTP_PUT,
    default_headers, user_headers, resource, &sign_len);

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
    sprintf(content_len, "Content-Length: %d",tmp_len);
    strncpy(data + http_len, content_len, strlen(content_len));
    http_len += strlen(content_len);
    memcpy(data  + http_len, cr_lf, sizeof(cr_lf));
    http_len += sizeof(cr_lf);
    strncpy(data + http_len, expect, strlen(expect));
    http_len += strlen(expect);
    memcpy(data  + http_len, cr_lf, sizeof(cr_lf));
    http_len += sizeof(cr_lf);
    memcpy(data  + http_len, cr_lf, sizeof(cr_lf));
    http_len += sizeof(cr_lf);
    
    oss_map_delete(default_headers);
    
    if(sign != NULL) {
        free(sign);
        sign = NULL;
    }

    if(resource != NULL) {
        free(resource);
        resource = NULL;
    }

    return NULL;
}

int
client_get_object_to_buffer_2nd(oss_client_t *client,
		oss_get_object_request_t *request, char *data)
{

    if(client == NULL || request == NULL ||data == NULL ) {
        printf("%s input para  have null, please check\n",__func__);
        return NULL;
    }
    unsigned int bucket_name_len = strlen(request->get_bucket_name(request));
    unsigned int key_len = strlen(request->get_key(request));
    unsigned int sign_len = 0;
    long start = 0;
    long length = 0;
    /** 
    * Resource: "/" + bucket_name + "/" + key
    */
    char *resource = (char *)malloc(sizeof(char) * (bucket_name_len + key_len + 16));
    char header_host[256]  = {0};
    char header_date[48]  = {0};
    char now[65];
    char header_auth[128] = {0};
    char header_if_modified_since[64] = {0};
    char header_if_unmodified_since[64] = {0};
    char header_range[32] = {0};	
    int http_len = 0;
    char cr_lf[2] = {ASCII_CR, ASCII_LF};
    char *cmd = "GET ";
    char *http_version = " HTTP/1.1";
    char *header_accept = "Accept: */*";
    
    oss_map_t *default_headers = oss_map_new(16);
    sprintf(resource, "/%s/%s", request->get_bucket_name(request),
    request->get_key(request));
    sprintf(header_host, "Host: %s", client->endpoint);
    get_gmt_time_info(now);
    sprintf(header_date, "Date: %s", now);
    strncpy(data, cmd, strlen(cmd));
    http_len += strlen(cmd);
    strncpy(data  + http_len, resource, strlen(resource));
    http_len += strlen(resource);
    strncpy(data + http_len, http_version, strlen(http_version));
    http_len += strlen(http_version);
    memcpy(data  + http_len, cr_lf, sizeof(cr_lf));
    http_len += sizeof(cr_lf);
    strncpy(data +http_len, header_host, strlen(header_host));
    http_len += strlen(header_host);
    memcpy(data  + http_len, cr_lf, sizeof(cr_lf));
    http_len += sizeof(cr_lf);
    strncpy(data + http_len, header_accept, strlen(header_accept));
    http_len += strlen(header_accept);
    memcpy(data  + http_len, cr_lf, sizeof(cr_lf));
    http_len += sizeof(cr_lf);
    strncpy(data +http_len, header_date, strlen(header_date));
    http_len += strlen(header_date);
    memcpy(data  + http_len, cr_lf, sizeof(cr_lf));
    http_len += sizeof(cr_lf);
    oss_map_put(default_headers, OSS_DATE, now);

    char *sign = (char *)generate_authentication(client->access_key, OSS_HTTP_GET,
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
    strncpy(data +http_len, header_auth, strlen(header_auth));
    http_len += strlen(header_auth);
    memcpy(data  + http_len, cr_lf, sizeof(cr_lf));
    http_len += sizeof(cr_lf);
    memcpy(data  + http_len, cr_lf, sizeof(cr_lf));
    http_len += sizeof(cr_lf);
    
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

