/*
 * Copyright (c) 2006, Swedish Institute of Computer Science
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include "contiki.h"
//#include <stdio.h>
//#include <string.h>
#include "ssv_lib.h"
#include "irq.h"


#if 0
#include "dev/spi.h"
#include "dev/xmem.h"
#include "dev/watchdog.h"
#endif

#include <ssv_types.h>
#include "cfs-coffee-arch.h"
#include "xmem.h"

#include "drv_flash.h"

#define DEBUG_XMEM (0)
#define DEBUG_XMEM_LEVEL2 (0)

#if (DEBUG_XMEM | DEBUG_XMEM_LEVEL2)
#define XMEM_PRINTF(...) printf(__VA_ARGS__)
#else
#define XMEM_PRINTF(...)
#endif

#define SPI_TEST_ADDR  (128*1024)


//#define SPI_CMD_READ_STATUS1 (0x05)
//#define SPI_CMD_READ_STATUS2 (0x35)


//#if DEBUG_XMEM
#if 0
int test4_flash_write()
{
#define TEST_DATA_SIZE 32
	int cnt=0;
	int max=1000;
	unsigned int au32data[TEST_DATA_SIZE];


	XMEM_PRINTF("test4: flash write\n");

	drv_flash_command_init();


	//write spi command
	//SET_SPI_CMD_ADDR();

	// flash_dump_data();

	//XMEM_PRINTF("--------------------------------------<before write-enable>\n");
	drv_flash_write_enable();
	drv_flash_wait_spi_status_regsiter();
	//XMEM_PRINTF("--------------------------------------<after write-enable>\n");

	//XMEM_PRINTF("--------------------------------------<before erase>\n");
	drv_flash_sector_erase(SPI_TEST_ADDR);

#if 0
    	XMEM_PRINTF("COMMAND_ADDR=%x\n", REG32(0xc0001028));
	XMEM_PRINTF("SRAM_ADDR=%x\n", REG32(0xc000100c));

	XMEM_PRINTF("--------------------------------------<after erase>\n");
	return 0;
#endif

	drv_flash_wait_spi_status_regsiter();
	//XMEM_PRINTF("--------------------------------------<after erase>\n");

	max = 1000000;
	for(cnt=0;cnt<max;cnt++){
		//XMEM_PRINTF("<%d/%d>wait erase done!!...\n",cnt,max);
		nop();
	}

	//flash_dump_data();

	drv_flash_write_enable();
	drv_flash_wait_spi_status_regsiter();

	XMEM_PRINTF("<test4_flash_write::page program>....\n");

#if 0	//check WEL bit
	return 0;
#endif

	for(cnt=0;cnt<TEST_DATA_SIZE;cnt++){
		au32data[cnt] = cnt + 1;
	}


	drv_flash_page_program( SPI_TEST_ADDR, &au32data[0], 16*4);
	drv_flash_wait_spi_status_regsiter();

	max = 1000000;
	for(cnt=0;cnt<max;cnt++){
		//XMEM_PRINTF("<%d/%d>wait erase done!!...\n",cnt,max);
		nop();
	}


	drv_dbg_flash_dump_data(SPI_TEST_ADDR,16);

	//flash_write_disable();

}
#endif

void cabrio_flash_init(void)
{
#if 0
    volatile unsigned int u32boot_reg;
    u32boot_reg = REG32(ADR_BOOT) & 0x1;

    if(0==u32boot_reg){
        u32flash_base_addr = FLASH_BOOT_FLASH_BASE_ADDR;        //flash base-address is 0x000-0000
    } else {
        u32flash_base_addr = SRAM_BOOT_FLASH_BASE_ADDR;         //flash base-address is 0x300-0000
    }
    //printf("\tcabrio_flash_init (boot_reg,u32flash_base_addr)=(0x%x,0x%x)\n",u32boot_reg, u32flash_base_addr);
#endif

    drv_flash_init();

}

U32 cabrio_flash_get_base_adddr()
{
    U32 u32flash_base_addr;
    u32flash_base_addr = drv_flash_get_base_addr();
    return u32flash_base_addr;
}

S32 cabrio_flash_read(U32 address, void *data, U32 length)
{
    S32 rlt=0;
    U32 u32flash_base_addr;

    u32flash_base_addr = drv_flash_get_base_addr();

    address = (address |u32flash_base_addr);

    XMEM_PRINTF("\tcabrio_flash_read (address,length)=(%x,%x)\n",address,length);
    //since cpu can read nor-flash dirctly in cbrio platform.

    memcpy(data, (const void *)(address), length);

    return rlt;
}


S32 cabrio_flash_write_only(U32 address, void *data, U32 length)
{
    S32 rlt=0;
    XMEM_PRINTF("\tcabrio_flash_write (addess,length)=(%x,%x)\n",address,length);

    irq_disable();

    rlt = drv_flash_write_enable();
    if(rlt!=0){
        goto error_exit;
    }

#if 0
    int i=0;
    for(i=0;i<300;i++){
    XMEM_PRINTF("<%d>............................\n",i);
    }
#endif

    rlt = drv_flash_page_program( address, data, length);

    if(rlt!=0){
        goto error_exit;
    }

error_exit:
    irq_enable();
    return rlt;
}

S32 cabrio_flash_read_erase_write_in_sector_range(U32 address, const void *data, U32 length)
{
    S32 rlt=0;
    U32 u32address_start;   //u32address_align_sector;
    U32 u32address_end;   //u32address_next_sector;
    U32 u32buffer_addr;
    U32 u32address_start_read;
    U32 u32write_size=256;

    U8* p_flash_cache_buffer = drv_flash_get_cache_addr();

    XMEM_PRINTF("\t***cabrio_flash_read_erase_write_in_sector_range (addess,length)=(%x,%x)\n",address,length);
    XMEM_PRINTF("\t\cabrio_flash_read_erase_write_in_sector_range buffer(start,end)=(%x,%x)\n",&p_flash_cache_buffer[0],&p_flash_cache_buffer[FLASH_PAGE_SIZE]);

    //clear buffer first
    memset(&p_flash_cache_buffer[0], 0,FLASH_PAGE_SIZE);

    //step1: flash read
    u32address_start = ((address>>12)<<12);  //align to 4KB
    u32address_end =  u32address_start + FLASH_PAGE_SIZE;


#if 1   //to-do remove drv_flash_get_base_addr() ?
    //u32address_start_read = u32address_start | u32flash_base_addr;
    u32address_start_read = u32address_start | drv_flash_get_base_addr();
#endif

#if 0
    memcpy(&flash_cache_buffer[0], (void*)u32address_start_read, FLASH_PAGE_SIZE);   //copy whole sector,
    memcpy(&flash_cache_buffer[address & 0xfff], (void*)data, length);
#endif

    memcpy(&p_flash_cache_buffer[0], (void*)u32address_start_read, FLASH_PAGE_SIZE);   //copy whole sector,
    memcpy(&p_flash_cache_buffer[address & 0xfff], (void*)data, length);


    //step2: flash erase
    rlt = drv_flash_write_enable();
    if(rlt!=0){
        XMEM_PRINTF("<ERR>cabrio_flash_read_erase_write_in_sector_range::drv_flash_write_enable rlt=%d\n",rlt);
        goto error_exit;
    }

    rlt = drv_flash_sector_erase(u32address_start);

    if(rlt!=0){
        XMEM_PRINTF("<ERR>cabrio_flash_read_erase_write_in_sector_range::drv_flash_sector_erase rlt=%d\n",rlt);
        goto error_exit;
    }

#if 0
    int i=0;
    for(i=0;i<300;i++){
    XMEM_PRINTF("<%d>............................\n",i);
    }
#endif

    //step3: flash write
    u32write_size = 256;
    u32buffer_addr = 0;

    for(address = u32address_start;address < u32address_end;){

        rlt = drv_flash_write_enable();
        if(rlt!=0){
            XMEM_PRINTF("<ERR>cabrio_flash_read_erase_write_in_sector_range::drv_flash_write_enable rlt=%d\n",rlt);
            goto error_exit;
        }

        XMEM_PRINTF("drv_flash_page_program (address,u32buffer_addr,u32write_size)=(%x,%x,%d)\n",address,u32buffer_addr,u32write_size);
        rlt = drv_flash_page_program( address, &p_flash_cache_buffer[u32buffer_addr], u32write_size);

        if(rlt!=0){
            XMEM_PRINTF("<ERR>cabrio_flash_read_erase_write_in_sector_range::drv_flash_page_program rlt=%d\n",rlt);
            goto error_exit;
        }

        address += u32write_size;
        u32buffer_addr += u32write_size;
    }

//exit:
    return rlt;

error_exit:
    if(rlt!=0){
        XMEM_PRINTF("cabrio_flash_read_erase_write_in_sector_range: error_exit! rlt=%d\n",rlt);
    }
    return rlt;
}



S32 cabrio_flash_read_erase_write(U32 address, const void *data, U32 length)
{
    S32 rlt = 0;
    U32 u32write_size=0;

    //address = address | u32flash_base_addr;

    XMEM_PRINTF("\t\cabrio_flash_read_erase_write >>> (addess,length)=(%x,%x)\n",address,length);


    irq_disable();

    while(length > 0){

        u32write_size = FLASH_PAGE_SIZE - (address & 0xfff);
        if(length < u32write_size){
            u32write_size = length;
        }
        XMEM_PRINTF("cabrio_flash_read_erase_write_in_sector_range (address,length,u32write_size)=(%x,%x,%x)\n",address,length,u32write_size);
        cabrio_flash_read_erase_write_in_sector_range( address, data, u32write_size);
        length -= u32write_size;
        address += u32write_size;
        data += u32write_size;
    }
//#endif
        //cabrio_flash_read_erase_write_in_sector_range(address,data,length);

    XMEM_PRINTF("\t\cabrio_flash_read_erase_write <<<\n",address,length);

    //XMEM_PRINTF("\t\irq_enable >>>\n");
    irq_enable();
    //XMEM_PRINTF("\t\irq_enable <<<\n");

    return rlt;
}


#if 0
int cabrio_flash_write_cache_buffer(U32 address, void *data, U32 length)
{
    drv_flash_write_cache_buffer( (address & 0xfff), data, length);
    return 0;
}
#endif


#if 0
unsigned int cabrio_flash_get_cache_buffer_address(){
    return (unsigned int)((&flash_cache_buffer[0]));
}
#endif


#if 0
int cabrio_flash_flush_cache_buffer(U32 address)
{
    int rlt=0;
    unsigned int u32address_start;   //u32address_align_sector;
    unsigned int u32address_end;   //u32address_next_sector;
    unsigned int u32buffer_addr;

     U32 u32write_size=256;

    printf("\t***cabrio_flash_flush_cache_buffer (addess,length)=(%x,%x)\n",address,FLASH_PAGE_SIZE);
    XMEM_PRINTF("\t\cabrio_flash_flush_cache_buffer buffer(start,end)=(%x,%x)\n",&flash_cache_buffer[0],&flash_cache_buffer[FLASH_PAGE_SIZE]);

    //step1: setup flash address
    u32address_start = ((address>>12)<<12);  //align to 4KB
    u32address_end =  u32address_start + FLASH_PAGE_SIZE;


    //step2: flash erase
    rlt = drv_flash_write_enable();
    if(rlt!=0){
        XMEM_PRINTF("<ERR>cabrio_flash_flush_cache_buffer::drv_flash_write_enable rlt=%d\n",rlt);
        goto error_exit;
    }

    rlt = drv_flash_sector_erase(u32address_start);

    if(rlt!=0){
        XMEM_PRINTF("<ERR>cabrio_flash_flush_cache_buffer::drv_flash_sector_erase rlt=%d\n",rlt);
        goto error_exit;
    }


#if 0
    int i=0;
    for(i=0;i<300;i++){
    XMEM_PRINTF("<%d>............................\n",i);
    }
#endif

    //step3: flash write
    u32write_size = 128;
    u32buffer_addr = 0;

    for(address = u32address_start;address < u32address_end;){

        rlt = drv_flash_write_enable();
        if(rlt!=0){
            XMEM_PRINTF("<ERR>cabrio_flash_flush_cache_buffer::drv_flash_write_enable rlt=%d\n",rlt);
            goto error_exit;
        }

        XMEM_PRINTF("drv_flash_page_program (address,u32buffer_addr,u32write_size)=(%x,%x,%d)\n",address,u32buffer_addr,u32write_size);
        rlt = drv_flash_page_program( address, &flash_cache_buffer[u32buffer_addr], u32write_size);

        if(rlt!=0){
            XMEM_PRINTF("<ERR>cabrio_flash_flush_cache_buffer::drv_flash_page_program rlt=%d\n",rlt);
            goto error_exit;
        }

        address += u32write_size;
        u32buffer_addr += u32write_size;
    }

//exit:
    return rlt;

error_exit:
    if(rlt!=0){
        XMEM_PRINTF("cabrio_flash_read_erase_write_in_sector_range: error_exit! rlt=%d\n",rlt);
    }
    return rlt;
}
#endif

S32 cabrio_flash_erase_marker_sector()
{
    S32 rlt=0;
//    S32 i=0;
//    U32 u32page_addr;

    XMEM_PRINTF("cabrio_flash_erase sector %d\n",sector);

    irq_disable();


    rlt = drv_flash_write_enable();
    if(rlt!=0){
        goto error_exit;
    }

    rlt = drv_flash_sector_erase(CABRIO_COFFEE_FS_MARKER_ADDRESS);

    if(rlt!=0){
        goto error_exit;
    }


    if(rlt!=0){
        goto error_exit;
    }

    error_exit:

    irq_enable();

    return rlt;
}


S32 cabrio_flash_erase(U8 sector)
{
#define CLEAN_BUF_SIZE  (256)    //current file descriptor size = 30, but I now clean 64 bytes!
    S32 rlt=0;
    S32 i=0;
    U32 u32page_addr;
    U8 buf[CLEAN_BUF_SIZE];  //it's something related to filie descriptor structure

    XMEM_PRINTF("cabrio_flash_erase sector %d\n",sector);

    irq_disable();

#if 0
    test4_flash_write();
    while(1);
#endif

    rlt = drv_flash_write_enable();
    if(rlt!=0){
        goto error_exit;
    }

    rlt = drv_flash_sector_erase(COFFEE_ADDRESS + COFFEE_SECTOR_SIZE * sector );

    if(rlt!=0){
        goto error_exit;
    }

#if 1
    for(i=0;i<CLEAN_BUF_SIZE;i++){
        buf[i] = 0x0;
    }

    for(u32page_addr=0;u32page_addr<COFFEE_SECTOR_SIZE;u32page_addr+=CLEAN_BUF_SIZE){

        rlt = drv_flash_write_enable();
        if(rlt!=0){
            goto error_exit;
        }

        //reset all to 0..............0
        rlt = drv_flash_page_program_for_erase((COFFEE_ADDRESS + COFFEE_SECTOR_SIZE * sector + u32page_addr), & buf[0],CLEAN_BUF_SIZE);
        if(rlt!=0){
            goto error_exit;
        }
    }
#endif

    if(rlt!=0){
        goto error_exit;
    }

    error_exit:

    irq_enable();

    return rlt;
}

