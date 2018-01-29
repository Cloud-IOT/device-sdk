include ../Makefile.mk

CFLAGS += -I../core/sys/ -I../platform/cabrio-e -I../core/net -I../core -I../include -I../cpu/cabrio-e

mac:
#	gcc -m32 $(CFLAGS) gen_mac_bin.c -DMODULE_ID=2 -o gen_mac_bin.exe
####	./gen_mac_bin.exe 11-22-33-44-55-66
	./gen_mac_bin.exe 90-22-33-44-55-66

flash_layout_boot_8k:
	echo CFLAGS = $(CFLAGS)
	gcc $(CFLAGS) print_layout.c -DMODULE_ID=5 $(PRODUCT_OPTIONS) -o print_layout.exe
	@./print_layout.exe

