/**
 *	@file    osdep.c
 *	@version 33ef80f (HEAD, tag: MATRIXSSL-3-7-2-OPEN, tag: MATRIXSSL-3-7-2-COMM, origin/master, origin/HEAD, master)
 *
 *	POSIX layer.
 *		Mac OSX 10.5
 *		Linux
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

#include "coreApi.h"

/******************************************************************************/
#ifdef METAL
/******************************************************************************/

/*
	Universal system POSIX headers and then PScore/coreApi.h
	OS-specific header includes should be added to PScore/osdep.h
*/
#include <stdlib.h>	/* abort() */
#include <fcntl.h>	/* open(), O_RDONLY, ect... */
#include <unistd.h>	/* close() */
#include <errno.h>	/* errno */
#include <clock.h>	/* gettimeofday */
#include <random.h>
#include <cfs/cfs.h>

#ifdef USE_PKCS11_ENTROPY
#include "cryptoki.h"
#endif

/******************************************************************************/
/*
	TIME FUNCTIONS
*/
#ifndef USE_HIGHRES_TIME
/******************************************************************************/
/*
	Module open and close
*/
int osdepTimeOpen(void)
{
	//contiki support clock time
	return PS_SUCCESS;
}

void osdepTimeClose(void)
{
}

/*
	PScore Public API implementations

	This function always returns seconds/ticks AND OPTIONALLY populates
	whatever psTime_t happens to be on the given platform.
*/
int32 psGetTime(psTime_t *t, void *userPtr)
{
	//Implement clock time here
	return get_ntp_time();
}

int32 psDiffMsecs(psTime_t then, psTime_t now, void *userPtr)
{
	return (now - then);
}

int32 psCompareTime(psTime_t a, psTime_t b, void *userPtr)
{
/*
	Time comparison.  1 if 'a' is less than or equal.  0 if 'a' is greater
*/
	if (a < b) {
		return 1;
	}
	return 0;
}
/******************************************************************************/
/*
	ENTROPY FUNCTIONS
*/
/******************************************************************************/
/*
	Module open and close
*/
int osdepEntropyOpen(void)
{
	//Can not support entropy at contiki.
	return PS_SUCCESS;
}

void osdepEntropyClose(void)
{
	//Can not support entropy at contiki.
}

/*
	PScore Public API implementations
*/
int32 psGetEntropy(unsigned char *bytes, uint32 size, void *userPtr)
{
	//Can not support entropy at contiki. So reply 0x12345678 for now.
	U32 i;

    for(i = 0; i < size; i++)
    {
        bytes[i] = (U8)(random_rand() & 0xFF);
            
    }
	return size;
}
#endif
/******************************************************************************/


/******************************************************************************/
/*
	RAW TRACE FUNCTIONS
*/
/******************************************************************************/

int osdepTraceOpen(void)
{
	return PS_SUCCESS;
}

void osdepTraceClose(void)
{
}

void _psTrace(char *msg)
{
	printf("%s", msg);
}

/* message should contain one %s, unless value is NULL */
void _psTraceStr(char *message, char *value)
{
	if (value) {
		printf(message, value);
	} else {
		printf("%s", message);
	}
}

/* message should contain one %d */
void _psTraceInt(char *message, int32 value)
{
	printf(message, value);
}

/* message should contain one %p */
void _psTracePtr(char *message, void *value)
{
	printf(message, value);
}

/******************************************************************************/

#ifdef HALT_ON_PS_ERROR
/******************************************************************************/
/*
	POSIX - abort() on psError when HALT_ON_PS_ERROR enabled
*/
void osdepBreak(void)
{
	abort();
}
#endif /* HALT_ON_PS_ERROR */


#ifdef MATRIX_USE_FILE_SYSTEM
/******************************************************************************/
/*
	FILE ACCESS FUNCTION
*/
/******************************************************************************/
/*
	Memory info:
	Caller must free 'buf' parameter on success
	Callers do not need to free buf on function failure
*/
int32 psGetFileBuf(psPool_t *pool, const char *fileName, unsigned char **buf,
				int32 *bufLen)
{
	//Do not support accesss file for now.
	
	int	fp;
    char tmpc;

	*bufLen = 0;
	*buf = NULL;

	if (fileName == NULL) {
		return PS_ARG_FAIL;
	}
	if ((fp = cfs_open(fileName, CFS_READ)) == -1) {
		psTraceStrCore("Unable to open %s\n", (char*)fileName);
		return PS_PLATFORM_FAIL;
	}

    //read the length
    while(1)
    {
        if(cfs_read(fp, &tmpc, 1) != 1)
        {
            cfs_seek(fp, 0, CFS_SEEK_SET);
            break;
        }
        else
        {
            *bufLen = *bufLen + 1;
        }
    }
    
	*buf = psMalloc(pool, *bufLen + 1);
	if (*buf == NULL) {
		return PS_MEM_FAIL;
	}
	memset(*buf, 0x0, *bufLen + 1);

	cfs_read(fp, *buf, *bufLen);
	cfs_close(fp);
	return PS_SUCCESS;
}
#endif /* MATRIX_USE_FILE_SYSTEM */

/******************************************************************************/
#endif /* METAL */
/******************************************************************************/
