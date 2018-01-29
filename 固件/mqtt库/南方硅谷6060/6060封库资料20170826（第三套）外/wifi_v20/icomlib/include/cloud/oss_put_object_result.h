

#ifndef OSS_PUT_OBJECT_RESULT_H
#define OSS_PUT_OBJECT_RESULT_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct oss_put_object_result_s oss_put_object_result_t;

struct oss_put_object_result_s {
	char *etag;

	const char * (*get_etag)(oss_put_object_result_t *result);

	void (*set_etag)(oss_put_object_result_t *result, const char *etag);
};

extern oss_put_object_result_t *
put_object_result_initialize();

extern void 
put_object_result_finalize(oss_put_object_result_t *result);

#endif
