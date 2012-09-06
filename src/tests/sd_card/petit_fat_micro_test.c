//#exe

///A program that demonstrates reading from a SD card

/**
 * @file
 * @author Visti Andresen
 * @ingroup tests
 * 
 * Red  blinking             = Start up delay<br>
 * Blue blinking, red  solid = Status OK <br>
 * Red  blinking, blue solid = An error has occurred<br>
 *<br>
 * Hardware:<br>
 * The minimus must operate at 3.3V (or level shifters must be used)<br>
 * The SD card must be supplied with 3.3V on SD.4 (Vcc) with enough current<br>
 * The SD card must be connected to GND on SD.3 (Vss1) and SD.6 (Vss2)<br>
 *<br>
 * PB0 (SS)    -> SD.1 (CS)<br>
 * PB1 (SCLK)  -> SD.5 (SCLK)<br>
 * PB2 (MOSI)  -> SD.2 (DI)<br>
 * PB3 (MISO)  <- SD.7 (DO)<br>
 * 
 * Also the test program writes its output to the serial port of the minimus (9600 baud 8N1)<br>
 * Remember to use an appropriate level shifter from 3.3V to regular RS232 voltages
 * (or a 3.3V Blue Tooth serial module)<br>
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
#include "pff.h"
#include "pff_diskio_brain_dead.h"

#define POWER_ON_DELAY (11)
#define BLINK_FATAL (100)
char blinkTimer=0;


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
void deselectSDCard(){ PORTB |= (1<<PB0); }

SD_CALLBACKS mySDCardIO = {
  selectSDCard,
  spi_io,
  deselectSDCard,
  0
};

void die(const char *string){
  async_serial_write_string(string);
  blinkTimer=BLINK_FATAL;
  while(1){};
}

int main(void) { 
  FATFS fs;
  FRESULT result;
  char buffer;
  WORD bytesRead=1;

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
  async_serial_write_string("=== Welcome to the SD-card FAT FS test ===\r\n");

  if (sd_register(&mySDCardIO)!=SDCARD_OK){
    die("SD-card register failed!!!");
  }
 

  result = pf_mount(&fs);
  if (result!=FR_OK) die("FAT FS Mount FAILED!!!\r\n");

  async_serial_write_string("FAT FS Mount OK\r\n");

  result = pf_open("hello.txt");
  if (result!=FR_OK) die("Open of hello.txt FAILED\r\n");

  async_serial_write_string("Reading hello.txt\r\n");
  
  while (bytesRead==1){
    if (pf_read(&buffer, 1, &bytesRead)!=FR_OK) bytesRead=0;
    if (bytesRead==1) async_serial_write_byte(buffer);
  }

  
  while (1) {
  }
  spi_release_bus(); //technically not needed since there are just one user of the bus
  return 0;
}
