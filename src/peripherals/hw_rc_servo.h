#ifndef MOD_HW_RC_SERVO_H
#define MOD_HW_RC_SERVO_H

/**
 * @file   hw_rc_servo.h
 * @author Visti Andresen <talpa@talpa.dk>
 * @ingroup per
 * 
 * @brief  A software servo that attempts to minimize the CPU usage by using TIMER1 subtimer C for variable length dalays
 * @see timer1_clock.h
 *
 * The useage of this function requires a running timer1_clock
 * 
 * 
 */


#include <stdint.h>
#include "timer1_clock.h"

typedef struct {
  uint16_t on_period;
  uint16_t remaining_period;
} hw_rc_servo;

typedef struct {
  uint8_t number_of_servos;
  uint8_t servo_index;
  uint16_t lastTick;
  uint32_t ticksRemainingIn50Hz;
  hw_rc_servo *servos;
} hw_rc_servos;

//20ms=50Hz
#define TICKS_PER_SERVO_PERIOD  (TIMER1_TICKS_PER_US*1000UL*20) 
#define TICKS_PER_BASE_PERIOD   (TIMER1_TICKS_PER_US*1000)
#define TICKS_PER_SIGNAL_PERIOD (TIMER1_TICKS_PER_US*1000)

hw_rc_servos *hw_rc_servos_;

/** 
 * Initializes the given hw_rc_servos struct centering all the servos
 * 
 * @param servos a pointer to the structure that is to be used for holding the needed servo information
 * @param servo_buffers a pointer to an array of hw_rc_servo
 * @param number_of_servos the number of servos that the structure is handling (size of servo_buffers)
 */
void hw_servo_init(hw_rc_servos *servos, hw_rc_servo *servo_buffers, uint8_t number_of_servos);

/** 
 * Sets new servo position for a given servo index
 * 
 * @param index the index number of the servo to set
 * @param position the new position, 0=minimum pos, 1024=maximum pos
 */
void hw_servo_set(uint8_t index, uint16_t position);


#endif /* MOD_HW_RC_SERVO_H */
