#ifndef HW_PC_FAN_H
#define HW_PC_FAN_H

#define USES_RESOURCE_TIMER0 1


#include <stdint.h>

#define HW_PC_FAN_MAX_POWER (79)
#define HW_PC_FAN_MIN_SAFE_POWER (HW_PC_FAN_MAX_POWER/3)

void hw_pc_fan_init(uint8_t power);
void hw_pc_fan_set(uint8_t power);
void hw_pc_fan_free(void);

#endif //HW_PC_FAN_H
