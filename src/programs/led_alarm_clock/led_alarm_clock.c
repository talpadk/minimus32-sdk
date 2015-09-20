//#exe
//#global_cflags -D MCU=ATMEGA328P

///A program for a LED/light based alarm clock

/**
 * @file
 * @author Visti Andresen
 * @ingroup prog
 */


#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>

#include "watchdog.h"
#include "sys_clock.h"
#include "vt100_codes.h"
#include "async_serial_0.h"
#include "int_print.h"
#include "sw_dither.h"
//#include "timer0_fast_timer.h"
#include "timer1_clock.h"
#include "led_alarm_clock_adc.h"

sw_dither blueLed;
uint8_t blueLedOn;

#define PANIC_LED_CURRENT (700)


ISR(TIMER0_COMPB_vect) {
  //current_=1000;
  /*  if (!adc_is_busy()){
    current_ = adc_get_result();
    PORTB ^= 0b00100000;
    adc_start_conversion();
    }*/
}

void ledAnimate(void *data){
  if(blueLedOn){
    blueLedOn = 0;
    sw_dither_set(&blueLed, 0);
  }
  else {
    blueLedOn = 1;
    sw_dither_set(&blueLed, 10);
  }
}

int16_t ledCurrentSP_ = 0;
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


void regulate(void *data){
  int16_t spBandSize = ledCurrentSP_/10;
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

void ledCurrentCallback(uint16_t current){
  if (current>PANIC_LED_CURRENT){
    setLedPWM(0);
  }
}

int main(void) {
  char inByte;
  char buffer[UINT16_PRINT_DECIMAL_NULL_SIZE];
  timer1_callback ledTimer;
  timer1_callback regulateTimer;
  
  watchdog_disable();
  clock_prescale_none();	
  
  PORTB &= 0b00000000; //turn off led
  
  DDRB  |= 0b00100000; //enable led as output
  sw_dither_init(&blueLed, 1023);


  PORTD = 0; //power the LED switch off
  DDRD |= 0b01000000; //OC0A as output

  setLedPWM(0);
  
  
  timer1_clock_init();
  timer1_clock_register_callback (0, 500, 1, &ledAnimate, 0, &ledTimer);
  timer1_clock_register_callback (0, 100, 1, &regulate, 0, &regulateTimer);
  initADCSystem();
  registerCurrentCallback(&ledCurrentCallback);
	
  async_serial_0_init(SERIAL_SPEED_9600);
  
  sei(); //global int enable
  
  async_serial_0_write_string(VT100_CURSOR_OFF);
  async_serial_0_write_string(VT100_CLEAR_SCREEN);
  
  while (1){
    if (async_serial_0_byte_ready()){
      inByte = async_serial_0_read_byte();
      if (inByte=='+' && ledCurrentSP_<600){
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
