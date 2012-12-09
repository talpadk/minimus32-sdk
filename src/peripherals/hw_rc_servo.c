#include "hw_rc_servo.h"

#include <avr/io.h>
#include <avr/interrupt.h>

void hw_servo_init(hw_rc_servos *servos, hw_rc_servo *servo_buffers, uint8_t number_of_servos){
  uint8_t i;
  hw_rc_servos_ = servos;
  servos->servos = servo_buffers;
  servos->number_of_servos = number_of_servos;
  servos->servo_index = 0;
  servos->lastTick = 0;
  servos->ticksRemainingIn50Hz = TICKS_PER_SERVO_PERIOD;
  //Set all servos to center position
  for (i=0; i!=number_of_servos; i++){
    servos->servos[i].on_period = TICKS_PER_BASE_PERIOD + TICKS_PER_SIGNAL_PERIOD/2;
    servos->servos[i].remaining_period = TICKS_PER_BASE_PERIOD + TICKS_PER_SIGNAL_PERIOD/2;
  }
  OCR1C = 0;  //next irq on 0 ticks
  TIMSK1 |= 1<<OCIE1C;  //enable irq 
}

void hw_servo_set(uint8_t index, uint16_t position){
  uint32_t fraction = (TICKS_PER_SIGNAL_PERIOD*(uint32_t)position)/1024UL;
  if (index >= hw_rc_servos_->number_of_servos) { return; }
  hw_rc_servos_->servos[index].on_period = ((uint16_t)fraction)+TICKS_PER_BASE_PERIOD;
}


ISR(TIMER1_COMPC_vect)
{
  uint16_t ticks;
  uint8_t index = hw_rc_servos_->servo_index;
  if (index==hw_rc_servos_->number_of_servos){
    //waiting for remainng time to pass 
    if (hw_rc_servos_->ticksRemainingIn50Hz>=(TIMER1_TICKS_PER_US*1000*2)){
      //delay one full cycle
      ticks = (TIMER1_TICKS_PER_US*1000);
    }
    else if (hw_rc_servos_->ticksRemainingIn50Hz>(TIMER1_TICKS_PER_US*1000)){
      //Not two full periods or more, but more than one. Delay half of the ramaining
      ticks = hw_rc_servos_->ticksRemainingIn50Hz >> 1;
    }
    else {
      ticks = hw_rc_servos_->ticksRemainingIn50Hz;
    }
  }
  else {
    //Handling servo delays
    ticks = hw_rc_servos_->servos[index].remaining_period;

    if (ticks>=(TIMER1_TICKS_PER_US*1000*2)){
      //delay one full cycle
      ticks = (TIMER1_TICKS_PER_US*1000);
    }
    else if (ticks>(TIMER1_TICKS_PER_US*1000)){
      ticks = hw_rc_servos_->ticksRemainingIn50Hz >> 1;
    }
    hw_rc_servos_->servos[index].remaining_period -= ticks;
  }
  
  hw_rc_servos_->ticksRemainingIn50Hz -= ticks;
  
}
