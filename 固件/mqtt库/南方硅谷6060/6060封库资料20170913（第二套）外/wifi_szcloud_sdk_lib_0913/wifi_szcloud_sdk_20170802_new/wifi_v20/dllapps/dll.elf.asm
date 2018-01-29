
dll.elf:     file format elf32-littlearm


Disassembly of section .text:

00000000 <dumptest_a>:
   0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
   4:	e24dd014 	sub	sp, sp, #20
   8:	e58d0004 	str	r0, [sp, #4]
   c:	e58d1000 	str	r1, [sp]
  10:	e3a03000 	mov	r3, #0
  14:	e58d3008 	str	r3, [sp, #8]
  18:	e3a03000 	mov	r3, #0
  1c:	e58d300c 	str	r3, [sp, #12]
  20:	e59f0078 	ldr	r0, [pc, #120]	; a0 <dumptest_a+0xa0>
  24:	e59f1078 	ldr	r1, [pc, #120]	; a4 <dumptest_a+0xa4>
  28:	e59d2004 	ldr	r2, [sp, #4]
  2c:	e59d3000 	ldr	r3, [sp]
  30:	e59fc070 	ldr	ip, [pc, #112]	; a8 <dumptest_a+0xa8>
  34:	e1a0e00f 	mov	lr, pc
  38:	e1a0f00c 	mov	pc, ip
  3c:	e3a03000 	mov	r3, #0
  40:	e58d3008 	str	r3, [sp, #8]
  44:	ea00000b 	b	78 <dumptest_a+0x78>
  48:	e59d3008 	ldr	r3, [sp, #8]
  4c:	e59d2004 	ldr	r2, [sp, #4]
  50:	e0823003 	add	r3, r2, r3
  54:	e5d33000 	ldrb	r3, [r3]
  58:	e59f004c 	ldr	r0, [pc, #76]	; ac <dumptest_a+0xac>
  5c:	e1a01003 	mov	r1, r3
  60:	e59f3040 	ldr	r3, [pc, #64]	; a8 <dumptest_a+0xa8>
  64:	e1a0e00f 	mov	lr, pc
  68:	e1a0f003 	mov	pc, r3
  6c:	e59d3008 	ldr	r3, [sp, #8]
  70:	e2833001 	add	r3, r3, #1
  74:	e58d3008 	str	r3, [sp, #8]
  78:	e59d2008 	ldr	r2, [sp, #8]
  7c:	e59d3000 	ldr	r3, [sp]
  80:	e1520003 	cmp	r2, r3
  84:	baffffef 	blt	48 <dumptest_a+0x48>
  88:	e59f0020 	ldr	r0, [pc, #32]	; b0 <dumptest_a+0xb0>
  8c:	e59f3014 	ldr	r3, [pc, #20]	; a8 <dumptest_a+0xa8>
  90:	e1a0e00f 	mov	lr, pc
  94:	e1a0f003 	mov	pc, r3
  98:	e28dd014 	add	sp, sp, #20
  9c:	e49df004 	pop	{pc}		; (ldr pc, [sp], #4)
  a0:	00000284 	andeq	r0, r0, r4, lsl #5
  a4:	000002f0 	strdeq	r0, [r0], -r0	; <UNPREDICTABLE>
  a8:	00000000 	andeq	r0, r0, r0
  ac:	000002b0 			; <UNDEFINED> instruction: 0x000002b0
  b0:	000002b8 			; <UNDEFINED> instruction: 0x000002b8

000000b4 <dumptest_b>:
  b4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
  b8:	e24dd014 	sub	sp, sp, #20
  bc:	e58d0004 	str	r0, [sp, #4]
  c0:	e58d1000 	str	r1, [sp]
  c4:	e3a03000 	mov	r3, #0
  c8:	e58d3008 	str	r3, [sp, #8]
  cc:	e3a03000 	mov	r3, #0
  d0:	e58d300c 	str	r3, [sp, #12]
  d4:	e59f0078 	ldr	r0, [pc, #120]	; 154 <dumptest_b+0xa0>
  d8:	e59f1078 	ldr	r1, [pc, #120]	; 158 <dumptest_b+0xa4>
  dc:	e59d2004 	ldr	r2, [sp, #4]
  e0:	e59d3000 	ldr	r3, [sp]
  e4:	e59fc070 	ldr	ip, [pc, #112]	; 15c <dumptest_b+0xa8>
  e8:	e1a0e00f 	mov	lr, pc
  ec:	e1a0f00c 	mov	pc, ip
  f0:	e3a03000 	mov	r3, #0
  f4:	e58d3008 	str	r3, [sp, #8]
  f8:	ea00000b 	b	12c <dumptest_b+0x78>
  fc:	e59d3008 	ldr	r3, [sp, #8]
 100:	e59d2004 	ldr	r2, [sp, #4]
 104:	e0823003 	add	r3, r2, r3
 108:	e5d33000 	ldrb	r3, [r3]
 10c:	e59f004c 	ldr	r0, [pc, #76]	; 160 <dumptest_b+0xac>
 110:	e1a01003 	mov	r1, r3
 114:	e59f3040 	ldr	r3, [pc, #64]	; 15c <dumptest_b+0xa8>
 118:	e1a0e00f 	mov	lr, pc
 11c:	e1a0f003 	mov	pc, r3
 120:	e59d3008 	ldr	r3, [sp, #8]
 124:	e2833001 	add	r3, r3, #1
 128:	e58d3008 	str	r3, [sp, #8]
 12c:	e59d2008 	ldr	r2, [sp, #8]
 130:	e59d3000 	ldr	r3, [sp]
 134:	e1520003 	cmp	r2, r3
 138:	baffffef 	blt	fc <dumptest_b+0x48>
 13c:	e59f0020 	ldr	r0, [pc, #32]	; 164 <dumptest_b+0xb0>
 140:	e59f3014 	ldr	r3, [pc, #20]	; 15c <dumptest_b+0xa8>
 144:	e1a0e00f 	mov	lr, pc
 148:	e1a0f003 	mov	pc, r3
 14c:	e28dd014 	add	sp, sp, #20
 150:	e49df004 	pop	{pc}		; (ldr pc, [sp], #4)
 154:	00000284 	andeq	r0, r0, r4, lsl #5
 158:	000002fc 	strdeq	r0, [r0], -ip
 15c:	00000000 	andeq	r0, r0, r0
 160:	000002b0 			; <UNDEFINED> instruction: 0x000002b0
 164:	000002b8 			; <UNDEFINED> instruction: 0x000002b8

00000168 <process_thread_hello_world_process>:
 168:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
 16c:	e24dd01c 	sub	sp, sp, #28
 170:	e58d000c 	str	r0, [sp, #12]
 174:	e1a03001 	mov	r3, r1
 178:	e58d2004 	str	r2, [sp, #4]
 17c:	e5cd300b 	strb	r3, [sp, #11]
 180:	e3a03001 	mov	r3, #1
 184:	e5cd3017 	strb	r3, [sp, #23]
 188:	e59d300c 	ldr	r3, [sp, #12]
 18c:	e5d32000 	ldrb	r2, [r3]
 190:	e5d33001 	ldrb	r3, [r3, #1]
 194:	e1823403 	orr	r3, r2, r3, lsl #8
 198:	e3530000 	cmp	r3, #0
 19c:	1a000021 	bne	228 <process_thread_hello_world_process+0xc0>
 1a0:	e59f00a8 	ldr	r0, [pc, #168]	; 250 <process_thread_hello_world_process+0xe8>
 1a4:	e59f30a8 	ldr	r3, [pc, #168]	; 254 <process_thread_hello_world_process+0xec>
 1a8:	e1a0e00f 	mov	lr, pc
 1ac:	e1a0f003 	mov	pc, r3
 1b0:	e3a03004 	mov	r3, #4
 1b4:	e5933000 	ldr	r3, [r3]
 1b8:	e59f0098 	ldr	r0, [pc, #152]	; 258 <process_thread_hello_world_process+0xf0>
 1bc:	e1a01003 	mov	r1, r3
 1c0:	e59f308c 	ldr	r3, [pc, #140]	; 254 <process_thread_hello_world_process+0xec>
 1c4:	e1a0e00f 	mov	lr, pc
 1c8:	e1a0f003 	mov	pc, r3
 1cc:	e3a03004 	mov	r3, #4
 1d0:	e5932000 	ldr	r2, [r3]
 1d4:	e59f3080 	ldr	r3, [pc, #128]	; 25c <process_thread_hello_world_process+0xf4>
 1d8:	e1520003 	cmp	r2, r3
 1dc:	9a000002 	bls	1ec <process_thread_hello_world_process+0x84>
 1e0:	e59f0078 	ldr	r0, [pc, #120]	; 260 <process_thread_hello_world_process+0xf8>
 1e4:	e3a01008 	mov	r1, #8
 1e8:	ebfffffe 	bl	0 <dumptest_a>
 1ec:	e3a0300c 	mov	r3, #12
 1f0:	e5933000 	ldr	r3, [r3]
 1f4:	e59f0068 	ldr	r0, [pc, #104]	; 264 <process_thread_hello_world_process+0xfc>
 1f8:	e1a01003 	mov	r1, r3
 1fc:	e59f3050 	ldr	r3, [pc, #80]	; 254 <process_thread_hello_world_process+0xec>
 200:	e1a0e00f 	mov	lr, pc
 204:	e1a0f003 	mov	pc, r3
 208:	e3a0300c 	mov	r3, #12
 20c:	e5932000 	ldr	r2, [r3]
 210:	e59f3050 	ldr	r3, [pc, #80]	; 268 <process_thread_hello_world_process+0x100>
 214:	e1520003 	cmp	r2, r3
 218:	8a000002 	bhi	228 <process_thread_hello_world_process+0xc0>
 21c:	e59f0048 	ldr	r0, [pc, #72]	; 26c <process_thread_hello_world_process+0x104>
 220:	e3a0100a 	mov	r1, #10
 224:	ebfffffe 	bl	b4 <dumptest_b>
 228:	e3a03000 	mov	r3, #0
 22c:	e5cd3017 	strb	r3, [sp, #23]
 230:	e59d300c 	ldr	r3, [sp, #12]
 234:	e3a02000 	mov	r2, #0
 238:	e5c32000 	strb	r2, [r3]
 23c:	e5c32001 	strb	r2, [r3, #1]
 240:	e3a03003 	mov	r3, #3
 244:	e1a00003 	mov	r0, r3
 248:	e28dd01c 	add	sp, sp, #28
 24c:	e49df004 	pop	{pc}		; (ldr pc, [sp], #4)
 250:	000002bc 			; <UNDEFINED> instruction: 0x000002bc
 254:	00000000 	andeq	r0, r0, r0
 258:	000002c8 	andeq	r0, r0, r8, asr #5
 25c:	5e11aa10 	vmovpl	sl, s2
 260:	00000000 	andeq	r0, r0, r0
 264:	000002dc 	ldrdeq	r0, [r0], -ip
 268:	e59ff014 	ldr	pc, [pc, #20]	; 284 <autostart_processes+0x8>
 26c:	00000000 	andeq	r0, r0, r0
 270:	206c6c64 	rsbcs	r6, ip, r4, ror #24
 274:	636f7270 	cmnvs	pc, #112, 4
 278:	00737365 	rsbseq	r7, r3, r5, ror #6

0000027c <autostart_processes>:
	...
 284:	2d2d2d2d 	stccs	13, cr2, [sp, #-180]!	; 0xffffff4c
 288:	2d2d2d2d 	stccs	13, cr2, [sp, #-180]!	; 0xffffff4c
 28c:	253c2d2d 	ldrcs	r2, [ip, #-3373]!	; 0xd2d
 290:	61203e73 	teqvs	r0, r3, ror lr
 294:	3d726464 	cfldrdcc	mvd6, [r2, #-400]!	; 0xfffffe70
 298:	78257830 	stmdavc	r5!, {r4, r5, fp, ip, sp, lr}
 29c:	7a697320 	bvc	1a5cf24 <autostart_processes+0x1a5cca8>
 2a0:	64253d65 	strtvs	r3, [r5], #-3429	; 0xd65
 2a4:	2d2d2d2d 	stccs	13, cr2, [sp, #-180]!	; 0xffffff4c
 2a8:	2d2d2d2d 	stccs	13, cr2, [sp, #-180]!	; 0xffffff4c
 2ac:	000a2d2d 	andeq	r2, sl, sp, lsr #26
 2b0:	32257830 	eorcc	r7, r5, #48, 16	; 0x300000
 2b4:	00002c78 	andeq	r2, r0, r8, ror ip
 2b8:	0000000a 	andeq	r0, r0, sl
 2bc:	6c6c6568 	cfstr64vs	mvdx6, [ip], #-416	; 0xfffffe60
 2c0:	6c64206f 	stclvs	0, cr2, [r4], #-444	; 0xfffffe44
 2c4:	000a786c 	andeq	r7, sl, ip, ror #16
 2c8:	33474552 	movtcc	r4, #30034	; 0x7552
 2cc:	78302832 	ldmdavc	r0!, {r1, r4, r5, fp, sp}
 2d0:	303d2934 	eorscc	r2, sp, r4, lsr r9
 2d4:	0a782578 	beq	1e098bc <autostart_processes+0x1e09640>
 2d8:	00000000 	andeq	r0, r0, r0
 2dc:	33474552 	movtcc	r4, #30034	; 0x7552
 2e0:	78302832 	ldmdavc	r0!, {r1, r4, r5, fp, sp}
 2e4:	303d2963 	eorscc	r2, sp, r3, ror #18
 2e8:	0a782578 	beq	1e098d0 <autostart_processes+0x1e09654>
 2ec:	00000000 	andeq	r0, r0, r0
 2f0:	706d7564 	rsbvc	r7, sp, r4, ror #10
 2f4:	74736574 	ldrbtvc	r6, [r3], #-1396	; 0x574
 2f8:	0000615f 	andeq	r6, r0, pc, asr r1
 2fc:	706d7564 	rsbvc	r7, sp, r4, ror #10
 300:	74736574 	ldrbtvc	r6, [r3], #-1396	; 0x574
 304:	0000625f 	andeq	r6, r0, pc, asr r2

Disassembly of section .data:

00000000 <hello_world_process>:
   0:	00000000 	andeq	r0, r0, r0
   4:	00000270 	andeq	r0, r0, r0, ror r2
	...

00000010 <array_a>:
  10:	03020100 	movweq	r0, #8448	; 0x2100
  14:	07060504 	streq	r0, [r6, -r4, lsl #10]

00000018 <array_b>:
  18:	0b0a0908 	bleq	282440 <autostart_processes+0x2821c4>
  1c:	0f0e0d0c 	svceq	0x000e0d0c
  20:	00001110 	andeq	r1, r0, r0, lsl r1

Disassembly of section .bss:

00000000 <process_para>:
   0:	00000000 	andeq	r0, r0, r0

Disassembly of section .comment:

00000000 <.comment>:
   0:	43434700 	movtmi	r4, #14080	; 0x3700
   4:	4728203a 			; <UNDEFINED> instruction: 0x4728203a
   8:	5420554e 	strtpl	r5, [r0], #-1358	; 0x54e
   c:	736c6f6f 	cmnvc	ip, #444	; 0x1bc
  10:	726f6620 	rsbvc	r6, pc, #32, 12	; 0x2000000
  14:	4d524120 	ldfmie	f4, [r2, #-128]	; 0xffffff80
  18:	626d4520 	rsbvs	r4, sp, #32, 10	; 0x8000000
  1c:	65646465 	strbvs	r6, [r4, #-1125]!	; 0x465
  20:	72502064 	subsvc	r2, r0, #100	; 0x64
  24:	7365636f 	cmnvc	r5, #-1140850687	; 0xbc000001
  28:	73726f73 	cmnvc	r2, #460	; 0x1cc
  2c:	2e342029 	cdpcs	0, 3, cr2, cr4, cr9, {1}
  30:	20342e38 	eorscs	r2, r4, r8, lsr lr
  34:	34313032 	ldrtcc	r3, [r1], #-50	; 0x32
  38:	35323730 	ldrcc	r3, [r2, #-1840]!	; 0x730
  3c:	65722820 	ldrbvs	r2, [r2, #-2080]!	; 0x820
  40:	7361656c 	cmnvc	r1, #108, 10	; 0x1b000000
  44:	5b202965 	blpl	80a5e0 <autostart_processes+0x80a364>
  48:	2f4d5241 	svccs	0x004d5241
  4c:	65626d65 	strbvs	r6, [r2, #-3429]!	; 0xd65
  50:	64656464 	strbtvs	r6, [r5], #-1124	; 0x464
  54:	385f342d 	ldmdacc	pc, {r0, r2, r3, r5, sl, ip, sp}^	; <UNPREDICTABLE>
  58:	6172622d 	cmnvs	r2, sp, lsr #4
  5c:	2068636e 	rsbcs	r6, r8, lr, ror #6
  60:	69766572 	ldmdbvs	r6!, {r1, r4, r5, r6, r8, sl, sp, lr}^
  64:	6e6f6973 	mcrvs	9, 3, r6, cr15, cr3, {3}
  68:	33313220 	teqcc	r1, #32, 4
  6c:	5d373431 	cfldrspl	mvf3, [r7, #-196]!	; 0xffffff3c
	...

Disassembly of section .ARM.attributes:

00000000 <.ARM.attributes>:
   0:	00002741 	andeq	r2, r0, r1, asr #14
   4:	61656100 	cmnvs	r5, r0, lsl #2
   8:	01006962 	tsteq	r0, r2, ror #18
   c:	0000001d 	andeq	r0, r0, sp, lsl r0
  10:	004d3305 	subeq	r3, sp, r5, lsl #6
  14:	01080106 	tsteq	r8, r6, lsl #2
  18:	01140412 	tsteq	r4, r2, lsl r4
  1c:	03170115 	tsteq	r7, #1073741829	; 0x40000005
  20:	01190118 	tsteq	r9, r8, lsl r1
  24:	061e011a 			; <UNDEFINED> instruction: 0x061e011a
