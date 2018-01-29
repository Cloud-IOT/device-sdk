
#include "ieee80211.h"

#ifndef __SEND_RAW_PACKET_H__
#define __SEND_RAW_PACKET_H__

struct raw_hdr_3addr {
    le16_t frame_control;
    le16_t duration_id;
    U8 addr1[6];
    U8 addr2[6];
    U8 addr3[6];
    le16_t seq_ctrl;
} __attribute__ ((packed));

typedef struct _raw_beacon {
    struct raw_hdr_3addr header;
    U64 timestamp;
    le16_t beacon_interval;
    le16_t capability;
} __attribute__ ((packed)) raw_beacon;

typedef struct _raw_probe_response {
    struct raw_hdr_3addr header;
    U64 timestamp;
    le16_t beacon_interval;
    le16_t capability;
} __attribute__ ((packed)) raw_probe_response;

typedef struct _raw_authentication {
    struct raw_hdr_3addr header;
    U16 auth_algorithm;
    U16 auth_seq_no;
    U16 status_code;
} __attribute__ ((packed)) raw_authentication;

typedef struct _raw_assoc_request {
    struct raw_hdr_3addr header;
    le16_t capability;
    le16_t listen_interval;
} __attribute__ ((packed)) raw_assoc_request;

typedef struct _raw_assoc_response {
    struct raw_hdr_3addr header;
    U16 capability;
    U16 status_code;
    U16 assoc_id;
} __attribute__ ((packed)) raw_assoc_response;

typedef enum
{
    SEND_RAW_PROBE_REQ = 0,
    SEND_RAW_PROBE_RSP,
    SEND_RAW_AUTH_REQ,
    SEND_RAW_AUTH_RSP,
    SEND_RAW_ASSOC_REQ,
    SEND_RAW_ASSOC_RSP,
    SEND_RAW_BEACON,
    SEND_RAW_USER_DATA,
    SEND_RAW_MAX_NUM,
} RAW_PACKET;

typedef struct t_RAW_PACKET_DATA
{
	U8 		local_mac[6];
	u8_t		remote_mac[6];
	u8_t		sup_starate[8];
	u8_t		sup_aprate[8];
	u8_t 		sup_ext_rate[6];
	char		ssid[32];
	u8_t		channel;
	u8_t		duration;
	u8_t		seq;
} RAW_PACKET_DATA;

void init_raw_packet_info(void);
void set_raw_packet_remote_mac(U8 *mac, U8 len);
void send_raw_probe_req(void);
void send_raw_probe_rsp(void);
void send_raw_auth_req(void);
void send_raw_auth_rsp(void);
void send_raw_assoc_req(void);
void send_raw_assoc_rsp(void);
void send_raw_beacon(void);
void send_raw_user_packet(U8 *data, U16 len);
void send_raw_packet(U8 num, U8 *data, U8 len);

#endif//SEND_RAW_PACKET_H