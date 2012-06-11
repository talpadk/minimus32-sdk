//#exe

///A small graphical demo for nokia LCD displays (6100 and others)

/**
 * @file
 * @author Visti Andresen
 * @ingroup tests
 */

/**
 * @file   
 * 
 * Hardware:
 *
 * LCD connection 
 *
 * Vcc = LCD.Pin1 (Vcc digital (3.3V))
 * 
 * PB4 = LCD.Pin2 (\Reset)
 * 
 * PB2 = LCD.Pin3 (Data (MOSI))
 * 
 * PB1 = LCD.Pin4 (Clock)
 * 
 * PB0 = LCD.Pin5 (Chip select)
 *
 * Vcc = LCD.Pin6 (Vcc Display (3.3V))
 * 
 * LCD.Pin7 (???)
 *
 * GND = LCD.Pin8 (Gnd)
 * 
 * LCD.Pin9 (backlight catode (gnd/minus)), you might use a booster
 *
 * LCD.Pin10 (backlight anode (+6-7V)), you might use a booster
 *
 * 
 */

#include <avr/io.h>
#include <avr/pgmspace.h> 
#include <avr/interrupt.h>

#include "minimus32.h"
#include "watchdog.h"
#include "timer1_clock.h"
#include "sys_clock.h"
#include "lcd_44780.h"
#include "bitfont_6x8.h"
#include "async_serial.h"


#define TERMINAL_BUFFER_HEIGHT (16+1)
#define TERMINAL_BUFFER_WIDTH (22)

uint8_t terminalBuffer[TERMINAL_BUFFER_HEIGHT][TERMINAL_BUFFER_WIDTH];
uint8_t terminalTail=0;
uint8_t terminalHead=0;
uint8_t terminalHeadXPos=0;

uint8_t terminalUpdateLine=0;
uint8_t terminalUpdatePos=0;


void initPorts(void){
  //PB0=chipsel, PB1=clk, PB2=data out, PB4=reset
  DDRB |= 0b00010111;
  

}

void lcdResetPin(char value){
  if (value) PORTB |= 1<<4;
  else PORTB &= ~(1<<4);
}

void lcdCS(char value){
  if (value) PORTB |= 1<<0;
  else PORTB &= ~(1<<0); 
}

void lcdClk(char value){
  if (value) PORTB |= 1<<1;
  else PORTB &= ~(1<<1); 
}

void lcdData(char value){
  if (value) PORTB |= 1<<2;
  else PORTB &= ~(1<<2); 
}


#define SLEEPOUT (0x11)
#define DISPOFF (0x28)
#define DISPON (0x29)
#define INVON (0x20)
#define COLMOD (0x3A)
#define MADCTL (0x36)
#define SETCON (0x25)
#define PASET (0x2B)
#define CASET (0x2A)
#define RAMWR (0x2C)

/*
 #define   DISON     0xAF      //   Display on
 #define   DISOFF    0xAE      //   Display off
 #define   DISNOR    0xA6      //   Normal display
 #define   DISINV    0xA7      //   Inverse display
 #define   COMSCN    0xBB      //   Common scan direction
 #define   DISCTL    0xCA      //   Display control
 #define   SLPIN     0x95      //   Sleep in
 #define   SLPOUT    0x94      //   Sleep out
 #define   PASET     0x75      //   Page address set
 #define   CASET     0x15      //   Column address set
 #define   DATCTL    0xBC      //   Data scan direction, etc.
 #define   RGBSET8   0xCE      //   256-color position set
 #define   RAMWR     0x5C      //   Writing to memory
 #define   RAMRD     0x5D      //   Reading from memory
 #define   PTLIN     0xA8      //   Partial display in
 #define   PTLOUT    0xA9      //   Partial display out
 #define   RMWIN     0xE0      //   Read and modify write
 #define   RMWOUT    0xEE      //   End
 #define   ASCSET    0xAA      //   Area scroll set
 #define   SCSTART   0xAB      //   Scroll start set
 #define   OSCON     0xD1      //   Internal oscillation on
 #define   OSCOFF    0xD2      //   Internal oscillation off
 #define   PWRCTR    0x20      //   Power control
 #define   VOLCTR    0x81      //   Electronic volume control
 #define   VOLUP     0xD6      //   Increment electronic control by 1
 #define   VOLDOWN   0xD7      //   Decrement electronic control by 1
 #define   TMPGRD    0x82      //   Temperature gradient set
 #define   EPCTIN    0xCD      //   Control EEPROM
 #define   EPCOUT    0xCC      //   Cancel EEPROM control
 #define   EPMWR     0xFC      //   Write into EEPROM
 #define   EPMRD     0xFD      //   Read from EEPROM
 #define   EPSRRD1   0x7C      //   Read register 1
 #define   EPSRRD2   0x7D      //   Read register 2
 #define   NOP       0x25      //   NOP instruction
*/

void writeByte(uint8_t byte, char isCommand){
  uint8_t i,j;


  if (isCommand) PORTB &= ~(1<<2);
  else PORTB |= 1<<2;
  PORTB |= 1<<1;
  PORTB &= ~(1<<1);

  i=128;
  j=8;
  while (j){
    j--;
    if (byte & i) PORTB |= 1<<2;
    else PORTB &= ~(1<<2);
    i=i>>1;
    PORTB |= 1<<1;
    PORTB &= ~(1<<1);
    
  }
}

void fastData(uint8_t data){
  PORTB |= 1<<2;
  PORTB |= 1<<1;
  PORTB &= ~(1<<1);

  //Master, MSB first, clock low=idle, sample on rising edge, divide the clock by 4
  SPCR = 0b01010000;

  SPDR = data;

  //Wait for transfer done
  while (!(SPSR&1<<SPIF)){}
  //SPI off
  SPCR = 0;
}


void lcdInit(void){
  uint16_t i;
  lcdClk(0);
  lcdCS(0);
  lcdResetPin(0);
  for (i=0; i<30000; i++){}
  lcdResetPin(1);
  for (i=0; i<30000; i++){}
  
  writeByte(SLEEPOUT, 1);

   writeByte(INVON,1);
  writeByte(COLMOD,1);
  //writeByte(0x02,0); //8-BIT
  writeByte(0x03,0); //12-BIT

  writeByte(MADCTL,1);
  //  writeByte(0xC8,0);
  writeByte(0x10,0);

  writeByte(SETCON,1);
  //writeByte(0x30,0);
  writeByte(56,0);

  writeByte(DISPON,1);
}



void drawChar(uint8_t x, uint8_t y, uint8_t character, const bitfont *font, uint16_t fg, uint16_t bg){
  const uint8_t *bits = font->bits+((uint16_t)font->char_size)*((uint16_t)(character-font->start));
  uint8_t currentByte;
  uint8_t bitCount = font->width*font->height;
  uint8_t bytePos;
  uint16_t colourA, colourB;

  //Set up region
  writeByte(CASET, 1);
  fastData(x);
  fastData(x+font->width-1);
  writeByte(PASET, 1);
  fastData(y);
  fastData(y+font->height-1);

  //Send data 
  writeByte(RAMWR,1);

  bytePos=8;
  currentByte = pgm_read_byte(bits++);
  //  currentByte = 0b01010101;
  while (bitCount!=0){
    bitCount-=2;
    if (currentByte & 128) colourA = fg;
    else colourA = bg;
    if (currentByte & 64) colourB = fg;
    else colourB = bg;
    bytePos-=2;
    if (bytePos==0){
      bytePos=8;
      currentByte = pgm_read_byte(bits++);
      //      currentByte = 0b01010101;
    }
    else {
      currentByte = currentByte << 2;
    }
    /*    //RRRRGGGGBBBB
      a=0xff & (colour>>4); //RG
      b=((colour & 0xf)<<4)|(0x0f&(colour>>8)); //BR
      c=colour & 0xff; //GB*/
    //mono colors could be done using a look up table
    fastData(0xff & (colourA>>4));
    fastData(((colourA & 0xf)<<4)|(0x0f&(colourB>>8)));
    fastData(colourB & 0xff);
    /*    fastData(0);
    fastData(0);
    fastData(0);*/
  }
}

void drawString(uint8_t x, uint8_t y, const char *string, const bitfont *font, uint16_t fg, uint16_t bg){
  uint8_t character=*(string++);
  while (character!=0){
    drawChar(x,y, character, font, fg, bg);
    x+=font->width;
    character=*(string++);
  }
}


void clearScreen(){
  uint8_t x,y;
  writeByte(PASET, 1);
  writeByte(0,0);
  writeByte(131,0);
  writeByte(CASET, 1);
  writeByte(0,0);
  writeByte(131,0);
  writeByte(RAMWR,1);
  for (y=0; y<132; y++){
    for (x=0; x<132/2; x++){
      fastData(0);
      fastData(0);
      fastData(0);
    }
  }
}

void updateLine(uint8_t *lineData, uint8_t lineNumber){
  uint8_t buf=*lineData;
  uint8_t x=0;
  while (x<TERMINAL_BUFFER_WIDTH && buf!='\n'){
     drawChar(x*6, lineNumber*8, buf, &bitfont_6x8, 0x0fff, 0x0000);
     x++;
     buf = *(++lineData);
  }
  while (x<TERMINAL_BUFFER_WIDTH){
     drawChar(x*6, lineNumber*8, ' ', &bitfont_6x8, 0x0fff, 0x0000);
     x++;
  }
}

int main(){
  uint8_t y;

  watchdog_disable();
  minimus32_init();
  clock_prescale_none();

  initPorts();
  lcdInit();

  sei(); //ints on
  async_serial_init(SERIAL_SPEED_115200);
  async_serial_rx_interrupt(1);
  clearScreen();
  for (y=0 ; y<TERMINAL_BUFFER_HEIGHT; y++){
    terminalBuffer[y][0]='\n';
  }

  while (1){
    for (y = 0; y<16; y++){
      updateLine(terminalBuffer[(terminalTail+y)%TERMINAL_BUFFER_HEIGHT], y);
      led_red_toggle();
    }
  }
}

ISR(USART1_RX_vect){
  uint8_t buf;
  buf = async_serial_read_byte();
  if (terminalHeadXPos<TERMINAL_BUFFER_WIDTH-1){
     terminalBuffer[terminalHead][terminalHeadXPos+1]='\n';
  }
  if (terminalHeadXPos<TERMINAL_BUFFER_WIDTH){
    terminalBuffer[terminalHead][terminalHeadXPos]=buf;
    terminalHeadXPos++;
  }
  
  if (buf=='\n'){
    terminalHead++;
    terminalHeadXPos=0;
    if (terminalHead>=TERMINAL_BUFFER_HEIGHT) terminalHead = 0;
    if (terminalHead==terminalTail){
      terminalTail++;
      if (terminalTail>=TERMINAL_BUFFER_HEIGHT){
	terminalTail=0;
      }
    }
  }

}
