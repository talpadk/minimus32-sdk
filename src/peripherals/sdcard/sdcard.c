/*
 *  sdcard.c      source file for low-level SD card support
 *
 *  This code was based on the original mmc_if.c file created
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
**  File mmc_if.c
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

/*
 *  The routines sd_writesector() and sd_response() were provided as
 *  an update to Jesper's original code by someone whose website I forgot
 *  to record.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "sdcard.h"


#ifndef  FALSE
#define  FALSE		0
#define  TRUE		!FALSE
#endif




static uint16_t				sd_response(uint8_t  response)		__attribute__((noinline));
static uint8_t				sd_get(void)						__attribute__((noinline));
static uint8_t				sd_datatoken(void)					__attribute__((noinline));
static void					sd_clock_and_release(void)			__attribute__((noinline));
static uint8_t				sd_send_command(uint8_t  command, uint16_t  px, uint16_t  py)    __attribute__((noinline));

static  unsigned char		inited = FALSE;


static  void				(*select)(void) = (void *)0;
static  unsigned char		(*xchg)(unsigned char  val) = (void *)0;
static  void				(*deselect)(void) = (void *)0;
static  void				(*power)(unsigned char  on) = (void *)0;



uint8_t  sd_register(SD_CALLBACKS  *pcallbacks)
{
	uint8_t					result;

	select = pcallbacks->select;
	xchg = pcallbacks->xchg;
	deselect = pcallbacks->deselect;
	power = pcallbacks->power;

	inited = FALSE;
	if (select && xchg && deselect)  inited = TRUE;
	if (inited)  result = sd_power(1);		// all registered, power up the SD card
	else         result = SDCARD_NOT_INITED;	// bad power-up or missing function handle
	return  result;
}



/*
 *  sd_configured      confirm that access to the SD card is possible
 *
 *  This routine returns SD_OK if the interface to the SD card has been configured
 *  (based on contents of callback structure passed to sd_register()) or SD_NOT_INITED
 *  if the interface is not set up.
 */
uint16_t  sd_configured(void)
{
	if (inited)  return  SDCARD_OK;
	else		 return  SDCARD_NOT_INITED;
}




/*
 *  sd_power      apply power and initialize the SC card
 *
 *  This routine will apply power to the SD card and start the initialization
 *  ritual.  Note that this routine will use the host-supplied power() callback
 *  function if it is available.  If no power() callback function was provided,
 *  this routine simply assumes the SD card was just recently powered and
 *  must go through initialization.
 *
 *  This routine will NOT work for MMC cards.
 *
 *  This routine will NOT work for V2 (>4GB) SD cards.
 */
uint8_t  sd_power(unsigned char  on)
{
	volatile uint16_t					i;
	volatile uint8_t					retry;
	volatile uint8_t					response;

	if (power)							// if host provided a power function...
	{
		power(on);						// use it to control power to the SD card (either on or off)
	}

	if (on)								// if turning on the card...
	{
/*
 *  Begin initialization by sending CMD0 and waiting until SD card
 *  responds with In Idle Mode (0x01).  If the response is not 0x01
 *  within a reasonable amount of time, there is no SD card on the bus.
 */
		deselect();						// always make sure
		for (i=0; i<10; i++)			// send several clocks while card power stabilizes
			xchg(0xff);

		for (i=0; i<0xfe; i++)
		{
			response = sd_send_command(0, 0, 0);	// send CMD0 - go to idle state
			deselect();
			xchg(0xff);						// give it some clocks
			xchg(0xff);
			if (response == 1)  break;
		}
		if (response != 1) return  SDCARD_NO_DETECT;

/*
 *  Tell the SD card to begin initialization.  For V1 SD cards, this is done
 *  with repeated sequences of CMD55 and ACMD41 until the card responds with
 *  a status of not idle (0x00).  If the card does not return a status of
 *  0x00 within a reasonable amount of time, declare a timeout to the calling
 *  routine.
 */
		for (i=0; i<0xfffe; i++)
		{
			response = sd_send_command(55, 0, 0);	// start of an ACMD41	
			deselect();
			xchg(0xff);

			response = sd_send_command(41, 0, 0);	// send CMD41 - check state
			deselect();
			xchg(0xff);

			if (response != 0x01)  break;
		}

   		if (response == 0x01)                 	// if we timed out above
      			return  SDCARD_TIMEOUT;		// return failure code
	}
/*
 *  At this point, the SD card has completed initialization.  The calling routine
 *  can now increase the SPI clock rate for the SD card to the maximum allowed by
 *  the SD card (typically, 20 MHz).
 */
	return  SDCARD_OK;					// if no power routine or turning off the card, call it good
}





		

uint16_t  sd_readcsd(uint8_t  *csdbuff)
{
	uint8_t						status;
	uint8_t						i;

	if (!inited)  return  SDCARD_NOT_INITED;

	sd_send_command(9, 0, 0);			// send CMD9 (read CSD)
	status = sd_datatoken();			// get status or command
	if (status != 0xfe)					// if no valid token
	{
		sd_clock_and_release();
		return  SDCARD_RWFAIL;
	}

	for (i=0; i<16; i++)				// for all bytes in CSD...
	{
		*csdbuff++ = xchg(0xff);
	}
    xchg(0xff);          		       	// ignore dummy checksum
    xchg(0xff);           		      	// ignore dummy checksum

    sd_clock_and_release();        		// cleanup

    return  SDCARD_OK;					// return success       
}





uint16_t  sd_readsector(uint32_t  lba, uint8_t  *buffer)
{
    uint16_t					i;
	uint8_t						status;

	if (!inited)  return  SDCARD_NOT_INITED;

/*
 *  Compute byte address of start of desired sector.
 */
    sd_send_command(17, (lba>>7)&0xffff, (lba<<9)&0xffff);	    // send read command and logical sector address
	status = sd_datatoken();			// get status or command
	if (status != 0xfe)					// if no valid token
    {
        sd_clock_and_release();			// cleanup and  
        return  SDCARD_RWFAIL;			// return error code
    }

    for (i=0; i<512; i++)           	// read sector data
        *buffer++ = xchg(0xff);

    xchg(0xff);                		 	// ignore dummy checksum
    xchg(0xff);                		 	// ignore dummy checksum

    sd_clock_and_release();				// cleanup

    return  SDCARD_OK;					// return success       
}






/*
 *  sd_writesector      write buffer of data to SD card
 *
 *  I've lost track of who created the original code for this
 *  routine; I found it somewhere on the web.
 *
 *	Write a single 512 byte sector from the MMC/SD card
 *	\param lba	Logical sectornumber to write
 *	\param buffer	Pointer to buffer to write from
 *	\return 0 on success, -1 on error
 */
uint16_t  sd_writesector(uint32_t  lba, uint8_t  *buffer)
{
	uint16_t				i;

	if (!inited)  return  SDCARD_NOT_INITED;

	// send read command and logical sector address
	sd_send_command(24, (lba>>7) & 0xffff, (lba<<9) & 0xffff);

	if ((sd_response(0x00)) == SDCARD_RWFAIL)	// if card does not send back 0...
	{
		sd_clock_and_release();			// cleanup and	
		return  SDCARD_RWFAIL;
	}

	// need to send token here
	xchg(0xfe);							// send data token /* based on PIC code example */

	for (i=0; i<512; i++)				// for all bytes in a sector...
	{
    	xchg(*buffer++);				// send each byte via SPI
	}

	xchg(0xff);							// ignore dummy checksum
	xchg(0xff);							// ignore dummy checksum

	// do we check the status here?
	if ((xchg(0xFF) & 0x0F) != 0x05)  return  SDCARD_RWFAIL;
	
	i = 0xffff;							// max timeout  (nasty timing-critical loop!)
	while (!xchg(0xFF) && (--i))  ;		// wait until we are not busy
	sd_clock_and_release();				// cleanup
	return  SDCARD_OK;					// return success		
}




/*
 *  sd_waitforready      wait until write operation completes
 *
 *  Normally not used, this function is provided to support Chan's FAT32 library.
 */
uint8_t  sd_waitforready(void)
{
   uint8_t				i;
   uint16_t				j;

	if (!inited)  return  SDCARD_NOT_INITED;

    select();							// enable CS
    xchg(0xff);         				// dummy byte

	j = 5000;
	do  {
		i = xchg(0xFF);
	} while ((i != 0xFF) && --j);

	sd_clock_and_release();

	if (i == 0xff)  return  SDCARD_OK;	// if SD card shows ready, report OK
	else			return  SDCARD_RWFAIL;		// else report an error
}




/*
 *  ==========================================================================
 *
 *  The following routines are local to this module and used solely to support
 *  SD card operations.  They should never be called by the host program.
 */

static  uint8_t  sd_send_command(uint8_t  command, uint16_t  px, uint16_t  py)
{
	uint8_t				response;
	uint8_t				i;

	if (!inited)  return SDCARD_NOT_INITED;				// if not registered, ignore

	select();							// enable CS

    xchg(0xff); 	        			// dummy byte

    xchg(command | 0x40);				// command always has bit 6 set!

	xchg(px>>8);						// send high byte of parameter x
	xchg(px&0xff);						// send low byte of parameter x

	xchg(py>>8);						// send high byte of parameter y
	xchg(py&0xff);						// send low byte of parameter y

    xchg(0x95);         				// correct CRC for first command (CMD0) in SPI          
                            			// after that CRC is ignored, so no problem with 
                            			// always sending 0x95                           

	xchg(0xff);							// one last null byte
	response = xchg(0xff);				// get the response
	return  response;					// let the caller sort it out
}







/*
 *  sd_response      read SD card until we get desired response or timeout
 *
 *  I've lost track of who created the original code for this
 *  routine; I found it somewhere on the web.
 *
 *  This function taken from the PIC CCS example
 */
static uint16_t  sd_response(uint8_t  response)
{
    uint16_t			count;			// 16bit repeat, it may be possible to shrink this to 8 bit but there is not much point

	if (!inited)  return  SDCARD_NOT_INITED;

	count = 0xFFFF;
    while ((xchg(0xFF) != response) && (--count > 0)) ;

    if (count==0) return  SDCARD_RWFAIL;	// loop was exited due to timeout
    else return  SDCARD_OK;					// loop was exited before timeout
}




/*
 *  sd_clock_and_release      deselect the SD card and provide additional clocks
 *
 *  The SD card does not release its DO (MISO) line until it sees a clock on SCK
 *  AFTER the CS has gone inactive (Chan, mmc_e.html).  This is not an issue if the
 *  only device on the SPI bus is the SD card, but it can be an issue if the
 *  card shares the bus with other SPI devices.
 *
 *  To be safe, this routine deselects the SD card, then issues eight clock pulses.
 */
static void  sd_clock_and_release(void)
{
	if (!inited)  return;

	deselect();				   				// first, release CS
	xchg(0xff);								// then send 8 final clocks
}






static uint8_t  sd_get(void)
{
    uint16_t 				i;
    uint8_t 				b;

	b = 0xff;
	i = 0xffff;
    while ((b == 0xff) && (--i)) 
    {
        b = xchg(0xff);
    }
    return b;
}




static uint8_t  sd_datatoken(void)
{
    uint16_t				i;
    uint8_t					b;
	volatile uint16_t		dly;

	b = 0xff;
	i = 0x200;
    while ((b != 0xfe) && (--i)) 
    {
		for (dly=0; dly<1000; dly++)  ;
        b = xchg(0xff);
    }
    return b;
}




