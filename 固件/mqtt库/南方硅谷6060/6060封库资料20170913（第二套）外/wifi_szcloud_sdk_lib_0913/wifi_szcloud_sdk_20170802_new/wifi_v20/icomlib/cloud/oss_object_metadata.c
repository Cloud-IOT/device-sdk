
#include <stdio.h>

#define _OSS_OBJECT_METADATA_H
#include "oss_object_metadata.h"
#undef _OSS_OBJECT_METADATA_H

static inline void 
_object_metadata_add_user_metadata(oss_object_metadata_t *metadata,
		const char *key,
		const char *value)
{
    oss_map_t *oss_map = metadata->user_metadata;
    oss_map_put(oss_map, key, value);
}

static inline void 
_object_metadata_add_default_metadata(oss_object_metadata_t *metadata,
		const char *key,
		const char *value)
{
    oss_map_t *oss_map = metadata->metadata;
    oss_map_put(oss_map, key, value);
}

static inline const char *
_object_metadata_get_cache_control(oss_object_metadata_t *metadata)
{
    char *buf;
    int result;
    oss_map_t *oss_map = metadata->metadata;

    result = oss_map_get(oss_map, OSS_CACHE_CONTROL, NULL, 0);
    if (result == 0)
        return NULL;

    buf = (char *)malloc(sizeof(char) * result);
    memset(buf, '\0', result);
    result = oss_map_get(oss_map, OSS_CACHE_CONTROL, buf, result);

    if (result == 0)
        return NULL;

    return buf;
}

static inline const char *
_object_metadata_get_content_disposition(oss_object_metadata_t *metadata)
{
    char *buf;
    int result;
    oss_map_t *oss_map = metadata->metadata;

    result = oss_map_get(oss_map, OSS_CONTENT_DISPOSITION, NULL, 0);
    if (result == 0)
        return NULL;
    buf = (char *)malloc(sizeof(char) * result);
    memset(buf, '\0', result);
    result = oss_map_get(oss_map, OSS_CONTENT_DISPOSITION, buf, result);
    if (result == 0)
        return NULL;
    return buf;
}

static inline const char *
_object_metadata_get_content_encoding(oss_object_metadata_t *metadata)
{
    char *buf;
    int result;
    oss_map_t *oss_map = metadata->metadata;

    result = oss_map_get(oss_map, OSS_CONTENT_ENCOING, NULL, 0);
    if (result == 0)
        return NULL;

    buf = (char *)malloc(sizeof(char) * result);
    memset(buf, '\0', result);
    result = oss_map_get(oss_map, OSS_CONTENT_ENCOING, buf, result);

    if (result == 0)
        return NULL;
    return buf;
}

static inline long
_object_metadata_get_content_length(oss_object_metadata_t *metadata)
{
    char *buf;
    int result;
    int len;
    oss_map_t *oss_map = metadata->metadata;

    result = oss_map_get(oss_map, OSS_CONTENT_LENGTH, NULL, 0);
    if (result == 0)
        return 0;
    buf = (char *)malloc(sizeof(char) * result);
    memset(buf, '\0', result);
    result = oss_map_get(oss_map, OSS_CONTENT_LENGTH, buf, result);

    if (result == 0)
        return 0;

    len = atoi(buf);
    free(buf);
    return len;
}

static inline const char *
_object_metadata_get_content_type(oss_object_metadata_t *metadata)
{
    char *buf;
    int result;
    oss_map_t *oss_map = metadata->metadata;

    result = oss_map_get(oss_map, OSS_CONTENT_TYPE, NULL, 0);
    if (result == 0)
        return NULL;
    buf = (char *)malloc(sizeof(char) * result);
    memset(buf, '\0', result);
    result = oss_map_get(oss_map, OSS_CONTENT_TYPE, buf, result);
    if (result == 0)
        return NULL;
    return buf;
}

static inline const char *
_object_metadata_get_etag(oss_object_metadata_t *metadata)
{
    char *buf;
    int result;
    oss_map_t *oss_map = metadata->metadata;

    result = oss_map_get(oss_map, OSS_CONTENT_MD5, NULL, 0);
    if (result == 0)
        return NULL;
    buf = (char *)malloc(sizeof(char) * result);
    memset(buf, '\0', result);
    result = oss_map_get(oss_map, OSS_CONTENT_MD5, buf, result);
    if (result == 0)
        return NULL;
    return buf;
}

static inline const char *
_object_metadata_get_expiration_time(oss_object_metadata_t *metadata)
{
    char *buf;
    int result;
    oss_map_t *oss_map = metadata->metadata;

    result = oss_map_get(oss_map, OSS_EXPIRES, NULL, 0);
    if (result == 0)
        return NULL;

    buf = (char *)malloc(sizeof(char) * result);
    memset(buf, '\0', result);
    result = oss_map_get(oss_map, OSS_EXPIRES, buf, result);

    if (result == 0)
        return NULL;
    return buf;
}

static inline const char *
_object_metadata_get_last_modified(oss_object_metadata_t *metadata)
{
    char *buf;
    int result;
    oss_map_t *oss_map = metadata->metadata;

    result = oss_map_get(oss_map, OSS_LAST_MODIFIED, NULL, 0);
    if (result == 0)
        return NULL;
    buf = (char *)malloc(sizeof(char) * result);
    memset(buf, '\0', result);
    result = oss_map_get(oss_map, OSS_LAST_MODIFIED, buf, result);
    if (result == 0)
        return NULL;
    return buf;
}

static inline oss_map_t *
_object_metadata_get_raw_metadata(oss_object_metadata_t *metadata)
{
    oss_map_t *oss_map = metadata->metadata;
    return oss_map;
}

static inline oss_map_t *
_object_metadata_get_user_metadata(oss_object_metadata_t *metadata)
{
    oss_map_t *oss_map = metadata->user_metadata;
    return oss_map;
}

static inline void
_object_metadata_set_cache_control(oss_object_metadata_t *metadata, 
		const char *cache_control)
{
    oss_map_t *oss_map = metadata->metadata;
    oss_map_put(oss_map, OSS_CACHE_CONTROL, cache_control);
}

static inline void
_object_metadata_set_content_disposition(oss_object_metadata_t *metadata,
		const char *disposition)
{
    oss_map_t *oss_map = metadata->metadata;
    oss_map_put(oss_map, OSS_CONTENT_DISPOSITION, disposition);
}

static inline void
_object_metadata_set_etag(oss_object_metadata_t *metadata,
		const char *etag)
{
    oss_map_t *oss_map = metadata->metadata;
    oss_map_put(oss_map, OSS_CONTENT_MD5, etag);
}

static inline void
_object_metadata_set_content_encoding(oss_object_metadata_t *metadata,
		const char *encoding)
{
    oss_map_t *oss_map = metadata->metadata;
    oss_map_put(oss_map, OSS_CONTENT_ENCOING, encoding);
}

static inline void
_object_metadata_set_content_length(oss_object_metadata_t *metadata,
		long content_length)
{
    char content_len[32] = {0};
    sprintf(content_len, "%ld", content_length);
    oss_map_t *oss_map = metadata->metadata;
    oss_map_put(oss_map, OSS_CONTENT_LENGTH, content_len);
}

static inline void
_object_metadata_set_content_type(oss_object_metadata_t *metadata,
		const char *content_type)
{
    oss_map_t *oss_map = metadata->metadata;
    oss_map_put(oss_map, OSS_CONTENT_TYPE, content_type);
}

static inline void
_object_metadata_set_expiration_time(oss_object_metadata_t *metadata,
		const char *expiration_time)
{
    oss_map_t *oss_map = metadata->metadata;
    oss_map_put(oss_map, OSS_EXPIRES, expiration_time);
}

static inline void
_object_metadata_set_header(oss_object_metadata_t *metadata,
		const char *key,
		const char *value)
{
    oss_map_t *oss_map = metadata->metadata;
    oss_map_put(oss_map, key, value);
}

static inline void
_object_metadata_set_last_modified(oss_object_metadata_t *metadata,
		const char *last_modified)
{
    oss_map_t *oss_map = metadata->metadata;
    oss_map_put(oss_map, OSS_LAST_MODIFIED, last_modified);
}

static inline void
_object_metadata_set_user_metadata(oss_object_metadata_t *metadata,
		oss_map_t *user_metadata)
{
    oss_map_t *oss_map = metadata->user_metadata;
    oss_map_delete(oss_map);
    metadata->user_metadata = user_metadata;
}

oss_object_metadata_t *
object_metadata_initialize()
{
    oss_object_metadata_t *metadata;
    metadata = (oss_object_metadata_t *)malloc(sizeof(oss_object_metadata_t));
    metadata->metadata = oss_map_new(128);
    metadata->user_metadata = oss_map_new(128);

    metadata->add_user_metadata       = _object_metadata_add_user_metadata;
    metadata->add_default_metadata    = _object_metadata_add_default_metadata;
    metadata->get_cache_control       = _object_metadata_get_cache_control;
    metadata->get_content_disposition = _object_metadata_get_content_disposition;
    metadata->get_content_encoding    = _object_metadata_get_content_encoding;
    metadata->get_content_length      = _object_metadata_get_content_length;
    metadata->get_content_type        = _object_metadata_get_content_type;
    metadata->get_etag                = _object_metadata_get_etag;
    metadata->get_expiration_time     = _object_metadata_get_expiration_time;
    metadata->get_last_modified       = _object_metadata_get_last_modified;
    metadata->get_raw_metadata        = _object_metadata_get_raw_metadata;
    metadata->get_user_metadata       = _object_metadata_get_user_metadata;

    metadata->set_cache_control       = _object_metadata_set_cache_control;
    metadata->set_content_disposition = _object_metadata_set_content_disposition;
    metadata->set_content_encoding    = _object_metadata_set_content_encoding;
    metadata->set_content_length      = _object_metadata_set_content_length;
    metadata->set_content_type        = _object_metadata_set_content_type;
    metadata->set_expiration_time     = _object_metadata_set_expiration_time;
    metadata->set_etag                = _object_metadata_set_etag;
    metadata->set_header              = _object_metadata_set_header;
    metadata->set_last_modified       = _object_metadata_set_last_modified;
    metadata->set_user_metadata       = _object_metadata_set_user_metadata;

    return metadata;
}

void
object_metadata_finalize(oss_object_metadata_t *metadata)
{

    if (metadata == NULL) {
        return;
    }

    if (metadata != NULL) {
        if (metadata->metadata != NULL) {
            //free(metadata->metadata);
            oss_map_delete(metadata->metadata);
            metadata->metadata = NULL;
        }
        if (metadata->user_metadata != NULL) {
            //free(metadata->user_metadata);
            oss_map_delete(metadata->user_metadata);
            metadata->user_metadata = NULL;
        }
        free(metadata);
        metadata = NULL;
    }
}
