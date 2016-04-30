#ifndef MOD_VT100_CODES_H
#define MOD_VT100_CODES_H

/**
 * @file
 * @brief A collection of VT100 terminal codes
 * 
 * Useful for programs that output to a terminal emulator connected to a serialport
 * @ingroup util
 */

///Clears the screen
extern const char *VT100_CLEAR_SCREEN;
extern const char *VT100_CURSOR_OFF;
///Returns the cursor to the home position
extern const char *VT100_CURSOR_HOME;

#endif
