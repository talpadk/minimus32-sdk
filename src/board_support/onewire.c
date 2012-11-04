#include "onewire.h"

#include <avr/interrupt.h>

#include "timer1_clock.h"
#include "external_interrupt.h"


uint8_t interrupt_id_;
volatile onewire_state state_;
onewire_substate substate_;

onewire_action pull_low_;
onewire_action release_;
onewire_get_bit get_bit_;

uint16_t bit_timer_;
uint8_t buffer_;
uint8_t buffer_remaining_;


//could be fine tuned, but safety first
#define LARGE_SLEEP (800*TIMER1_TICKS_PER_US)
#define SMALL_SLEEP (LARGE_SLEEP/2)
/** 
 * Returns false if we still need to sleep 
 * 
 * @return true/false
 */
uint8_t onewire_sleep_action()
{
  if (bit_timer_ == 0) return 1;

  if (bit_timer_ > LARGE_SLEEP*2){
    //large sleep
    bit_timer_ -= LARGE_SLEEP;
    timer1_sub_timer_b_delay(LARGE_SLEEP);
  }
  else if (bit_timer_ >= SMALL_SLEEP*2) {
    //sleep the remainding 1-2 large sleeps in small chuncks
    bit_timer_ -= SMALL_SLEEP;
    timer1_sub_timer_b_delay(SMALL_SLEEP);
  }
  else {
    //sleep the last bit in one go
    timer1_sub_timer_b_delay(bit_timer_);
    bit_timer_ = 0;
  }
  return 0;    
}


void onewire_small_delay(){
  asm volatile(
	       "nop\n\t"
	       "nop\n\t"
	       "nop\n\t"
	       "nop\n\t"
	       "nop\n\t"
	       "nop\n\t"
	       "nop\n\t"
	       "nop\n\t"
	       "nop\n\t"
	       "nop\n\t"
	       "nop\n\t"
	       "nop\n\t"
	       "nop\n\t"
	       "nop\n\t"
	       "nop\n\t"
	       "nop\n\t"
             ::);
}

void onewire_send_bit(){
  (*pull_low_)();
  if (buffer_&1){
    onewire_small_delay();
    (*release_)();
  }
  substate_ = onewire_pulling_low; //Slight abuse of this substate

  bit_timer_ = 80*TIMER1_TICKS_PER_US;
  onewire_sleep_action();

  buffer_ = buffer_>>1;
  buffer_remaining_--;
}

void onewire_read_bit(){
  substate_ = onewire_high_period;
  timer1_sub_timer_b_delay(70*TIMER1_TICKS_PER_US);
  (*pull_low_)();
  onewire_small_delay();
  (*release_)();
  onewire_small_delay();
  buffer_ = (buffer_>>1) | (*get_bit_)();
}


ISR(TIMER1_COMPB_vect)
{
  switch (state_){
  default: //========================== Error states
  case onewire_error_presence_pulse_missing:
    timer1_sub_timer_b_unset(); //disable ints
    break;
  case onewire_idle:
    timer1_sub_timer_b_unset();
    break;
  case onewire_sending_reset: //======================== reset pulse states ====================
    switch (substate_){
    default: //should never happen
      break;
    case onewire_pulling_low:
      if (onewire_sleep_action()){
	(*release_)();
	substate_ = onewire_waiting_for_presence_pulse;
	//init new sleep cycle, waiting for the presence pulse
	bit_timer_ = 120*TIMER1_TICKS_PER_US;
	onewire_sleep_action();
	enable_external_interrupt_input(interrupt_id_, falling);
      }
      break;
    case onewire_waiting_for_presence_pulse:
      if (onewire_sleep_action()){
	state_ = onewire_error_presence_pulse_missing;
      }
      break;
    case onewire_waiting_reset_rest:
      if (onewire_sleep_action()){
	state_ = onewire_idle;
      }
    }
    break;
  case onewire_sending_byte:  //=========================== Sending byte =====================
    switch (substate_){
    default:
    case onewire_pulling_low:
      //Bit timeings faster than 400 us check not needed
      (*release_)();
      bit_timer_ = (120-80)*TIMER1_TICKS_PER_US;
      onewire_sleep_action();
      substate_=onewire_high_period;
      break;
    case onewire_high_period:
      //Bit timeings faster than 400 us check not needed
      if (buffer_remaining_==0){
	state_ = onewire_idle;
	timer1_sub_timer_b_unset();	
      }
      else {
	onewire_send_bit();
      }
    }
    break;
  case onewire_read_byte: //=========================== Reading byte =====================
    switch (substate_){
    default:
      break;
    case onewire_pulling_low:
      //Bit timeings faster than 400 us check not needed
      buffer_ = (buffer_>>1) | (*get_bit_)();
      bit_timer_ = (70-10)*TIMER1_TICKS_PER_US;
      onewire_sleep_action();
      substate_ = onewire_high_period;
      break;
    case onewire_high_period:
      //Bit timeings faster than 400 us check not needed
      buffer_remaining_--;
      if (buffer_remaining_==0) {
	state_ = onewire_idle;
	timer1_sub_timer_b_unset();
      }
      else {
	onewire_read_bit();
      }
      break;
    }
  }
}

void onewire_reset(void){
  bit_timer_ = 500*TIMER1_TICKS_PER_US;
  state_     = onewire_sending_reset;
  substate_  = onewire_pulling_low;
  onewire_sleep_action();
  (*pull_low_)();
}

void onewire_init(uint8_t interrupt_id, onewire_action pull_low, onewire_action release, onewire_get_bit get_bit){
  interrupt_id_ = interrupt_id;
  pull_low_ = pull_low;
  release_ = release;
  get_bit_ = get_bit;

  onewire_reset();
}

 //Handles the edge interrupts
 void onewire_interrupt(void){
   switch (state_){
   default: //Should never happen
     break;
   case onewire_sending_reset:
     if (substate_ == onewire_waiting_for_presence_pulse){
       //presence detected, now rest
       bit_timer_ = 500*TIMER1_TICKS_PER_US;
       onewire_sleep_action();
       substate_ = onewire_waiting_reset_rest;
       state_ = onewire_sending_reset;
       disable_external_interrupt_input(interrupt_id_);
    }
    break;
  }
}


void onewire_send_byte(uint8_t byte){
  state_ = onewire_sending_byte;
  buffer_ = byte;
  buffer_remaining_ = 8;
  onewire_send_bit();
}

onewire_state onewire_get_state(void){
  return state_;
}

void onewire_wait_idle(void){
  while(!(state_==onewire_idle ||
	  state_==onewire_error_presence_pulse_missing)){}
}

void onewire_start_read_byte(void){
  state_ = onewire_read_byte;
  buffer_ = 0;
  buffer_remaining_ = 8;
  onewire_read_bit();
}

uint8_t onewire_get_buffer(void){
  return buffer_;
}
