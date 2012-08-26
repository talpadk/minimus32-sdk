#include "pcd8544.h"

#include "spi.h"

pcd8544_io pcd8544_io_copy;

//The code in this file must leave the command line in data mode when idle (helps speed)
//Also the chip sel must be left unselected and the SPI bus unobtained


void pcd8544_init(pcd8544_io *io, uint8_t contrast){
  //Each clock at 16Mhz is 62.5 ns
  io->assert_reset();
  pcd8544_io_copy.assert_reset       = io->assert_reset;
  pcd8544_io_copy.desert_reset       = io->desert_reset;
  pcd8544_io_copy.assert_chip_delect = io->assert_chip_delect;
  pcd8544_io_copy.desert_chip_select = io->desert_chip_select;
  pcd8544_io_copy.desert_reset();
  pcd8544_io_copy.assert_command     = io->assert_command;
  pcd8544_io_copy.desert_command     = io->desert_command;

  pcd8544_io_copy.assert_command();
  pcd8544_io_copy.assert_chip_delect();
  
  spi_obtain_bus(1);
  spi_setup(SPI_DIVIDER_4, 0); //4 Mhz @ 16Mhz

  spi_io(PCD8544_CMD_FUNCTION_SET|PCD8544_CMD_BIT_EXTENDED);
  spi_io(PCD8544_EXT_VOP|contrast);
  spi_io(PCD8544_EXT_TEMP_C|0);
  spi_io(PCD8544_EXT_BIAS|4); //1:48
  spi_io(PCD8544_CMD_FUNCTION_SET|PCD8544_CMD_BIT_VERTICAL);
  spi_io(PCD8544_CMD_DISPLAY_CONTROL_NORMAL);

  pcd8544_io_copy.desert_command();
  pcd8544_io_copy.desert_chip_select();

  spi_release_bus();
}

void pcd8544_test(uint8_t pattern) {
  uint8_t i;
  pattern = 1<<pattern;

  pcd8544_io_copy.assert_command();
  pcd8544_io_copy.assert_chip_delect();
  
  spi_obtain_bus(1);
  spi_setup(SPI_DIVIDER_4, 0); //4 Mhz @ 16Mhz
  spi_io(PCD8544_CMD_SET_Y);
  spi_io(PCD8544_CMD_SET_X);
  pcd8544_io_copy.desert_command();

  for (i=0; i<250; i++) spi_io(pattern);
  for (i=0; i<254; i++) spi_io(pattern);

  pcd8544_io_copy.desert_chip_select();

  spi_release_bus();

}
