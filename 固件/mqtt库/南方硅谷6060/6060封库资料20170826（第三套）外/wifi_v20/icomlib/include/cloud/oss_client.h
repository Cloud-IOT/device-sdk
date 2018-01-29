
#ifndef OSS_CLIENT_H
#define OSS_CLIENT_H

#define _OSS_GET_OBJECT_REQUEST_H
#include "oss_get_object_request.h"
#undef _OSS_GET_OBJECT_REQUEST_H

#define _OSS_OBJECT_METADATA_H
#include "oss_object_metadata.h"
#undef _OSS_OBJECT_METADATA_H

#define _OSS_PUT_OBJECT_RESULT_H
#include "oss_put_object_result.h"
#undef _OSS_PUT_OBJECT_RESULT_H

typedef struct oss_client_s {
	char *endpoint;
	char *access_id;
	char *access_key;
}oss_client_t;

typedef struct curl_request_param_s curl_request_param_t;

struct param_buffer_s {
	char *ptr;
	FILE *fp;
	unsigned int left;
	unsigned int allocated;
	unsigned short code;
};

typedef struct param_buffer_s param_buffer_t;

struct curl_request_param_s {
	param_buffer_t *send_buffer; /**< send buffer */
	param_buffer_t *recv_buffer; /**< receive buffer */
	param_buffer_t *header_buffer; /**< header buffer */
};

extern oss_client_t *
client_initialize_with_endpoint(
		const char *access_id,
		const char *access_key,
		const char *endpoint);

extern void 
client_finalize(oss_client_t *client);

int
client_get_bucket_acl(oss_client_t *client,
		const char *bucket_name,
		unsigned short *retcode, char *data);

extern const char *
client_get_endpoint(oss_client_t *client);

int
client_get_object_to_buffer_2nd(oss_client_t *client,
		oss_get_object_request_t *request, char *data);

extern oss_put_object_result_t *
client_put_object_from_buffer(oss_client_t *client,
		const char *bucket_name,
		const char *key,
		int tmp_len,
		oss_object_metadata_t *metadata, char *data);

int client_create_bucket(oss_client_t *client, const char *bucket_name, char *data);
int client_delete_bucket(oss_client_t *client, const char *bucket_name, char *data);

#endif // OSS_CLIENT_H
