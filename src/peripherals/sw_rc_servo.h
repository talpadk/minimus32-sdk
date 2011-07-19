#ifndef MOD_SW_RC_SERVO_H
#define MOD_SW_RC_SERVO_H

///Code for generating RC servo control signals in software

/**
 * @file
 * @author Visti Andresen
 * @ingroup per
 */

#include <stdint.h>

typedef struct {
  uint16_t pos;
  uint8_t ticks_per_ms;
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
  servo->counter--;
  if (servo->counter==0){
    servo->counter = ((uint16_t)servo->ticks_per_ms)*20;
  }
  else if (servo->counter < servo->pos){
    return 1;
  }
  return 0;
}

/** 
 * Updates the set-point for the rc servo
 * 
 * @param servo a pointer to the servo struct to update
 * @param pos the new set point, 0=min pos, number of ticks per ms=max pos
 */
void sw_rc_servo_set_pos(sw_rc_servo *servo, uint8_t pos);


#endif //MOD_SW_RC_SERVO_H
