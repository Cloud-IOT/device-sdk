
#define _OSS_CONSTANTS_H
#include "oss_constants.h"
#undef _OSS_CONSTANTS_H
#include "uthash.h"
#include "oss_map.h"
#include "aliyun_base64.h"
#include "hash_alg.h"

#define CANONICALIZED_HEADERS_BUFFER_SIZE 4096
#define HMAC_SHA1_OUT_LEN 21
#define SIGNED_VALUE_LEN 65


typedef struct user_headers_s user_headers_t;

struct user_headers_s {
    char *key;
    char *value;
    UT_hash_handle hh;
};

static char canonicalized_headers[CANONICALIZED_HEADERS_BUFFER_SIZE];
static user_headers_t *oss_user_headers = NULL;

static char *key_iter = NULL;

static int oss_sort_headers_by_key(user_headers_t *a, user_headers_t *b)
{
    return strcmp(a->key, b->key);
}

static void oss_sort_headers()
{
    HASH_SORT(oss_user_headers, oss_sort_headers_by_key);
}

static void oss_add_headers(const char *key, const char *value)
{
    if(key == NULL || value == NULL) {
        printf("%s input para  have null, please check\n",__func__);
        return NULL;
    }
    
    user_headers_t *s = (user_headers_t *)malloc(sizeof(user_headers_t));
    unsigned int key_len = strlen(key);
    unsigned int value_len = strlen(value);
    
    s->key = (char *)malloc(sizeof(char) * key_len + 1);
    s->value = (char *)malloc(sizeof(char) * value_len + 1);
    
    memset(s->key, '\0', key_len + 1);
    memset(s->value, '\0', value_len + 1);
    
    strncpy(s->key, key, key_len);
    strncpy(s->value, value, value_len);
    
    HASH_ADD_INT(oss_user_headers, key, s);
}

static void oss_delete_all_headers()
{
    user_headers_t *current_user_header, *tmp;
    HASH_ITER(hh, oss_user_headers, current_user_header, tmp) {
        free(current_user_header->key);
        free(current_user_header->value);
        HASH_DEL(oss_user_headers,current_user_header);  /* delete; users advances to next */
        free(current_user_header);            /* optional- if you want to free  */
    }
}

void fill_canonicalized_headers()
{
    user_headers_t *s;
    for (s = oss_user_headers; s != NULL; s = s->hh.next) {
        unsigned int offset = 0;
        offset = sprintf(key_iter, "%s:%s\n", s->key, s->value);
        key_iter += offset;
    }
}

static void iter_user_headers(const char *key, const char *value, const void *obj)
{
    oss_add_headers(key, value);
}

char *
generate_authentication(const char *access_key, const char *method,
		oss_map_t *default_headers, oss_map_t *user_headers,
		const char *resource, unsigned int *sign_len)
{
    if(default_headers == NULL || access_key == NULL ||method == NULL   ||resource == NULL) {
        printf("%s input para  have null, please check\n",__func__);
        return NULL;
    }
    char *content_md5 = NULL;
    char *content_type = NULL;
    char *date = NULL;
    
    char *string_to_sign = (char *)malloc(sizeof(char) * CANONICALIZED_HEADERS_BUFFER_SIZE *4);
    unsigned int access_key_len = strlen(access_key);
    
    char *hmac_sha1_out = (char *)malloc(sizeof(char) * HMAC_SHA1_OUT_LEN);
    
    char *signed_value = (char *)malloc(sizeof(char) * SIGNED_VALUE_LEN);
    
    memset(string_to_sign, '\0', CANONICALIZED_HEADERS_BUFFER_SIZE * 4);
    memset(canonicalized_headers, '\0', CANONICALIZED_HEADERS_BUFFER_SIZE);
    memset(hmac_sha1_out, '\0', HMAC_SHA1_OUT_LEN);
    memset(signed_value, '\0', SIGNED_VALUE_LEN);
    
    key_iter = NULL;
    
    unsigned int result_len = oss_map_get(default_headers, OSS_CONTENT_MD5, NULL, 0);
    if (result_len != 0) {
        content_md5 = (char *)malloc(sizeof(char) * result_len);
        memset(content_md5, '\0', result_len);
        oss_map_get(default_headers, OSS_CONTENT_MD5, content_md5, result_len);
    }
    
    result_len = oss_map_get(default_headers, OSS_CONTENT_TYPE, NULL, 0);
    if (result_len != 0) {
        content_type = (char *)malloc(sizeof(char) * result_len);
        memset(content_type, '\0', result_len);
        oss_map_get(default_headers, OSS_CONTENT_TYPE, content_type, result_len);
    }
    
    result_len = oss_map_get(default_headers, OSS_DATE, NULL, 0);
    if (result_len != 0) {
        date = (char *)malloc(sizeof(char) * result_len);
        memset(date, '\0', result_len);
        oss_map_get(default_headers, OSS_DATE, date, result_len);
    }
    
    key_iter = canonicalized_headers;
    
    if (user_headers != NULL)
    oss_map_enum(user_headers, iter_user_headers, NULL);
    
    oss_sort_headers();
    
    fill_canonicalized_headers();
    key_iter = NULL;
    
    // printf("canonicalized_headers: %s", canonicalized_headers);
    
    if (content_md5 != NULL && content_type != NULL) 
        sprintf(string_to_sign, "%s\n%s\n%s\n%s\n%s%s", method,
        content_md5, content_type, date, canonicalized_headers, resource);
    else if (content_md5 == NULL && content_type != NULL)
        sprintf(string_to_sign, "%s\n\n%s\n%s\n%s%s", method, content_type,
        date, canonicalized_headers, resource);
    else if (content_md5 == NULL && content_type == NULL)
        sprintf(string_to_sign, "%s\n\n\n%s\n%s%s", method,
        date, canonicalized_headers, resource);
    
    unsigned int string_to_sign_len = strlen(string_to_sign);
    
    //hmac_sha1(access_key, access_key_len, string_to_sign, string_to_sign_len, hmac_sha1_out);
    hmac_sha1_cal(access_key,access_key_len, string_to_sign, string_to_sign_len, hmac_sha1_out);
    base64_encode(hmac_sha1_out, HMAC_SHA1_OUT_LEN - 1, signed_value, SIGNED_VALUE_LEN);
    *sign_len = strlen(signed_value);
    
    /* *
    * clean up.
    * */
    free(string_to_sign);
    free(hmac_sha1_out);
    oss_delete_all_headers();
    if (content_md5 != NULL) free(content_md5);
    if (content_type != NULL) free(content_type);
    if (date != NULL) free(date);
    
    return signed_value;
}
