/**
 *	@file    aes.c
 *	@version 33ef80f (HEAD, tag: MATRIXSSL-3-7-2-OPEN, tag: MATRIXSSL-3-7-2-COMM, origin/master, origin/HEAD, master)
 *
 *	AES CBC block cipher implementation.
 */
/*
 *	Copyright (c) 2013-2015 INSIDE Secure Corporation
 *	Copyright (c) PeerSec Networks, 2002-2011
 *	All Rights Reserved
 *
 *	The latest version of this code is available at http://www.matrixssl.org
 *
 *	This software is open source; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This General Public License does NOT permit incorporating this software
 *	into proprietary programs.  If you are unable to comply with the GPL, a
 *	commercial license for this software may be purchased from INSIDE at
 *	http://www.insidesecure.com/eng/Company/Locations
 *
 *	This program is distributed in WITHOUT ANY WARRANTY; without even the
 *	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *	See the GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *	http://www.gnu.org/copyleft/gpl.html
 */
/******************************************************************************/

#include "cryptoApi.h"

#ifdef USE_AES
#ifndef USE_AES_CBC_EXTERNAL
/******************************************************************************/

/* The precomputed tables for AES */
/*
Te0[x] = S [x].[02, 01, 01, 03];
Te1[x] = S [x].[03, 02, 01, 01];
Te2[x] = S [x].[01, 03, 02, 01];
Te3[x] = S [x].[01, 01, 03, 02];
Te4[x] = S [x].[01, 01, 01, 01];

Td0[x] = Si[x].[0e, 09, 0d, 0b];
Td1[x] = Si[x].[0b, 0e, 09, 0d];
Td2[x] = Si[x].[0d, 0b, 0e, 09];
Td3[x] = Si[x].[09, 0d, 0b, 0e];
Td4[x] = Si[x].[01, 01, 01, 01];
*/

#define rot1(x) (((x) << 24) | ((x) >> 8))
#define rot2(x) (((x) << 16) | ((x) >> 16))
#define rot3(x) (((x) <<  8) | ((x) >> 24))

#define mt  0x80808080
#define ml  0x7f7f7f7f
#define mh  0xfefefefe
#define mm  0x1b1b1b1b
#define mul2(x,t)	((t)=((x)&mt), \
			((((x)+(x))&mh)^(((t)-((t)>>7))&mm)))

			#define inv_mix_col(x,f2,f4,f8,f9) (\
			(f2)=mul2(x,f2), \
			(f4)=mul2(f2,f4), \
			(f8)=mul2(f4,f8), \
			(f9)=(x)^(f8), \
			(f8)=((f2)^(f4)^(f8)), \
			(f2)^=(f9), \
			(f4)^=(f9), \
			(f8)^=rot3(f2), \
			(f8)^=rot2(f4), \
			(f8)^rot1(f9))

static const uint32 TE0[] = {
	0xc66363a5UL, 0xf87c7c84UL, 0xee777799UL, 0xf67b7b8dUL,
	0xfff2f20dUL, 0xd66b6bbdUL, 0xde6f6fb1UL, 0x91c5c554UL,
	0x60303050UL, 0x02010103UL, 0xce6767a9UL, 0x562b2b7dUL,
	0xe7fefe19UL, 0xb5d7d762UL, 0x4dababe6UL, 0xec76769aUL,
	0x8fcaca45UL, 0x1f82829dUL, 0x89c9c940UL, 0xfa7d7d87UL,
	0xeffafa15UL, 0xb25959ebUL, 0x8e4747c9UL, 0xfbf0f00bUL,
	0x41adadecUL, 0xb3d4d467UL, 0x5fa2a2fdUL, 0x45afafeaUL,
	0x239c9cbfUL, 0x53a4a4f7UL, 0xe4727296UL, 0x9bc0c05bUL,
	0x75b7b7c2UL, 0xe1fdfd1cUL, 0x3d9393aeUL, 0x4c26266aUL,
	0x6c36365aUL, 0x7e3f3f41UL, 0xf5f7f702UL, 0x83cccc4fUL,
	0x6834345cUL, 0x51a5a5f4UL, 0xd1e5e534UL, 0xf9f1f108UL,
	0xe2717193UL, 0xabd8d873UL, 0x62313153UL, 0x2a15153fUL,
	0x0804040cUL, 0x95c7c752UL, 0x46232365UL, 0x9dc3c35eUL,
	0x30181828UL, 0x379696a1UL, 0x0a05050fUL, 0x2f9a9ab5UL,
	0x0e070709UL, 0x24121236UL, 0x1b80809bUL, 0xdfe2e23dUL,
	0xcdebeb26UL, 0x4e272769UL, 0x7fb2b2cdUL, 0xea75759fUL,
	0x1209091bUL, 0x1d83839eUL, 0x582c2c74UL, 0x341a1a2eUL,
	0x361b1b2dUL, 0xdc6e6eb2UL, 0xb45a5aeeUL, 0x5ba0a0fbUL,
	0xa45252f6UL, 0x763b3b4dUL, 0xb7d6d661UL, 0x7db3b3ceUL,
	0x5229297bUL, 0xdde3e33eUL, 0x5e2f2f71UL, 0x13848497UL,
	0xa65353f5UL, 0xb9d1d168UL, 0x00000000UL, 0xc1eded2cUL,
	0x40202060UL, 0xe3fcfc1fUL, 0x79b1b1c8UL, 0xb65b5bedUL,
	0xd46a6abeUL, 0x8dcbcb46UL, 0x67bebed9UL, 0x7239394bUL,
	0x944a4adeUL, 0x984c4cd4UL, 0xb05858e8UL, 0x85cfcf4aUL,
	0xbbd0d06bUL, 0xc5efef2aUL, 0x4faaaae5UL, 0xedfbfb16UL,
	0x864343c5UL, 0x9a4d4dd7UL, 0x66333355UL, 0x11858594UL,
	0x8a4545cfUL, 0xe9f9f910UL, 0x04020206UL, 0xfe7f7f81UL,
	0xa05050f0UL, 0x783c3c44UL, 0x259f9fbaUL, 0x4ba8a8e3UL,
	0xa25151f3UL, 0x5da3a3feUL, 0x804040c0UL, 0x058f8f8aUL,
	0x3f9292adUL, 0x219d9dbcUL, 0x70383848UL, 0xf1f5f504UL,
	0x63bcbcdfUL, 0x77b6b6c1UL, 0xafdada75UL, 0x42212163UL,
	0x20101030UL, 0xe5ffff1aUL, 0xfdf3f30eUL, 0xbfd2d26dUL,
	0x81cdcd4cUL, 0x180c0c14UL, 0x26131335UL, 0xc3ecec2fUL,
	0xbe5f5fe1UL, 0x359797a2UL, 0x884444ccUL, 0x2e171739UL,
	0x93c4c457UL, 0x55a7a7f2UL, 0xfc7e7e82UL, 0x7a3d3d47UL,
	0xc86464acUL, 0xba5d5de7UL, 0x3219192bUL, 0xe6737395UL,
	0xc06060a0UL, 0x19818198UL, 0x9e4f4fd1UL, 0xa3dcdc7fUL,
	0x44222266UL, 0x542a2a7eUL, 0x3b9090abUL, 0x0b888883UL,
	0x8c4646caUL, 0xc7eeee29UL, 0x6bb8b8d3UL, 0x2814143cUL,
	0xa7dede79UL, 0xbc5e5ee2UL, 0x160b0b1dUL, 0xaddbdb76UL,
	0xdbe0e03bUL, 0x64323256UL, 0x743a3a4eUL, 0x140a0a1eUL,
	0x924949dbUL, 0x0c06060aUL, 0x4824246cUL, 0xb85c5ce4UL,
	0x9fc2c25dUL, 0xbdd3d36eUL, 0x43acacefUL, 0xc46262a6UL,
	0x399191a8UL, 0x319595a4UL, 0xd3e4e437UL, 0xf279798bUL,
	0xd5e7e732UL, 0x8bc8c843UL, 0x6e373759UL, 0xda6d6db7UL,
	0x018d8d8cUL, 0xb1d5d564UL, 0x9c4e4ed2UL, 0x49a9a9e0UL,
	0xd86c6cb4UL, 0xac5656faUL, 0xf3f4f407UL, 0xcfeaea25UL,
	0xca6565afUL, 0xf47a7a8eUL, 0x47aeaee9UL, 0x10080818UL,
	0x6fbabad5UL, 0xf0787888UL, 0x4a25256fUL, 0x5c2e2e72UL,
	0x381c1c24UL, 0x57a6a6f1UL, 0x73b4b4c7UL, 0x97c6c651UL,
	0xcbe8e823UL, 0xa1dddd7cUL, 0xe874749cUL, 0x3e1f1f21UL,
	0x964b4bddUL, 0x61bdbddcUL, 0x0d8b8b86UL, 0x0f8a8a85UL,
	0xe0707090UL, 0x7c3e3e42UL, 0x71b5b5c4UL, 0xcc6666aaUL,
	0x904848d8UL, 0x06030305UL, 0xf7f6f601UL, 0x1c0e0e12UL,
	0xc26161a3UL, 0x6a35355fUL, 0xae5757f9UL, 0x69b9b9d0UL,
	0x17868691UL, 0x99c1c158UL, 0x3a1d1d27UL, 0x279e9eb9UL,
	0xd9e1e138UL, 0xebf8f813UL, 0x2b9898b3UL, 0x22111133UL,
	0xd26969bbUL, 0xa9d9d970UL, 0x078e8e89UL, 0x339494a7UL,
	0x2d9b9bb6UL, 0x3c1e1e22UL, 0x15878792UL, 0xc9e9e920UL,
	0x87cece49UL, 0xaa5555ffUL, 0x50282878UL, 0xa5dfdf7aUL,
	0x038c8c8fUL, 0x59a1a1f8UL, 0x09898980UL, 0x1a0d0d17UL,
	0x65bfbfdaUL, 0xd7e6e631UL, 0x844242c6UL, 0xd06868b8UL,
	0x824141c3UL, 0x299999b0UL, 0x5a2d2d77UL, 0x1e0f0f11UL,
	0x7bb0b0cbUL, 0xa85454fcUL, 0x6dbbbbd6UL, 0x2c16163aUL,
};

static const uint32 Te4[] = {
	0x63636363UL, 0x7c7c7c7cUL, 0x77777777UL, 0x7b7b7b7bUL,
	0xf2f2f2f2UL, 0x6b6b6b6bUL, 0x6f6f6f6fUL, 0xc5c5c5c5UL,
	0x30303030UL, 0x01010101UL, 0x67676767UL, 0x2b2b2b2bUL,
	0xfefefefeUL, 0xd7d7d7d7UL, 0xababababUL, 0x76767676UL,
	0xcacacacaUL, 0x82828282UL, 0xc9c9c9c9UL, 0x7d7d7d7dUL,
	0xfafafafaUL, 0x59595959UL, 0x47474747UL, 0xf0f0f0f0UL,
	0xadadadadUL, 0xd4d4d4d4UL, 0xa2a2a2a2UL, 0xafafafafUL,
	0x9c9c9c9cUL, 0xa4a4a4a4UL, 0x72727272UL, 0xc0c0c0c0UL,
	0xb7b7b7b7UL, 0xfdfdfdfdUL, 0x93939393UL, 0x26262626UL,
	0x36363636UL, 0x3f3f3f3fUL, 0xf7f7f7f7UL, 0xccccccccUL,
	0x34343434UL, 0xa5a5a5a5UL, 0xe5e5e5e5UL, 0xf1f1f1f1UL,
	0x71717171UL, 0xd8d8d8d8UL, 0x31313131UL, 0x15151515UL,
	0x04040404UL, 0xc7c7c7c7UL, 0x23232323UL, 0xc3c3c3c3UL,
	0x18181818UL, 0x96969696UL, 0x05050505UL, 0x9a9a9a9aUL,
	0x07070707UL, 0x12121212UL, 0x80808080UL, 0xe2e2e2e2UL,
	0xebebebebUL, 0x27272727UL, 0xb2b2b2b2UL, 0x75757575UL,
	0x09090909UL, 0x83838383UL, 0x2c2c2c2cUL, 0x1a1a1a1aUL,
	0x1b1b1b1bUL, 0x6e6e6e6eUL, 0x5a5a5a5aUL, 0xa0a0a0a0UL,
	0x52525252UL, 0x3b3b3b3bUL, 0xd6d6d6d6UL, 0xb3b3b3b3UL,
	0x29292929UL, 0xe3e3e3e3UL, 0x2f2f2f2fUL, 0x84848484UL,
	0x53535353UL, 0xd1d1d1d1UL, 0x00000000UL, 0xededededUL,
	0x20202020UL, 0xfcfcfcfcUL, 0xb1b1b1b1UL, 0x5b5b5b5bUL,
	0x6a6a6a6aUL, 0xcbcbcbcbUL, 0xbebebebeUL, 0x39393939UL,
	0x4a4a4a4aUL, 0x4c4c4c4cUL, 0x58585858UL, 0xcfcfcfcfUL,
	0xd0d0d0d0UL, 0xefefefefUL, 0xaaaaaaaaUL, 0xfbfbfbfbUL,
	0x43434343UL, 0x4d4d4d4dUL, 0x33333333UL, 0x85858585UL,
	0x45454545UL, 0xf9f9f9f9UL, 0x02020202UL, 0x7f7f7f7fUL,
	0x50505050UL, 0x3c3c3c3cUL, 0x9f9f9f9fUL, 0xa8a8a8a8UL,
	0x51515151UL, 0xa3a3a3a3UL, 0x40404040UL, 0x8f8f8f8fUL,
	0x92929292UL, 0x9d9d9d9dUL, 0x38383838UL, 0xf5f5f5f5UL,
	0xbcbcbcbcUL, 0xb6b6b6b6UL, 0xdadadadaUL, 0x21212121UL,
	0x10101010UL, 0xffffffffUL, 0xf3f3f3f3UL, 0xd2d2d2d2UL,
	0xcdcdcdcdUL, 0x0c0c0c0cUL, 0x13131313UL, 0xececececUL,
	0x5f5f5f5fUL, 0x97979797UL, 0x44444444UL, 0x17171717UL,
	0xc4c4c4c4UL, 0xa7a7a7a7UL, 0x7e7e7e7eUL, 0x3d3d3d3dUL,
	0x64646464UL, 0x5d5d5d5dUL, 0x19191919UL, 0x73737373UL,
	0x60606060UL, 0x81818181UL, 0x4f4f4f4fUL, 0xdcdcdcdcUL,
	0x22222222UL, 0x2a2a2a2aUL, 0x90909090UL, 0x88888888UL,
	0x46464646UL, 0xeeeeeeeeUL, 0xb8b8b8b8UL, 0x14141414UL,
	0xdedededeUL, 0x5e5e5e5eUL, 0x0b0b0b0bUL, 0xdbdbdbdbUL,
	0xe0e0e0e0UL, 0x32323232UL, 0x3a3a3a3aUL, 0x0a0a0a0aUL,
	0x49494949UL, 0x06060606UL, 0x24242424UL, 0x5c5c5c5cUL,
	0xc2c2c2c2UL, 0xd3d3d3d3UL, 0xacacacacUL, 0x62626262UL,
	0x91919191UL, 0x95959595UL, 0xe4e4e4e4UL, 0x79797979UL,
	0xe7e7e7e7UL, 0xc8c8c8c8UL, 0x37373737UL, 0x6d6d6d6dUL,
	0x8d8d8d8dUL, 0xd5d5d5d5UL, 0x4e4e4e4eUL, 0xa9a9a9a9UL,
	0x6c6c6c6cUL, 0x56565656UL, 0xf4f4f4f4UL, 0xeaeaeaeaUL,
	0x65656565UL, 0x7a7a7a7aUL, 0xaeaeaeaeUL, 0x08080808UL,
	0xbabababaUL, 0x78787878UL, 0x25252525UL, 0x2e2e2e2eUL,
	0x1c1c1c1cUL, 0xa6a6a6a6UL, 0xb4b4b4b4UL, 0xc6c6c6c6UL,
	0xe8e8e8e8UL, 0xddddddddUL, 0x74747474UL, 0x1f1f1f1fUL,
	0x4b4b4b4bUL, 0xbdbdbdbdUL, 0x8b8b8b8bUL, 0x8a8a8a8aUL,
	0x70707070UL, 0x3e3e3e3eUL, 0xb5b5b5b5UL, 0x66666666UL,
	0x48484848UL, 0x03030303UL, 0xf6f6f6f6UL, 0x0e0e0e0eUL,
	0x61616161UL, 0x35353535UL, 0x57575757UL, 0xb9b9b9b9UL,
	0x86868686UL, 0xc1c1c1c1UL, 0x1d1d1d1dUL, 0x9e9e9e9eUL,
	0xe1e1e1e1UL, 0xf8f8f8f8UL, 0x98989898UL, 0x11111111UL,
	0x69696969UL, 0xd9d9d9d9UL, 0x8e8e8e8eUL, 0x94949494UL,
	0x9b9b9b9bUL, 0x1e1e1e1eUL, 0x87878787UL, 0xe9e9e9e9UL,
	0xcecececeUL, 0x55555555UL, 0x28282828UL, 0xdfdfdfdfUL,
	0x8c8c8c8cUL, 0xa1a1a1a1UL, 0x89898989UL, 0x0d0d0d0dUL,
	0xbfbfbfbfUL, 0xe6e6e6e6UL, 0x42424242UL, 0x68686868UL,
	0x41414141UL, 0x99999999UL, 0x2d2d2d2dUL, 0x0f0f0f0fUL,
	0xb0b0b0b0UL, 0x54545454UL, 0xbbbbbbbbUL, 0x16161616UL,
};

static const uint32 TD0[] = {
	0x51f4a750UL, 0x7e416553UL, 0x1a17a4c3UL, 0x3a275e96UL,
	0x3bab6bcbUL, 0x1f9d45f1UL, 0xacfa58abUL, 0x4be30393UL,
	0x2030fa55UL, 0xad766df6UL, 0x88cc7691UL, 0xf5024c25UL,
	0x4fe5d7fcUL, 0xc52acbd7UL, 0x26354480UL, 0xb562a38fUL,
	0xdeb15a49UL, 0x25ba1b67UL, 0x45ea0e98UL, 0x5dfec0e1UL,
	0xc32f7502UL, 0x814cf012UL, 0x8d4697a3UL, 0x6bd3f9c6UL,
	0x038f5fe7UL, 0x15929c95UL, 0xbf6d7aebUL, 0x955259daUL,
	0xd4be832dUL, 0x587421d3UL, 0x49e06929UL, 0x8ec9c844UL,
	0x75c2896aUL, 0xf48e7978UL, 0x99583e6bUL, 0x27b971ddUL,
	0xbee14fb6UL, 0xf088ad17UL, 0xc920ac66UL, 0x7dce3ab4UL,
	0x63df4a18UL, 0xe51a3182UL, 0x97513360UL, 0x62537f45UL,
	0xb16477e0UL, 0xbb6bae84UL, 0xfe81a01cUL, 0xf9082b94UL,
	0x70486858UL, 0x8f45fd19UL, 0x94de6c87UL, 0x527bf8b7UL,
	0xab73d323UL, 0x724b02e2UL, 0xe31f8f57UL, 0x6655ab2aUL,
	0xb2eb2807UL, 0x2fb5c203UL, 0x86c57b9aUL, 0xd33708a5UL,
	0x302887f2UL, 0x23bfa5b2UL, 0x02036abaUL, 0xed16825cUL,
	0x8acf1c2bUL, 0xa779b492UL, 0xf307f2f0UL, 0x4e69e2a1UL,
	0x65daf4cdUL, 0x0605bed5UL, 0xd134621fUL, 0xc4a6fe8aUL,
	0x342e539dUL, 0xa2f355a0UL, 0x058ae132UL, 0xa4f6eb75UL,
	0x0b83ec39UL, 0x4060efaaUL, 0x5e719f06UL, 0xbd6e1051UL,
	0x3e218af9UL, 0x96dd063dUL, 0xdd3e05aeUL, 0x4de6bd46UL,
	0x91548db5UL, 0x71c45d05UL, 0x0406d46fUL, 0x605015ffUL,
	0x1998fb24UL, 0xd6bde997UL, 0x894043ccUL, 0x67d99e77UL,
	0xb0e842bdUL, 0x07898b88UL, 0xe7195b38UL, 0x79c8eedbUL,
	0xa17c0a47UL, 0x7c420fe9UL, 0xf8841ec9UL, 0x00000000UL,
	0x09808683UL, 0x322bed48UL, 0x1e1170acUL, 0x6c5a724eUL,
	0xfd0efffbUL, 0x0f853856UL, 0x3daed51eUL, 0x362d3927UL,
	0x0a0fd964UL, 0x685ca621UL, 0x9b5b54d1UL, 0x24362e3aUL,
	0x0c0a67b1UL, 0x9357e70fUL, 0xb4ee96d2UL, 0x1b9b919eUL,
	0x80c0c54fUL, 0x61dc20a2UL, 0x5a774b69UL, 0x1c121a16UL,
	0xe293ba0aUL, 0xc0a02ae5UL, 0x3c22e043UL, 0x121b171dUL,
	0x0e090d0bUL, 0xf28bc7adUL, 0x2db6a8b9UL, 0x141ea9c8UL,
	0x57f11985UL, 0xaf75074cUL, 0xee99ddbbUL, 0xa37f60fdUL,
	0xf701269fUL, 0x5c72f5bcUL, 0x44663bc5UL, 0x5bfb7e34UL,
	0x8b432976UL, 0xcb23c6dcUL, 0xb6edfc68UL, 0xb8e4f163UL,
	0xd731dccaUL, 0x42638510UL, 0x13972240UL, 0x84c61120UL,
	0x854a247dUL, 0xd2bb3df8UL, 0xaef93211UL, 0xc729a16dUL,
	0x1d9e2f4bUL, 0xdcb230f3UL, 0x0d8652ecUL, 0x77c1e3d0UL,
	0x2bb3166cUL, 0xa970b999UL, 0x119448faUL, 0x47e96422UL,
	0xa8fc8cc4UL, 0xa0f03f1aUL, 0x567d2cd8UL, 0x223390efUL,
	0x87494ec7UL, 0xd938d1c1UL, 0x8ccaa2feUL, 0x98d40b36UL,
	0xa6f581cfUL, 0xa57ade28UL, 0xdab78e26UL, 0x3fadbfa4UL,
	0x2c3a9de4UL, 0x5078920dUL, 0x6a5fcc9bUL, 0x547e4662UL,
	0xf68d13c2UL, 0x90d8b8e8UL, 0x2e39f75eUL, 0x82c3aff5UL,
	0x9f5d80beUL, 0x69d0937cUL, 0x6fd52da9UL, 0xcf2512b3UL,
	0xc8ac993bUL, 0x10187da7UL, 0xe89c636eUL, 0xdb3bbb7bUL,
	0xcd267809UL, 0x6e5918f4UL, 0xec9ab701UL, 0x834f9aa8UL,
	0xe6956e65UL, 0xaaffe67eUL, 0x21bccf08UL, 0xef15e8e6UL,
	0xbae79bd9UL, 0x4a6f36ceUL, 0xea9f09d4UL, 0x29b07cd6UL,
	0x31a4b2afUL, 0x2a3f2331UL, 0xc6a59430UL, 0x35a266c0UL,
	0x744ebc37UL, 0xfc82caa6UL, 0xe090d0b0UL, 0x33a7d815UL,
	0xf104984aUL, 0x41ecdaf7UL, 0x7fcd500eUL, 0x1791f62fUL,
	0x764dd68dUL, 0x43efb04dUL, 0xccaa4d54UL, 0xe49604dfUL,
	0x9ed1b5e3UL, 0x4c6a881bUL, 0xc12c1fb8UL, 0x4665517fUL,
	0x9d5eea04UL, 0x018c355dUL, 0xfa877473UL, 0xfb0b412eUL,
	0xb3671d5aUL, 0x92dbd252UL, 0xe9105633UL, 0x6dd64713UL,
	0x9ad7618cUL, 0x37a10c7aUL, 0x59f8148eUL, 0xeb133c89UL,
	0xcea927eeUL, 0xb761c935UL, 0xe11ce5edUL, 0x7a47b13cUL,
	0x9cd2df59UL, 0x55f2733fUL, 0x1814ce79UL, 0x73c737bfUL,
	0x53f7cdeaUL, 0x5ffdaa5bUL, 0xdf3d6f14UL, 0x7844db86UL,
	0xcaaff381UL, 0xb968c43eUL, 0x3824342cUL, 0xc2a3405fUL,
	0x161dc372UL, 0xbce2250cUL, 0x283c498bUL, 0xff0d9541UL,
	0x39a80171UL, 0x080cb3deUL, 0xd8b4e49cUL, 0x6456c190UL,
	0x7bcb8461UL, 0xd532b670UL, 0x486c5c74UL, 0xd0b85742UL,
};

static const uint32 Td4[] = {
	0x52525252UL, 0x09090909UL, 0x6a6a6a6aUL, 0xd5d5d5d5UL,
	0x30303030UL, 0x36363636UL, 0xa5a5a5a5UL, 0x38383838UL,
	0xbfbfbfbfUL, 0x40404040UL, 0xa3a3a3a3UL, 0x9e9e9e9eUL,
	0x81818181UL, 0xf3f3f3f3UL, 0xd7d7d7d7UL, 0xfbfbfbfbUL,
	0x7c7c7c7cUL, 0xe3e3e3e3UL, 0x39393939UL, 0x82828282UL,
	0x9b9b9b9bUL, 0x2f2f2f2fUL, 0xffffffffUL, 0x87878787UL,
	0x34343434UL, 0x8e8e8e8eUL, 0x43434343UL, 0x44444444UL,
	0xc4c4c4c4UL, 0xdedededeUL, 0xe9e9e9e9UL, 0xcbcbcbcbUL,
	0x54545454UL, 0x7b7b7b7bUL, 0x94949494UL, 0x32323232UL,
	0xa6a6a6a6UL, 0xc2c2c2c2UL, 0x23232323UL, 0x3d3d3d3dUL,
	0xeeeeeeeeUL, 0x4c4c4c4cUL, 0x95959595UL, 0x0b0b0b0bUL,
	0x42424242UL, 0xfafafafaUL, 0xc3c3c3c3UL, 0x4e4e4e4eUL,
	0x08080808UL, 0x2e2e2e2eUL, 0xa1a1a1a1UL, 0x66666666UL,
	0x28282828UL, 0xd9d9d9d9UL, 0x24242424UL, 0xb2b2b2b2UL,
	0x76767676UL, 0x5b5b5b5bUL, 0xa2a2a2a2UL, 0x49494949UL,
	0x6d6d6d6dUL, 0x8b8b8b8bUL, 0xd1d1d1d1UL, 0x25252525UL,
	0x72727272UL, 0xf8f8f8f8UL, 0xf6f6f6f6UL, 0x64646464UL,
	0x86868686UL, 0x68686868UL, 0x98989898UL, 0x16161616UL,
	0xd4d4d4d4UL, 0xa4a4a4a4UL, 0x5c5c5c5cUL, 0xccccccccUL,
	0x5d5d5d5dUL, 0x65656565UL, 0xb6b6b6b6UL, 0x92929292UL,
	0x6c6c6c6cUL, 0x70707070UL, 0x48484848UL, 0x50505050UL,
	0xfdfdfdfdUL, 0xededededUL, 0xb9b9b9b9UL, 0xdadadadaUL,
	0x5e5e5e5eUL, 0x15151515UL, 0x46464646UL, 0x57575757UL,
	0xa7a7a7a7UL, 0x8d8d8d8dUL, 0x9d9d9d9dUL, 0x84848484UL,
	0x90909090UL, 0xd8d8d8d8UL, 0xababababUL, 0x00000000UL,
	0x8c8c8c8cUL, 0xbcbcbcbcUL, 0xd3d3d3d3UL, 0x0a0a0a0aUL,
	0xf7f7f7f7UL, 0xe4e4e4e4UL, 0x58585858UL, 0x05050505UL,
	0xb8b8b8b8UL, 0xb3b3b3b3UL, 0x45454545UL, 0x06060606UL,
	0xd0d0d0d0UL, 0x2c2c2c2cUL, 0x1e1e1e1eUL, 0x8f8f8f8fUL,
	0xcacacacaUL, 0x3f3f3f3fUL, 0x0f0f0f0fUL, 0x02020202UL,
	0xc1c1c1c1UL, 0xafafafafUL, 0xbdbdbdbdUL, 0x03030303UL,
	0x01010101UL, 0x13131313UL, 0x8a8a8a8aUL, 0x6b6b6b6bUL,
	0x3a3a3a3aUL, 0x91919191UL, 0x11111111UL, 0x41414141UL,
	0x4f4f4f4fUL, 0x67676767UL, 0xdcdcdcdcUL, 0xeaeaeaeaUL,
	0x97979797UL, 0xf2f2f2f2UL, 0xcfcfcfcfUL, 0xcecececeUL,
	0xf0f0f0f0UL, 0xb4b4b4b4UL, 0xe6e6e6e6UL, 0x73737373UL,
	0x96969696UL, 0xacacacacUL, 0x74747474UL, 0x22222222UL,
	0xe7e7e7e7UL, 0xadadadadUL, 0x35353535UL, 0x85858585UL,
	0xe2e2e2e2UL, 0xf9f9f9f9UL, 0x37373737UL, 0xe8e8e8e8UL,
	0x1c1c1c1cUL, 0x75757575UL, 0xdfdfdfdfUL, 0x6e6e6e6eUL,
	0x47474747UL, 0xf1f1f1f1UL, 0x1a1a1a1aUL, 0x71717171UL,
	0x1d1d1d1dUL, 0x29292929UL, 0xc5c5c5c5UL, 0x89898989UL,
	0x6f6f6f6fUL, 0xb7b7b7b7UL, 0x62626262UL, 0x0e0e0e0eUL,
	0xaaaaaaaaUL, 0x18181818UL, 0xbebebebeUL, 0x1b1b1b1bUL,
	0xfcfcfcfcUL, 0x56565656UL, 0x3e3e3e3eUL, 0x4b4b4b4bUL,
	0xc6c6c6c6UL, 0xd2d2d2d2UL, 0x79797979UL, 0x20202020UL,
	0x9a9a9a9aUL, 0xdbdbdbdbUL, 0xc0c0c0c0UL, 0xfefefefeUL,
	0x78787878UL, 0xcdcdcdcdUL, 0x5a5a5a5aUL, 0xf4f4f4f4UL,
	0x1f1f1f1fUL, 0xddddddddUL, 0xa8a8a8a8UL, 0x33333333UL,
	0x88888888UL, 0x07070707UL, 0xc7c7c7c7UL, 0x31313131UL,
	0xb1b1b1b1UL, 0x12121212UL, 0x10101010UL, 0x59595959UL,
	0x27272727UL, 0x80808080UL, 0xececececUL, 0x5f5f5f5fUL,
	0x60606060UL, 0x51515151UL, 0x7f7f7f7fUL, 0xa9a9a9a9UL,
	0x19191919UL, 0xb5b5b5b5UL, 0x4a4a4a4aUL, 0x0d0d0d0dUL,
	0x2d2d2d2dUL, 0xe5e5e5e5UL, 0x7a7a7a7aUL, 0x9f9f9f9fUL,
	0x93939393UL, 0xc9c9c9c9UL, 0x9c9c9c9cUL, 0xefefefefUL,
	0xa0a0a0a0UL, 0xe0e0e0e0UL, 0x3b3b3b3bUL, 0x4d4d4d4dUL,
	0xaeaeaeaeUL, 0x2a2a2a2aUL, 0xf5f5f5f5UL, 0xb0b0b0b0UL,
	0xc8c8c8c8UL, 0xebebebebUL, 0xbbbbbbbbUL, 0x3c3c3c3cUL,
	0x83838383UL, 0x53535353UL, 0x99999999UL, 0x61616161UL,
	0x17171717UL, 0x2b2b2b2bUL, 0x04040404UL, 0x7e7e7e7eUL,
	0xbabababaUL, 0x77777777UL, 0xd6d6d6d6UL, 0x26262626UL,
	0xe1e1e1e1UL, 0x69696969UL, 0x14141414UL, 0x63636363UL,
	0x55555555UL, 0x21212121UL, 0x0c0c0c0cUL, 0x7d7d7d7dUL,
	0xe1f27f3aUL, 0xf5710fb0UL, 0xada0e5c4UL, 0x98e4c919UL
};

#ifdef PS_AES_IMPROVE_PERF_INCREASE_CODESIZE
static const uint32 Tks0[] = {
0x00000000UL, 0x0e090d0bUL, 0x1c121a16UL, 0x121b171dUL, 0x3824342cUL, 0x362d3927UL, 0x24362e3aUL, 0x2a3f2331UL,
0x70486858UL, 0x7e416553UL, 0x6c5a724eUL, 0x62537f45UL, 0x486c5c74UL, 0x4665517fUL, 0x547e4662UL, 0x5a774b69UL,
0xe090d0b0UL, 0xee99ddbbUL, 0xfc82caa6UL, 0xf28bc7adUL, 0xd8b4e49cUL, 0xd6bde997UL, 0xc4a6fe8aUL, 0xcaaff381UL,
0x90d8b8e8UL, 0x9ed1b5e3UL, 0x8ccaa2feUL, 0x82c3aff5UL, 0xa8fc8cc4UL, 0xa6f581cfUL, 0xb4ee96d2UL, 0xbae79bd9UL,
0xdb3bbb7bUL, 0xd532b670UL, 0xc729a16dUL, 0xc920ac66UL, 0xe31f8f57UL, 0xed16825cUL, 0xff0d9541UL, 0xf104984aUL,
0xab73d323UL, 0xa57ade28UL, 0xb761c935UL, 0xb968c43eUL, 0x9357e70fUL, 0x9d5eea04UL, 0x8f45fd19UL, 0x814cf012UL,
0x3bab6bcbUL, 0x35a266c0UL, 0x27b971ddUL, 0x29b07cd6UL, 0x038f5fe7UL, 0x0d8652ecUL, 0x1f9d45f1UL, 0x119448faUL,
0x4be30393UL, 0x45ea0e98UL, 0x57f11985UL, 0x59f8148eUL, 0x73c737bfUL, 0x7dce3ab4UL, 0x6fd52da9UL, 0x61dc20a2UL,
0xad766df6UL, 0xa37f60fdUL, 0xb16477e0UL, 0xbf6d7aebUL, 0x955259daUL, 0x9b5b54d1UL, 0x894043ccUL, 0x87494ec7UL,
0xdd3e05aeUL, 0xd33708a5UL, 0xc12c1fb8UL, 0xcf2512b3UL, 0xe51a3182UL, 0xeb133c89UL, 0xf9082b94UL, 0xf701269fUL,
0x4de6bd46UL, 0x43efb04dUL, 0x51f4a750UL, 0x5ffdaa5bUL, 0x75c2896aUL, 0x7bcb8461UL, 0x69d0937cUL, 0x67d99e77UL,
0x3daed51eUL, 0x33a7d815UL, 0x21bccf08UL, 0x2fb5c203UL, 0x058ae132UL, 0x0b83ec39UL, 0x1998fb24UL, 0x1791f62fUL,
0x764dd68dUL, 0x7844db86UL, 0x6a5fcc9bUL, 0x6456c190UL, 0x4e69e2a1UL, 0x4060efaaUL, 0x527bf8b7UL, 0x5c72f5bcUL,
0x0605bed5UL, 0x080cb3deUL, 0x1a17a4c3UL, 0x141ea9c8UL, 0x3e218af9UL, 0x302887f2UL, 0x223390efUL, 0x2c3a9de4UL,
0x96dd063dUL, 0x98d40b36UL, 0x8acf1c2bUL, 0x84c61120UL, 0xaef93211UL, 0xa0f03f1aUL, 0xb2eb2807UL, 0xbce2250cUL,
0xe6956e65UL, 0xe89c636eUL, 0xfa877473UL, 0xf48e7978UL, 0xdeb15a49UL, 0xd0b85742UL, 0xc2a3405fUL, 0xccaa4d54UL,
0x41ecdaf7UL, 0x4fe5d7fcUL, 0x5dfec0e1UL, 0x53f7cdeaUL, 0x79c8eedbUL, 0x77c1e3d0UL, 0x65daf4cdUL, 0x6bd3f9c6UL,
0x31a4b2afUL, 0x3fadbfa4UL, 0x2db6a8b9UL, 0x23bfa5b2UL, 0x09808683UL, 0x07898b88UL, 0x15929c95UL, 0x1b9b919eUL,
0xa17c0a47UL, 0xaf75074cUL, 0xbd6e1051UL, 0xb3671d5aUL, 0x99583e6bUL, 0x97513360UL, 0x854a247dUL, 0x8b432976UL,
0xd134621fUL, 0xdf3d6f14UL, 0xcd267809UL, 0xc32f7502UL, 0xe9105633UL, 0xe7195b38UL, 0xf5024c25UL, 0xfb0b412eUL,
0x9ad7618cUL, 0x94de6c87UL, 0x86c57b9aUL, 0x88cc7691UL, 0xa2f355a0UL, 0xacfa58abUL, 0xbee14fb6UL, 0xb0e842bdUL,
0xea9f09d4UL, 0xe49604dfUL, 0xf68d13c2UL, 0xf8841ec9UL, 0xd2bb3df8UL, 0xdcb230f3UL, 0xcea927eeUL, 0xc0a02ae5UL,
0x7a47b13cUL, 0x744ebc37UL, 0x6655ab2aUL, 0x685ca621UL, 0x42638510UL, 0x4c6a881bUL, 0x5e719f06UL, 0x5078920dUL,
0x0a0fd964UL, 0x0406d46fUL, 0x161dc372UL, 0x1814ce79UL, 0x322bed48UL, 0x3c22e043UL, 0x2e39f75eUL, 0x2030fa55UL,
0xec9ab701UL, 0xe293ba0aUL, 0xf088ad17UL, 0xfe81a01cUL, 0xd4be832dUL, 0xdab78e26UL, 0xc8ac993bUL, 0xc6a59430UL,
0x9cd2df59UL, 0x92dbd252UL, 0x80c0c54fUL, 0x8ec9c844UL, 0xa4f6eb75UL, 0xaaffe67eUL, 0xb8e4f163UL, 0xb6edfc68UL,
0x0c0a67b1UL, 0x02036abaUL, 0x10187da7UL, 0x1e1170acUL, 0x342e539dUL, 0x3a275e96UL, 0x283c498bUL, 0x26354480UL,
0x7c420fe9UL, 0x724b02e2UL, 0x605015ffUL, 0x6e5918f4UL, 0x44663bc5UL, 0x4a6f36ceUL, 0x587421d3UL, 0x567d2cd8UL,
0x37a10c7aUL, 0x39a80171UL, 0x2bb3166cUL, 0x25ba1b67UL, 0x0f853856UL, 0x018c355dUL, 0x13972240UL, 0x1d9e2f4bUL,
0x47e96422UL, 0x49e06929UL, 0x5bfb7e34UL, 0x55f2733fUL, 0x7fcd500eUL, 0x71c45d05UL, 0x63df4a18UL, 0x6dd64713UL,
0xd731dccaUL, 0xd938d1c1UL, 0xcb23c6dcUL, 0xc52acbd7UL, 0xef15e8e6UL, 0xe11ce5edUL, 0xf307f2f0UL, 0xfd0efffbUL,
0xa779b492UL, 0xa970b999UL, 0xbb6bae84UL, 0xb562a38fUL, 0x9f5d80beUL, 0x91548db5UL, 0x834f9aa8UL, 0x8d4697a3UL
};

static const uint32 Tks1[] = {
0x00000000UL, 0x0b0e090dUL, 0x161c121aUL, 0x1d121b17UL, 0x2c382434UL, 0x27362d39UL, 0x3a24362eUL, 0x312a3f23UL,
0x58704868UL, 0x537e4165UL, 0x4e6c5a72UL, 0x4562537fUL, 0x74486c5cUL, 0x7f466551UL, 0x62547e46UL, 0x695a774bUL,
0xb0e090d0UL, 0xbbee99ddUL, 0xa6fc82caUL, 0xadf28bc7UL, 0x9cd8b4e4UL, 0x97d6bde9UL, 0x8ac4a6feUL, 0x81caaff3UL,
0xe890d8b8UL, 0xe39ed1b5UL, 0xfe8ccaa2UL, 0xf582c3afUL, 0xc4a8fc8cUL, 0xcfa6f581UL, 0xd2b4ee96UL, 0xd9bae79bUL,
0x7bdb3bbbUL, 0x70d532b6UL, 0x6dc729a1UL, 0x66c920acUL, 0x57e31f8fUL, 0x5ced1682UL, 0x41ff0d95UL, 0x4af10498UL,
0x23ab73d3UL, 0x28a57adeUL, 0x35b761c9UL, 0x3eb968c4UL, 0x0f9357e7UL, 0x049d5eeaUL, 0x198f45fdUL, 0x12814cf0UL,
0xcb3bab6bUL, 0xc035a266UL, 0xdd27b971UL, 0xd629b07cUL, 0xe7038f5fUL, 0xec0d8652UL, 0xf11f9d45UL, 0xfa119448UL,
0x934be303UL, 0x9845ea0eUL, 0x8557f119UL, 0x8e59f814UL, 0xbf73c737UL, 0xb47dce3aUL, 0xa96fd52dUL, 0xa261dc20UL,
0xf6ad766dUL, 0xfda37f60UL, 0xe0b16477UL, 0xebbf6d7aUL, 0xda955259UL, 0xd19b5b54UL, 0xcc894043UL, 0xc787494eUL,
0xaedd3e05UL, 0xa5d33708UL, 0xb8c12c1fUL, 0xb3cf2512UL, 0x82e51a31UL, 0x89eb133cUL, 0x94f9082bUL, 0x9ff70126UL,
0x464de6bdUL, 0x4d43efb0UL, 0x5051f4a7UL, 0x5b5ffdaaUL, 0x6a75c289UL, 0x617bcb84UL, 0x7c69d093UL, 0x7767d99eUL,
0x1e3daed5UL, 0x1533a7d8UL, 0x0821bccfUL, 0x032fb5c2UL, 0x32058ae1UL, 0x390b83ecUL, 0x241998fbUL, 0x2f1791f6UL,
0x8d764dd6UL, 0x867844dbUL, 0x9b6a5fccUL, 0x906456c1UL, 0xa14e69e2UL, 0xaa4060efUL, 0xb7527bf8UL, 0xbc5c72f5UL,
0xd50605beUL, 0xde080cb3UL, 0xc31a17a4UL, 0xc8141ea9UL, 0xf93e218aUL, 0xf2302887UL, 0xef223390UL, 0xe42c3a9dUL,
0x3d96dd06UL, 0x3698d40bUL, 0x2b8acf1cUL, 0x2084c611UL, 0x11aef932UL, 0x1aa0f03fUL, 0x07b2eb28UL, 0x0cbce225UL,
0x65e6956eUL, 0x6ee89c63UL, 0x73fa8774UL, 0x78f48e79UL, 0x49deb15aUL, 0x42d0b857UL, 0x5fc2a340UL, 0x54ccaa4dUL,
0xf741ecdaUL, 0xfc4fe5d7UL, 0xe15dfec0UL, 0xea53f7cdUL, 0xdb79c8eeUL, 0xd077c1e3UL, 0xcd65daf4UL, 0xc66bd3f9UL,
0xaf31a4b2UL, 0xa43fadbfUL, 0xb92db6a8UL, 0xb223bfa5UL, 0x83098086UL, 0x8807898bUL, 0x9515929cUL, 0x9e1b9b91UL,
0x47a17c0aUL, 0x4caf7507UL, 0x51bd6e10UL, 0x5ab3671dUL, 0x6b99583eUL, 0x60975133UL, 0x7d854a24UL, 0x768b4329UL,
0x1fd13462UL, 0x14df3d6fUL, 0x09cd2678UL, 0x02c32f75UL, 0x33e91056UL, 0x38e7195bUL, 0x25f5024cUL, 0x2efb0b41UL,
0x8c9ad761UL, 0x8794de6cUL, 0x9a86c57bUL, 0x9188cc76UL, 0xa0a2f355UL, 0xabacfa58UL, 0xb6bee14fUL, 0xbdb0e842UL,
0xd4ea9f09UL, 0xdfe49604UL, 0xc2f68d13UL, 0xc9f8841eUL, 0xf8d2bb3dUL, 0xf3dcb230UL, 0xeecea927UL, 0xe5c0a02aUL,
0x3c7a47b1UL, 0x37744ebcUL, 0x2a6655abUL, 0x21685ca6UL, 0x10426385UL, 0x1b4c6a88UL, 0x065e719fUL, 0x0d507892UL,
0x640a0fd9UL, 0x6f0406d4UL, 0x72161dc3UL, 0x791814ceUL, 0x48322bedUL, 0x433c22e0UL, 0x5e2e39f7UL, 0x552030faUL,
0x01ec9ab7UL, 0x0ae293baUL, 0x17f088adUL, 0x1cfe81a0UL, 0x2dd4be83UL, 0x26dab78eUL, 0x3bc8ac99UL, 0x30c6a594UL,
0x599cd2dfUL, 0x5292dbd2UL, 0x4f80c0c5UL, 0x448ec9c8UL, 0x75a4f6ebUL, 0x7eaaffe6UL, 0x63b8e4f1UL, 0x68b6edfcUL,
0xb10c0a67UL, 0xba02036aUL, 0xa710187dUL, 0xac1e1170UL, 0x9d342e53UL, 0x963a275eUL, 0x8b283c49UL, 0x80263544UL,
0xe97c420fUL, 0xe2724b02UL, 0xff605015UL, 0xf46e5918UL, 0xc544663bUL, 0xce4a6f36UL, 0xd3587421UL, 0xd8567d2cUL,
0x7a37a10cUL, 0x7139a801UL, 0x6c2bb316UL, 0x6725ba1bUL, 0x560f8538UL, 0x5d018c35UL, 0x40139722UL, 0x4b1d9e2fUL,
0x2247e964UL, 0x2949e069UL, 0x345bfb7eUL, 0x3f55f273UL, 0x0e7fcd50UL, 0x0571c45dUL, 0x1863df4aUL, 0x136dd647UL,
0xcad731dcUL, 0xc1d938d1UL, 0xdccb23c6UL, 0xd7c52acbUL, 0xe6ef15e8UL, 0xede11ce5UL, 0xf0f307f2UL, 0xfbfd0effUL,
0x92a779b4UL, 0x99a970b9UL, 0x84bb6baeUL, 0x8fb562a3UL, 0xbe9f5d80UL, 0xb591548dUL, 0xa8834f9aUL, 0xa38d4697UL
};

static const uint32 Tks2[] = {
0x00000000UL, 0x0d0b0e09UL, 0x1a161c12UL, 0x171d121bUL, 0x342c3824UL, 0x3927362dUL, 0x2e3a2436UL, 0x23312a3fUL,
0x68587048UL, 0x65537e41UL, 0x724e6c5aUL, 0x7f456253UL, 0x5c74486cUL, 0x517f4665UL, 0x4662547eUL, 0x4b695a77UL,
0xd0b0e090UL, 0xddbbee99UL, 0xcaa6fc82UL, 0xc7adf28bUL, 0xe49cd8b4UL, 0xe997d6bdUL, 0xfe8ac4a6UL, 0xf381caafUL,
0xb8e890d8UL, 0xb5e39ed1UL, 0xa2fe8ccaUL, 0xaff582c3UL, 0x8cc4a8fcUL, 0x81cfa6f5UL, 0x96d2b4eeUL, 0x9bd9bae7UL,
0xbb7bdb3bUL, 0xb670d532UL, 0xa16dc729UL, 0xac66c920UL, 0x8f57e31fUL, 0x825ced16UL, 0x9541ff0dUL, 0x984af104UL,
0xd323ab73UL, 0xde28a57aUL, 0xc935b761UL, 0xc43eb968UL, 0xe70f9357UL, 0xea049d5eUL, 0xfd198f45UL, 0xf012814cUL,
0x6bcb3babUL, 0x66c035a2UL, 0x71dd27b9UL, 0x7cd629b0UL, 0x5fe7038fUL, 0x52ec0d86UL, 0x45f11f9dUL, 0x48fa1194UL,
0x03934be3UL, 0x0e9845eaUL, 0x198557f1UL, 0x148e59f8UL, 0x37bf73c7UL, 0x3ab47dceUL, 0x2da96fd5UL, 0x20a261dcUL,
0x6df6ad76UL, 0x60fda37fUL, 0x77e0b164UL, 0x7aebbf6dUL, 0x59da9552UL, 0x54d19b5bUL, 0x43cc8940UL, 0x4ec78749UL,
0x05aedd3eUL, 0x08a5d337UL, 0x1fb8c12cUL, 0x12b3cf25UL, 0x3182e51aUL, 0x3c89eb13UL, 0x2b94f908UL, 0x269ff701UL,
0xbd464de6UL, 0xb04d43efUL, 0xa75051f4UL, 0xaa5b5ffdUL, 0x896a75c2UL, 0x84617bcbUL, 0x937c69d0UL, 0x9e7767d9UL,
0xd51e3daeUL, 0xd81533a7UL, 0xcf0821bcUL, 0xc2032fb5UL, 0xe132058aUL, 0xec390b83UL, 0xfb241998UL, 0xf62f1791UL,
0xd68d764dUL, 0xdb867844UL, 0xcc9b6a5fUL, 0xc1906456UL, 0xe2a14e69UL, 0xefaa4060UL, 0xf8b7527bUL, 0xf5bc5c72UL,
0xbed50605UL, 0xb3de080cUL, 0xa4c31a17UL, 0xa9c8141eUL, 0x8af93e21UL, 0x87f23028UL, 0x90ef2233UL, 0x9de42c3aUL,
0x063d96ddUL, 0x0b3698d4UL, 0x1c2b8acfUL, 0x112084c6UL, 0x3211aef9UL, 0x3f1aa0f0UL, 0x2807b2ebUL, 0x250cbce2UL,
0x6e65e695UL, 0x636ee89cUL, 0x7473fa87UL, 0x7978f48eUL, 0x5a49deb1UL, 0x5742d0b8UL, 0x405fc2a3UL, 0x4d54ccaaUL,
0xdaf741ecUL, 0xd7fc4fe5UL, 0xc0e15dfeUL, 0xcdea53f7UL, 0xeedb79c8UL, 0xe3d077c1UL, 0xf4cd65daUL, 0xf9c66bd3UL,
0xb2af31a4UL, 0xbfa43fadUL, 0xa8b92db6UL, 0xa5b223bfUL, 0x86830980UL, 0x8b880789UL, 0x9c951592UL, 0x919e1b9bUL,
0x0a47a17cUL, 0x074caf75UL, 0x1051bd6eUL, 0x1d5ab367UL, 0x3e6b9958UL, 0x33609751UL, 0x247d854aUL, 0x29768b43UL,
0x621fd134UL, 0x6f14df3dUL, 0x7809cd26UL, 0x7502c32fUL, 0x5633e910UL, 0x5b38e719UL, 0x4c25f502UL, 0x412efb0bUL,
0x618c9ad7UL, 0x6c8794deUL, 0x7b9a86c5UL, 0x769188ccUL, 0x55a0a2f3UL, 0x58abacfaUL, 0x4fb6bee1UL, 0x42bdb0e8UL,
0x09d4ea9fUL, 0x04dfe496UL, 0x13c2f68dUL, 0x1ec9f884UL, 0x3df8d2bbUL, 0x30f3dcb2UL, 0x27eecea9UL, 0x2ae5c0a0UL,
0xb13c7a47UL, 0xbc37744eUL, 0xab2a6655UL, 0xa621685cUL, 0x85104263UL, 0x881b4c6aUL, 0x9f065e71UL, 0x920d5078UL,
0xd9640a0fUL, 0xd46f0406UL, 0xc372161dUL, 0xce791814UL, 0xed48322bUL, 0xe0433c22UL, 0xf75e2e39UL, 0xfa552030UL,
0xb701ec9aUL, 0xba0ae293UL, 0xad17f088UL, 0xa01cfe81UL, 0x832dd4beUL, 0x8e26dab7UL, 0x993bc8acUL, 0x9430c6a5UL,
0xdf599cd2UL, 0xd25292dbUL, 0xc54f80c0UL, 0xc8448ec9UL, 0xeb75a4f6UL, 0xe67eaaffUL, 0xf163b8e4UL, 0xfc68b6edUL,
0x67b10c0aUL, 0x6aba0203UL, 0x7da71018UL, 0x70ac1e11UL, 0x539d342eUL, 0x5e963a27UL, 0x498b283cUL, 0x44802635UL,
0x0fe97c42UL, 0x02e2724bUL, 0x15ff6050UL, 0x18f46e59UL, 0x3bc54466UL, 0x36ce4a6fUL, 0x21d35874UL, 0x2cd8567dUL,
0x0c7a37a1UL, 0x017139a8UL, 0x166c2bb3UL, 0x1b6725baUL, 0x38560f85UL, 0x355d018cUL, 0x22401397UL, 0x2f4b1d9eUL,
0x642247e9UL, 0x692949e0UL, 0x7e345bfbUL, 0x733f55f2UL, 0x500e7fcdUL, 0x5d0571c4UL, 0x4a1863dfUL, 0x47136dd6UL,
0xdccad731UL, 0xd1c1d938UL, 0xc6dccb23UL, 0xcbd7c52aUL, 0xe8e6ef15UL, 0xe5ede11cUL, 0xf2f0f307UL, 0xfffbfd0eUL,
0xb492a779UL, 0xb999a970UL, 0xae84bb6bUL, 0xa38fb562UL, 0x80be9f5dUL, 0x8db59154UL, 0x9aa8834fUL, 0x97a38d46UL
};

static const uint32 Tks3[] = {
0x00000000UL, 0x090d0b0eUL, 0x121a161cUL, 0x1b171d12UL, 0x24342c38UL, 0x2d392736UL, 0x362e3a24UL, 0x3f23312aUL,
0x48685870UL, 0x4165537eUL, 0x5a724e6cUL, 0x537f4562UL, 0x6c5c7448UL, 0x65517f46UL, 0x7e466254UL, 0x774b695aUL,
0x90d0b0e0UL, 0x99ddbbeeUL, 0x82caa6fcUL, 0x8bc7adf2UL, 0xb4e49cd8UL, 0xbde997d6UL, 0xa6fe8ac4UL, 0xaff381caUL,
0xd8b8e890UL, 0xd1b5e39eUL, 0xcaa2fe8cUL, 0xc3aff582UL, 0xfc8cc4a8UL, 0xf581cfa6UL, 0xee96d2b4UL, 0xe79bd9baUL,
0x3bbb7bdbUL, 0x32b670d5UL, 0x29a16dc7UL, 0x20ac66c9UL, 0x1f8f57e3UL, 0x16825cedUL, 0x0d9541ffUL, 0x04984af1UL,
0x73d323abUL, 0x7ade28a5UL, 0x61c935b7UL, 0x68c43eb9UL, 0x57e70f93UL, 0x5eea049dUL, 0x45fd198fUL, 0x4cf01281UL,
0xab6bcb3bUL, 0xa266c035UL, 0xb971dd27UL, 0xb07cd629UL, 0x8f5fe703UL, 0x8652ec0dUL, 0x9d45f11fUL, 0x9448fa11UL,
0xe303934bUL, 0xea0e9845UL, 0xf1198557UL, 0xf8148e59UL, 0xc737bf73UL, 0xce3ab47dUL, 0xd52da96fUL, 0xdc20a261UL,
0x766df6adUL, 0x7f60fda3UL, 0x6477e0b1UL, 0x6d7aebbfUL, 0x5259da95UL, 0x5b54d19bUL, 0x4043cc89UL, 0x494ec787UL,
0x3e05aeddUL, 0x3708a5d3UL, 0x2c1fb8c1UL, 0x2512b3cfUL, 0x1a3182e5UL, 0x133c89ebUL, 0x082b94f9UL, 0x01269ff7UL,
0xe6bd464dUL, 0xefb04d43UL, 0xf4a75051UL, 0xfdaa5b5fUL, 0xc2896a75UL, 0xcb84617bUL, 0xd0937c69UL, 0xd99e7767UL,
0xaed51e3dUL, 0xa7d81533UL, 0xbccf0821UL, 0xb5c2032fUL, 0x8ae13205UL, 0x83ec390bUL, 0x98fb2419UL, 0x91f62f17UL,
0x4dd68d76UL, 0x44db8678UL, 0x5fcc9b6aUL, 0x56c19064UL, 0x69e2a14eUL, 0x60efaa40UL, 0x7bf8b752UL, 0x72f5bc5cUL,
0x05bed506UL, 0x0cb3de08UL, 0x17a4c31aUL, 0x1ea9c814UL, 0x218af93eUL, 0x2887f230UL, 0x3390ef22UL, 0x3a9de42cUL,
0xdd063d96UL, 0xd40b3698UL, 0xcf1c2b8aUL, 0xc6112084UL, 0xf93211aeUL, 0xf03f1aa0UL, 0xeb2807b2UL, 0xe2250cbcUL,
0x956e65e6UL, 0x9c636ee8UL, 0x877473faUL, 0x8e7978f4UL, 0xb15a49deUL, 0xb85742d0UL, 0xa3405fc2UL, 0xaa4d54ccUL,
0xecdaf741UL, 0xe5d7fc4fUL, 0xfec0e15dUL, 0xf7cdea53UL, 0xc8eedb79UL, 0xc1e3d077UL, 0xdaf4cd65UL, 0xd3f9c66bUL,
0xa4b2af31UL, 0xadbfa43fUL, 0xb6a8b92dUL, 0xbfa5b223UL, 0x80868309UL, 0x898b8807UL, 0x929c9515UL, 0x9b919e1bUL,
0x7c0a47a1UL, 0x75074cafUL, 0x6e1051bdUL, 0x671d5ab3UL, 0x583e6b99UL, 0x51336097UL, 0x4a247d85UL, 0x4329768bUL,
0x34621fd1UL, 0x3d6f14dfUL, 0x267809cdUL, 0x2f7502c3UL, 0x105633e9UL, 0x195b38e7UL, 0x024c25f5UL, 0x0b412efbUL,
0xd7618c9aUL, 0xde6c8794UL, 0xc57b9a86UL, 0xcc769188UL, 0xf355a0a2UL, 0xfa58abacUL, 0xe14fb6beUL, 0xe842bdb0UL,
0x9f09d4eaUL, 0x9604dfe4UL, 0x8d13c2f6UL, 0x841ec9f8UL, 0xbb3df8d2UL, 0xb230f3dcUL, 0xa927eeceUL, 0xa02ae5c0UL,
0x47b13c7aUL, 0x4ebc3774UL, 0x55ab2a66UL, 0x5ca62168UL, 0x63851042UL, 0x6a881b4cUL, 0x719f065eUL, 0x78920d50UL,
0x0fd9640aUL, 0x06d46f04UL, 0x1dc37216UL, 0x14ce7918UL, 0x2bed4832UL, 0x22e0433cUL, 0x39f75e2eUL, 0x30fa5520UL,
0x9ab701ecUL, 0x93ba0ae2UL, 0x88ad17f0UL, 0x81a01cfeUL, 0xbe832dd4UL, 0xb78e26daUL, 0xac993bc8UL, 0xa59430c6UL,
0xd2df599cUL, 0xdbd25292UL, 0xc0c54f80UL, 0xc9c8448eUL, 0xf6eb75a4UL, 0xffe67eaaUL, 0xe4f163b8UL, 0xedfc68b6UL,
0x0a67b10cUL, 0x036aba02UL, 0x187da710UL, 0x1170ac1eUL, 0x2e539d34UL, 0x275e963aUL, 0x3c498b28UL, 0x35448026UL,
0x420fe97cUL, 0x4b02e272UL, 0x5015ff60UL, 0x5918f46eUL, 0x663bc544UL, 0x6f36ce4aUL, 0x7421d358UL, 0x7d2cd856UL,
0xa10c7a37UL, 0xa8017139UL, 0xb3166c2bUL, 0xba1b6725UL, 0x8538560fUL, 0x8c355d01UL, 0x97224013UL, 0x9e2f4b1dUL,
0xe9642247UL, 0xe0692949UL, 0xfb7e345bUL, 0xf2733f55UL, 0xcd500e7fUL, 0xc45d0571UL, 0xdf4a1863UL, 0xd647136dUL,
0x31dccad7UL, 0x38d1c1d9UL, 0x23c6dccbUL, 0x2acbd7c5UL, 0x15e8e6efUL, 0x1ce5ede1UL, 0x07f2f0f3UL, 0x0efffbfdUL,
0x79b492a7UL, 0x70b999a9UL, 0x6bae84bbUL, 0x62a38fb5UL, 0x5d80be9fUL, 0x548db591UL, 0x4f9aa883UL, 0x4697a38dUL
};
#endif

#define Te0(x) TE0[x]
#define Te1(x) ROR(TE0[x], 8)
#define Te2(x) ROR(TE0[x], 16)
#define Te3(x) ROR(TE0[x], 24)

#define Td0(x) TD0[x]
#define Td1(x) ROR(TD0[x], 8)
#define Td2(x) ROR(TD0[x], 16)
#define Td3(x) ROR(TD0[x], 24)

#define Te4_0 0x000000FF & Te4
#define Te4_1 0x0000FF00 & Te4
#define Te4_2 0x00FF0000 & Te4
#define Te4_3 0xFF000000 & Te4

static const uint32 rcon[] = {
	0x01000000UL, 0x02000000UL, 0x04000000UL, 0x08000000UL,
	0x10000000UL, 0x20000000UL, 0x40000000UL, 0x80000000UL,
	0x1B000000UL, 0x36000000UL, /* for 128-bit blocks, AES never uses more than 10 rcon values */
};

int pspadding_enc(U8 *in, U32 inlen, U8 *out, U32 *outlen, PADDING_MODE mode, BLOCK_SIZE blocksize)
{
    U32 expectlen, blockmask;
    U8 fillinc;
    int i;

    if(blocksize != BLOCK08 && blocksize != BLOCK16)
        return -1;
    
    blockmask = ~(blocksize - 1);
    if(mode == ZERO && in[inlen - 1] == 0)
        return -1;

    if(mode == ZERO)
    {
        if(inlen & (blocksize - 1))
            expectlen = (inlen & blockmask) + blocksize;
        else
            expectlen = inlen;
    }
    else
    {
        expectlen = (inlen & blockmask) + blocksize;
    }

    if(expectlen > *outlen)
        return -1;

    if(mode == ZERO)
        fillinc = 0;
    else
        fillinc = blocksize - (inlen & (blocksize - 1));

    if(out != in)
        memcpy(out, in, inlen);
    
    for(i = inlen; i < expectlen; i++)
    {
        out[i] = fillinc;
    }
    *outlen = expectlen;
    
    return 0;
}

int pspadding_dec(U8 *in, U32 inlen, U8* out, U32 *outlen, PADDING_MODE mode, BLOCK_SIZE blocksize)
{
    U32 expectlen = 0;
    U8 fillinc;
    int ret = -1, i;

    if(blocksize != BLOCK08 && blocksize != BLOCK16)
         goto exit;;

    if(inlen & (blocksize - 1))
         goto exit;;

    if(mode == ZERO)
    {
        for(i = inlen - 1; i >= inlen - blocksize; i--)
        {
            if(in[i] != 0)
            {
                expectlen = i + 1;
                break;
            }
        }

        if(expectlen == 0)
            expectlen = inlen - blocksize;
    }
    else
    {
        if(in[inlen -1] > blocksize)
            goto exit;
            
        fillinc = in[inlen -1];
        
        if(inlen < fillinc)
            goto exit;
            
        expectlen = inlen - fillinc;
        for(i = inlen - fillinc; i < inlen; i++)
        {
            if(in[i] != fillinc)
            {
                expectlen = 0;
                goto exit;
            }
        }
    }
        
    if(out != in)
        memcpy(out, in, expectlen);

    ret = 0;
exit:
    *outlen = expectlen;
    return ret;
}
#ifdef USE_BURN_STACK
static void _aes_ecb_decrypt(const unsigned char *ct, unsigned char *pt,
				psAesKey_t *skey);
static void _aes_ecb_encrypt(const unsigned char *pt, unsigned char *ct,
				psAesKey_t *skey);
#endif

static uint32 setup_mix(uint32 temp)
{
	return	(Te4_3[byte(temp, 2)]) ^
			(Te4_2[byte(temp, 1)]) ^
			(Te4_1[byte(temp, 0)]) ^
			(Te4_0[byte(temp, 3)]);
}

#ifndef PS_AES_IMPROVE_PERF_INCREASE_CODESIZE
static uint32 setup_mix2(uint32 temp)
{
	return	Td0(255 & Te4[byte(temp, 3)]) ^
			Td1(255 & Te4[byte(temp, 2)]) ^
			Td2(255 & Te4[byte(temp, 1)]) ^
			Td3(255 & Te4[byte(temp, 0)]);
}
#endif /* PS_AES_IMPROVE_PERF_INCREASE_CODESIZE */

/*
	Software implementation of AES CBC APIs
 */
#ifndef USE_AES_CBC_EXTERNAL
int32 psAesInit(psCipherContext_t *ctx, unsigned char *IV,
				  unsigned char *key, uint32 keylen)
{
	int32		x, err;

	if (IV == NULL || key == NULL || ctx == NULL) {
		psTraceCrypto("psAesInit arg fail\n");
		return PS_ARG_FAIL;
	}
	memset(ctx, 0x0, sizeof(psCipherContext_t));
/*
	setup cipher
 */
	if ((err = psAesInitKey(key, keylen, &ctx->aes.key)) != PS_SUCCESS) {
		return err;
	}
/*
	copy IV
 */
	ctx->aes.blocklen = 16;
	for (x = 0; x < ctx->aes.blocklen; x++) {
		ctx->aes.IV[x] = IV[x];
	}
	return PS_SUCCESS;
}

int32 psAesEncrypt(psCipherContext_t *ctx, unsigned char *pt,
					 unsigned char *ct, uint32 len)
{
	int32			x;
	uint32			i;
	unsigned char	tmp[MAXBLOCKSIZE];

	if (pt == NULL || ct == NULL || ctx == NULL || (len & 0x7) != 0) {
		psTraceCrypto("Bad parameters to psAesEncrypt\n");
		return PS_ARG_FAIL;
	}

/*
	is blocklen valid?
 */
	if (ctx->aes.blocklen < 0 || (ctx->aes.blocklen >
			(int32)sizeof(ctx->aes.IV))) {
		psTraceCrypto("Bad blocklen in psAesEncrypt\n");
		return PS_LIMIT_FAIL;
	}

	for (i = 0; i < len; i += ctx->aes.blocklen) {
/*
		xor IV against plaintext
 */
		for (x = 0; x < ctx->aes.blocklen; x++) {
			tmp[x] = pt[x] ^ ctx->aes.IV[x];
		}
/*
		encrypt
 */
		psAesEncryptBlock(tmp, ct, &ctx->aes.key);

/*
		store IV [ciphertext] for a future block
 */
		for (x = 0; x < ctx->aes.blocklen; x++) {
			ctx->aes.IV[x] = ct[x];
		}
		ct += ctx->aes.blocklen;
		pt += ctx->aes.blocklen;
	}

	memset(tmp, 0x0, sizeof(tmp));
	return len;
}

int32 psAesDecrypt(psCipherContext_t *ctx, unsigned char *ct,
					 unsigned char *pt, uint32 len)
{
	int32			x;
	uint32			i;
	unsigned char	tmp[MAXBLOCKSIZE], tmp2[MAXBLOCKSIZE];

	if (pt == NULL || ct == NULL || ctx == NULL || (len & 0x7) != 0) {
		psTraceCrypto("Bad parameters to psAesDecrypt\n");
		return PS_ARG_FAIL;
	}

/*
	is blocklen valid?
 */
	if (ctx->aes.blocklen < 0 || (ctx->aes.blocklen >
			(int32)sizeof(ctx->aes.IV))) {
		psTraceCrypto("Bad blocklen in psAesDecrypt\n");
		return PS_LIMIT_FAIL;
	}
	for (i = 0; i < len; i += ctx->aes.blocklen) {
/*
		decrypt the block from ct into tmp
 */
		psAesDecryptBlock(ct, tmp, &ctx->aes.key);
/*
		xor IV against the plaintext of the previous step
 */
		for (x = 0; x < ctx->aes.blocklen; x++) {
/*
			copy CT in case ct == pt
 */
			tmp2[x] = ct[x];
/*
			actually decrypt the byte
 */
			pt[x] =	tmp[x] ^ ctx->aes.IV[x];
		}
/*
		replace IV with this current ciphertext
 */
		for (x = 0; x < ctx->aes.blocklen; x++) {
			ctx->aes.IV[x] = tmp2[x];
		}
		ct += ctx->aes.blocklen;
		pt += ctx->aes.blocklen;
	}
	memset(tmp, 0x0, sizeof(tmp));
	memset(tmp2, 0x0, sizeof(tmp2));

	return len;
}

#endif /* USE_AES_CBC_EXTERNAL */


/******************************************************************************/
/*
	Initialize the AES (Rijndael) block cipher

	key: The symmetric key you wish to pass
	keylen:  The key length in bytes
	skey: The key in as scheduled by this function.
*/

int32 psAesInitKey(const unsigned char *key, uint32 keylen, psAesKey_t *skey)
{
	int32		i, j;
	uint32		temp, *rk, *rrk;

	if (key == NULL || skey == NULL) {
		psTraceCrypto("Bad args to psAesInitKey\n");
		return PS_ARG_FAIL;
	}

	if (keylen != 16 && keylen != 24 && keylen != 32) {
		psTraceCrypto("Invalid AES key length\n");
		return CRYPT_INVALID_KEYSIZE;
	}

	memset(skey, 0x0, sizeof(psAesKey_t));
	skey->Nr = 10 + ((keylen/8)-2)*2;

/*
	setup the forward key
 */
	i				= 0;
	rk				= skey->eK;
	LOAD32H(rk[0], key	   );
	LOAD32H(rk[1], key +  4);
	LOAD32H(rk[2], key +  8);
	LOAD32H(rk[3], key + 12);
	if (keylen == 16) {
		j = 44;
		for (;;) {
			temp  = rk[3];
			rk[4] = rk[0] ^ setup_mix(temp) ^ rcon[i];
			rk[5] = rk[1] ^ rk[4];
			rk[6] = rk[2] ^ rk[5];
			rk[7] = rk[3] ^ rk[6];
			if (++i == 10) {
				break;
			}
			rk += 4;
		}
	} else if (keylen == 24) {
		j = 52;
		LOAD32H(rk[4], key + 16);
		LOAD32H(rk[5], key + 20);
		for (;;) {
		#ifdef _MSC_VER
			temp = skey->eK[rk - skey->eK + 5];
		#else
			temp = rk[5];
		#endif /* _MSC_VER */
			rk[ 6] = rk[ 0] ^ setup_mix(temp) ^ rcon[i];
			rk[ 7] = rk[ 1] ^ rk[ 6];
			rk[ 8] = rk[ 2] ^ rk[ 7];
			rk[ 9] = rk[ 3] ^ rk[ 8];
			if (++i == 8) {
				break;
			}
			rk[10] = rk[ 4] ^ rk[ 9];
			rk[11] = rk[ 5] ^ rk[10];
			rk += 6;
		}
	} else if (keylen == 32) {
		j = 60;
		LOAD32H(rk[4], key + 16);
		LOAD32H(rk[5], key + 20);
		LOAD32H(rk[6], key + 24);
		LOAD32H(rk[7], key + 28);
		for (;;) {
		#ifdef _MSC_VER
			temp = skey->eK[rk - skey->eK + 7];
		#else
			temp = rk[7];
		#endif /* _MSC_VER */
			rk[ 8] = rk[ 0] ^ setup_mix(temp) ^ rcon[i];
			rk[ 9] = rk[ 1] ^ rk[ 8];
			rk[10] = rk[ 2] ^ rk[ 9];
			rk[11] = rk[ 3] ^ rk[10];
			if (++i == 7) {
				break;
			}
			temp = rk[11];
			rk[12] = rk[ 4] ^ setup_mix(ROR(temp, 8));
			rk[13] = rk[ 5] ^ rk[12];
			rk[14] = rk[ 6] ^ rk[13];
			rk[15] = rk[ 7] ^ rk[14];
			rk += 8;
		}
	} else {
/*
		this can't happen
 */
		return PS_FAILURE;
	}

/*
	setup the inverse key now
 */
	rk		= skey->dK;
	rrk		= skey->eK + j - 4;

/*
	apply the inverse MixColumn transform to all round keys but
	the first and the last:
 */
	/* copy first */
	*rk++	= *rrk++;
	*rk++	= *rrk++;
	*rk++	= *rrk++;
	*rk		= *rrk;
	rk -= 3; rrk -= 3;

	for (i = 1; i < skey->Nr; i++) {
		rrk	-= 4;
		rk	+= 4;
#ifndef PS_AES_IMPROVE_PERF_INCREASE_CODESIZE
		temp = rrk[0];
		rk[0] = setup_mix2(temp);
		temp = rrk[1];
		rk[1] = setup_mix2(temp);
		temp = rrk[2];
		rk[2] = setup_mix2(temp);
		temp = rrk[3];
		rk[3] = setup_mix2(temp);
#else /* PS_AES_IMPROVE_PERF_INCREASE_CODESIZE */
		temp = rrk[0];
		rk[0] =
			Tks0[byte(temp, 3)] ^
			Tks1[byte(temp, 2)] ^
			Tks2[byte(temp, 1)] ^
			Tks3[byte(temp, 0)];
		temp = rrk[1];
		rk[1] =
			Tks0[byte(temp, 3)] ^
			Tks1[byte(temp, 2)] ^
			Tks2[byte(temp, 1)] ^
			Tks3[byte(temp, 0)];
		temp = rrk[2];
		rk[2] =
			Tks0[byte(temp, 3)] ^
			Tks1[byte(temp, 2)] ^
			Tks2[byte(temp, 1)] ^
			Tks3[byte(temp, 0)];
		temp = rrk[3];
		rk[3] =
			Tks0[byte(temp, 3)] ^
			Tks1[byte(temp, 2)] ^
			Tks2[byte(temp, 1)] ^
			Tks3[byte(temp, 0)];
#endif /* PS_AES_IMPROVE_PERF_INCREASE_CODESIZE */
	}

	/* copy last */
	rrk	-= 4;
	rk	+= 4;
	*rk++ = *rrk++;
	*rk++ = *rrk++;
	*rk++ = *rrk++;
	*rk	  = *rrk;

	return PS_SUCCESS;
}


#ifdef USE_BURN_STACK
void psAesEncryptBlock(const unsigned char *pt, unsigned char *ct,
				psAesKey_t *skey)
{
	_aes_ecb_encrypt(pt, ct, skey);
	psBurnStack(sizeof(uint32)*8 + sizeof(uint32*) + sizeof(int32)*2);
}
static void _aes_ecb_encrypt(const unsigned char *pt, unsigned char *ct,
				psAesKey_t *skey)
#else
void psAesEncryptBlock(const unsigned char *pt, unsigned char *ct,
				psAesKey_t *skey)
#endif /* USE_BURN_STACK */
{
	uint32	s0, s1, s2, s3, t0, t1, t2, t3, *rk;
	int32	Nr, r;

	if (pt == NULL || ct == NULL || skey == NULL) {
		return;
	}

	Nr = skey->Nr;
	rk = skey->eK;

/*
	map byte array block to cipher state
	and add initial round key:
 */
	LOAD32H(s0, pt     ); s0 ^= rk[0];
	LOAD32H(s1, pt +  4); s1 ^= rk[1];
	LOAD32H(s2, pt +  8); s2 ^= rk[2];
	LOAD32H(s3, pt + 12); s3 ^= rk[3];

#ifndef PS_AES_IMPROVE_PERF_INCREASE_CODESIZE
	for (r = 0; ; r++) {
		rk += 4;
		t0 =
			Te0(byte(s0, 3)) ^
			Te1(byte(s1, 2)) ^
			Te2(byte(s2, 1)) ^
			Te3(byte(s3, 0)) ^
			rk[0];
		t1 =
			Te0(byte(s1, 3)) ^
			Te1(byte(s2, 2)) ^
			Te2(byte(s3, 1)) ^
			Te3(byte(s0, 0)) ^
			rk[1];
		t2 =
			Te0(byte(s2, 3)) ^
			Te1(byte(s3, 2)) ^
			Te2(byte(s0, 1)) ^
			Te3(byte(s1, 0)) ^
			rk[2];
		t3 =
			Te0(byte(s3, 3)) ^
			Te1(byte(s0, 2)) ^
			Te2(byte(s1, 1)) ^
			Te3(byte(s2, 0)) ^
			rk[3];
		if (r == Nr-2) {
			break;
		}
		s0 = t0; s1 = t1; s2 = t2; s3 = t3;
	}
	rk += 4;
#else /* PS_AES_IMPROVE_PERF_INCREASE_CODESIZE */

/*
	Nr - 1 full rounds:
 */
	r = Nr >> 1;
	for (;;) {

		t0 =
			Te0(byte(s0, 3)) ^
			Te1(byte(s1, 2)) ^
			Te2(byte(s2, 1)) ^
			Te3(byte(s3, 0)) ^
			rk[4];
		t1 =
			Te0(byte(s1, 3)) ^
			Te1(byte(s2, 2)) ^
			Te2(byte(s3, 1)) ^
			Te3(byte(s0, 0)) ^
			rk[5];
		t2 =
			Te0(byte(s2, 3)) ^
			Te1(byte(s3, 2)) ^
			Te2(byte(s0, 1)) ^
			Te3(byte(s1, 0)) ^
			rk[6];
		t3 =
			Te0(byte(s3, 3)) ^
			Te1(byte(s0, 2)) ^
			Te2(byte(s1, 1)) ^
			Te3(byte(s2, 0)) ^
			rk[7];

		rk += 8;
		if (--r == 0) {
			break;
		}

		s0 =
			Te0(byte(t0, 3)) ^
			Te1(byte(t1, 2)) ^
			Te2(byte(t2, 1)) ^
			Te3(byte(t3, 0)) ^
			rk[0];
		s1 =
			Te0(byte(t1, 3)) ^
			Te1(byte(t2, 2)) ^
			Te2(byte(t3, 1)) ^
			Te3(byte(t0, 0)) ^
			rk[1];
		s2 =
			Te0(byte(t2, 3)) ^
			Te1(byte(t3, 2)) ^
			Te2(byte(t0, 1)) ^
			Te3(byte(t1, 0)) ^
			rk[2];
		s3 =
			Te0(byte(t3, 3)) ^
			Te1(byte(t0, 2)) ^
			Te2(byte(t1, 1)) ^
			Te3(byte(t2, 0)) ^
			rk[3];
	}
#endif /* PS_AES_IMPROVE_PERF_INCREASE_CODESIZE */

/*
		apply last round and map cipher state to byte array block:
 */
	s0 =
		(Te4_3[byte(t0, 3)]) ^
		(Te4_2[byte(t1, 2)]) ^
		(Te4_1[byte(t2, 1)]) ^
		(Te4_0[byte(t3, 0)]) ^
		rk[0];
	STORE32H(s0, ct);
	s1 =
		(Te4_3[byte(t1, 3)]) ^
		(Te4_2[byte(t2, 2)]) ^
		(Te4_1[byte(t3, 1)]) ^
		(Te4_0[byte(t0, 0)]) ^
		rk[1];
	STORE32H(s1, ct+4);
	s2 =
		(Te4_3[byte(t2, 3)]) ^
		(Te4_2[byte(t3, 2)]) ^
		(Te4_1[byte(t0, 1)]) ^
		(Te4_0[byte(t1, 0)]) ^
		rk[2];
	STORE32H(s2, ct+8);
	s3 =
		(Te4_3[byte(t3, 3)]) ^
		(Te4_2[byte(t0, 2)]) ^
		(Te4_1[byte(t1, 1)]) ^
		(Te4_0[byte(t2, 0)]) ^
		rk[3];
	STORE32H(s3, ct+12);
}

#ifdef USE_BURN_STACK
void psAesDecryptBlock(const unsigned char *ct, unsigned char *pt,
				psAesKey_t *skey)
{
	_aes_ecb_decrypt(ct, pt, skey);
	psBurnStack(sizeof(uint32)*8 + sizeof(uint32*) + sizeof(int32)*2);
}
static void _aes_ecb_decrypt(const unsigned char *ct, unsigned char *pt,
				psAesKey_t *skey)
#else
void psAesDecryptBlock(const unsigned char *ct, unsigned char *pt,
				psAesKey_t *skey)
#endif /* USE_BURN_STACK */
{
	uint32		s0, s1, s2, s3, t0, t1, t2, t3, *rk;
	int32		Nr, r;

	if (pt == NULL || ct == NULL || skey == NULL) {
		return;
	}

	Nr = skey->Nr;
	rk = skey->dK;

/*
	map byte array block to cipher state and add initial round key:
 */
	LOAD32H(s0, ct		); s0 ^= rk[0];
	LOAD32H(s1, ct	+  4); s1 ^= rk[1];
	LOAD32H(s2, ct	+  8); s2 ^= rk[2];
	LOAD32H(s3, ct	+ 12); s3 ^= rk[3];

#ifndef PS_AES_IMPROVE_PERF_INCREASE_CODESIZE
	for (r = 0; ; r++) {
		rk += 4;
		t0 =
			Td0(byte(s0, 3)) ^
			Td1(byte(s3, 2)) ^
			Td2(byte(s2, 1)) ^
			Td3(byte(s1, 0)) ^
			rk[0];
		t1 =
			Td0(byte(s1, 3)) ^
			Td1(byte(s0, 2)) ^
			Td2(byte(s3, 1)) ^
			Td3(byte(s2, 0)) ^
			rk[1];
		t2 =
			Td0(byte(s2, 3)) ^
			Td1(byte(s1, 2)) ^
			Td2(byte(s0, 1)) ^
			Td3(byte(s3, 0)) ^
			rk[2];
		t3 =
			Td0(byte(s3, 3)) ^
			Td1(byte(s2, 2)) ^
			Td2(byte(s1, 1)) ^
			Td3(byte(s0, 0)) ^
			rk[3];
		if (r == Nr-2) {
			break;
		}
		s0 = t0; s1 = t1; s2 = t2; s3 = t3;
	}
	rk += 4;

#else /* PS_AES_IMPROVE_PERF_INCREASE_CODESIZE */

/*
	Nr - 1 full rounds:
 */
	r = Nr >> 1;
	for	(;;) {

		t0 =
			Td0(byte(s0, 3)) ^
			Td1(byte(s3, 2)) ^
			Td2(byte(s2, 1)) ^
			Td3(byte(s1, 0)) ^
			rk[4];
		t1 =
			Td0(byte(s1, 3)) ^
			Td1(byte(s0, 2)) ^
			Td2(byte(s3, 1)) ^
			Td3(byte(s2, 0)) ^
			rk[5];
		t2 =
			Td0(byte(s2, 3)) ^
			Td1(byte(s1, 2)) ^
			Td2(byte(s0, 1)) ^
			Td3(byte(s3, 0)) ^
			rk[6];
		t3 =
			Td0(byte(s3, 3)) ^
			Td1(byte(s2, 2)) ^
			Td2(byte(s1, 1)) ^
			Td3(byte(s0, 0)) ^
			rk[7];

		rk += 8;
		if (--r == 0) {
			break;
		}

		s0 =
			Td0(byte(t0, 3)) ^
			Td1(byte(t3, 2)) ^
			Td2(byte(t2, 1)) ^
			Td3(byte(t1, 0)) ^
			rk[0];
		s1 =
			Td0(byte(t1, 3)) ^
			Td1(byte(t0, 2)) ^
			Td2(byte(t3, 1)) ^
			Td3(byte(t2, 0)) ^
			rk[1];
		s2 =
			Td0(byte(t2, 3)) ^
			Td1(byte(t1, 2)) ^
			Td2(byte(t0, 1)) ^
			Td3(byte(t3, 0)) ^
			rk[2];
		s3 =
			Td0(byte(t3, 3)) ^
			Td1(byte(t2, 2)) ^
			Td2(byte(t1, 1)) ^
			Td3(byte(t0, 0)) ^
			rk[3];
	}
#endif /* PS_AES_IMPROVE_PERF_INCREASE_CODESIZE */

/*
	apply last round and map cipher state to byte array block:
 */
	s0 =
		(Td4[byte(t0, 3)] & 0xff000000) ^
		(Td4[byte(t3, 2)] & 0x00ff0000) ^
		(Td4[byte(t2, 1)] & 0x0000ff00) ^
		(Td4[byte(t1, 0)] & 0x000000ff) ^
		rk[0];
	STORE32H(s0, pt);
	s1 =
		(Td4[byte(t1, 3)] & 0xff000000) ^
		(Td4[byte(t0, 2)] & 0x00ff0000) ^
		(Td4[byte(t3, 1)] & 0x0000ff00) ^
		(Td4[byte(t2, 0)] & 0x000000ff) ^
		rk[1];
	STORE32H(s1, pt+4);
	s2 =
		(Td4[byte(t2, 3)] & 0xff000000) ^
		(Td4[byte(t1, 2)] & 0x00ff0000) ^
		(Td4[byte(t0, 1)] & 0x0000ff00) ^
		(Td4[byte(t3, 0)] & 0x000000ff) ^
		rk[2];
	STORE32H(s2, pt+8);
	s3 =
		(Td4[byte(t3, 3)] & 0xff000000) ^
		(Td4[byte(t2, 2)] & 0x00ff0000) ^
		(Td4[byte(t1, 1)] & 0x0000ff00) ^
		(Td4[byte(t0, 0)] & 0x000000ff) ^
		rk[3];
	STORE32H(s3, pt+12);
}

/******************************************************************************/
void AES_convert_key2(psAesKey_t *ctx)
{
    int i;
    unsigned int *k,w,t1,t2,t3,t4;

    k = ctx->eK;
    k += 4;

    for (i= ctx->Nr*4; i > 4; i--)
    {
        w= *k;
        w = inv_mix_col(w,t1,t2,t3,t4);
        *k++ =w;
    }
}
/******************************************************************************/
int aes_wrap2 ( const void *kek, const void *src, void *dest, int nblk )
{
	unsigned char *A = dest;
	unsigned char B[16];
	unsigned char *R;
	int i, j;
	psAesKey_t keyctx;

	psAesInitKey(kek, 16, &keyctx);

	/* Set up */
	memset ( A, 0xA6, 8 );
	memmove ( dest + 8, src, nblk * 8 );

	/* Wrap */
	for ( j = 0; j < 6; j++ ) {
		R = dest + 8;
		for ( i = 1; i <= nblk; i++ ) {
			memcpy ( B, A, 8 );
			memcpy ( B + 8, R, 8 );
			psAesEncryptBlock (B, B, &keyctx);
			memcpy ( A, B, 8 );
			A[7] ^= ( nblk * j ) + i;
			memcpy ( R, B + 8, 8 );
			R += 8;
		}
	}

	return 0;
}
/******************************************************************************/
int aes_unwrap2 ( const void *kek, const void *src, void *dest, int nblk )
{
	unsigned char A[8], B[16];
	unsigned char *R;
	int i, j;
	psAesKey_t keyctx;

	psAesInitKey(kek, 16, &keyctx);

	/* Set up */
	memcpy ( (void *)A, src, 8 );
	memmove ( dest, src + 8, nblk * 8 );

	/* Unwrap */
	AES_convert_key2 ( &keyctx );

	for ( j = 5; j >= 0; j-- ) {
		R = dest + ( nblk - 1 ) * 8;
		for ( i = nblk; i >= 1; i-- ) {
			memcpy ( B, A, 8 );
			memcpy ( B + 8, R, 8 );
			B[7] ^= ( nblk * j ) + i;
			psAesDecryptBlock (B, B, &keyctx);
			memcpy ( A, B, 8 );
			memcpy ( R, B + 8, 8 );
			R -= 8;
		}
	}

	/* Check IV */
	for ( i = 0; i < 8; i++ ) {
		if ( A[i] != 0xA6 )
		{
			printf("AES decrypt fail\n");
			return -1;
		}
	}

	return 0;
}
/******************************************************************************/
/******************************************************************************/
#endif /* !USE_AES_CBC_EXTERNAL */
#endif /* USE_AES */
/******************************************************************************/

