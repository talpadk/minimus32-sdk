/*
 *  sdcard.h      header file for low-level SD card support
 *
 *  This file was based on the original mmc_if.h file created
 *  by Jesper Hansen; I've retained his copyright notice below
 *  for reference.
 *
 *  This code was renamed and modified to focus on SD cards;
 *  I don't think it will work any longer with MMC cards, but I
 *  can't tell for sure because I don't own any MMC cards.
 */


/* ***********************************************************************
**
**  Copyright (C) 2006  Jesper Hansen <jesper@redegg.net> 
**
**
**  Interface functions for MMC/SD cards
**
**  File mmc_if.h
**
*************************************************************************
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software Foundation, 
**  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
**
*************************************************************************/

#ifndef __SD_IF_H__
#define __SD_IF_H__




/*
 *  Define error codes that can be returned by these library routines.
 */
#define  SDCARD_OK					0			/* success */
#define  SDCARD_NO_DETECT			1			/* unable to detect SD card */
#define  SDCARD_TIMEOUT				2			/* last operation timed out */
#define  SDCARD_RWFAIL				-1			/* read/write command failed */
#define  SDCARD_NOT_INITED			-2			/* sd_register() not called yet */



/*
 *  Define a structure for holding the required function pointers.
 */
typedef  struct  SD_callbacks_t
{
	void					(*select)(void);
	unsigned char			(*xchg)(unsigned char  val);
	void					(*deselect)(void);
	void					(*power)(unsigned char);
}  SD_CALLBACKS;

	




/*
 *  sd_register      register a set of callback functions with the SD card library
 *
 *  The argument pcallbacks points to an SD_CALLBACKS structure holding pointers
 *  to functions for accessing an SD card via SPI.
 *
 *  This routine records the contents of the structure's fields; upon return from
 *  this routine, the structure is no longer accessed by the SD library.
 *
 *  If the minimum required functions have been provided, this routine also
 *  calls the internal sd_power() function, which initializes the SD card.
 *  Therefore, you must make sure the SPI connections to the SD card are valid
 *  before calling sd_register().
 *
 *  Upon exit, this routine returns the result of invoking sd_power().
 */
uint8_t		sd_register(SD_CALLBACKS  *pcallbacks);




/*
 *  sd_power      apply/remove power to the SD card
 *
 *  This routine can be used to control power to the SD card.  This routine
 *  is called internally by sd_register() and is not ever called again by the
 *  SD library routines.  This routine is provided solely for use by the
 *  upper-level program, should power consumption be an issue and the
 *  design requires control of the SD card's power.
 *
 *  The argument power is TRUE (non-zero) to apply power to the SD card
 *  and is FALSE (zero) to remove power from the SD card.
 *
 *  Note that this routine uses the .power callback function in the SD_CALLBACKS
 *  structure referenced in the call to sd_register().  If you want to
 *  control SD card power, you must define this .power field!  If
 *  it is not defined (left as zero), calls to sd_power() will not be able
 *  to alter power to the SD card..
 *
 *  When this routine is invoked to power-on the SD card, this routine
 *  also performs the SD card initialization ritual, including issuing
 *  CMD 0 and CMD 1 to the card.  Note that this initialization happens
 *  even if the .power field in the SD_CALLBACKS structure is zero.
 *
 *  Upon exit when powering up the SD card, this routine returns
 *  SDCARD_OK if the SD card powered up and responded properly.
 *  This routine returns SDCARD_TIMEOUT if the SD card never responded
 *  to CMD 1 after 65K tries.  This routine returns SDCARD_NODETECT if
 *  the SD card never responded to CMD 0.
 *
 *  Upon exit when powering down the SD card, this routine always
 *  returns SDCARD_OK.
 */
uint8_t  sd_power(uint8_t  on);


 
/*
 *  sd_configured      test if the SD card interface has been set up
 *
 *  This routine does NOT initialize the SD card; use sd_power() for that.
 *  This routine reports to the caller whether the SD card has been
 *  initialized.  This allows a separate module (usually the FatFS library)
 *  to determine that the SD card interface has been set up by the
 *  code in a project's main() routine.  This removes the need for the
 *  FatFS library to understand how the hardware for a project is wired.
 *
 *  Upon exit, this routine returns SDCARD_OK if the SD card interface is
 *  ready to use, else it returns SDCARD_NOT_INITED.
 */
uint16_t	sd_configured(void);


uint16_t	sd_readsector(uint32_t lba, uint8_t *buffer);
uint16_t	sd_writesector(uint32_t  lba, uint8_t  *buffer);
uint16_t	sd_readcsd(uint8_t  *csdbuff);


/*
 *  sd_waitforready      wait for SD card to flush its write buffer
 *
 *  This routine is used solely by Chan's FatFS library routines to ensure
 *  that the SD card has completed a write operation; see the FatFS project
 *  and diskio.c for usage.
 */
uint8_t		sd_waitforready(void);


#endif
