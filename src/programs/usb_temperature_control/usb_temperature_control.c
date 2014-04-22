//#exe
#include "minimus32.h"
#include "watchdog.h"
#include "sys_clock.h"

#include "hw_pc_fan.h"

int main(void){
  watchdog_disable();
  minimus32_init();
  clock_prescale_none();

  hw_pc_fan_init(HW_PC_FAN_MIN_SAFE_POWER);

  while(1){
  }
  return 0;
}
