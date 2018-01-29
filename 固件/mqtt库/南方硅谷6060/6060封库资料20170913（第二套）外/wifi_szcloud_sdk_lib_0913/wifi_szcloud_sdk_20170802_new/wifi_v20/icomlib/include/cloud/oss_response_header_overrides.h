
#ifndef OSS_RESPONSE_HEADER_OVERRIDES_H
#define OSS_RESPONSE_HEADER_OVERRIDES_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define _OSS_CONSTANTS_H
#include "oss_constants.h"
#undef _OSS_CONSTANTS_H

typedef struct oss_response_header_overrides_s oss_response_header_overrides_t;

struct oss_response_header_overrides_s {
	char *cache_control;
	char *content_disposition;
	char *content_encoding;
	char *content_language;
	char *content_type;
	char * expires;

	const char * (*get_cache_control)(oss_response_header_overrides_t *overrides);

	void (*set_cache_control)(oss_response_header_overrides_t *overrides,
			const char *cache_control);

	const char * (*get_content_disposition)(oss_response_header_overrides_t *overrides);

	void (*set_content_disposition)(oss_response_header_overrides_t *overrides,
			const char *content_disposition);

	const char * (*get_content_encoding)(oss_response_header_overrides_t *overrides);

	void (*set_content_encoding)(oss_response_header_overrides_t *overrides,
			const char *content_encoding);

	const char * (*get_content_language)(oss_response_header_overrides_t *overrides);

	void (*set_content_language)(oss_response_header_overrides_t *overrides,
			const char *content_language);

	const char * (*get_content_type)(oss_response_header_overrides_t *overrides);

	void (*set_content_type)(oss_response_header_overrides_t *overrides,
			const char *content_type);

	const char * (*get_expires)(oss_response_header_overrides_t *overrides);

	void (*set_expires)(oss_response_header_overrides_t *overrides,
			const char *expires);
};

extern oss_response_header_overrides_t *
response_header_overrides_initialize(void);

extern void 
response_header_overrides_finalize(oss_response_header_overrides_t *overrides);

#endif
