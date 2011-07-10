#include "sw_rc_servo.h"

void sw_rc_servo_init(sw_rc_servo *servo, uint8_t ticks_per_ms){
  servo->pos = ticks_per_ms/2;
  servo->ticks_per_ms = ticks_per_ms;
  servo->state = 0;
  servo->counter = 0;
}

void sw_rc_servo_set_pos(sw_rc_servo *servo, uint8_t pos){
  servo->pos = pos;
}
