#ifndef MOD_SPI_H
#define MOD_SPI_H

#include <stdint.h>
#include <avr/io.h>

/**
 * @file   spi.h
 * @author Visti Andresen <talpa@Hermes.talpa.dk>
 * @date   Sat Aug 25 16:13:49 2012
 * 
 * @brief  Code for using the SPI hardware
 * @ingroup bsp
 * 
 * 
 */

/// A type def for defining the divider that degerates the SPI clock
typedef enum {
  SPI_DIVIDER_4=0,
  SPI_DIVIDER_16,
  SPI_DIVIDER_64,
  SPI_DIVIDER_128,
  SPI_DIVIDER_2,
  SPI_DIVIDER_8,
  SPI_DIVIDER_32,
  SPI_DIVIDER_64_X2} SpiSpeed;

///Send the data with the most significan bit first (default)
#define SPI_FLAG_MSB_FIRST (0);

///Sendt the data with the least significan bit first
#define SPI_FLAG_LSB_FIRST (1<<DORD);

///Be the bus master (default)
/**
 * When master mode consideration should be give to the useage of the SS pin (slave select)
 * If the SS pin is configured as an input and the SS pin is pulled low the SPI hardware
 * will exit master mode and become a bus slave.
 * 
 * Recomandation: use the SS pin as a chip select (OUTPUT).
 */
 //inverse of data sheet bit
#define SPI_FLAG_BUS_MASTER (0)

///The hardware is to be a bus slave
//inverse of data sheet bit
#define SPI_FLAG_BUS_SLAVE (1<<MSTR)

///Enable the SPI hardware (default)
//inverse of data sheet bit
#define SPI_FLAG_ENABLE_SPI (0)

///Disable the SPI hardware
//inverse of data sheet bit
#define SPI_FLAG_DISABLE_SPI (1<<SPE)

///Don't use interrupts (default)
#define SPI_FLAG_NO_INTERRUPT (0)

///Enable the transfer compleate interrupt
/**
 * You must enable the global interrupts.
 * Remeber to implement the SPI_STC_vect interrupt vector in your code 
 */
#define SPI_FLAG_INTERRUPT (1<<SPIE)

///Sample on the rising clock edge (default)
#define SPI_FLAG_CLOCK_RISING             ((0<<CPOL)|(0<<CPHA))

///Sample on the rising clock edge with a phase delay
#define SPI_FLAG_CLOCK_RISING_PHASE_DELAY ((1<<CPOL)|(1<<CPHA))

///Sample on the faling clock edge
#define SPI_FLAG_CLOCK_FALING             ((1<<CPOL)|(0<<CPHA))

///Sample on the faling clock edge with a phase delay
#define SPI_FLAG_CLOCK_FALING_PHASE_DELAY ((0<<CPOL)|(1<<CPHA))


/** 
 * Obtains the right to use the SPI bus, the bus should be obtained prior using it on order
 * to be able to share the bus among multible devices.
 * 
 * @param blocking if true the call will not return before the bus could be obtained
 * 
 * @return returns true (1) if the bus has been obtained
 */
uint8_t spi_obtain_bus(uint8_t blocking);

/** 
 * Releases the bus after haveing been obtained
 * @see spi_obtain_bus
 * 
 */
void spi_release_bus(void);

/** 
 * Configures the SPI hardware for a given useage.
 *
 * The function dosn't configure the IO pins.
 *
 * The setup clears any waiting data and write collisions from the HW
 *
 * @note Biefly the SPI will be disabled (while changing the clock speed) regardless of enable flag.
 * You should configure the IO pins prior to calling this
 * 
 * @param speed specifies the clock speed by how much the Fosc (system clock) should be divided
 * @param flags Flags that modify how the SPI hardware is to behave
 *
 * @see spi_config_io_for_master_mode
 */
void spi_setup(SpiSpeed speed, uint8_t flags);

/** 
 * Sends and recieves a byte on the SPI bus.
 *
 * This function should only be called on a idle bus and it leaves bus idle when done.
 *
 * @note This is a blocking IO opperation.
 * 
 * @param txData the data to send to the SPI slave
 * 
 * @return the data from the SPI slave
 */
uint8_t spi_io(uint8_t txData);

/** 
 * Configures the IO pins for SPI as a bus master.
 *
 * SS (output)
 * MOSI (output)
 * MISO (input)
 * SCLK (output)
 *
 * @note This is just one possible configuration for master mode, there are other valid configurations
 */
void spi_config_io_for_master_mode(void);


#endif //MOD_SPI_H
