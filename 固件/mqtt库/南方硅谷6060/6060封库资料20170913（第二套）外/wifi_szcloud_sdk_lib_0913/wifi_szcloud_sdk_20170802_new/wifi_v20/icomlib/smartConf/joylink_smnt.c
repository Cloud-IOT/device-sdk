#include <stdio.h>
#include <string.h>
#include "userSmart.h"
#include "ieee80211_mgmt.h"
#include "ieee80211.h"
#include "joylink_smnt.h"
#include "aes_jd.h"
#include "ssv6200_rx.h"
#include "ssv_types.h"
#include "sys_status_api.h"

#if (CFG_SUPPORT_MTK_SMNT==0)

PROCESS(jdsmart_conf_process, "jdsmart_conf_process");

void joylink_cfg_DataAction(PHEADER_802_11 pHeader, int length) ATTRIBUTE_SECTION_SRAM;
    
#define PAYLOAD_MIN		6
#define PAYLOAD_MAX		80+1

#define printf_high printf

static uint8 getCrc(uint8 *ptr, uint8 len);
static void dump8(uint8* p, int split, int len);
static int  payLoadCheck(void);
static void muticastAdd(uint8* pAddr);
static void broadcastAdd(int ascii);
static void airkissAdd(int ascii);

enum smart_status
{
	SMART_CH_INIT =		0x1,
	SMART_CH_LOCKING =	0x2,
	SMART_CH_LOCKED =	0x4,
	SMART_FINISH =          0x8
};

typedef struct _joylinkSmnt
{
	uint8 state;			// 当前状态
	uint8 syncFirst;		// 首次同步字节
	uint8 syncCount;		// 同步状态表征连续合法包次数, 已同步时表征是否可以持续获得包
	uint8 syncStepPoint;		// 同步过程中阶跃点的位置
	uint8 directTimerSkip;		// 直接跳过Timer的次数
	
	uint8 broadcastVersion;		// 广播数据的版本
	uint8 muticastVersion;		// 组播数据的版本

	uint8  mutiSeriesCount;		// 组播连续同步次数
	int8   mutiLastIndex;		// 上次组播地址索引

	uint8  broadIndex;		// 广播帧
	uint8  broadBuffer[5];		// 广播帧缓冲区

	uint8  lastIsUplink;		// 上次合法数据是否是上行数据包
	uint16 lastLength;			// 上一个包的包长
	uint16 lastUploadSeq;		// 最近的上行SEQ
	uint16 lastDownSeq;		// 最近的下行SEQ

	uint8  syncAppMac[6];		// 锁定住的手机MAC地址
	uint8  syncBssid[6];		// 锁定住的AP的SSID
	uint16 syncUploadSeq;		// 锁定时的上行SEQ
	uint16 syncDownSeq;			// 锁定时的下行SEQ
	uint16 syncIsUplink;		// 是否锁定为上行数据包

	uint8 chCurrentIndex;		// 当前处理的CH的Index
	uint8 chCurrentProbability;	// 当前信道是否有必要继续锁定

	uint8 isProbeReceived;		// 是否收到过合法的Probe包
	uint8 flagProbe;		// bit0=passwd, bit1=ssid

	uint8 payload[128];		// 接收到的Payload数据
	joylinkResult_t result;		// 最终结论
}joylinkSmnt_t;


static joylinkSmnt_t* pSmnt = NULL;

static unsigned char sn_joylinkBuf[1024]={0};
static joylinkResult_t sn_pRet={0};
static char sn_joylinkkey[17]="1234567812345678\0";
static char sn_joylinkiv[17]={0,0};
static char sn_joylinkdecode[0x100]={0};
static int sn_joylinkdecode_len=0;

static int payLoadCheck(void)
{
	uint8 crc = getCrc(pSmnt->payload + 1, pSmnt->payload[1]+1);
	if ((pSmnt->payload[1] > PAYLOAD_MIN) &&
		(pSmnt->payload[1] < PAYLOAD_MAX) &&
        (pSmnt->payload[0] == crc)
    )
	{
		joylinkResult_t* pRet = &pSmnt->result;
		memcpy(pRet->encData, pSmnt->payload+1, pSmnt->payload[1]+1);
		pRet->type = pSmnt->result.type;
                pSmnt->state = SMART_FINISH;
                return 0;

	}
	dump8(pSmnt->payload, 1, 40);
	printf_high("CRC=%d", crc);

	return 1;
}


void joylink_cfg_init(unsigned char* pBuffer)
{
	pSmnt = (void*)pBuffer;
    memset(pSmnt, 0, sizeof(joylinkSmnt_t) );
	memset(pSmnt->payload, 0xFF, 128);	// 在一定程度上防止CRC错误
}


int  joylink_cfg_Result(joylinkResult_t* pRet)
{
	pRet->type = 0;
	if (pSmnt && (pSmnt->state== SMART_FINISH) )
	{
		memcpy(pRet, &pSmnt->result, sizeof(joylinkResult_t));
		//printf_high("-------------------->(CH=%d,RET=%d)-SSID:%s, PASS:%s\n", pSmnt->chCurrentIndex + 1, pRet->type, pRet->ssid, pRet->pass);
		return 0;
	}
	return 1;
}

int joylink_cfg_50msTimer(void)
{
	if (pSmnt->directTimerSkip)
	{
		pSmnt->directTimerSkip--;
		return 50;
	}
	if (pSmnt->state == SMART_FINISH)
	{
		pSmnt->directTimerSkip = 10000/50;
		return 50;
	}
	if (pSmnt->isProbeReceived >0 )
	{
		printf_high("-------------------->Probe Stay(CH:%d) %d\n", pSmnt->chCurrentIndex + 1, pSmnt->isProbeReceived);
		pSmnt->isProbeReceived = 0;
		pSmnt->directTimerSkip = 5000 / 50;
		return 50;
	}
	if (pSmnt->chCurrentProbability > 0)	// 如果中间收到特征数据包,继续停留在当前频道
	{
		pSmnt->chCurrentProbability--;
		//printf_high("------------------->SYNC (CH:%d) %d\n", pSmnt->chCurrentIndex + 1, pSmnt->chCurrentProbability);
		return 50;
	}
             //printf("before pSmnt->chCurrentIndex: %d\n", pSmnt->chCurrentIndex);
	pSmnt->chCurrentIndex = (pSmnt->chCurrentIndex + 1) % 13;
             //printf("after pSmnt->chCurrentIndex: %d\n", pSmnt->chCurrentIndex);
	//adp_changeCh(pSmnt->chCurrentIndex +1);
	//userSmartSetChannel(pSmnt->chCurrentIndex +1);
          	SmartSetChannel(pSmnt->chCurrentIndex +1);
	pSmnt->state = SMART_CH_LOCKING;
	pSmnt->syncStepPoint = 0;
	pSmnt->syncCount = 0;
	pSmnt->chCurrentProbability = 0;
	
	printf_high("CH=%d, T=%d\n", pSmnt->chCurrentIndex +1, 50);
	return 50;
}

//APSendProbeAction(RX_BLK* pRxBlk)
void joylink_cfg_CtrlAction(PHEADER_802_11 pHeader, int length)
{
	char str[33] = {0};
	uint8* pBuffer = (uint8*)pHeader;
	int probeLength = pBuffer[25];
	if (pHeader->FC.SubType == SUBTYPE_PROBE_REQ)
	{
		memcpy(str, pBuffer + 26, probeLength);
		pBuffer[probeLength] = 0;
		if (memcmp(str, "JD", 2)==0)
		{
			/*if (str[2] >= '0' && str[2] <= '9' && probeLength>6)
			{
				pSmnt->flagProbe |= 0x1;
				pSmnt->isProbeReceived = 1;
				memcpy(pSmnt->result.pass, str + 6, probeLength - 6);
				pSmnt->result.pass[probeLength - 6] = 0;
				printf_high("(ProbePASS=%d)%s\n", probeLength - 6, pSmnt->result.pass);
			}*/
		}
		if (memcmp(str, "SSID", 4) == 0)
		{
			/*pSmnt->flagProbe |= 0x2;
			pSmnt->isProbeReceived = 1;
			memcpy(pSmnt->result.ssid, str + 4, probeLength - 4);
			pSmnt->result.ssid[probeLength - 4] = 0;*/

			//printf_high("(ProbeSSID=%d)%s\n", probeLength - 4, pSmnt->result.ssid);
		}
		if (memcmp(str, "ZOUTAO", 6) == 0)
		{
			return;
		}
		if (pSmnt->flagProbe == 0x3)
		{
			pSmnt->result.type = 1;
			pSmnt->state = SMART_FINISH;
		}
		
		//printf_high("DBG->%s\n", str);
	}
	return;
}

static void  airkissAdd(int ascii)
{
	uint8 isFlag = (ascii >> 8) & 0x1;
	if (isFlag)
	{
	}
	else
	{
	} //if (isFlag)
}


static void  broadcastAdd(int ascii)
{
	uint8 isFlag = (ascii >> 8) & 0x1;
	if (isFlag)
	{
		pSmnt->broadBuffer[0] = ascii;
		pSmnt->broadIndex = 1;
		return;
	}
	if (pSmnt->broadIndex)
	{
		pSmnt->broadBuffer[pSmnt->broadIndex++] = ascii;
		if (pSmnt->broadIndex > 4)
		{
			if (pSmnt->chCurrentProbability < 20) pSmnt->chCurrentProbability += 4;
			pSmnt->broadIndex = 0;
			uint8 crc = pSmnt->broadBuffer[0] & 0x7;
			uint8 index = pSmnt->broadBuffer[0] >> 3;
			uint8 rCrc = getCrc(pSmnt->broadBuffer + 1, 4) & 0x7;
			if ((index>0) && (index<33) && (rCrc == crc))
			{
				memcpy(pSmnt->payload + (index - 1) * 4, pSmnt->broadBuffer + 1, 4);
				//printf_high("B%02d(CH=%d,UP=%d)--%02X:(%02X,%02X,%02X,%02X)\n", index, pSmnt->chCurrentIndex + 1, pSmnt->lastIsUplink,
				//	pSmnt->broadBuffer[0], pSmnt->broadBuffer[1], pSmnt->broadBuffer[2], pSmnt->broadBuffer[3], pSmnt->broadBuffer[4]);
				index = payLoadCheck();
				if (index == 0)
				{
					pSmnt->result.type = 2;
				}
			}
		}
		else if (pSmnt->broadIndex == (2+1))
		{
			uint8 index = pSmnt->broadBuffer[0] >> 3;
			if (index == 0)
			{
				pSmnt->broadIndex = 0;
				uint8 crc = pSmnt->broadBuffer[0] & 0x7;
				uint8 rCrc = getCrc(pSmnt->broadBuffer + 1, 2) & 0x7;
				if (rCrc == crc)
				{
					pSmnt->broadcastVersion = pSmnt->broadBuffer[1];
				}
			}
		}
	} //if (isFlag)
}

/*
Input: Muticast Addr
Output: -1:Unkown Packet, 0:Parse OK, 1:Normal Process
*/
static void muticastAdd(uint8* pAddr)
{// 239.0.{Version}.4  +239.{(0*1bit)((byte[i]^byte[i+1])*1bit)(Index*6bit)}.{byte[i]}.{byte[i+1]}
	int8 index = 0;
	if (pAddr[3] == 0)
	{
		index = 0 - pAddr[3];
	}
	else if ((pAddr[3] >> 6) == ((pAddr[4] ^ pAddr[5]) & 0x1))
	{
		index = pAddr[3] & 0x3F;
	}
	else
	{
		return;
	}
	int expectLength = 1 + pSmnt->mutiLastIndex;
	pSmnt->mutiLastIndex = index;
	if (expectLength == index)
	{
		//printf_high("Muticast Add Probability %d \n", pSmnt->chCurrentProbability);
		uint8 payloadIndex = index - 1;
		if (payloadIndex > 64)
			return;
		if (pSmnt->chCurrentProbability < 20) pSmnt->chCurrentProbability += 3;			// Delay CH switch!
		printf_high("M%02d(CH=%d)--%02X:(%02X,%02X)\n", index, pSmnt->chCurrentIndex + 1, pAddr[3], pAddr[4], pAddr[5]); 
		pSmnt->payload[payloadIndex * 2]     = pAddr[4];
		pSmnt->payload[payloadIndex * 2 + 1] = pAddr[5];
		if (payLoadCheck() == 0)
		{
			pSmnt->result.type = 3;
			return;
		}
	}
	return;
}



void joylink_cfg_DataAction(PHEADER_802_11 pHeader, int length)
{
	uint8 isUplink = 1;				// 是否上行线路
	uint8 packetType = 0;			// 0=普通数据包, 1=组播数据包, 2=广播数据包
	uint8 isDifferentAddr = 0;
	uint8 *pDest, *pSrc, *pBssid;
	uint16 lastLength = 0;
	uint16 lastSeq = 0;
	if (pSmnt == NULL)
		return;
	if ((length > 100) && (pSmnt->state != SMART_CH_LOCKED))	// 在锁定之前收到大包不用处理
		return;
	
	if (pHeader->FC.ToDs)
	{
		// 上行88, 下行08, 下行长2字节
		// 对于数据包: Addr1=Dest,Addr2=BSSID,Addr3=Src
		pBssid = pHeader->Addr1;// 88 上行
		pSrc = pHeader->Addr2;
		pDest = pHeader->Addr3;
		length -= 2;
		lastSeq = pSmnt->lastUploadSeq;
		pSmnt->lastUploadSeq = pHeader->Sequence;
	}
	else
	{
		isUplink = 0;
		pDest = pHeader->Addr1;	// 08 下行
		pBssid = pHeader->Addr2;
		pSrc  = pHeader->Addr3;
		lastSeq = pSmnt->lastDownSeq;
		pSmnt->lastDownSeq = pHeader->Sequence;
	}
	lastLength = pSmnt->lastLength;
	pSmnt->lastLength = length;

	if (memcmp(pDest, "\xFF\xFF\xFF\xFF\xFF\xFF", 6) == 0)
	{
		if (pSmnt->state == SMART_CH_LOCKING)
			printf_high("(%02x-%04d)->%d, =(0x%02x)\n", *((uint8*)pHeader) & 0xFF, pHeader->Sequence, length, (uint8)(length - pSmnt->syncFirst +1));
		packetType = 2;
	}
	else if (memcmp(pDest, "\x01\x00\x5E", 3) == 0)
	{
		if (pSmnt->state == SMART_CH_LOCKING)
			printf_high("(%02x-%04d):%02x:%02x:%02x->%d\n", *((uint8*)pHeader) & 0xFF, pHeader->Sequence, pDest[3], pDest[4], pDest[5], (uint8)length);
		packetType = 1;
	}

	if (memcmp(pSrc, pSmnt->syncAppMac, 6) != 0)
	{
		isDifferentAddr = 1;
	}

	if(pSmnt->state == SMART_CH_LOCKING)
	{
		if (packetType == 0) return;
		if (pHeader->Sequence == lastSeq) return;
		if(!isDifferentAddr)
		{
			if (packetType != 0)
			{
				if (packetType == 1)
				{
					// 239.0.{Version}.4  +239.{(0*1bit)((byte[i]^byte[i+1])*1bit)(Index*6bit)}.{byte[i]}.{byte[i+1]}
					muticastAdd(pDest);	// Internal state machine could delay the ch switching
				}
				if (lastLength == length)
					return;
				int expectLength = 1 + pSmnt->syncFirst + pSmnt->syncCount%4 - (pSmnt->syncStepPoint?4:0);
				int isStep = (pSmnt->syncStepPoint == 0 && length == (expectLength - 4));	
				//printf_high("B.SYNC(Firt=%d, CONT=%d, STP=%d, LEN=%d) EXP=%d\n", pSmnt->syncFirst, pSmnt->syncCount, pSmnt->syncStepPoint, length, expectLength);
				if ( ( length == expectLength ) || isStep)
				{
					pSmnt->syncCount++;
					pSmnt->chCurrentProbability += 3;
					if (isStep)
						pSmnt->syncStepPoint = pSmnt->syncCount;
					if (pSmnt->syncCount >= 3)	// Achive SYNC count!
					{
						pSmnt->syncFirst = pSmnt->syncFirst + pSmnt->syncStepPoint - (pSmnt->syncStepPoint ? 4 : 0);	// Save sync world
						memcpy(pSmnt->syncBssid, pBssid, 6);
						pSmnt->syncIsUplink = isUplink;
						pSmnt->state = SMART_CH_LOCKED;
						printf_high("SYNC:(SRC=%02X%02X%02X%02X%02X%02X,BSSID=%02X%02X%02X%02X%02X%02X)------->:CH=%d, WD=%d, Uplink=%d\n",
							pSrc[0], pSrc[1], pSrc[2], pSrc[3], pSrc[4], pSrc[5],
							pBssid[0], pBssid[1], pBssid[2], pBssid[3], pBssid[4], pBssid[5],
							pSmnt->chCurrentIndex+1, pSmnt->syncFirst, pSmnt->syncIsUplink);
						dump8(pSmnt->payload, 1, 36);
					}
					return;
				}
				if (pSmnt->syncCount)
				{
					pSmnt->syncStepPoint = 0;
					pSmnt->syncCount = 0;
					memcpy(pSmnt->syncAppMac, pSrc, 6);
					pSmnt->syncFirst = length;
					printf_high("SYNC LOST\n");
				}
			} // if (packetType != 0)
			return;	// Skip noise packets from same address
		}
		memcpy(pSmnt->syncAppMac, pSrc, 6);
		pSmnt->syncFirst = length;
		printf_high("Try to SYNC!\n");
		return;
	}
	else if (pSmnt->state == SMART_CH_LOCKED)
	{
		if (isDifferentAddr) return;
		if (length < pSmnt->syncFirst) return;
		if (packetType == 1)
		{// Muticast"01:00:5E: (0*1bit)((byte[i]^byte[i+1])*1bit)(Index*6bit): byte[i] : byte[i+1]"
			muticastAdd(pDest);
			return;
		}
		if ( (packetType != 1)&&(memcmp(pDest, pSmnt->syncBssid, 6) != 0) )
		{
			
			// BroadCast Data!
			packetType = 2;
			int ascii = length - pSmnt->syncFirst + 1;
			if (((length + 4 - lastLength) % 4 == 1) && (length - pSmnt->syncFirst) < 4)  // There are SYNC packets even ch locked.
			{
				if (pSmnt->chCurrentProbability < 20) pSmnt->chCurrentProbability++;
			}

			if (pSmnt->broadIndex == 0)
			{
				if (pSmnt->lastIsUplink != isUplink)  // By pass the same Up and Down Length.
				{
					pSmnt->lastIsUplink = isUplink;
					if (lastLength == length)
						return;
				}
				broadcastAdd(ascii);	// Joylink state
				airkissAdd(ascii);		// airkiss state
			}
			else
			{
				if (pSmnt->lastIsUplink != isUplink) return;
				if (pHeader->Sequence != (lastSeq + 1)) return;
				//printf_high("(%02x-%04d):(0x%02x->%d=%x)\n", *((uint8*)pHeader) & 0xFF, pHeader->Sequence, length, (uint8)ascii, (uint8)ascii);
				broadcastAdd(ascii);	// Joylink state
				airkissAdd(ascii);		// airkiss state
			}
		}
		
	}
	else if (pSmnt->state == SMART_FINISH)
	{
		// printf_high("SMART_FINISH-1\n");
	}
	else // (pSmnt->state == SMART_CH_INIT)
	{
		pSmnt->state = SMART_CH_LOCKING;
		memcpy(pSmnt->syncAppMac, pSrc, 6);
		pSmnt->syncFirst = length;
		pSmnt->syncStepPoint = 0;
		pSmnt->syncCount = 0;
		printf_high("Reset All State\n");
	}
	return;
}

static uint8 getCrc(uint8 *ptr, uint8 len)
{
	unsigned char crc;
	unsigned char i;
	crc = 0;
	while (len--)
	{
		crc ^= *ptr++;
		for (i = 0; i < 8; i++)
		{
			if (crc & 0x01)
			{
				crc = (crc >> 1) ^ 0x8C;
			}
			else
				crc >>= 1;
		}
	}
	return crc;
}

static void dump8(uint8* p, int split, int len)
{
	int i;
	char buf[512];
	int index = 0;
	for (i = 0; i < len; i++)
	{
		if (split != 0 && ((i + 1) % split) == 0)
		{
			index += sprintf(buf + index, "%02x,", p[i]);
		}
		else
			index += sprintf(buf + index, "%02x ", p[i]);
	}
	printf_high("Addr=%d,Len=%d:%s\n", p, len, buf);
}

void initJdSmart()
{
    int ret = 0;
    printf("==========initJdSmart==========\n");    
    clear_mac_bssid();
    enableSmartHwFilter();
    
    joylink_cfg_init(&sn_joylinkBuf);
    process_start(&jdsmart_conf_process, NULL);
}

void clearJdSmartBuf()
{
#if 1
    printf("==========clearJdSmartBuf==========\n");
    joylink_cfg_init(&sn_joylinkBuf);

    clearRxBuffer();
#endif
}

void setJdResult()
{
    U8 offset = sn_joylinkdecode[0] + 7;
    smart_set_ssid_pass(&sn_joylinkdecode[offset], sn_joylinkdecode_len - offset,&sn_joylinkdecode[1] ,sn_joylinkdecode[0]);
}

void rx_process_jdsmart(U8 *data, U32 len)
{
    joylink_cfg_DataAction((void *)data, len);
}

PROCESS_THREAD(jdsmart_conf_process, ev, data)
{
    static struct etimer timer;
    
    PROCESS_BEGIN();

    startrecordAP();
    set_smart_timer(50);
    while(1) {
        etimer_set(&timer, get_smart_timer() * CLOCK_MINI_SECOND);
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
        if(joylink_cfg_Result(&sn_pRet) == 0){	
            printf("zohar# ###################joylink ok #########################\n");
            int i = 0;
            sn_joylinkdecode_len = device_aes_decrypt((unsigned char*)&sn_joylinkkey[0],
                                                                                                16,
                                                                                                (unsigned char*)&sn_joylinkiv[0],
                                                                                                (unsigned char*)&sn_pRet.encData[1],
                                                                                                sn_pRet.encData[0],
                                                                                                (unsigned char*)&sn_joylinkdecode[0],
                                                                                                0x100);
            setJdResult();
            etimer_stop(&timer);
            break;
        }
        set_smart_timer(joylink_cfg_50msTimer());
    }    

    disableSmartHwFilter();
    if(checkAndConnAP() == 0) {
        printf("BREAK\n");
    }
    stoprecordAP();

    PROCESS_END();	
}

#endif

