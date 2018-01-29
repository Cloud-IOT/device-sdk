#ifndef _ATCMD_ICOMM_H_
#define _ATCMD_ICOMM_H_

#include "uip.h"
#include "atcmd.h"

int ip2int (char *pStr, uip_ip4addr_t *ipaddr);
void At_RespOK (char* pStr);
int At_GetLocalMac (stParam *param);
int At_GetVersion (stParam *param);
int At_GetManufactureInfo (stParam *param);
int At_SetWifiConfig (stParam *param);
int At_GetConfigure (stParam *param);
int At_SetIfConfigure (stParam *param);
int At_GetNetStatus (stParam *param);
int At_Ping(stParam *param);
int At_Help(stParam *param);
int At_GetApStatus(stParam *param);
int At_GetSoftapStatus(stParam *param);
int At_GetWifiStatus(stParam *param);
int At_GetMRX(stParam *param);
int At_TestMemRead(stParam *param);
int At_TestMemInfo();
int At_MboxDump(stParam *param);
int At_PbufDump(stParam *param);
int At_WifiReset(stParam *param);
int At_GetIPconf(stParam *param);
int At_NetScan (stParam *param);
int At_NetScan_Custom (stParam *param);
int At_Connect (stParam *param);
int At_ReConnect (stParam *param);
int At_Disconnect (stParam *param);
int At_POWERSAVE (stParam *param);
int At_WriteReg32 (stParam *param);
int At_ReadReg32 (stParam *param);
int At_SetChannel (stParam *param);
int At_GetRfStatus (stParam *param);
int At_ShowRfCommand (stParam *param);
int AT_RemoveCfsConf(stParam *param);
int At_bmode_count (stParam *param);
int At_bmode_reset (stParam *param);
int At_gmode_count (stParam *param);
int At_gmode_reset (stParam *param);
int At_filter_reset(stParam *param);
int At_filter_count(stParam *param);
int At_set_rateindex(stParam *param);
S32 at_cmd_start();
S32 at_cmd_end();
#if UIP_CONF_ICMP_CNT
int At_ShowPingCount (stParam *param);
int At_ClearPingCount (stParam *param);
#endif

#endif
