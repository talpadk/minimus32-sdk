//#exe


/**
 * @file
 * @author Visti Andresen
 * @ingroup tests
 *
 * @brief A test/demo program that runs a stepmotor using a L298N based driver
 */


#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>

#include "minimus32.h"
#include "watchdog.h"
#include "sys_clock.h"
#include "timer1_clock.h"

typedef enum {
  IDLE,
  FULL_STEP,
  TORQUE_STEP,
  HALF_STEP,
} DemoState;

DemoState demoState_=IDLE;
uint8_t stepperState_ = 0;
char buttonLast_ = 0;

#define A_FLOAT (0)
#define A_POS (0b01100)
#define A_NEG (0b10100)
#define B_FLOAT (0)
#define B_POS (0b10100000)
#define B_NEG (0b11000000)

timer1_callback stepCallBack_;


void performStep(void *data){
  uint8_t blinkState=stepperState_&7;
  led_blue_toggle();

  
  //Amimate stepper
  switch (demoState_){
  default:
  case IDLE:
    PORTB = A_FLOAT|B_FLOAT;
    led_red(0);
    break;
  case FULL_STEP:
    if (blinkState==0) led_red(1);
    else led_red(0);
    switch (stepperState_){
    case 0:
    case 1:
      PORTB=A_POS|B_FLOAT;
      break;
    case 2:
    case 3:
      PORTB=B_POS|A_FLOAT;
      break;
    case 4:
    case 5:
      PORTB=A_NEG|B_FLOAT;
      break;
    case 6:
    case 7:
      PORTB=B_NEG|A_FLOAT;
      break;
    }
    break;
  case TORQUE_STEP:
    if (blinkState==0 || blinkState==3) led_red(1);
    else led_red(0);
    switch (stepperState_){
    case 0:
    case 1:
      PORTB=A_POS|B_POS;
      break;
    case 2:
    case 3:
      PORTB=A_NEG|B_POS;
      break;
    case 4:
    case 5:
      PORTB=A_NEG|B_NEG;
      break;
    case 6:
    case 7:
      PORTB=A_POS|B_NEG;
      break;
    }
    break;
  case HALF_STEP:
    if (blinkState==0 || blinkState==3 || blinkState==5) led_red(1);
    else led_red(0);
    switch (stepperState_){
    case 0:
      PORTB=A_POS|B_FLOAT;
      break;
    case 1:
      PORTB=A_POS|B_POS;
      break;
    case 2:
      PORTB=A_FLOAT|B_POS;
      break;
    case 3:
      PORTB=A_NEG|B_POS;
      break;
    case 4:
      PORTB=A_NEG|B_FLOAT;
      break;
    case 5:
      PORTB=A_NEG|B_NEG;
      break;
    case 6:
      PORTB=A_FLOAT|B_NEG;
      break;
    case 7:
      PORTB=A_POS|B_NEG;
      break;
    }
    break;

  }
  //Update stepper pos
  if (demoState_!=IDLE){
    stepperState_++;
    if (stepperState_>7) stepperState_=0;
  }
}

void buttonHandler(void *data){
  char button = hwb_is_pressed(); //debounced due to "sample speed" (100ms)
  if (button!=buttonLast_ && button){ 
    if (demoState_<HALF_STEP) demoState_++; 
    else demoState_=IDLE;
  }
  buttonLast_ = button; 
}


int main(void) {
  timer1_callback buttonCallBack;

  watchdog_disable();
  minimus32_init();
  clock_prescale_none();
  
  DDRB = 0b11111100; //setup PB2-PB7 as output PB0 and PB1 are reserved don't use

  sei(); //global int enable

  //Setup a blink pattern indicating a working MCU
  timer1_clock_init();
  /*
   * Note: the stepping speed could also be modified by reregistering the callback with a different period.
   * It would be more efficient for the CPU, but de button read out delay 
   */
  timer1_clock_register_callback(0, 100, 1, &performStep, 0, &stepCallBack_); 
  timer1_clock_register_callback(0, 100, 1, &buttonHandler, 0, &buttonCallBack); 
  
  while (1){
  }
  return 0;
}
