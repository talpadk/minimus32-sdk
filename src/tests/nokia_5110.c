//#exe

///A program that demonstrates the usage om a Nokia 5110 (PCD8544 based) display

/**
 * @file
 * @author Visti Andresen
 * @ingroup tests
 * 
 * Red  blinking             = Status OK<br>
 *
 * Hardware:<br>
 * The minimus must operate at 3.3V (or level shifters must be used)<br>
 * The N5110 must be supplied with 3.3V on N5110 (Vcc) with enough current<br>
 * The N5110 must be connected to GND on N5100 (GND)<br>
 *<br>
 * PB1 (SCLK)  -> N5110.7 (SCLK)<br>
 * PB2 (MOSI)  -> N5110.6 (DNK)<br>
 * 
 * PB4         -> N5110.3 (SCE)<br>
 * PB5         -> N5110.4 (RST)<br>
 * PB6         -> N5110.5 (D/C)<br>
 *<br>
 * Optional<br>
 * PB -> 62Ohm -> N5110.8 (LED) I haven't found any data on the LED,
 * but a current limiting resistor of 62 Ohm seems safe (~7mA) at 3.3V<br>
 */

#include <avr/interrupt.h>

#include "minimus32.h"
#include "watchdog.h"
#include "sys_clock.h"
#include "timer1_clock.h"
#include "spi.h"
#include "pcd8544.h"
#include "sw_dither.h"
#include "vertical_byte_font_6x8.h"

#define STATE_LENGTH (100)
#define GREY_HACK_STATE 3
uint8_t bar_=0;
uint8_t state_=0;
uint8_t lastState_=255;
uint8_t stateTimer_=STATE_LENGTH;

sw_dither backlight;
void handleBackLight(void *data){
  if (sw_dither_animate(&backlight)) PORTB |= (1<<PB7);
  else PORTB &= ~(1<<PB7);
}

void blink(void *data){
  led_red_toggle();
  switch (state_){
  case 0:
    if (lastState_!=state_){
      pcd8544_fill_screen(0);
      pcd8544_print(4,0, "MOVEING LINES", &vertical_byte_font_6x8);
      pcd8544_print(0,1, "==============", &vertical_byte_font_6x8);
      pcd8544_print(0,2, "Displays a", &vertical_byte_font_6x8);
      pcd8544_print(0,3, "pattern that", &vertical_byte_font_6x8);
      pcd8544_print(0,4, "tests every", &vertical_byte_font_6x8);
      pcd8544_print(0,5, "pixel", &vertical_byte_font_6x8);
    }
    break;
  case 1:
    pcd8544_test(bar_);
    bar_++;
    if (bar_==8) bar_=0;
    break;
  case 2:
    if (lastState_!=state_){
      pcd8544_fill_screen(0);
      pcd8544_print(16,0, "4 COLOURS", &vertical_byte_font_6x8);
      pcd8544_print(0,1, "==============", &vertical_byte_font_6x8);
      pcd8544_print(0,2, "Fakes 4 grey", &vertical_byte_font_6x8);
      pcd8544_print(0,3, "shades by", &vertical_byte_font_6x8);
      pcd8544_print(0,4, "flickering", &vertical_byte_font_6x8);
      pcd8544_print(0,5, "the display", &vertical_byte_font_6x8);
    }
    break;
  }

    
    lastState_ = state_;
  stateTimer_--;
  if (stateTimer_==0){
    state_++;
    if (state_>GREY_HACK_STATE) state_=0;
    stateTimer_=STATE_LENGTH;
  }
}

void assertReset()   { PORTB &= ~(1<<PB5); }
void desertReset()   { PORTB |=  (1<<PB5); }
void assertCS()      { PORTB &= ~(1<<PB4); }
void desertCS()      { PORTB |=  (1<<PB4); }
void assertCMD()     { PORTB &= ~(1<<PB6); }
void desertCMD()     { PORTB |=  (1<<PB6); }


pcd8544_io pcdIO = {
  assertReset,
  desertReset,
  assertCS,
  desertCS,
  assertCMD,
  desertCMD  
};

void greyScaleHack(uint8_t callNumber){
  uint8_t x,colour,bits;

  spi_obtain_bus(1);

  pcdIO.assert_command();
  pcdIO.assert_chip_delect();
  
  spi_setup(SPI_DIVIDER_4, 0); //4 Mhz @ 16Mhz
  spi_io(PCD8544_CMD_SET_Y);
  spi_io(PCD8544_CMD_SET_X);
  pcdIO.desert_command();

  colour=0;
  for (x=0; x<84; x++){
    if (colour>callNumber) bits=0xff;
    else bits=0;
    spi_io(bits);
    spi_io(bits);
    spi_io(bits);
    spi_io(bits);
    spi_io(bits);
    spi_io(bits);

    if ((x&7)==0)  colour++;
    colour &= 3;
  }
  
  pcdIO.desert_chip_select();   
  spi_release_bus();
}

int main(void) { 
  uint16_t i,j;
  uint8_t s;

  timer1_callback blink_call_back;
  timer1_callback backlight_call_back;

  watchdog_disable();
  minimus32_init();
  clock_prescale_none();
  spi_config_io_for_master_mode();
  DDRB |= 0b11110000; //IO setup for the reset,cs, cmd and led pin

  pcd8544_init(&pcdIO, 45);

  //Setup a blink pattern indicating a working MCU
  timer1_clock_init();
  timer1_clock_register_callback(0, 100, 1, &blink, 0, &blink_call_back);

  //Enable the pulsed output that controls the backlight
  sw_dither_init(&backlight, 1023);
  timer1_clock_register_callback(0, 1, 1, &handleBackLight, 0, &backlight_call_back);

  for (i=0; i<1024; i++){
    for(j=0; j<2000; j++){}
    sw_dither_set(&backlight, i);
  } 

  //Show grey scale hack
  while (1) {
    while (state_==GREY_HACK_STATE){
      greyScaleHack(s);
      s++;
      if (s>2) s=0;
      for(j=0; j<7500; j++){}
    }
  }
  return 0;
}
