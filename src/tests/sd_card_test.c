//#exe

///A program that demonstrates reading from a SD card

/**
 * @file
 * @author Visti Andresen
 * @ingroup tests
 * 
 * Red  blinking             = Start up delay
 * Blue blinking, red  solid = Status OK 
 * Red  blinking, blue solid = An error has occurred
 *
 * Hardware:<br>
 * The minimus must operate at 3.3V (or level shifters must be used)
 * The SD card must be supplied with 3.3V on SD.4 (Vcc) with enough current
 * The SD card must be connected to GND on SD.3 (Vss1) and SD.6 (Vss2)
 *
 * PB0 (SS)    -> SD.1 (CS)
 * PB1 (SCLK)  -> SD.5 (SCLK)
 * PB2 (MOSI)  -> SD.2 (DI)
 * PB3 (MISO)  <- SD.7 (DO)
 * 
 * Also the test program writes its output to the serial port of the minimus (9600 baud 8N1)
 * Remember to use an appropriate level shifter from 3.3V to regular RS232 voltages
 * (or a 3.3V Blue Tooth serial module)
 *
 * @TODO update the code to increase the SPI bus speed
 */

#include <avr/interrupt.h>

#include "minimus32.h"
#include "watchdog.h"
#include "sys_clock.h"
#include "timer1_clock.h"
#include "vt100_codes.h"
#include "async_serial.h"
#include "spi.h"
#include "sdcard.h"

#define POWER_ON_DELAY (11)
#define BLINK_FATAL (100)
char blinkTimer=0;

//Buffer for IO blocks
uint8_t block[512];


void blink(void *data){
  if (blinkTimer==BLINK_FATAL){
    led_red_toggle();
    led_blue(1);
  }
  else {
    if (blinkTimer!=POWER_ON_DELAY){
      led_red_toggle();
      blinkTimer++;
    }
    else {
      led_blue_toggle();
    }
  }

}

void selectSDCard(){ PORTB &= ~(1<<PB0); }
void deselectSDCard(){ PORTB &= (1<<PB0); }

SD_CALLBACKS mySDCardIO = {
  selectSDCard,
  spi_io,
  deselectSDCard,
  0
};


void writeHexNibble(uint8_t data){
  if (data<10) async_serial_write_byte(data+'0');
  else  async_serial_write_byte(data+'A'-10);
}

void writeHex(uint8_t byte){
  writeHexNibble(byte>>4);
  writeHexNibble(byte&0x0F);
}

//Little indian
void writeLongHex(uint8_t *buf){
  writeHex(*(buf+3));
  writeHex(*(buf+2));
  writeHex(*(buf+1));
  writeHex(*(buf+0));
}


char swapFound = -1;
uint32_t swapOffset=0;

void analyzePartitionTableRecord(char index){
  uint16_t offset = 446+(index<<4);
  async_serial_write_string("\r\nPrimary Partition #");
  async_serial_write_byte(index+'0');

  async_serial_write_string(" start=0x");
  writeLongHex(block+offset+8);

  async_serial_write_string(" length=0x");
  writeLongHex(block+offset+12);

  async_serial_write_string(" type=0x");
  writeHex(block[offset+4]);
  async_serial_write_string("\r\n");

  //Found a Linux Swap partition
  if (block[offset+4]==0x82 && swapFound==-1){
    swapFound = index;
    swapOffset=(((uint32_t)block[offset+8+3])<<24)+
               (((uint32_t)block[offset+8+2])<<16)+
               (((uint32_t)block[offset+8+1])<<8)+
                            block[offset+8+0];
  }
}

void analyzePartitionTable(){
  async_serial_write_string("\r\nPartition table contents:\n\r");
  analyzePartitionTableRecord(0);
  analyzePartitionTableRecord(1);
  analyzePartitionTableRecord(2);
  analyzePartitionTableRecord(3);
}

void die(const char *string){
  async_serial_write_string(string);
  blinkTimer=BLINK_FATAL;
  while(1){};
}

int main(void) { 
  timer1_callback blink_call_back;

  watchdog_disable();
  minimus32_init();
  clock_prescale_none();

  sei(); //global int enable

  async_serial_init(SERIAL_SPEED_9600);

  spi_obtain_bus(1); //technically not needed since there are just one user of the bus
  spi_config_io_for_master_mode();
  spi_setup(SPI_DIVIDER_128, 0);

  //Setup a blink pattern indicating a working MCU
  timer1_clock_init();
  timer1_clock_register_callback(0, 100, 1, &blink, 0, &blink_call_back);

  while (blinkTimer!=POWER_ON_DELAY){} //wait for stable power supply at the SD card

  async_serial_write_string(VT100_CURSOR_OFF);
  async_serial_write_string(VT100_CLEAR_SCREEN);
  async_serial_write_string("=== Welcome to the SD-card test ===\r\n");


  if (sd_register(&mySDCardIO)!=SDCARD_OK){
      die("SD-card register failed!!!");
  }

  if (sd_readsector(0, block)!=SDCARD_OK){
      die("SD-card read failed!!!");
  }

  analyzePartitionTable();

  if (swapFound!=-1){
     async_serial_write_string("\r\nLinux Swap partition found at #");
     async_serial_write_byte(swapFound+'0');
     async_serial_write_string("\r\n\r\n--- Dump BEGIN ---\r\n");

     //Read first block of swap data
     if (sd_readsector(swapOffset, block)!=SDCARD_OK){
       die("Swap read failed!!!");
     }
     block[511]=0; //ensure null termination
     async_serial_write_string((char*)block);
     async_serial_write_string("\r\n--- Dump END ---");
  }

  while (1) {
  }
  spi_release_bus(); //technically not needed since there are just one user of the bus
  return 0;
}
