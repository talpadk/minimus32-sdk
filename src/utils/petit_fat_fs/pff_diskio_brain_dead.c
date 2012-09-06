#include "pff_diskio_brain_dead.h"

#include <stdint.h>
//#include <string.h>

#include "sdcard.h"

DSTATUS disk_initialize (void){
  return RES_OK; //So brain dead it doesn't even check if the SD card has been registered
}



/*-----------------------------------------------------------------------*/
/* Read Partial Sector                                                   */
/*-----------------------------------------------------------------------*/

DRESULT disk_readp (BYTE* dest,			/* Pointer to the destination object */
	DWORD sector,		/* Sector number (LBA) */
	WORD sofs,			/* Offset in the sector */
	WORD count			/* Byte count (bit15:destination) */
)
{
  uint8_t block[512]; //No global disk cache, brain dead implementation!

  if (sd_readsector(sector, block)!=SDCARD_OK) return RES_ERROR; //read failed
  memcpy(dest, block+sofs, count);
  return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Partial Sector                                                  */
/*-----------------------------------------------------------------------*/

DRESULT disk_writep (
	const BYTE* buff,		/* Pointer to the data to be written, NULL:Initiate/Finalize write operation */
	DWORD sc		/* Sector number (LBA) or Number of bytes to send */
) {
	return RES_NOTRDY; //Fails horrible for write ops
}
