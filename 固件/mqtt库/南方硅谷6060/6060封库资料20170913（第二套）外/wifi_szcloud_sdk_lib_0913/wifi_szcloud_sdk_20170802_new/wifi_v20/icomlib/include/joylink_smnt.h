#ifndef _JOYLINK_SMNT_H_
#define _JOYLINK_SMNT_H_

#define MAC_ADDR_LEN            6
#define SUBTYPE_PROBE_REQ       4

typedef  unsigned char        uint8;
typedef  char                  int8;
typedef  unsigned short      uint16;

// 2-byte Frame control field
typedef    struct    GNU_PACKED {
    uint16        Ver:2;                // Protocol version
    uint16        Type:2;                // MSDU type
    uint16        SubType:4;            // MSDU subtype
    uint16        ToDs:1;                // To DS indication
    uint16        FrDs:1;                // From DS indication
    uint16        MoreFrag:1;            // More fragment bit
    uint16        Retry:1;            // Retry status bit
    uint16        PwrMgmt:1;            // Power management bit
    uint16        MoreData:1;            // More data bit
    uint16        Wep:1;                // Wep data
    uint16        Order:1;            // Strict order expected
} FRAME_CONTROL;

typedef    struct    _HEADER_802_11    {
    FRAME_CONTROL   FC;
    uint16          Duration;
    uint8           Addr1[MAC_ADDR_LEN];
    uint8           Addr2[MAC_ADDR_LEN];
    uint8            Addr3[MAC_ADDR_LEN];
    uint16            Frag:4;
    uint16            Sequence:12;
    uint8            Octet[0];
} *PHEADER_802_11;


typedef struct _joylinkResult
{
	unsigned char type;	                        // 0:NotReady, 1:ControlPacketOK, 2:BroadcastOK, 3:MulticastOK
        unsigned char encData[1+1+32+6+32];            // length + EncodeData
}joylinkResult_t;

void joylink_cfg_init(unsigned char* pBuffer);                                  // 传入一个1024字节的存储空间
int  joylink_cfg_Result(joylinkResult_t* pRet);					// Get Result
int  joylink_cfg_50msTimer(void);						// 20Hz 50ms Timer
void  joylink_cfg_DataAction(PHEADER_802_11 pHeader, int length);		// State ma Enc Data Packets
void  joylink_cfg_CtrlAction(PHEADER_802_11 pHeader, int length);		// Process Manage Packets
void initJdSmart();
void clearJdSmartBuf();
void rx_process_jdsmart(U8 *data, U32 len) ATTRIBUTE_SECTION_SRAM;

#endif
