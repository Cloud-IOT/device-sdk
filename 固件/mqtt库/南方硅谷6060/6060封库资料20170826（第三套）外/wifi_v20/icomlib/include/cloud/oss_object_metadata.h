
#ifndef OSS_OBJECT_METADATA_H
#define OSS_OBJECT_METADATA_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "oss_map.h"

#define _OSS_CONSTANTS_H
#include "oss_constants.h"
#undef _OSS_CONSTANTS_H


typedef struct oss_object_metadata_s oss_object_metadata_t;


struct oss_object_metadata_s {
	oss_map_t *metadata;
	oss_map_t *user_metadata;

	void (*add_user_metadata)(oss_object_metadata_t *metadata,
			const char *key,
			const char *value);

	void (*add_default_metadata)(oss_object_metadata_t *metadata,
			const char *key,
			const char *value);

	const char * (*get_cache_control)(oss_object_metadata_t *metadata);

	const char * (*get_content_disposition)(oss_object_metadata_t *metadata);

	const char * (*get_content_encoding)(oss_object_metadata_t *metadata);

	long (*get_content_length)(oss_object_metadata_t *metadata);

	const char * (*get_content_type)(oss_object_metadata_t *metadata);

	const char * (*get_etag)(oss_object_metadata_t *metadata);

	const char * (*get_expiration_time)(oss_object_metadata_t *metadata);

	const char * (*get_last_modified)(oss_object_metadata_t *metadata);

	oss_map_t * (*get_raw_metadata)(oss_object_metadata_t *metadata);

	oss_map_t * (*get_user_metadata)(oss_object_metadata_t *metadata);

	void (*set_cache_control)(oss_object_metadata_t *metadata, 
			const char *cache_control);

	void (*set_content_disposition)(oss_object_metadata_t *metadata,
			const char *disposition);

	void (*set_content_encoding)(oss_object_metadata_t *metadata,
			const char *encoding);

	void (*set_content_length)(oss_object_metadata_t *metadata,
			long content_length);

	void (*set_content_type)(oss_object_metadata_t *metadata,
			const char *content_type);

	void (*set_expiration_time)(oss_object_metadata_t *metadata,
			const char *expiration_time);

	void (*set_etag)(oss_object_metadata_t *metadata, const char *etag);

	void (*set_header)(oss_object_metadata_t *metadata,
			const char *key,
			const char *value);

	void (*set_last_modified)(oss_object_metadata_t *metadata,
			const char *last_modified);

	void (*set_user_metadata)(oss_object_metadata_t *metadata,
			oss_map_t *user_metadata);
};

extern oss_object_metadata_t *
object_metadata_initialize();

extern void
object_metadata_finalize(oss_object_metadata_t *metadata);

#endif // OSS_OBJECT_METADATA_H

