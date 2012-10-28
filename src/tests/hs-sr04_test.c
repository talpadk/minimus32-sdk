/**
 * @file   hs-sr04_test.c
 * @author Visti Andresen <talpa@Hermes.talpa.dk>
 * 
 * @brief  A test program for the ultra sonic sensor HS-SR04
 * @ingroup tests
 * @ingroup tests_sensor
 * 
 * Red Led change  = Ping send
 * Blue Led change = Measurement done and send over RS-232
 *
 * PC7 = Echo
 * PC6 = Trig
 * 
 * Output is done on the serial port (9600 Baud)
 *
 */


//#exe
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "minimus32.h"
#include "watchdog.h"
#include "sys_clock.h"
#include "timer1_clock.h"
#include "external_interrupt.h"
#include "vt100_codes.h"
#include "async_serial.h"

typedef enum {
  SAMPLE_NA=0,
  PING_SEND,
  MEASUREING_PULSE,
  SAMPLE_DONE  
} SampleState;

#define ECHO_PIN (7)
#define TRIG_PIN (6)

volatile SampleState sampleState_ = SAMPLE_NA;

volatile uint16_t pingBeginMS, pingBeginTick;
volatile uint16_t pingEndMS, pingEndTick;


ISR(INT4_vect){
  uint16_t ms,ticks;
  switch(sampleState_){
  default:
    //Should never happen
    break;
  case PING_SEND:
    timer1_clock_get_micro_time(&ms, &ticks);
    pingBeginMS = ms;
    pingBeginTick = ticks;
    sampleState_ = MEASUREING_PULSE;
    enable_external_interrupt_input (4, falling);
    break;
  case MEASUREING_PULSE:
    timer1_clock_get_micro_time(&ms, &ticks);
    pingEndMS = ms;
    pingEndTick = ticks;
    sampleState_ = SAMPLE_DONE;
    disable_external_interrupt_input(4);
    break;
  }
}

void sample(void *data){
  if (sampleState_!=SAMPLE_NA) return;
  sampleState_ = PING_SEND;
  led_red_toggle();
  //Enable the IRQ for INT4
  enable_external_interrupt_input (4, rising); 

  //Create trigger pulse
  PORTC |= 1<<TRIG_PIN;
  _delay_us(50);
  PORTC &= ~(1<<TRIG_PIN);
}

int main(void) {
  uint32_t pulseLength;
  uint32_t pulseLengthMM;
  uint16_t tmpPingBeginMS, tmpPingBeginTick;
  uint16_t tmpPingEndMS, tmpPingEndTick;
  int i;
  char buffer[10];
  timer1_callback sample_call_back;

  watchdog_disable();
  minimus32_init();
  clock_prescale_none();

  DDRC |= 1<<TRIG_PIN; //output

  sei(); //global int enable

  timer1_clock_init();
  timer1_clock_register_callback(0, 50, 1, &sample, 0, &sample_call_back);

  async_serial_init(SERIAL_SPEED_9600);
  async_serial_write_string(VT100_CURSOR_OFF);
  async_serial_write_string(VT100_CLEAR_SCREEN);
  async_serial_write_string(VT100_CURSOR_HOME);
  while(1){
    while(sampleState_!=SAMPLE_DONE){} //wait for sample 
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
      sampleState_     = SAMPLE_NA;
      tmpPingEndMS     = pingEndMS;
      tmpPingBeginMS   = pingBeginMS;
      tmpPingEndTick   = pingEndTick;
      tmpPingBeginTick = pingBeginTick;
    }
    pulseLength = ((uint32_t)tmpPingEndMS-(uint32_t)tmpPingBeginMS)*TIMER1_TICKS_PER_US*1000;
    pulseLength += (uint32_t)tmpPingEndTick - (uint32_t)tmpPingBeginTick;
   

    pulseLengthMM = (pulseLength*10)/(16*58);

    for (i=8; i>=0; i--){
      buffer[i]=(pulseLength%10)+'0';
      pulseLength/=10;
    }
    buffer[9]=0;
    async_serial_write_string(VT100_CURSOR_HOME);
    async_serial_write_string("Got ping: ticks=");
    async_serial_write_string(buffer);

    for (i=8; i>=0; i--){
      buffer[i]=(pulseLengthMM%10)+'0';
      pulseLengthMM/=10;
    }
    buffer[9]=0;
    async_serial_write_string("\r\n             mm=");
    async_serial_write_string(buffer);
    led_blue_toggle();
  }
}
