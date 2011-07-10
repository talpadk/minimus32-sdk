#ifndef MOD_SW_RC_SERVO_H
#define MOD_SW_RC_SERVO_H

#include <stdint.h>

typedef struct {
  uint8_t pos;
  uint8_t ticks_per_ms;
  uint8_t state;
  uint16_t counter;
} sw_rc_servo;

/** 
 * Initializes a SW RC servo
 * 
 * @param servo a pointer to the servo struct to initialize
 * @param ticks_per_ms the number of ticks per mili second
 */
void sw_rc_servo_init(sw_rc_servo *servo, uint8_t ticks_per_ms);

/** 
 * Animates the servo, must be called at a regular interval(ticks) defined at init
 * 
 * @param servo a pointer to the servo struct to animate
 * 
 * @return if 1 you must send a high signal 0 send a low.
 */
static inline uint8_t sw_rc_servo_animate(sw_rc_servo *servo) {
  switch (servo->state){
  default:
  case 0: //send 1 ms high (-1 tick)
    servo->counter++;
    if (servo->counter>=servo->ticks_per_ms){
      servo->state=1;
      servo->counter=0;
    }
    return 1;
  case 1: //extend puls according to pos+1
    if (servo->counter > servo->pos){
      servo->state=2;
      servo->counter=19*servo->ticks_per_ms - servo->pos;
    }
    servo->counter++;
    return 1;
  case 2: //off to satisfy 20ms period 
    servo->counter--;
    if (servo->counter==0){
      servo->state=0;
      servo->counter=0;
    }
    return 0;
  }
}

/** 
 * Updates the set-point for the rc servo
 * 
 * @param servo a pointer to the servo struct to update
 * @param pos the new set point, 0=min pos, number of ticks per ms=max pos
 */
void sw_rc_servo_set_pos(sw_rc_servo *servo, uint8_t pos);


#endif //MOD_SW_RC_SERVO_H
