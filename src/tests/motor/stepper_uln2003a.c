//#exe


/**
 * @file
 * @author Visti Andresen
 * @ingroup tests
 * @ingroup tests_motor
 *
 * @brief A test/demo program that runs a stepmotor using ULN2003A based hardware.
 *
 * <pre>
 * LEDS: 
 * ===== 
 * Blue LED blinks with the step speed
 *
 * RED OFF          = Idle 
 * RED one blink    = Full Step 
 * RED two blinks   = Torque Step 
 * RED three blinks = Half Step 
 *
 * HWB: 
 * ==== 
 * Short    (<1 sec) press: Cycles through the drive type as indicated by the red LED 
 * Long    (1-5 sec) press: Cycles through the drive speeds (low, med and fast) 
 * Very long (>5sec) press: Reverses the rotation direction (clockwise as default)
 *
 * Code tested with a: 
 * ULN2003 based motor controller and 28BYJ-48 5VDC step motor "kit"
 * 
 * Links: 
 * ====== 
 * http://dx.com/p/28ybt-48-stepper-motor-with-uln2003-driver-dc-5v-126409?item=3
 *
 * Hardware: 
 * ========= 
 * CTRL.-   -> External GND
 * CTRL.+   -> External 5V
 * (USB 5V might be capable of supplying enough power, take care if you deside to use it)
 *
 * CTRL.IN1 -> PB2
 * CTRL.IN2 -> PB3 
 * CTRL.IN3 -> PB4 
 * CTRL.IN4 -> PB5 
 * CTRL.-   -> GND
 * </pre>
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

#define A (1<<2)
#define B (1<<3)
#define C (1<<4)
#define D (1<<5)

timer1_callback stepCallBack_;
DemoState demoState_=IDLE;
int8_t stepperState_ = 0;
int8_t stepperDirection_ = 1;
uint8_t currentStepDelay_ = 100;
void performStep(void *data){
  led_blue_toggle();

  //Amimate stepper
  switch (demoState_){
  default:
  case IDLE:
    PORTB = 0;
    break;
  case FULL_STEP:
    switch (stepperState_){
    case 0:
    case 1:
      PORTB=A;
      break;
    case 2:
    case 3:
      PORTB=B;
      break;
    case 4:
    case 5:
      PORTB=C;
      break;
    case 6:
    case 7:
      PORTB=D;
      break;
    }
    break;
  case TORQUE_STEP:
    switch (stepperState_){
    case 0:
    case 1:
      PORTB=A+B;
      break;
    case 2:
    case 3:
      PORTB=B+C;
      break;
    case 4:
    case 5:
      PORTB=C+D;
      break;
    case 6:
    case 7:
      PORTB=D+A;
      break;
    }
    break;
  case HALF_STEP:
    switch (stepperState_){
    case 0:
      PORTB=A;
      break;
    case 1:
      PORTB=A+B;
      break;
    case 2:
      PORTB=B;
      break;
    case 3:
      PORTB=B+C;
      break;
    case 4:
      PORTB=C;
      break;
    case 5:
      PORTB=C+D;
      break;
    case 6:
      PORTB=D;
      break;
    case 7:
      PORTB=D+A;
      break;
    }
    break;    break;

  }
  //Update stepper pos
  if (demoState_!=IDLE){
    stepperState_+=stepperDirection_;
    if (demoState_!=HALF_STEP) stepperState_+=stepperDirection_;
    if (stepperState_>7) stepperState_=0;
    if (stepperState_<0) stepperState_=7;
  }

}

char buttonLast_ = 0;
uint8_t buttonPressTime_ = 0;
uint8_t blinkState_=0;

void buttonHandler(void *data){
  char button = hwb_is_pressed(); //debounced due to "sample speed" (100ms)
  
  //Detect button release
  if (button!=buttonLast_ && !button){ 
    if (buttonPressTime_<10) {
      //Short press (<1 sec)
      if (demoState_<HALF_STEP) demoState_++; 
      else demoState_=IDLE;
    }
    else if (buttonPressTime_<50){
      //Long press (1-5 sec)
      if (currentStepDelay_==100) currentStepDelay_=50;
      else if (currentStepDelay_==50) currentStepDelay_=2;
      else currentStepDelay_=100;

      timer1_clock_unregister_callback(&stepCallBack_);
      timer1_clock_register_callback(0, currentStepDelay_, 1, &performStep, 0, &stepCallBack_); 
    }
    else {
      //Very long press (>5 sec)
      stepperDirection_ = -stepperDirection_;
    }
  }
  
  //Register depressed time
  if (button) {
    if (buttonPressTime_<255) buttonPressTime_++;
  }
  else buttonPressTime_ = 0;
  buttonLast_ = button; 

  //Blink state update
  blinkState_++;
  if (blinkState_>10) blinkState_=0;
  switch (demoState_){
  default:
  case IDLE:
    led_red(0);
    break;
  case FULL_STEP:
    if (blinkState_==0) led_red(1);
    else led_red(0);
    break;
  case TORQUE_STEP:
    if (blinkState_==0 || blinkState_==3) led_red(1);
    else led_red(0);
    break;
  case HALF_STEP:
    if (blinkState_==0 || blinkState_==3 || blinkState_==5) led_red(1);
    else led_red(0);
    break;
  }
}


int main(void) {
  timer1_callback buttonCallBack;

  watchdog_disable();
  minimus32_init();
  clock_prescale_none();
  
  DDRB = 0b00111100; //setup PB2-PB5 as output PB0, PB1, PB6, PB7 are reserved don't use

  sei(); //global int enable

  //Setup a blink pattern indicating a working MCU
  timer1_clock_init();
  timer1_clock_register_callback(0, currentStepDelay_, 1, &performStep, 0, &stepCallBack_); 
  timer1_clock_register_callback(0, 100, 1, &buttonHandler, 0, &buttonCallBack); 
  
  while (1){
  }
  return 0;
}
