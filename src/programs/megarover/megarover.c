//#exe

#include <avr/io.h>
#include <avr/interrupt.h>

#include "minimus32.h"
#include "watchdog.h"
#include "sys_clock.h"
#include "timer1_clock.h"

#include "h-bridge-3pin.h"


HBridge3PinHandle bSide_;

void blink(void *data){
  led_red_toggle();
}

char sign_=1;

void signChanger(void *data){
  led_blue_toggle();
  if (sign_==1) sign_=-1;
  else sign_=1;
}

void powerCtrl(void *data){
  static int16_t power=0;
  static int16_t divider=0;

  divider++;
  if (divider>100){
    divider=0;
    if (hwb_is_pressed() && power<50) power++;
    else if (power>0) power--;
  }

  if (sign_==1) hBridge3PinSetPower(power, &bSide_);
  else  hBridge3PinSetPower(-power, &bSide_);

  hBridge3PinAnimate(&bSide_);
  
}


void bSideIO(uint8_t enable, uint8_t aSide, uint8_t bSide){
  enable &= 1<<7;
  bSide  &= 1<<6;
  aSide  &= 1<<5;
  PORTB = enable|aSide|bSide;
}

int main(void) {
  timer1_callback blinkCallback;
  timer1_callback powerCallback;
  timer1_callback signCallback;

  watchdog_disable();
  minimus32_init();
  clock_prescale_none();
  
  DDRB = 0b11111100; //setup PB2-PB7 as output PB0 and PB1 are reserved don't use

  hBridge3PinInit(126, &bSideIO, &bSide_);

  sei(); //global int enable

  //Setup a blink pattern indicating a working MCU
  timer1_clock_init();
  timer1_clock_register_callback(0, 100, 1, &blink, 0, &blinkCallback); 
  timer1_clock_register_callback(0, 2, 1, &powerCtrl, 0, &powerCallback);
  timer1_clock_register_callback(10, 9, 1, &signChanger, 0, &signCallback);
  
  while (1){
  }
  return 0;
}
