
#include "ssv_lib.h"
#include "oss_client.h"

oss_client_t * _client_initialize(const char *access_id, unsigned int access_id_len,
                                                        const char *access_key, unsigned int access_key_len,
                                                        const char *endpoint, unsigned int endpoint_len)
{
    if(access_id == NULL || access_key == NULL ||endpoint == NULL ) {
        printf("%s input para  have null, please check\n",__func__);
        return NULL;
    }
    
    unsigned int byte_of_char = sizeof(char);
    oss_client_t *client = NULL;
    
    client = (oss_client_t *)malloc(sizeof(oss_client_t));
    
    client->access_id= (char *)malloc(byte_of_char * access_id_len+ 1);
    client->access_key = (char *)malloc(byte_of_char * access_key_len + 1);
    client->endpoint = (char *)malloc(byte_of_char * endpoint_len + 1);
    
    strncpy(client->access_id, access_id, access_id_len);
    strncpy(client->access_key, access_key, access_key_len);
    strncpy(client->endpoint, endpoint, endpoint_len);
    
    (client->access_id)[access_id_len] = '\0';
    (client->access_key)[access_key_len] = '\0';
    (client->endpoint)[endpoint_len] = '\0';
    
    return client;
}

oss_client_t * client_initialize_with_endpoint(const char *access_id,
                                                                                   const char *access_key,
                                                                                   const char *endpoint)
{
    if(access_id == NULL || access_key == NULL ||endpoint == NULL ) {
        printf("%s input para  have null, please check\n",__func__);
        return NULL;
    }
    
    unsigned int access_id_len = strlen(access_id);
    unsigned int access_key_len = strlen(access_key);
    unsigned int endpoint_len = strlen(endpoint); 
    return _client_initialize(access_id, access_id_len,
                                                access_key, access_key_len,
                                                endpoint, endpoint_len);
}

void client_finalize(oss_client_t *client) 
{
    if(client == NULL) {
        printf("%s input para  have null, please check\n",__func__);
        return;
    }
    
    if (client->access_id != NULL) {
        free(client->access_id);
        client->access_id = NULL;
    }
    
    if (client->access_key != NULL) {
        free(client->access_key);
        client->access_key = NULL;
    }
    
    if (client->endpoint != NULL) {
        free(client->endpoint);
        client->endpoint = NULL;
    }
    
    free(client);
}
