//#exe


/**
 * @file
 * @author Visti Andresen
 * @ingroup tests
 * @ingroup tests_motor
 *
 * @brief A test/demo program that runs a stepmotor using L298N based hardware.
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
 * "Keyes L298" motorcontroller (L298N based, powered by a 7.4V LiPo battery)
 * Mercury Motor SM-42BYG011-25 (12V bipolar step motor)
 * 
 * Links: 
 * ====== 
 * http://dx.com/p/l298n-stepper-motor-driver-controller-board-for-arduino-120542?item=4
 * http://www.geekonfire.com/index.php?main_page=product_info&cPath=50&products_id=48
 * http://www.geekonfire.com/wiki/index.php?title=Dual_H-Bridge_Motor_Driver
 * http://www.let-elektronik.dk/stepper-motor-with-cable.html
 * https://www.sparkfun.com/products/9238
 *
 * Hardware: 
 * ========= 
 * Motor red wire   (A) -> L298N MotorA.1 (Output 2) 
 * Motor green wire (C) -> L298N MotorA.2 (Output 1) 
 * Motor yello wire (B) -> L298N MotorB.1 (Output 4) 
 * Motor blue wire  (D) -> L298N MotorB.2 (Output 3) 
 *
 * L298N.ENA -> PB2 
 * L298N.IN1 -> PB3 
 * L298N.IN2 -> PB4 
 * L298N.IN3 -> PB5 
 * L298N.IN4 -> PB6 
 * L298N.INB -> PB7 
 * L298N.GND -> GND 
 * L298N.+5V -> Vcc (optional and ONLY if the Minimus has been modyfied to be powered by the Vcc pin)
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

#define A_FLOAT (0)
#define A_POS (0b01100)
#define A_NEG (0b10100)
#define B_FLOAT (0)
#define B_POS (0b10100000)
#define B_NEG (0b11000000)

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
    PORTB = A_FLOAT|B_FLOAT;
    break;
  case FULL_STEP:
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
  
  DDRB = 0b11111100; //setup PB2-PB7 as output PB0 and PB1 are reserved don't use

  sei(); //global int enable

  //Setup a blink pattern indicating a working MCU
  timer1_clock_init();
  timer1_clock_register_callback(0, currentStepDelay_, 1, &performStep, 0, &stepCallBack_); 
  timer1_clock_register_callback(0, 100, 1, &buttonHandler, 0, &buttonCallBack); 
  
  while (1){
  }
  return 0;
}
