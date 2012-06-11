#ifndef MOD_SERIAL_H
#define MOD_SERIAL_H

/**
 * @file   serial.h
 * @author Visti Andresen <talpa@talpa.dk>
 * 
 * @ingroup bsp
 *
 * @brife Connon defines and headers for serial ports
 * 
 */


///Defines serial port baud rates
typedef enum {
  SERIAL_SPEED_9600=0,
  SERIAL_SPEED_19200,
  SERIAL_SPEED_38400,
  SERIAL_SPEED_76800,
  SERIAL_SPEED_115200
  
  
} SerialSpeed ;

#endif //MOD_SERIAL_H
