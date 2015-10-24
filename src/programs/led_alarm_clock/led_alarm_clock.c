//#exe
//#global_cflags -D MCU=ATMEGA328P

///A program for a LED/light based alarm clock

/**
 * @file
 * @author Visti Andresen
 * @ingroup prog
 */

/*
 * PC0 = N.C. (Used to be battery voltage)
 * PC1 = LED current signal
 * PC2 = LED temperature (from LM35)
 * PC3 = LED voltage
 *
 * PD3 = Encoder A
 * PD7 = Encoder B
 * PB0 = Encoder button
 *
 * PD5 = LED FAN (OC0B)
 * PD6 = LED PWM (OC0A)
 * 
 * PB2 = LCD CS
 * PB3 = LCD MOSI
 * PB4 = LCD MISO
 * PB5 = LCD SCK
 * PD2 = LDC D/C
 * PD4 = LDC RESET
 */



#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "watchdog.h"
#include "sys_clock.h"
#include "vt100_codes.h"
#include "async_serial_0.h"
#include "int_print.h"
//#include "timer0_fast_timer.h"
#include "timer1_clock.h"
#include "led_alarm_clock_adc.h"
#include "twi.h"
#include "lcd_ili9341.h"
#include "quadrature_decoder.h"

#include "bitfont_18x24.h"

//550mA (550*1024/579)
#define PANIC_LED_RAW_CURRENT (973)


int16_t ledCurrentSP_ = 10;
uint16_t ledPWM_ = 0;

void setLedPWM(uint16_t setPoint){
  ledPWM_ = setPoint;
  if (setPoint>0){
    TCCR0A = 0b10000011; //Timer 0 A output fast pwm
    TCCR0B = 1; //no prescaling
    OCR0A =  (setPoint-1); //Power
    OCR0B =  190; //cur sample   
  }
  else {
    TCCR0A = 0b00000011; //OC0A normal port operation
    PORTD &= ~0b01000000; //turn it off (probably not needed)
  }
}

void setDataCommandPin(unsigned char value){
  value &= 0b00000100;
  PORTD = (PORTD & 0b11111011)|value;
}

void setChipSelectPin(unsigned char value){
  value &= 0b00000100;
  PORTB = (PORTB & 0b11111011)|value;
}

unsigned char encoderA(){
  return PIND & 1<<3;
}
unsigned char encoderB(){
  return PIND & 1<<7;
}

void encoderCallback(unsigned char up){
  if (up){
    if (ledCurrentSP_<400) { ledCurrentSP_+=1; }
  }
  else {
    if (ledCurrentSP_>0){ ledCurrentSP_-=1; }
  }
}

QuadratureDecoder decoder = {
  &encoderA,
  &encoderB,
  &encoderCallback,
  0  
};

ISR(PCINT2_vect, ISR_BLOCK){
  quadrature_decoder_handleEvent(&decoder);
}

lcd_ili9341_device display = {
  &setChipSelectPin,
  &setDataCommandPin
};

void updateClockDisplayFunction(void *data){
  timer1_wall_time time;
  char timeString[3*3];
  char buffer[UINT16_PRINT_DECIMAL_NULL_SIZE];
  timer1_clock_get_time(&time);
  uint8Print(time.hour, 2, timeString+0);
  timeString[2]=':';
  uint8Print(time.min,  2, timeString+3);
  timeString[3+2]=':';
  uint8Print(time.sec,  2, timeString+6);
  
  lcd_ili9341_drawBitFontCenteredString(0,320,44, timeString, &bitfont_18x24, ILI9341_COLOUR_WHITE, ILI9341_COLOUR_BLACK);


  uint16PrintDecimalNull(getLEDTemperature(), 1, buffer);
  replaceLeadingZeros(buffer);
  lcd_ili9341_drawBitFontString(10,144, buffer, &bitfont_18x24, ILI9341_COLOUR_WHITE, ILI9341_COLOUR_BLACK);
  lcd_ili9341_drawBitFontString(10+18*6,144, "C", &bitfont_18x24, ILI9341_COLOUR_WHITE, ILI9341_COLOUR_BLACK);

  uint16PrintNull(getLEDCurrent(), buffer);
  replaceLeadingZeros(buffer);
  lcd_ili9341_drawBitFontString(10,144+24*1, buffer, &bitfont_18x24, ILI9341_COLOUR_WHITE, ILI9341_COLOUR_BLACK);

  uint16PrintNull(ledCurrentSP_, buffer);
  replaceLeadingZeros(buffer);
  lcd_ili9341_drawBitFontString(10,144+24*2, buffer, &bitfont_18x24, ILI9341_COLOUR_WHITE, ILI9341_COLOUR_BLACK);
}

void regulate(void *data){
  int16_t spBandSize = ledCurrentSP_/20;
  int16_t ledCurrent = getLEDCurrent();

  if (spBandSize<2) { spBandSize=2; }
    
  if (ledCurrentSP_==0){
    ledPWM_  = 0;
  }
  else {
    if (ledPWM_<=255 && ledCurrentSP_-spBandSize>ledCurrent){
      ledPWM_++;
    }
    else if (ledPWM_>0 && ledCurrentSP_+spBandSize<ledCurrent){
      ledPWM_--;
    }
  }
  setLedPWM(ledPWM_);
}

void ledCurrentCallback(uint16_t rawCurrent){
  if (rawCurrent>PANIC_LED_RAW_CURRENT){
    setLedPWM(0);
  }
}

int main(void) {
  char inByte;
  char buffer[UINT16_PRINT_DECIMAL_NULL_SIZE];
  timer1_callback regulateTimer;
  timer1_callback updateClockDisplay;
  
  watchdog_disable();
  clock_prescale_none();	
  spi_config_io_for_master_mode();
  
  DDRD  |= 0b01110100; //PD6,PD5,PD4,PD2 as output

  PORTD |= (1<<3)|(1<<7); //Enable pullup for the quad encoder
  PORTB |= (1);
  PCMSK2=1<<7|1<<3; //Enable masking for pin change irq.
  quadrature_decoder_init(&decoder);
  PCICR=1<<2; //Enable the IRQs
  
  PORTD &=~0b00010000; //reset LCD
  _delay_ms(10);
  PORTD |= 0b00010000;
  _delay_ms(100);
  


  setLedPWM(0);
  

  timer1_clock_init();
  timer1_clock_register_callback (0, 100, 1, &regulate, 0, &regulateTimer);
  timer1_clock_register_callback (0, 500, 1, &updateClockDisplayFunction, 0, &updateClockDisplay);
  initADCSystem();
  registerCurrentCallback(&ledCurrentCallback);
	
  async_serial_0_init(SERIAL_SPEED_9600);
 
  sei(); //global int enable


  lcd_ili9341_obtainBus(1);
  lcd_ili9341_selectDevice(&display);
  lcd_ili9341_init();
  lcd_ili9341_setDisplayMode(ILI9341_DISPLAY_MODE_BGR|ILI9341_DISPLAY_MODE_LANDSCAPE);
  lcd_ili9341_drawFilledRectangle(ILI9341_COLOUR_BLACK, 0, 319, 0, 239);
  lcd_ili9341_drawBitFontString(20,20,"Testing", &bitfont_18x24, ILI9341_COLOUR_WHITE, ILI9341_COLOUR_BLACK);
  async_serial_0_write_string(VT100_CURSOR_OFF);
  async_serial_0_write_string(VT100_CLEAR_SCREEN);

  
  while (1){
    if (async_serial_0_byte_ready()){
      inByte = async_serial_0_read_byte();
      if (inByte=='+' && ledCurrentSP_<500){
	ledCurrentSP_+=10;
      }
      else if (inByte=='-' && ledCurrentSP_>0){
	ledCurrentSP_-=10;
      }
      
    }

    async_serial_0_write_string(VT100_CURSOR_HOME);
    async_serial_0_write_string("=== Hello world ===\r\n");

    
    uint16PrintDecimalNull(getInputVoltage(), 2, buffer);
    replaceLeadingZeros(buffer);
    async_serial_0_write_string(buffer);
    async_serial_0_write_string(" V\r\n");

    uint16Print(getLEDCurrent(), buffer);
    buffer[UINT16_PRINT_SIZE]=0;
    replaceLeadingZeros(buffer);
    async_serial_0_write_string(buffer);
    async_serial_0_write_string(" mA\r\n");
    
    uint16Print(ledPWM_, buffer);
    buffer[UINT16_PRINT_SIZE]=0;
    replaceLeadingZeros(buffer);
    async_serial_0_write_string(buffer);
    async_serial_0_write_string(" PWM\r\n");

    uint16Print(ledCurrentSP_, buffer);
    buffer[UINT16_PRINT_SIZE]=0;
    replaceLeadingZeros(buffer);
    async_serial_0_write_string(buffer);
    async_serial_0_write_string(" SP\r\n");
  }
  return 0;
}
