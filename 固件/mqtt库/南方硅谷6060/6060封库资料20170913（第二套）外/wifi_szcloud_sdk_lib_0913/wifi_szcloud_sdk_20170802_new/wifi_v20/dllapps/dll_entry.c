/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
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
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         A very simple Contiki application showing how Contiki programs look
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "contiki.h"
#include <stdio.h> /* For printf() */
#include <string.h>
/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "dll process");
AUTOSTART_PROCESSES(&hello_world_process);
/*---------------------------------------------------------------------------*/

#define REG32(addr)        (*(volatile unsigned int *)(addr))

U8 array_a[] = { 0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7 };
U8 array_b[] = { 0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf,0x10,0x11 };

void dumptest_a (U8 *pArray,int size)
{
    volatile int loop_i=0;
    volatile unsigned int *ptr=0;

    printf("----------<%s> addr=0x%x size=%d----------\n",__func__,pArray,size);
    for(loop_i=0;loop_i<size;loop_i++)
    {
        printf("0x%2x,",pArray[loop_i]);
    }
    printf("\n");

}


void dumptest_b(U8 *pArray,int size)
{
    volatile int loop_i=0;
    volatile unsigned int *ptr=0;


    printf("----------<%s> addr=0x%x size=%d----------\n",__func__,pArray,size);


    for(loop_i=0;loop_i<size;loop_i++)
    {
        printf("0x%2x,",pArray[loop_i]);
    }
    printf("\n");
}




PROCESS_THREAD(hello_world_process, ev, data)
{
    PROCESS_BEGIN();


    printf("hello dllx\n");

    printf("REG32(0x4)=0x%x\n",REG32(0x4)); //0x5e11aa10
    if(REG32(0x4) > 0x5e11aa10){
        dumptest_a(&array_a[0],sizeof(array_a));
    }

    printf("REG32(0xc)=0x%x\n",REG32(0xc)); //0xe59ff014
    if(REG32(0xc) < 0xe59ff015)
    {
        dumptest_b(&array_b[0],sizeof(array_b));
    } else {

    }

  
  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
