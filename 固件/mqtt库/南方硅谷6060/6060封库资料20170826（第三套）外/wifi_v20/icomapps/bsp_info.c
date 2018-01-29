#include <ssv_config.h>
#include <ssv_types.h>
#include <ssv_lib.h>
#include <bsp_def.h>
#include <flash_api.h>

#include <systemconf.h>

#include "dbg-atcmd-api.h"


extern IEEE80211STATUS gwifistatus;
extern int g_modulator_pll_reset;

//Some info are embedded in flash
void bsp_get_boot_info(BOOL b_dump)
{
    volatile BOOT_INFO *p_boot_info = (BOOT_INFO *)CFG_BOOT_INFO_START_ADDR;

    if(p_boot_info->magic_num != BOOT_INFO_MAGIC_NUMBER){
        printf("<Error>: boot_info magic number = 0x%x != 0x%x\n",p_boot_info->magic_num,BOOT_INFO_MAGIC_NUMBER);
    }

    if(b_dump){
        printf("---------------os attribute---------------\n");
	  printf("magic number   = 0x%8x\n", p_boot_info->magic_num);
        printf("boot version   = 0x%8x\n", p_boot_info->version);
        printf("stack end      = 0x%8x\n", p_boot_info->stack_end);
        printf("irq stack_size = 0x%8x\n", p_boot_info->irq_stack_size);
        printf("normal stack_size = 0x%8x\n", p_boot_info->svc_stack_size);
        printf("flash size     = 0x%8x\n", p_boot_info->flash_size);
        printf("ram size       = 0x%8x\n", p_boot_info->ram_size);
        printf("---------------flash jedec info---------------\n");
        spi_flash_read_jedec();
   }
}

U32 get_boot_stack_fence()
{
    volatile BOOT_INFO *p = (BOOT_INFO *)CFG_BOOT_INFO_START_ADDR;
    
    U32 stack_fence = p->stack_end - 
                                (p->und_stack_size) - 
                                (p->abt_stack_size) - 
                                (p->fiq_stack_size) - 
                                (p->irq_stack_size) - 
                                (p->svc_stack_size);

    //printf("stack_fence = 0x%x\n", stack_fence);
    return stack_fence;
}

void bsp_get_mp_info(BOOL b_dump)
{
    volatile MP_INFO *p_mp_info = (MP_INFO *)CFG_MP_START_ADDR;
    volatile U8 *p_TableEntry = (U8 *)CFG_MP_START_ADDR;
//    volatile U32 *p_TableInfo32;    //access table index and size!
    S32 table_num=0;
    S32 loop_i = 0;
    S32 table_index;
    S32 table_size;
    unsigned char local_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    //u32 reg_mac_31_0;
    //u32 reg_mac_47_32;
  

    if(TRUE==b_dump){
        printf("---------------mp attribute---------------\n");
    }

    if(p_mp_info->magic_num != MP_INFO_MAGIC_NUMBER){
        printf("<Error>: mp_info magic number = 0x%x != 0x%x\n",p_mp_info->magic_num,MP_INFO_MAGIC_NUMBER);
    }

    table_num = p_mp_info->table_num;
    p_TableEntry += sizeof(MP_INFO); 

    
    if(table_num < 0x1 ){
        goto error_exit;
    }
        
    
   //printf("table_num = 0x%x\n",table_num);

    for(loop_i=0;loop_i<table_num;loop_i++){
        table_index =  (p_TableEntry[3]);
        table_size   =  ((p_TableEntry[2]<<16) |  (p_TableEntry[1]<<8)  | (p_TableEntry[0]<<0));
        p_TableEntry += 4;  //table+index is a 4-bytes field

        switch(table_index) {
            case MP_MAC_ADDR:
                local_mac[0] = p_TableEntry[5];
                local_mac[1] = p_TableEntry[4];
                local_mac[2] = p_TableEntry[3];
                local_mac[3] = p_TableEntry[2];
                local_mac[4] = p_TableEntry[1];
                local_mac[5] = p_TableEntry[0];

                memcpy( (&gwifistatus.local_mac[0]), (&local_mac[0]),sizeof(local_mac));

                _DBG_AT_SET_MAC (&local_mac[0]);
                //reg_mac_31_0 = GET_STA_MAC_31_0;
                //reg_mac_47_32 = GET_STA_MAC_47_32;
                
                if(TRUE==b_dump){
                    //printf("loop_i=%d\n",loop_i);
                    printf("MAC address = %02x:%02x:%02x:%02x:%02x:%02x\n",p_TableEntry[5],
                                                                                                        p_TableEntry[4],
                                                                                                        p_TableEntry[3],
                                                                                                        p_TableEntry[2],
                                                                                                        p_TableEntry[1],
                                                                                                        p_TableEntry[0]);
                    
                    //printf("get_mp_info:GET_STA_MAC_31_0=0x%x  GET_STA_MAC_47_32=0x%x\n",reg_mac_31_0,reg_mac_47_32);
                }
            break;        
            case MP_RF_TABLE:
                if(TRUE==b_dump){            
                    printf("RT Table: %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d   Freqoffset %2d %2d LDO %2d %2d PA %2d %2d %2d\n",
                    p_TableEntry[0], p_TableEntry[1], p_TableEntry[2], p_TableEntry[3], p_TableEntry[4], p_TableEntry[5], p_TableEntry[6], p_TableEntry[7],
                    p_TableEntry[8], p_TableEntry[9], p_TableEntry[10], p_TableEntry[11], p_TableEntry[12], p_TableEntry[13], p_TableEntry[14], p_TableEntry[15],
                    p_TableEntry[16], p_TableEntry[17], p_TableEntry[18], p_TableEntry[19], p_TableEntry[20]);
                }
            break;       
            
            case MP_HT_TABLE:
                if(TRUE==b_dump){            
                    printf("HT Table: %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d   Freqoffset %2d %2d LDO %2d %2d PA %2d %2d %2d\n",
                    p_TableEntry[0], p_TableEntry[1], p_TableEntry[2], p_TableEntry[3], p_TableEntry[4], p_TableEntry[5], p_TableEntry[6], p_TableEntry[7],
                    p_TableEntry[8], p_TableEntry[9], p_TableEntry[10], p_TableEntry[11], p_TableEntry[12], p_TableEntry[13], p_TableEntry[14], p_TableEntry[15],
                    p_TableEntry[16], p_TableEntry[17], p_TableEntry[18], p_TableEntry[19], p_TableEntry[20]);
                }
            break;       
            case MP_LT_TABLE:
                if(TRUE==b_dump){            
                    printf("LT Table: %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d   Freqoffset %2d %2d LDO %2d %2d PA %2d %2d %2d\n",
                    p_TableEntry[0], p_TableEntry[1], p_TableEntry[2], p_TableEntry[3], p_TableEntry[4], p_TableEntry[5], p_TableEntry[6], p_TableEntry[7],
                    p_TableEntry[8], p_TableEntry[9], p_TableEntry[10], p_TableEntry[11], p_TableEntry[12], p_TableEntry[13], p_TableEntry[14], p_TableEntry[15],
                    p_TableEntry[16], p_TableEntry[17], p_TableEntry[18], p_TableEntry[19], p_TableEntry[20]);
                }
            break;       
            case MP_TEMP_BOUNDARY:
               if(TRUE==b_dump){            
                    printf("Temp. Boundary: \t%d %d \n",p_TableEntry[0], p_TableEntry[1]);
                }
            break;          
            case MP_SERIAL_NO:
                if(TRUE==b_dump){            
                    printf("Serial Number: \t%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x\n",
                    p_TableEntry[0], p_TableEntry[1], p_TableEntry[2], p_TableEntry[3], p_TableEntry[4], p_TableEntry[5],
                    p_TableEntry[6], p_TableEntry[7], p_TableEntry[8], p_TableEntry[9], p_TableEntry[10], p_TableEntry[11],
                    p_TableEntry[12], p_TableEntry[13], p_TableEntry[14], p_TableEntry[15]);
                }
            break;       
            default:
                printf("wrong table_index = 0x%x\n",table_index);
                goto error_exit;
            break;

        }

        p_TableEntry += table_size; //table content is a 4~8 bytes field
        
    }


//exit:
    return;
error_exit:
    return;
}



void bsp_get_define_info(BOOL b_dump)
{
    if(b_dump){
        printf("---------------define attribute---------------\n");
  //      printf("CFG_MAX_AP_LISTS = %d\n",CFG_MAX_AP_LISTS);
  //      printf("CFG_AT_BUFFER_SIZE = %d\n",CFG_AT_BUFFER_SIZE);
        printf("CFG_BOOT_SECTOR_START_ADDR = 0x%x\n", CFG_BOOT_SECTOR_START_ADDR);
        printf("CFG_MP_START_ADDR = 0x%x\n", CFG_MP_START_ADDR);
        printf("CFG_CONFIG_START_ADDR = 0x%x\n", CFG_CONFIG_START_ADDR);
        printf("CFG_BOOT_INFO_START_ADDR = 0x%x\n", CFG_BOOT_INFO_START_ADDR);
        printf("CFG_CUSTOMER_START_ADDR = 0x%x\n", CFG_CUSTOMER_START_ADDR);
        printf("CFG_FILE_SYSTEM_ADDR = 0x%x\n", CFG_FILE_SYSTEM_ADDR);

        printf("CFG_FLASH_SIZE = 0x%x = %d(KB)\n",CFG_FLASH_SIZE,(CFG_FLASH_SIZE>>10));
        printf("CFG_BOOT_SECTOR_SIZE = 0x%x = %d(KB)\n",CFG_BOOT_SECTOR_SIZE,(CFG_BOOT_SECTOR_SIZE>>10));
        printf("CFG_MP_SIZE = 0x%x = %d(KB)\n",CFG_MP_SIZE,(CFG_MP_SIZE>>10));
        printf("CFG_CONFIG_SIZE = 0x%x = %d(KB)\n",CFG_CONFIG_SIZE,(CFG_CONFIG_SIZE>>10));
        printf("CFG_IMAGE0_SIZE = 0x%x = %d(KB)\n",CFG_IMAGE0_SIZE,(CFG_IMAGE0_SIZE>>10));
        printf("CFG_IMAGE1_SIZE = 0x%x = %d(KB)\n",CFG_IMAGE1_SIZE,(CFG_IMAGE1_SIZE>>10));
        printf("CFG_CUSTOMER_SIZE = 0x%x = %d(KB)\n",CFG_CUSTOMER_SIZE,(CFG_CUSTOMER_SIZE>>10));
        printf("CFG_FILE_SYSTEM_SIZE = 0x%x = %d(KB)\n", CFG_FILE_SYSTEM_SIZE, (CFG_FILE_SYSTEM_SIZE>>10));

        printf("g_modulator_pll_reset=%d\n",g_modulator_pll_reset);
    }
}

//#if (SUPPORT_CONFIG_INFO==1)
#if 1
void bsp_get_config_info(BOOL b_dump,COMMON_CONFIG *gp_config)
{
    CONFIG_INFO *p_config_info = (CONFIG_INFO *)CFG_CONFIG_START_ADDR;
    U8 *p_TableEntry = (U8 *)CFG_CONFIG_START_ADDR;
//    volatile U32 *p_TableInfo32;    //access table index and size!
    S32 table_num=0;
    S32 loop_i = 0;
//    S32 loop_j = 0;
    S32 table_index;
    S32 table_size;
  
    if(TRUE==b_dump){
        printf("---------------config attribute---------------\n");
    }


    if(p_config_info->magic_num != CONFIG_INFO_MAGIC_NUMBER){
        printf("<Error>: config_info magic number = 0x%x != 0x%x\n",p_config_info->magic_num,CONFIG_INFO_MAGIC_NUMBER);
        goto error_exit;
    }

    table_num = p_config_info->table_num;
    p_TableEntry += sizeof(CONFIG_INFO); 

   
    if(table_num < 0x1 ){
        goto error_exit;
    }
        
    
   printf("entry_num = 0x%x\n",table_num);

    for(loop_i=0;loop_i<table_num;loop_i++){
        table_index =  (p_TableEntry[3]);
        table_size   =  ((p_TableEntry[2]<<16) |  (p_TableEntry[1]<<8)  | (p_TableEntry[0]<<0));
        p_TableEntry += 4;  //table+index is a 4-bytes field

        //printf("addr=0x%x(index,size)=(%d,%d)\n", (U32)p_TableEntry,table_index, table_size);

        switch(table_index) {
            case CONFIG_SOFTAP_SSID:   //   n-bytes
                //gp_config->ssid = p_TableEntry[0];            
                memcpy( &gp_config->softap_ssid[0], &p_TableEntry[0],table_size);    //table_size should be 32 here!!
                if(TRUE==b_dump){            
                    printf("softap_ssid: %s\n",gp_config->softap_ssid);
                }
                memcpy( &gwifistatus.softap_ssid[0], &gp_config->softap_ssid[0], sizeof(gwifistatus.softap_ssid));
            break;

            
            case CONFIG_SOFTAP_CHANNEL:   //   1-bytes
                gp_config->softap_channel = p_TableEntry[0];            
                if(TRUE==b_dump){            
                    printf("softap_channel: 0x%2x\n",gp_config->softap_channel);
                }
                gwifistatus.softap_channel = gp_config->softap_channel;
            break;
            case CONFIG_SOFTAP_ACK_TIMEOUT:   //   4-bytes
                gp_config->softap_ack_timeout = (p_TableEntry[3]<<24) | (p_TableEntry[2]<<16) | (p_TableEntry[1]<<8) | (p_TableEntry[0]<<0);
                if(TRUE==b_dump){            
                    printf("softap_ack_timeout: 0x%2x\n",gp_config->softap_ack_timeout);
                }
                gwifistatus.softap_ack_timeout = gp_config->softap_ack_timeout;
            break;
            case CONFIG_SOFTAP_DATA_TIMEOUT:   //   4-bytes
                gp_config->softap_data_timeout = (p_TableEntry[3]<<24) | (p_TableEntry[2]<<16) | (p_TableEntry[1]<<8) | (p_TableEntry[0]<<0);
                if(TRUE==b_dump){            
                    printf("softap_data_timeout: 0x%2x\n",gp_config->softap_data_timeout);
                }
                gwifistatus.softap_data_timeout = gp_config->softap_data_timeout;
            break;
            case CONFIG_AUTO_CONNECT:   //   1-bytes
                gp_config->auto_connect = p_TableEntry[0];            
                if(TRUE==b_dump){            
                    printf("auto_connect: 0x%2x\n",gp_config->auto_connect);
                }
                gwifistatus.auto_connect= gp_config->auto_connect;
            break;
            case CONFIG_REGION_CODE:   //   4-bytes
                gp_config->region_code= (p_TableEntry[3]<<24) | (p_TableEntry[2]<<16) | (p_TableEntry[1]<<8) | (p_TableEntry[0]<<0);
                if(TRUE==b_dump){            
                    printf("region_code: 0x%x\n",gp_config->region_code);
                }
                gwifistatus.region_code= gp_config->region_code;
            break;
            case CONFIG_SLINK_MODE:   //   1-bytes
                gp_config->slink_mode= p_TableEntry[0];            
                if(TRUE==b_dump){            
                    printf("slink_mode: 0x%x\n",gp_config->slink_mode);
                }
                gwifistatus.slink_mode= gp_config->slink_mode;
            break;
           case CONFIG_SOFTAP_SSID_LENGTH:   //   1-bytes
                gp_config->softap_ssid_length = p_TableEntry[0];            
                if(TRUE==b_dump){            
                    printf("softap_ssid_length: 0x%x\n",gp_config->softap_ssid_length);
                }
                gwifistatus.softap_ssid_length= gp_config->softap_ssid_length;
            break;

           case CONFIG_WATCH_DOG_COUNT:   //   4-bytes
                gp_config->watch_dog_count = (p_TableEntry[3]<<24) | (p_TableEntry[2]<<16) | (p_TableEntry[1]<<8) | (p_TableEntry[0]<<0);
                if(TRUE==b_dump){            
                    printf("watch_dog_count: %d\n",gp_config->watch_dog_count);
                }
                gwifistatus.watch_dog_count= gp_config->watch_dog_count;
            break;
           case CONFIG_SOFTAP_ENCRYPT_MODE:   //   1-bytes
                gp_config->softap_encryt_mode = (p_TableEntry[0]<<0);
                if(TRUE==b_dump){            
                    printf("softap_encryt_mode: 0x%x\n",gp_config->softap_encryt_mode);
                }
                gwifistatus.softap_encryt_mode= gp_config->softap_encryt_mode;
            break;
           case CONFIG_SOFTAP_KEY:   //   n-bytes
                memcpy( &gp_config->softap_key[0], &p_TableEntry[0],table_size);    //table_size should be 32 here!!
                if(TRUE==b_dump){            
                    printf("softap_key: %s\n", &gp_config->softap_key[0]);
                }
                memcpy( &gwifistatus.softap_key[0], &gp_config->softap_key[0], sizeof(gwifistatus.softap_key));
            break;
           case CONFIG_SOFTAP_KEYLEN:   //   1-bytes
                gp_config->softap_keylen = (p_TableEntry[0]<<0);
                if(TRUE==b_dump){            
                    printf("softap_keylen: %d\n",gp_config->softap_keylen);
                }
                gwifistatus.softap_keylen= gp_config->softap_keylen;
            break;
            
#if 0              
            case CONFIG_DEBUG_LEVEL:   //   4-bytes
                gp_config->debug_level= (p_TableEntry[3]<<24) | (p_TableEntry[2]<<16) | (p_TableEntry[1]<<8) | (p_TableEntry[0]<<0);
                if(TRUE==b_dump){            
                    printf("debugLevel: 0x%x\n",gp_config->debug_level);
                }
            break;
            case CONFIG_DEBUG_GROUP:   //   4-bytes
                gp_config->debug_group= (p_TableEntry[3]<<24) | (p_TableEntry[2]<<16) | (p_TableEntry[1]<<8) | (p_TableEntry[0]<<0);
                if(TRUE==b_dump){            
                    printf("debugGroup: 0x%x\n",gp_config->debug_group);
                }
            break;
            
          
            case CONFIG_WIFI_MODE:   //   1-bytes
                gp_config->wifi_mode = p_TableEntry[0];
                if(TRUE==b_dump){            
                    printf("wifi_mode: 0x%2x\n",gp_config->wifi_mode);
                }
            break;       
            case CONFIG_DHCP_ENABLE:   //   1-bytes
                gp_config->dhcp_enable= p_TableEntry[0];            
                if(TRUE==b_dump){            
                    printf("dhcp_enable: 0x%2x\n",gp_config->dhcp_enable);
                }
            break;       

            case CONFIG_INTO_SMART_LINK:   //   1-bytes
                gp_config->intoSmartLink = p_TableEntry[0];            
                if(TRUE==b_dump){            
                    printf("intoSmartLink: 0x%2x\n",gp_config->intoSmartLink);
                }
            break;       
            case CONFIG_LOCAL_IP_ADDR:   //   4-bytes
                gp_config->local_ip_addr.u8[0]= p_TableEntry[0];
                gp_config->local_ip_addr.u8[1]= p_TableEntry[1];
                gp_config->local_ip_addr.u8[2]= p_TableEntry[2];
                gp_config->local_ip_addr.u8[3]= p_TableEntry[3];
                
                if(TRUE==b_dump){            
                    printf("local_ip_addr: 0x%2x-%2x-%2x-%2x\n",gp_config->local_ip_addr.u8[0],
                                                                                        gp_config->local_ip_addr.u8[1],
                                                                                        gp_config->local_ip_addr.u8[2],
                                                                                        gp_config->local_ip_addr.u8[3]);
                }
            break;       
            case CONFIG_NET_MASK:   //   4-bytes
                gp_config->net_mask.u8[0]= p_TableEntry[0];
                gp_config->net_mask.u8[1]= p_TableEntry[1];
                gp_config->net_mask.u8[2]= p_TableEntry[2];
                gp_config->net_mask.u8[3]= p_TableEntry[3];
                
                if(TRUE==b_dump){            
                    printf("net_mask: 0x%2x-%2x-%2x-%2x\n",     gp_config->net_mask.u8[0],
                                                                                        gp_config->net_mask.u8[1],
                                                                                        gp_config->net_mask.u8[2],
                                                                                        gp_config->net_mask.u8[3]);
                }
            break;       
            case CONFIG_GATEWAY_IP_ADDR:   //   4-bytes
                gp_config->gateway_ip_addr.u8[0]= p_TableEntry[0];
                gp_config->gateway_ip_addr.u8[1]= p_TableEntry[1];
                gp_config->gateway_ip_addr.u8[2]= p_TableEntry[2];
                gp_config->gateway_ip_addr.u8[3]= p_TableEntry[3];
                
                if(TRUE==b_dump){            
                    printf("gateway_ip_addr: 0x%2x-%2x-%2x-%2x\n",   gp_config->gateway_ip_addr.u8[0],
                                                                                                gp_config->gateway_ip_addr.u8[1],
                                                                                                gp_config->gateway_ip_addr.u8[2],
                                                                                                gp_config->gateway_ip_addr.u8[3]);
                }
            break;       
            case CONFIG_AP_COUNT:   //   1-bytes
                gp_config->APcount = p_TableEntry[0];
                if(TRUE==b_dump){            
                    printf("APcount: 0x%2x\n",gp_config->APcount);
                }
            break;   
            case CONFIG_AP_RECORD:   //   n-bytes
                for(loop_j=0;loop_j < gp_config->APcount; loop_j++){
                    memcpy(&gp_config->APrecord[loop_j], &p_TableEntry[0], sizeof(AP_DETAIL_INFO));
                
                    if(TRUE==b_dump){            
                        //printf("APcount: 0x%2x\n",gp_config->APcount);
                        printf(">>>>AP %%d <<<<\n",loop_j);
                        printf("ssid=%s\n", &gp_config->APrecord[loop_j].ssid[0]);
                        printf("ssid_len=%d\n", gp_config->APrecord[loop_j].ssid_len);
                        printf("key=%s\n", &gp_config->APrecord[loop_j].key[0]);
                        printf("keylen = %d\n", gp_config->APrecord[loop_j].key_len);
                        printf("mac = 0x%x-%x-%x-%x-%x-%x\n", gp_config->APrecord[loop_j].mac[0],
                                                                                        gp_config->APrecord[loop_j].mac[1],
                                                                                        gp_config->APrecord[loop_j].mac[2],
                                                                                        gp_config->APrecord[loop_j].mac[3],
                                                                                        gp_config->APrecord[loop_j].mac[4],
                                                                                        gp_config->APrecord[loop_j].mac[5]);
                        printf("pmk=%s\n", &gp_config->APrecord[loop_j].pmk[0]);
                        printf("channel=%d\n", gp_config->APrecord[loop_j].channel);
                    }
                    p_TableEntry += sizeof(AP_DETAIL_INFO);
                }
            break;  
#endif            
            default:
                printf("wrong table_index = 0x%x\n",table_index);
                goto error_exit;
            break;
        }

        p_TableEntry += table_size; //table content is a 4~8 bytes field
        
    }
    printf("----------------------------------------------\n");


#if 0
    printf("softap_ssid = %s\n", gwifistatus.softap_ssid);
    printf("softap_channel = %d\n", gwifistatus.softap_channel);
    printf("softap_ack_timeout = %d\n", gwifistatus.softap_ack_timeout);
    printf("softap_data_timeout = %d\n", gwifistatus.softap_data_timeout);
    printf("beacon_interval = %d\n",gwifistatus.softap_beacon_interval);
#endif


    return;

error_exit:
    return;
}
#endif


