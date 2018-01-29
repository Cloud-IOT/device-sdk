
#ifndef GENERATE_AUTHENTICATION_H
#define GENERATE_AUTHENTICATION_H

#include "oss_map.h"

extern char *
generate_authentication(const char *access_key, const char *method,
		oss_map_t *default_headers, oss_map_t *user_headers,
		const char *resource, unsigned int *sign_len);

#endif // GENERATE_AUTHENTICATION_H
