
#include "ssv_lib.h"
#include "ssv_types.h"
#include "skbuff.h"
#include "airkiss.h"
#include "smartConf.h"
#include "smartComm.h"
#include "joylink_smnt.h"
#include "ieee80211_mgmt.h"
#include "ieee80211.h"
#include <ssv6200_reg.h>
#include "systemconf.h"
#include "Cabrio-conf.h"
#include "dbg-atcmd-api.h"
#include "ssv6200_rx.h"
#include "hash_alg.h"
#include "smartSetting.h"

struct smart_feature *choice_feature = NULL; 

#if (FUNC_SSV_SMART_ON==1)
const struct smart_feature ivv_smart = {
	"ivv_smart",
	initSmartLink,
	clearSmartLinkBuf,
	rx_process_smartConf,
};
#endif

#if (FUNC_AIRKISS_ON==1)
const struct smart_feature airkiss = {
	"airkiss",
	initAirKiss,
	clearAirKissBuf,
	rx_process_airkiss,
};
#endif

#if (FUNC_JDSMART_ON==1)
const struct smart_feature jdsmart = {
	"jdsmart",
	initJdSmart,
	clearJdSmartBuf,
	rx_process_jdsmart,
};
#endif

void init_smart_feature(U8 slink_mode)
{
    if(slink_mode != NOSMART || slink_mode != USER) {
        if(choice_feature != NULL)
            choice_feature->init();
    }
}

void clear_smart_feature_buf(U8 slink_mode)
{
    if(slink_mode != NOSMART || slink_mode != USER) {
        if(choice_feature != NULL)
            choice_feature->clear();
    }
}

void smart_feature_rxdata(U8 slink_mode, U8 *data, U32 len)
{
    if(slink_mode != NOSMART || slink_mode != USER) {
        if(choice_feature != NULL)
            choice_feature->rece(data, len);
    }
}

extern U8 sys_slink_mode;

void register_smart_feature(U8 slink_mode)
{
    choice_feature = NULL;
    if(slink_mode == NOSMART) {
        printf("no smart enable\n");
#if (FUNC_SSV_SMART_ON==1)
    } else if(slink_mode == ICOMM) {
        printf("into smart link mode(ICOMM MODE), please wait\n");
		sys_slink_mode = 1;
        choice_feature = &ivv_smart;
#endif
#if (FUNC_AIRKISS_ON==1)
    } else if(slink_mode == WECHAT) {
        printf("into smart link mode(WECHAT MODE), please wait\n");
        choice_feature = &airkiss;
#endif
#if (FUNC_JDSMART_ON==1)
    } else if(slink_mode == JDSMART) {
        printf("into smart link mode(JDSMART MODE), please wait\n");
        choice_feature = &jdsmart;
#endif
    } else if(slink_mode == USER) {
        printf("into smart link mode(USER MODE), please wait\n");
    }else {
        printf("unknown slink_mode: %d\n", slink_mode);
    }

    init_smart_feature(slink_mode);
}