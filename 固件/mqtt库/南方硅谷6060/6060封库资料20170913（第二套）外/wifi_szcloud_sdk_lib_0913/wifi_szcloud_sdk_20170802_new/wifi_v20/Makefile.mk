

####============================================================================
-include ./Option_Custom.mk
-include ../Option_Custom.mk
-include ./Prj.tmp
####============================================================================



CROSS_COMPILE ?= arm-none-eabi-
CROSS ?= $(CROSS_COMPILE)

### Compiler definitions
CC       = $(CROSS)gcc
LD       = $(CROSS)ld
AS       = $(CROSS)as
NM       = $(CROSS)nm
OBJCOPY  = $(CROSS)objcopy
STRIP    = $(CROSS)strip
OBJDUMP  = $(CROSS)objdump

AROPTS = rcf

######################
#	MODULE SWITCH    #
######################
BOOT_SECTOR_UPDATE=0
DISABLE_ICOMM_DISCOVER=0
MODULE_S092=1
#MODULE_6060Z=2   #obsoleted#
MODULE_6060P=3
#MODULE_ID_SKIP=4
MODULE_HIFLYING=5
MODULE_ID=5

######################
#	FUNCTION SWITCH  #
######################
FUNC_OTA_ON=0

ifdef UA_OTA_SUPPORT
  ifeq ($(strip $(UA_OTA_SUPPORT)),TRUE)
  	FUNC_OTA_ON := 1
  endif
endif

ENABLE_ICOMM_DEMO=0
FUNC_CLOUD_ON=0
FUNC_SEND_RAW_DATA_ON=0
FUNC_SSV_SMART_ON=1
FUNC_AIRKISS_ON=0
FUNC_JDSMART_ON=0
FUNC_DLL_ON=0			#	This feature is still under experiment #

##	Custom Image Size: 4, 16 KB, default 4K
UA_FLASH_CUSTOM_IMAGE_SIZE=4

#####################
#	VARIABLES  		#
#####################
4KB=$(shell echo       4\*1024 | bc)
8KB=$(shell echo       8\*1024 | bc)
16KB=$(shell echo     16\*1024 | bc)
64KB=$(shell echo     64\*1024 | bc)
256KB=$(shell echo   256\*1024 | bc)
384KB=$(shell echo   384\*1024 | bc)
512KB=$(shell echo   512\*1024 | bc)
1024KB=$(shell echo 1024\*1024 | bc)
2048KB=$(shell echo 2048\*1024 | bc)
4096KB=$(shell echo 4096\*1024 | bc)
####
976KB=$(shell echo   976\*1024 | bc)
944KB=$(shell echo   944\*1024 | bc)
512KB=$(shell echo   512\*1024 | bc)
496KB=$(shell echo   496\*1024 | bc)
492KB=$(shell echo   492\*1024 | bc)
488KB=$(shell echo   488\*1024 | bc)
480KB=$(shell echo   480\*1024 | bc)
476KB=$(shell echo   476\*1024 | bc)
472KB=$(shell echo   472\*1024 | bc)
468KB=$(shell echo   468\*1024 | bc)
464KB=$(shell echo   464\*1024 | bc)
460KB=$(shell echo   460\*1024 | bc)
448KB=$(shell echo   448\*1024 | bc)
428KB=$(shell echo   428\*1024 | bc)
####

ifeq ($(MODULE_ID),$(MODULE_S092))
LINKERSCRIPT=cabrio.lds
CFG_FLASH_SIZE=$(512KB)
endif

ifeq ($(MODULE_ID),$(MODULE_6060P))
LINKERSCRIPT=cabrio.lds
CFG_FLASH_SIZE=$(512KB)
endif

ifeq ($(MODULE_ID),$(MODULE_HIFLYING))
LINKERSCRIPT=cabrio.lds
CFG_FLASH_SIZE=$(512KB)
endif

CFG_BOOT_SECTOR_SIZE=8192
CFG_MP_SIZE=4096
CFG_CONFIG_SIZE=4096

BOOT_CODE_AND_CONFIG_SIZE=$(16KB)

#ifeq ($(FUNC_OTA_ON),1)
CFG_IMAGE0_SIZE=$(384KB)
#else
#CFG_IMAGE0_SIZE=$(472KB)
#CFG_IMAGE0_SIZE=$(428KB)
#CFG_IMAGE0_SIZE=$(476KB)
#endif

ifdef PROJECT_NAME
  ifeq ($(strip $(PROJECT_NAME)),UA402_HOMI_CUSTOMER)
  	 CFG_IMAGE0_SIZE:=$(476KB)
  	 UA_FLASH_CUSTOM_IMAGE_SIZE:=4
  endif
  ifeq ($(strip $(PROJECT_NAME)),UA402_HEKR_CUSTOMER)
  	 CFG_IMAGE0_SIZE:=$(476KB)
  	 UA_FLASH_CUSTOM_IMAGE_SIZE:=4
  endif
  ifeq ($(strip $(PROJECT_NAME)),UA402_WIFI_GN)
  	 CFG_IMAGE0_SIZE:=$(464KB)
  	 UA_FLASH_CUSTOM_IMAGE_SIZE:=16
  endif
  ifeq ($(strip $(PROJECT_NAME)),UA420_LampGate)
  	 CFG_IMAGE0_SIZE:=$(464KB)
  	 UA_FLASH_CUSTOM_IMAGE_SIZE:=16
  endif
  ifeq ($(strip $(PROJECT_NAME)),UA402_WifiModule)
  	 CFG_IMAGE0_SIZE:=$(464KB)
  	 UA_FLASH_CUSTOM_IMAGE_SIZE:=16
  endif
endif


ifeq ($(FUNC_OTA_ON),0)
CFG_IMAGE1_SIZE=0
endif

ifeq ($(FUNC_OTA_ON),1)
CFG_IMAGE1_SIZE=$(CFG_IMAGE0_SIZE)
endif

ifeq ($(UA_FLASH_CUSTOM_IMAGE_SIZE),4)
CFG_CUSTOMER_SIZE=$(4KB)
else
CFG_CUSTOMER_SIZE=$(16KB)
endif


ifeq ($(CFG_FLASH_SIZE),$(512KB))		#case1: 0x80000=512KB#
CFG_FILE_SYSTEM_SIZE=$(16KB)
endif

ifeq ($(CFG_FLASH_SIZE),$(1024KB))		#case1: 0x100000=1024KB#
CFG_FILE_SYSTEM_SIZE=$(64KB)
endif

ifeq ($(CFG_FLASH_SIZE),$(2048KB))		#case1: 0x200000=2048KB#
CFG_FILE_SYSTEM_SIZE=$(64KB)
endif

####	仅用于测试的, 目前没用到
CFG_ROM_SIZE=$(492KB)

####
ifeq ($(FUNC_OTA_ON),0)
##	Flash = 512KB
ifeq ($(UA_FLASH_CUSTOM_IMAGE_SIZE),8)		#8K
CFG_CUSTOMER_SIZE:=$(8KB)
CFG_IMAGE0_SIZE:=$(472KB)
CFG_ROM_SIZE:=$(488KB)
endif
ifeq ($(UA_FLASH_CUSTOM_IMAGE_SIZE),16)		#16K
CFG_CUSTOMER_SIZE:=$(16KB)
CFG_IMAGE0_SIZE:=$(464KB)
CFG_ROM_SIZE:=$(480KB)
endif

CFG_ROM_SIZE:=$(512KB)

endif

ifeq ($(FUNC_OTA_ON),1)

CFG_FLASH_SIZE=$(1024KB)

##	Flash = 1024KB
##	固定	8 + 4 + 4 + 16 + 64 KB
##	未验证 by u 20160914
ifeq ($(CFG_FLASH_SIZE),$(1024KB))		#case1: 0x100000=1024KB#
CFG_IMAGE0_SIZE:=$(464KB)
endif

ifeq ($(CFG_FLASH_SIZE),$(2048KB))		#case1: 0x200000=2048KB#
CFG_IMAGE0_SIZE=$(976KB)
endif

CFG_CUSTOMER_SIZE:=$(16KB)
CFG_IMAGE1_SIZE:=$(CFG_IMAGE0_SIZE)
CFG_FILE_SYSTEM_SIZE:=$(64KB)

endif

####


FLASH_TOTAL_USAGE=$(shell echo $(CFG_BOOT_SECTOR_SIZE)\+$(CFG_MP_SIZE)\+$(CFG_CONFIG_SIZE)\+$(CFG_IMAGE0_SIZE)\+$(CFG_IMAGE1_SIZE)\+$(CFG_CUSTOMER_SIZE) | bc)


ifeq ($(shell test $(FLASH_TOTAL_USAGE) -gt $(CFG_FLASH_SIZE) ; echo $$?),0)
$(error ***Error  FLASH_TOTAL_USAGE=$(FLASH_TOTAL_USAGE) > $(CFG_FLASH_SIZE)=CFG_FLASH_SIZE ***)
endif


PRODUCT_OPTIONS = -DFUNC_OTA_ON=$(FUNC_OTA_ON) -DCFG_FLASH_SIZE=$(CFG_FLASH_SIZE) -DCFG_BOOT_SECTOR_SIZE=$(CFG_BOOT_SECTOR_SIZE) -DCFG_MP_SIZE=$(CFG_MP_SIZE) -DCFG_CONFIG_SIZE=$(CFG_CONFIG_SIZE) -DCFG_IMAGE0_SIZE=$(CFG_IMAGE0_SIZE)  -DCFG_IMAGE1_SIZE=$(CFG_IMAGE1_SIZE) -DCFG_CUSTOMER_SIZE=$(CFG_CUSTOMER_SIZE) -DCFG_FILE_SYSTEM_SIZE=$(CFG_FILE_SYSTEM_SIZE)



####	Custom Define Macro
ifdef	D_CFLAG_CUSTOM
CFLAGS += $(D_CFLAG_CUSTOM)
endif
####



