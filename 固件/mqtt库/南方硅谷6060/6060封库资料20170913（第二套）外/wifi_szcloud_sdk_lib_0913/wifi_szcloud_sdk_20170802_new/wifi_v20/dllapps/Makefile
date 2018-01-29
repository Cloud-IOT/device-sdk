include ../Makefile.mk


# -nostdlib -falign-functions=4 -falign-jumps -falign-loops -falign-labels
DBGFLAGS   = -g
OPTFLAGS   = -O0 -fomit-frame-pointer
INCLUDE    = -I../include -I../icomlib/include  -I../icomlib/include/sys -I../icomlib/include/dev -I../icomlib/include/net -I../icomlib/include/net/mac -I../icomlib/include/net/ieee80211_bss -I../icomlib/include/matrixssl  -I../icomlib/include/cloud


CPPFLAGS  += $(INCLUDE) $(OPTFLAGS) $(DBGFLAGS)
ARCH_FLAGS = -march=armv3m -mno-thumb-interwork 
CFLAGS    += -Wall -Wno-trigraphs -mlong-calls -mword-relocations -fno-common -fno-builtin $(CPPFLAGS) $(ARCH_FLAGS) -DMODULE_ID=$(MODULE_ID) -Werror=implicit-function-declaration -Werror=maybe-uninitialized
CFLAGS += -DAUTOSTART_ENABLE


CLEAN += %.elf $(CONTIKI_PROJECT).elf

### Compilation rules

# Don't treat %.elf %.bin as an imtermediate file!
.PRECIOUS: %.elf %.bin

OBJECTDIR=objs

CONTIKI_SRC = \
dll_entry.c

CONTIKI_OBJS=${addprefix $(OBJECTDIR)/,$(CONTIKI_SRC:.c=.o)}

all:
	@mkdir -p objs
	make dll.elf 

dll.elf: $(CONTIKI_OBJS)
	#echo "$(AR) $(AROPTS) $@ $^"
	#$(AR) $(AROPTS) $@ $^
	arm-none-eabi-ld -nostartfiles -T ./dll.ld -Map=dll.map --relocatable  $(CONTIKI_OBJS) -o $@
	$(STRIP) -K _init -K _fini --strip-unneeded -g -x $@
	xxd -i $@ > ../icomlib/platform/dll_array.c
	readelf -a $@ > $@.txt
	arm-none-eabi-objdump -D $@ > $@.asm 

print:
	echo "CONTIKI_SRC=$(CONTIKI_SRC)"
	echo "CONTIKI_OBJS=$(CONTIKI_OBJS)"

$(OBJECTDIR)/%.o: %.c | $(OBJECTDIR)
	@echo "-----------------make z1 target:$@ $(makefn3)-----------------"
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	/bin/rm -f *.o *.elf *.a *.bin *.asm *.map
	/bin/rm -rf $(OBJECTDIR)
#	/bin/rm -rf ../icomlib/platform/dll_array.c

