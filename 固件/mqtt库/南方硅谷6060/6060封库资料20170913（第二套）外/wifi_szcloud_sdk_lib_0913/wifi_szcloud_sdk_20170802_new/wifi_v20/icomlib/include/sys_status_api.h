#ifndef _SYS_STATUS_API_H_
#define _SYS_STATUS_API_H_

#include "systemconf.h"

U8 get_local_ipaddr(uip_ipaddr_t *local_ipaddr);
U8 get_local_mac(U8 *mac, U8 len);
U8 get_connectAP_mac(U8 *mac, U8 len);
ieee80211_state get_wifi_connect_status();
U8 get_wifi_connected();
U8 get_softap_sta_connected();
U8 get_ap_lsit_total_num();
void set_slink_ready_and_ack(struct process *p);

void set_nslookup(char *pIp);

/**
 * @brief Configures the network interfaces. 
 * @param dhcpen		[IN] enable\disable DHCP functionally. 0 : disable DHCP, 1 : enable DHCP.
 * @param ipaddr		[IN] The IP address of this host.
 * @param submask      [IN] The IP network mask for this interface.
 * @param gateway	[IN] The default router's IP address.
 * @param dnsserver	[IN] The DNS server IP address.
 * @return the result. 0 : Successful, -1 : Failed.
 */
int set_if_config(U8 dhcpen, uip_ipaddr_t ipaddr, uip_ipaddr_t submask, uip_ipaddr_t gateway, uip_ipaddr_t dnsserver);

/**
 * @brief Get the configuration of network interfaces. 
 * @param pdhcpen	[OUT] DHCP functionally. 0 : disable DHCP, 1 : enable DHCP.
 * @param pipaddr		[OUT] The IP address of this host.
 * @param psubmask    [OUT] The IP network mask for this interface.
 * @param pgateway	[OUT] The default router's IP address.
 * @param pdnsserver	[OUT] The DNS server IP address.
 * @return the result. 0 : Successful, -1 : Failed.
 */
int get_if_config(U8 *pdhcpen, uip_ipaddr_t *pipaddr, uip_ipaddr_t *psubmask, uip_ipaddr_t *pgateway, uip_ipaddr_t *pdnsserver);

/**
 * @brief Configures the wifi settings. 
 * @param pssid		[IN] AP's SSID.
 * @param ssidlen		[IN] Length of SSID.
 * @param pkey            [IN] AP's Passphrase. It can be NULL if AP dosen't need passphrase.
 * @param keylen         [IN] Length of passphrase.
 * @param pmac     	[IN] AP's mac address. It only need to be filled when there are several AP with the same SSID.
                                          STA will connect to specific AP when input mac address. If input NULL, STA will  
                                          connect to the AP with the strongest signal.
 * @param maclen     	[IN] Length of mac address.
 * @return the result. 0 : Successful, -1 : Failed.
 */
int set_wifi_config(U8 *pssid, U8 ssidlen, U8 *pkey, U8 keylen, U8 *pmac, U8 maclen);

/**
 * @brief Get the configuration of  the wifi settings. 
 * @param pssid		[OUT] AP's SSID.
 * @param ssidlen		[OUT] SSID.
 * @param pkey            [OUT] AP's Passphrase.
 * @param keylen         [OUT] Length of passphrase.
 * @param pmac     	[OUT] AP's mac address.
 * @param maclen     	[OUT] Length of mac address.
 * @return the result. 0 : Successful, -1 : Failed.
 */
int get_wifi_config(U8 *pssid, U8 *pssidlen, U8 *pkey, U8 *pkeylen, U8 *pMac, U8 keylen);

/**
 * @brief Get the AP information. 
 * @param plist		[OUT] A pointer to a buffer that receives AP information.
 * @param avaliable	[INOUT] IN : The avalible element of plist.
                                                OUT : The number of element received.
 * @return the result. 0 : Successful, -1 : Failed.
 */
int get_ap_list(TAG_AP_INFO *plist, U8 *avaliable);

/**
 * @brief Get the connected AP information. 
 * @param papinfo		[OUT] A pointer to a buffer that receives connected AP information.
 * @return the result. 0 : Successful, -1 : Failed.
 */
int get_connected_ap_info(TAG_AP_INFO *papinfo);

/**
 * @brief Start scan AP. THe callback process will be excuted when scan process is ended.
 * @param p      		[IN] the pointer of callback process.
 * @return none.
 */
void scan_AP(struct process * p);

/**
 * @brief Start connect to AP. THe callback process will be excuted when wifi status is changed.
 * @param p		      [IN] the pointer of callback process.
 * @return none.
 */
void wifi_connect (struct process *p);

/**
 * @brief Start connect to AP. This API is used for connect to the AP with hidden SSID.
 * @param pssid		[OUT] AP's SSID.
 * @param ssidlen		[OUT] SSID.
 * @param pkey            [OUT] AP's Passphrase.
 * @param keylen         [OUT] Length of passphrase.
 * @param papinfo		[IN] the pointer of callback process.
 * @return none.
 */
void wifi_connect_active (U8 *pssid, U8 ssidlen, U8 *pkey, U8 keylen, struct process *p);

/**
 * @brief Disconnect with the current AP. 
 * @return none.
 */
void wifi_disconnect ();

/**
 * @brief Get current mode of module, STA or AP 
 * @return the result. 0 : STA mode, 1 : AP mode
 */
U8 get_wifi_mode();

/**
 * @brief Get the configuration of  ap mode. 
 * @param pssid		[OUT] SSID of AP mode
 * @param pssidlen		[OUT] Length of SSID.
 * @param pencryt            [OUT] encryt of AP mode, 0:open, 1:WPA2
 * @param pkey         [OUT] AP's Passphrase.
 * @param pkeylen     	[OUT] Length of passphrase.
 * @return the result. 0 : Successful, -1 : Failed.
 */
int get_apmode_config(U8 *pssid, U8 *pssidlen,  U8* pencryt, U8 *pkey, U8 *pkeylen);

#endif
