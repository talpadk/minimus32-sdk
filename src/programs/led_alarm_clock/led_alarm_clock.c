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
#include "adc_single_ended.h"
#include "int_print.h"
#include "sw_dither.h"
//#include "timer0_fast_timer.h"
#include "timer1_clock.h"

sw_dither blueLed;
uint8_t blueLedOn;

uint32_t current_=0;


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

int main(void) {
  char inByte;
  uint32_t adcValue;
  uint32_t supplyVoltage;
  uint32_t printValue;
  char buffer[UINT16_PRINT_DECIMAL_NULL_SIZE];
  timer1_callback ledTimer;
  
  watchdog_disable();
  clock_prescale_none();	
  
  PORTB &= 0b00000000; //turn off led
  
  DDRB  |= 0b00100000; //enable led as output
  sw_dither_init(&blueLed, 1023);


  PORTD = 0; //power the LED switch off
  DDRD |= 0b01000000; //OC0A as output

  setLedPWM(0);
  //TIMSK0 = 1<<OCIE0B;
  
  
  timer1_clock_init();
  timer1_clock_register_callback (0, 500, 1, &ledAnimate, 0, &ledTimer);
  

  
	
  async_serial_0_init(SERIAL_SPEED_9600);

  //ADCSRB |= 1<<ACME;
  //adc_set_reference_and_adj(ADC_REF_1V1, 0);
  adc_set_reference_and_adj(ADC_REF_AVCC, 0);
  adc_set_channel(ADC1);
  DIDR0 = 3;
  //128kHZ @ 16MHZ
  adc_set_speed_and_enable(ADC_DIV_128, 0, 0);
  
  sei(); //global int enable
  
  async_serial_0_write_string(VT100_CURSOR_OFF);
  async_serial_0_write_string(VT100_CLEAR_SCREEN);
  while (1){

    
    //PORTB ^= 0b00100000;
    
    if (async_serial_0_byte_ready()){
      inByte = async_serial_0_read_byte();
      if (inByte=='+' && ledPWM_<255){
	setLedPWM(ledPWM_+1);
      }
      else if (inByte=='-' && ledPWM_>0){
	setLedPWM(ledPWM_-1);
      }
      
    }

    adc_set_channel(ADC0);
    adc_start_conversion();
    while(adc_is_busy()){}
    adcValue = adc_get_result();
    //supplyVoltage = ((supplyVoltage*31)+adcValue)/32;
    supplyVoltage = adcValue;
    
    //adc_set_channel(ADC_GND);
    adc_set_channel(ADC1);
    adc_start_conversion();
    while(adc_is_busy()){}
    adcValue = adc_get_result();
    //current_ = ((current_*31)+adcValue)/32;
    current_ = adcValue;
    
    async_serial_0_write_string(VT100_CURSOR_HOME);
    async_serial_0_write_string("=== Hello world ===\r\n");

    
    //1024=5V from a 100k + 220k voltage divider
    printValue = supplyVoltage*1600/1024;
    uint16PrintDecimalNull(printValue, 2, buffer);
    replaceLeadingZeros(buffer);
    async_serial_0_write_string(buffer);
    async_serial_0_write_string(" V\r\n");

    //1024=5V 0.1 Ohm resistor and 19 times gain
    //5V = 2631.57 mA @0.1 Ohm
    //1V1 = 0.5789 @0.1 Ohm
    //5V = 887.19 mA @ 0.3 Ohm
    printValue = current_;
    //printValue = printValue*579/1024; //1.1V reference
    printValue = printValue*887/1024; //5V reference
    uint16Print(printValue, buffer);
    buffer[UINT16_PRINT_SIZE]=0;
    replaceLeadingZeros(buffer);
    async_serial_0_write_string(buffer);
    async_serial_0_write_string(" mA\r\n");
    
    uint16Print(ledPWM_, buffer);
    buffer[UINT16_PRINT_SIZE]=0;
    replaceLeadingZeros(buffer);
    async_serial_0_write_string(buffer);
    async_serial_0_write_string(" PWM\r\n");
  }
  return 0;
}
