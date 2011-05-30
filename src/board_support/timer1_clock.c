#include "timer1_clock.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>

timer1_wall_time timer1_clock_walltimer;

timer1_callback *timer1_callback_chain;


void timer1_clock_insert_into_chain(timer1_callback *callback_struct);

//Checks is a callback needs execution
uint8_t timer1_clock_check(timer1_callback *callback){
  if ((timer1_clock_walltimer.day  > callback->time.day)  ||

      (timer1_clock_walltimer.day == callback->time.day&&
       timer1_clock_walltimer.hour > callback->time.hour )||

      (timer1_clock_walltimer.day == callback->time.day&&
       timer1_clock_walltimer.hour== callback->time.hour&&
       timer1_clock_walltimer.min  > callback->time.min  )||

      (timer1_clock_walltimer.day == callback->time.day&&
       timer1_clock_walltimer.hour== callback->time.hour&&
       timer1_clock_walltimer.min == callback->time.min&&
       timer1_clock_walltimer.sec  > callback->time.sec)  ||

      (timer1_clock_walltimer.day == callback->time.day&&
       timer1_clock_walltimer.hour== callback->time.hour&&
       timer1_clock_walltimer.min == callback->time.min&&
       timer1_clock_walltimer.sec == callback->time.sec &&
       timer1_clock_walltimer.msec>= callback->time.msec)){

    if (callback->recurring){
      //reregister callback
      timer1_clock_register_callback(callback->time.freerunning_sec,
				     callback->recurring_msec,
				     callback->recurring,
				     callback->callback,
				     callback->user_data,
				     callback);
    }
    else {
      timer1_clock_unregister_callback(callback);
    }
    return 1;
  }  
  return 0;
}

//todo use a tmp time buffer to move the run check outside of atomic space
ISR(TIMER1_COMPA_vect)
{ 
  timer1_callback *next=0;
  ATOMIC_BLOCK(ATOMIC_FORCEON)
  {
    next = timer1_callback_chain;

    timer1_clock_walltimer.msec++;
    if (timer1_clock_walltimer.msec>=1000){
      timer1_clock_walltimer.msec=0;
      timer1_clock_walltimer.freerunning_sec++;
      timer1_clock_walltimer.sec++;
      if (timer1_clock_walltimer.sec>=60){
	timer1_clock_walltimer.sec=0;
	timer1_clock_walltimer.min++;
	if (timer1_clock_walltimer.min>=60){
	  timer1_clock_walltimer.min=0;
	  timer1_clock_walltimer.hour++;
	  if (timer1_clock_walltimer.hour>=24){
	    timer1_clock_walltimer.hour=0;
	    timer1_clock_walltimer.day++;
	  }
	}
      }
    }

    while (next!=0){
      //found one
      if (timer1_clock_check(next)){break;}
      next = next->next;
    }
  }
  //non atomic execution to avoid problems with longer that 1ms execution times
  if (next!=0){
    //we did find one
    next->callback(next->user_data);
  }
}

void timer1_clock_init(){
  timer1_callback_chain = 0;

  timer1_clock_walltimer.msec = 0;
  timer1_clock_walltimer.sec = 0;
  timer1_clock_walltimer.min = 0;
  timer1_clock_walltimer.hour = 0;
  timer1_clock_walltimer.day = 0;
  timer1_clock_walltimer.freerunning_sec = 0;

  OCR1AH = 62; //set OCR1A to 16000
  OCR1AL = 128; 
  
  TCCR1A = 0; //prescaler = 1 & count up to OCR1A function 
  TCCR1B = 1<<WGM12|1<<CS10;

  TIMSK1 = 1<<ICIE1|1<<OCIE1A; //enable timer1 int and compare a int

}

//does nothing if element already is in the chain, else insterts it
void timer1_clock_insert_into_chain(timer1_callback *callback_struct){
  uint8_t done=0;
  timer1_callback **next =  &timer1_callback_chain;

  while (!done){
    if (*next == callback_struct) done=1;
    else if (*next==0){
      *next = callback_struct;
      callback_struct->next = 0;
      done=1;
    }
    else {
      next = &((*next)->next);
    }
  }
}


void timer1_clock_unregister_callback(timer1_callback *callback_struct)
{
  timer1_callback *previous = timer1_callback_chain;
  timer1_callback *next = 0;

  //empty chain
  if (timer1_callback_chain == 0) return;

  //first element is a hit
  if (timer1_callback_chain == callback_struct){
    //and end of chain
    if (timer1_callback_chain->next == 0){
       timer1_callback_chain = 0;
    }
    else {
      timer1_callback_chain = timer1_callback_chain->next;
    }
    return;
  }
  
  //run though the remaining part of the linked list
  next = timer1_callback_chain->next;
  while (next!=0){
    //not a hit move on
    if (next!=callback_struct){
      previous = next;
      next = next->next;      
    }
    //found it
    else {
      previous->next = next->next;
      next = next->next;
      return;
    }
  }

}

void timer1_clock_register_callback(uint16_t sec, uint16_t msec, uint8_t recurring, void (*callback)(void *), void *user_data, timer1_callback *callback_struct){
  uint16_t tmp;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    timer1_clock_insert_into_chain(callback_struct);

    ///Store the delay for further usage if recurring;
    callback_struct->recurring_msec = msec;
    callback_struct->time.freerunning_sec = sec;

    callback_struct->time.sec  = 0;
    callback_struct->time.min  = 0;
    callback_struct->time.hour = 0;
    callback_struct->time.day  = 0;

    //Handle msec
    tmp = timer1_clock_walltimer.msec;
    tmp += msec;
    if (tmp>=1000){
      sec++;
      tmp -= 1000;
    }
    callback_struct->time.msec = tmp;

    //Handle sec
    tmp = timer1_clock_walltimer.sec;
    tmp += sec % 60;
    tmp += callback_struct->time.sec;
    if (tmp>=60){
      callback_struct->time.min++;
      tmp -= 60;
    }
    callback_struct->time.sec = tmp;
    sec /= 60;

    //Handle min
    tmp = timer1_clock_walltimer.min;
    tmp += sec % 60;
    tmp += callback_struct->time.min;
    if (tmp>=60){
      callback_struct->time.hour++;
      tmp -= 60;
    }
    callback_struct->time.min = tmp;
    sec /= 60;
    
    //Handle hour
    tmp = timer1_clock_walltimer.hour;
    tmp += sec; //modulo not neede max val is 18 (2^16/60/60)
    tmp += callback_struct->time.hour;
    if (tmp>=24){
      callback_struct->time.day++;
      tmp -= 24;
    }
    callback_struct->time.hour = tmp;

    //Handle day
    callback_struct->time.day += timer1_clock_walltimer.day;


    callback_struct->recurring = recurring;
    callback_struct->user_data = user_data;
    callback_struct->callback = callback;
  }
}
