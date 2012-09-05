/*-----------------------------------------------------------------------
/  PFF - Low level disk interface modlue include file    (C)ChaN, 2010
/-----------------------------------------------------------------------*/

#ifndef _PFFDISKIO
#define _PFFDISKIO

#include "pffinteger.h"
#include "pff.h"


/* Status of Disk Functions */
typedef BYTE	DSTATUS;

#define STA_NOINIT		0x01	/* Drive not initialized */
#define STA_NODISK		0x02	/* No medium in the drive */


/* Results of Disk Functions */
typedef enum {
	RES_OK = 0,		/* 0: Function succeeded */
	RES_ERROR,		/* 1: Disk error */
	RES_NOTRDY,		/* 2: Not ready */
	RES_PARERR		/* 3: Invalid parameter */
} DRESULT;


/*---------------------------------------*/
/* Prototypes for disk control functions */

DSTATUS disk_initialize (void);
#if _PFF_USE_READ
DRESULT disk_readp (BYTE*, DWORD, WORD, WORD);
#endif
#if _PFF_USE_WRITE
DRESULT disk_writep (BYTE*, DWORD);
#endif

#endif
