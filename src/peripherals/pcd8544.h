#ifndef MOD_PCD8544_H
#define MOD_PCD8544_H

#include <stdint.h>


#define PCD8544_CMD_FUNCTION_SET            (0x20)
#define PCD8544_CMD_BIT_EXTENDED   (0x01)
#define PCD8544_CMD_BIT_VERTICAL   (0x02)
#define PCD8544_CMD_BIT_POWER_DOWN (0x04)

#define PCD8544_CMD_DISPLAY_CONTROL_BLANK   (0x08)
#define PCD8544_CMD_DISPLAY_CONTROL_NORMAL  (0x0C)
#define PCD8544_CMD_DISPLAY_CONTROL_ALL_ON  (0x09)
#define PCD8544_CMD_DISPLAY_CONTROL_INVERSE (0x0D)
#define PCD8544_CMD_SET_Y                   (0x40)
#define PCD8544_CMD_SET_X                   (0x80)

//Extended commands
#define PCD8544_EXT_TEMP_C                  (0x04)
#define PCD8544_EXT_BIAS                    (0x10)
#define PCD8544_EXT_VOP                     (0x80)

typedef struct {
  void (*assert_reset)(void);
  void (*desert_reset)(void);
  void (*assert_chip_delect)(void);
  void (*desert_chip_select)(void);
  void (*assert_command)(void);
  void (*desert_command)(void);
} pcd8544_io;

/** 
 * Initialization of the display, must be called as soon as possible. 
 * 
 * @note This function must be called within 100ms of the supply voltage going high.
 * 
 * @param io a pointer to the io functions should be used for 
 * @param contrast setting range 0-127, suggestion could be 96
 */
void pcd8544_init(pcd8544_io *io, uint8_t contrast);

/** 
 * Displays a test pattern on the display
 * 
 * @param pattern the numer of the test range 0-7
 */
void pcd8544_test(uint8_t pattern);

#endif //MOD_PCD8544_H
