include ../Makefile.mk


# -nostdlib -falign-functions=4 -falign-jumps -falign-loops -falign-labels
DBGFLAGS   = -g
OPTFLAGS   = -Os -fomit-frame-pointer
INCLUDE    = -I../include -I../icomlib/include  -I../icomlib/include/sys -I../icomlib/include/dev -I../icomlib/include/net -I../icomlib/include/net/mac -I../icomlib/include/net/ieee80211_bss -I../icomlib/include/matrixssl  -I../icomlib/include/cloud -I../contikilib


CPPFLAGS  += $(INCLUDE) $(OPTFLAGS) $(DBGFLAGS)
ARCH_FLAGS = -march=armv3m -mno-thumb-interwork
CFLAGS    += -Wall -Wno-trigraphs -fno-builtin $(CPPFLAGS) $(ARCH_FLAGS) -fdata-sections -ffunction-sections -DMODULE_ID=$(MODULE_ID) -DFUNC_OTA_ON=$(FUNC_OTA_ON) -DBOOT_SECTOR_UPDATE=$(BOOT_SECTOR_UPDATE) -DDISABLE_ICOMM_DISCOVER=$(DISABLE_ICOMM_DISCOVER) -DENABLE_ICOMM_DEMO=$(ENABLE_ICOMM_DEMO) -DFUNC_CLOUD_ON=$(FUNC_CLOUD_ON) -DFUNC_SEND_RAW_DATA_ON=$(FUNC_SEND_RAW_DATA_ON) $(PRODUCT_OPTIONS) -Werror=implicit-function-declaration -Werror=maybe-uninitialized
#CFLAGS += -DAUTOSTART_ENABLE


CLEAN += %.elf $(CONTIKI_PROJECT).elf

### Compilation rules

# Don't treat %.elf %.bin as an imtermediate file!
.PRECIOUS: %.elf %.bin

OBJECTDIR=objs

CONTIKI_SRC = \
atcmd.c \
atcmd_ssl.c \
atcmd_mqtt.c \
bsp_info.c \
iperf_cmd.c \
socket_app.c \
soft_ap_init.c \
xmem.c 


ifeq ($(FUNC_OTA_ON), 1)
CONTIKI_SRC+= \
ota_api.c
endif

#ifeq ($(ENABLE_ICOMM_DEMO), 1)
ifndef AC_CLOUD_SUPPORT
CONTIKI_SRC+= 
endif

####============================================================================
#CONTIKI_SRC    += MyThread.c MyATCmd.c UAMisc.c UAVersion.c MyCustSave.c

#UACRC.c

#ifdef UACLOUD_SUPPORT
#  ifeq ($(strip $(UACLOUD_SUPPORT)),TRUE)
#    CONTIKI_SRC    += UACloud.c
#  endif
#endif

#ifdef UA_CLOUDMODULE_SUPPORT
#  ifeq ($(strip $(UA_CLOUDMODULE_SUPPORT)),TRUE)
#    CONTIKI_SRC    += UACloudModule.c
#    CONTIKI_SRC    += UASmartlink_Ext.c
#  endif
#endif

#ifdef UA_WIFI2UART_SUPPORT
#  ifeq ($(strip $(UA_WIFI2UART_SUPPORT)),TRUE)
#    CONTIKI_SRC    += UA_Wifi2Uart.c
#  endif
#endif

#ifdef UA_LAMPGATE_SUPPORT
#  ifeq ($(strip $(UA_LAMPGATE_SUPPORT)),TRUE)
#    CONTIKI_SRC    += UALampGate.c
#  endif
#endif

ifdef UA_KEY_DEMO_SUPPORT
  ifeq ($(strip $(UA_KEY_DEMO_SUPPORT)),TRUE)
 # 	 CONTIKI_SRC    += key.c
  endif
endif

#ifdef UA_TIME_CLOCK_SUPPORT
#  ifeq ($(strip $(UA_TIME_CLOCK_SUPPORT)),TRUE)
#  	 CONTIKI_SRC    += UA_TIME_clock.c
#  endif
#endif

#ifdef UA_SZCLOUD_SUPPORT
	#LNK_OBJS := szcloud/dt_mqtt.o
	LNK_OBJS += $(wildcard $(OBJECTDIR)/*.o)
	LNK_OBJS += $(wildcard $(OBJECTDIR)/szcloud/*.o)
	CONTIKI_SRC += szcloud/szcloud_intf.c szcloud/szcloud_uart.c
#endif

####============================================================================

CONTIKI_OBJS=${addprefix $(OBJECTDIR)/,$(CONTIKI_SRC:.c=.o) $(CONTIKI_SRC:.c=.o)}

all:
	@mkdir -p objs/szcloud objs
	-rm -f $(LNK_LIB)
	make icomapps.a

icomapps.a: $(CONTIKI_OBJS) $(LNK_LIB)
	echo "$(AR) $(AROPTS) $@ $(LNK_OBJS) (LNK_LIB) $^"
	$(AR) $(AROPTS) $@ $(LNK_OBJS) $^


print:
	echo "CONTIKI_SRC=$(CONTIKI_SRC)"
	echo "CONTIKI_OBJS=$(CONTIKI_OBJS)"
	echo "LNK_OBJS=$(LNK_OBJS)"

$(OBJECTDIR)/%.o: %.c | $(OBJECTDIR)
	@echo "-----------------make z1 target:$@ $(makefn3)-----------------"
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	/bin/rm -f *.a *.bin *.asm
#	/bin/rm -rf $(OBJECTDIR)
#	make -C able_cloud clean

fsdata: 
	cd webserver;./makefsdata -d httpd-fs -o httpd-fsdata.c;touch httpd-fs.c
