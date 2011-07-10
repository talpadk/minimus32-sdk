#include "sw_rc_servo.h"

void sw_rc_servo_init(sw_rc_servo *servo, uint8_t ticks_per_ms){
  servo->pos = (ticks_per_ms/2)+ticks_per_ms;
  servo->ticks_per_ms = ticks_per_ms;
  servo->counter = 20*ticks_per_ms;
}

void sw_rc_servo_set_pos(sw_rc_servo *servo, uint8_t pos){
  servo->pos = ((uint16_t)pos)+servo->ticks_per_ms;
}
