
#include "ssv_lib.h"
#include "sys_status_api.h"
#include "ieee80211_mac.h"
#include "ieee80211_mgmt.h"
#include "smartComm.h"
#include "hash_alg.h"
#include "dbg-atcmd-api.h"
#include "resolv.h"

extern IEEE80211STATUS gwifistatus;
extern TAG_CABRIO_CONFIGURATION gCabrioConf;

/*
*当长按键进入配网模式时，在键一直按着，还未松开的这段时间，为让led按不同的频率闪烁，需此值
*初始值为-1
*当按键时长为3-10秒时，值为0,表示smartLink配网模式
*时长超过10秒后，值为1，表示softap配网模式
*/
int gKeyLedMode = -1;


U8 get_local_ipaddr(uip_ipaddr_t *local_ipaddr)
{
    U8 status = 0;

    if(local_ipaddr == NULL) {
        status = -1;
    } else {
        memcpy(local_ipaddr, &gCabrioConf.local_ip_addr, sizeof(uip_ipaddr_t));
    }
    return status;
}

U8 get_local_mac(U8 *mac, U8 len)
{
    U8 status = 0;
    if(len > ETH_ALEN) {
        printf("over local mac len: (%d), please check\n", len);
        status = -1;
    } else if(len < ETH_ALEN) {
        printf("less local mac len: (%d), please check\n", len);
        status = -1;
    } else {
        memcpy(mac, &(gwifistatus).local_mac[0], ETH_ALEN);
    }
    return status;
}

U8 get_connectAP_mac(U8 *mac, U8 len)
{
    U8 status = 0;
    if(len > ETH_ALEN) {
        printf("over local mac len: (%d), please check\n", len);
        status = -1;
    } else {
        memcpy(mac, &(gwifistatus).connAP.mac[0], ETH_ALEN);
    }
    return status;
}

ieee80211_state get_wifi_connect_status()
{
    return gwifistatus.status;
}

U8 get_wifi_connected()
{
    return gwifistatus.wifi_connect;
}

U8 get_softap_sta_connected()
{
    if( (1==gwifistatus.softap_enable) && (gwifistatus.softap_sta_connected_number>=1) )
    {
        return 1;
    } else {
        return 0;
    }
}


U8 get_ap_lsit_total_num()
{
    return getAvailableIndex();
}

void set_nslookup(char *pIp)
{
    bss_nslookup(pIp);
}

void set_slink_ready_and_ack(struct process *p)
{
    set_slink_ready_ack(p);
}

//STA mode API
int set_if_config(U8 dhcpen, uip_ipaddr_t ipaddr, uip_ipaddr_t submask, uip_ipaddr_t gateway, uip_ipaddr_t dnsserver)
{
    if(dhcpen != 0 && dhcpen != 1)
        return -1;
    
    gCabrioConf.dhcp_enable = dhcpen;
    
    if (gCabrioConf.dhcp_enable == 0) 
    {
        gCabrioConf.local_ip_addr = ipaddr;
        gCabrioConf.net_mask = submask;
        gCabrioConf.gateway_ip_addr = gateway;
        gCabrioConf.dns_server = dnsserver;
        
        uip_sethostaddr(&gCabrioConf.local_ip_addr);
        uip_setnetmask(&gCabrioConf.net_mask);
        uip_setdraddr(&gCabrioConf.gateway_ip_addr);
        resolv_conf(&gCabrioConf.dns_server);
    }
    else
    {
    }

    return 0;
}

int get_if_config(U8 *pdhcpen, uip_ipaddr_t *pipaddr, uip_ipaddr_t *psubmask, uip_ipaddr_t *pgateway, uip_ipaddr_t *pdnsserver)
{
    if(pdhcpen == NULL || pipaddr == NULL || psubmask == NULL || pgateway == NULL || pdnsserver == NULL || gwifistatus.wifi_connect == 0)
        return -1;

    *pdhcpen = gCabrioConf.dhcp_enable;
    memcpy(pipaddr, &gCabrioConf.local_ip_addr, sizeof(uip_ipaddr_t));
    memcpy(psubmask, &gCabrioConf.net_mask, sizeof(uip_ipaddr_t));
    memcpy(pgateway, &gCabrioConf.gateway_ip_addr, sizeof(uip_ipaddr_t));
    memcpy(pdnsserver, &gCabrioConf.dns_server, sizeof(uip_ipaddr_t));
 
    return 0;
}

int set_wifi_config(U8 *pssid, U8 ssidlen, U8 *pkey, U8 keylen, U8 *pmac, U8 maclen)
{
    int i;

    //check ssid and mac information is correct.
    if ((pssid == NULL) || (ssidlen == 0) || (ssidlen > 32) || (pmac != NULL && maclen != ETH_ALEN))
    {
		return -1;
    }

    //Search AP in the AP list by ssid and mac.
    for (i = 0; i < getAvailableIndex(); i++)
    {
        if((ssidlen == ap_list[i].name_len) && (memcmp (pssid, ap_list[i].name, ap_list[i].name_len) == 0))
        {
            if((pmac == NULL) || (pmac && (memcmp (pmac, ap_list[i].mac, ETH_ALEN) != 0)))
            {
                break;
            }
        }
    }
    if (i == getAvailableIndex())
    {
        return -1;
    }

    //Check passphrase is correct.
    if(ap_list[i].security_type != 0)
    {
        if(pkey == NULL || keylen == 0)
        {
            return -1;
        }
        
        if(ap_list[i].security_type == 1)
        {
            if((keylen != 5) && (keylen != 13) && (keylen != 10) && (keylen != 26))
            {
                return -1;
            }
        }
        else
        {
            if((keylen < 8) && (64 < keylen))
            {
                return -1;
            }
        }
    }
    
    memcpy (&(gwifistatus).connAP.mac, &(ap_list[i]).mac, 6);
    gwifistatus.connAP.channel = ap_list[i].channel;
    gwifistatus.connAP.security_type = ap_list[i].security_type;
    gwifistatus.connAP.security_subType = ap_list[i].security_subType;
    memcpy ((char*)gwifistatus.connAP.ssid, pssid, ssidlen);
    gwifistatus.connAP.ssid_len = ssidlen;
    if(pkey)
    {
        memcpy ((char*)gwifistatus.connAP.key, pkey, keylen);
        gwifistatus.connAP.key_len = keylen;
        
        if((gwifistatus.connAP.security_type != 0) && (gwifistatus.connAP.security_type != 1))
        {
            pbkdf2_sha1(gwifistatus.connAP.key, gwifistatus.connAP.key_len, gwifistatus.connAP.ssid, gwifistatus.connAP.ssid_len, 4096, (unsigned char*)gwifistatus.connAP.pmk, 32);
        }
    }
    else
    {
        gwifistatus.connAP.key_len = 0;
    }
    
    _DBG_AT_SET_CHANNEL(gwifistatus.connAP.channel);
    _DBG_AT_SET_PEERMAC(&(gwifistatus).connAP.mac[0], 0);
    _DBG_AT_SET_BSSID(&(gwifistatus).connAP.mac[0]);
    
    return 0;
}

int get_wifi_config(U8 *pssid, U8 *pssidlen, U8 *pkey, U8 *pkeylen, U8 *pMac, U8 maclen)
{
    if(pssid == NULL || *pssidlen < gwifistatus.connAP.ssid_len)
        return -1;

    if(pkey == NULL || *pkeylen < gwifistatus.connAP.key_len)
        return -1;
    
    if(pMac == NULL || maclen < ETH_ALEN)
        return -1;

    memcpy(pssid, gwifistatus.connAP.ssid, gwifistatus.connAP.ssid_len);
    *pssidlen = gwifistatus.connAP.ssid_len;
    memcpy(pkey, gwifistatus.connAP.key, gwifistatus.connAP.key_len);
    *pkeylen = gwifistatus.connAP.key_len;
    memcpy(pMac, gwifistatus.connAP.mac, ETH_ALEN);

    return 0;
}

int get_ap_list(TAG_AP_INFO *plist, U8 *avaliable)
{
    if(avaliable == NULL || plist== NULL)
        return -1;

    if(*avaliable > getAvailableIndex())
        *avaliable = getAvailableIndex();

    memcpy(plist, ap_list, *avaliable * sizeof(TAG_AP_INFO));

    return 0;
}

int get_connected_ap_info(TAG_AP_INFO *papinfo)
{
    if(papinfo == NULL || gwifistatus.wifi_connect == 0)
        return -1;

    memcpy(papinfo, &ap_list[gwifistatus.aplist_index], sizeof(TAG_AP_INFO));

    return 0;
}
void scan_AP(struct process * p)
{
    bss_mgmt_scan(p);
}

void wifi_connect (struct process *p)
{
    bss_mgmt_connect (p);
    return;
}

void wifi_connect_active (U8 *pssid, U8 ssidlen, U8 *pkey, U8 keylen, struct process *p)
{
    if ((pssid == NULL) || (ssidlen == 0) || (ssidlen > 32))
        return -1;
    
    bss_mgmt_connect_active (pssid, ssidlen, pkey, keylen, p);
    return;
}

void wifi_disconnect ()
{
    bss_mgmt_disconnect ();
    return;
}

U8 get_wifi_mode()
{
    return gwifistatus.softap_enable;
}

int get_apmode_config(U8 *pssid, U8 *pssidlen,  U8* pencryt, U8 *pkey, U8 *pkeylen)
{
    if(pssid == NULL || *pssidlen < gwifistatus.softap_ssid_length)
        return -1;

    if(pkey == NULL || *pkeylen < gwifistatus.softap_keylen)
        return -1;
    
    if(pencryt == NULL)
        return -1;

    memcpy(pssid, gwifistatus.softap_ssid, gwifistatus.softap_ssid_length);
    *pssidlen = gwifistatus.softap_ssid_length;
    memcpy(pkey, gwifistatus.softap_key, gwifistatus.softap_keylen);
    *pkeylen = gwifistatus.softap_keylen;
    *pencryt = gwifistatus.softap_encryt_mode;
	
    return 0;
}

U8 sys_slink_mode = 0;
U8 get_slink_mode()
{
	return sys_slink_mode;
}


/*
*对gKeyLedMode的操作
*当长按键进入配网模式时，在键一直按着，还未松开的这段时间，为让led按不同的频率闪烁，需此值
*初始值为-1
*当按键时长为3-10秒时，值为0,表示smartLink配网模式
*时长超过10秒后，值为1，表示softap配网模式
*/

void gConf_set_key_led_mode(int mode)
{
	gKeyLedMode = mode;

	return;
}

int gConf_get_key_led_mode(void)
{
	return gKeyLedMode;
}


