#ifndef MOD_TIMER1_CLOCK_H
#define MOD_TIMER1_CLOCK_H

#include <stdint.h>

/** 
 * @file
 * timer1_clock takes ownership/uses the timer1 module of the MCU
 * 
 * timer1_clock provides a mili second resolution clock with callback functionality at given intervals and wall times.
 */


/** 
 * A struct that keeps track of time since timer1_clock_init
 * 
 */
typedef struct {
  ///Mili seconds since init, range 0-999.
  uint16_t msec; 
  ///Seconds since init, range 0-59.
  uint8_t sec;
  ///Minutes since init, range 0-59.
  uint8_t min;
  ///Hours since init, range 0-23.
  uint8_t hour;
  ///Days since init, range 0-65535. Note: overflows after roughly 179.6 years
  uint16_t day; 
  ///A free running counter of seconds since init, range 0-65535. Note: overflows after roughly 18.2 hours
  uint16_t freerunning_sec; 
} timer1_wall_time;

///Struct used for holding callback information
typedef struct {
  ///The next callback in the linked list
  timer1_callback *next;
  ///An abused wall time struct, used for determining "next" call time
  timer1_wall_time time;

  ///If true the alarm is recurring, only valid for period alarms
  uint8_t recurring;

  ///Data that will be passed on to the call back function
  void *user_data;
  
  void (*callback)(void *);

} timer1_callback;

/** 
 * Initializes the timekeeper and sets up the clock dividers.
 * This function assumes that Tsys=16MHz
 * @see clock_prescale_none
 * 
 */
void timer1_clock_init(void);

/** 
 * Registers a callback into the clock
 * 
 * @param sec number of seconds that should pass before calling the callback
 * @param msec aditional numer of mili seconds, range 0-999 is valid
 * @param recurring if true callback will be registered when called, otherwise it will be removed
 * @param callback a pointer to the funtion that will be called
 * @param user_data a pointer to user data that will be passed to the function
 * @param callback_struct a pointer to the struct that will be initialized and linked into the clock
 */
void timer1_clock_register_callback(uint16_t sec, uint16_t msec, uint8_t recurring, void (*callback)(void *), void *user_data, timer1_callback *callback_struct)

#endif //MOD_TIMER1_CLOCK_H
