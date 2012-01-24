//#exe

///Reads the temperatur and displays it on a LCD

/**
 * @file
 * @author Visti Andresen
 * @ingroup tests
 */




#include <avr/io.h>

#include "minimus32.h"
#include "watchdog.h"
#include "timer1_clock.h"
#include "sys_clock.h"
#include "lcd_44780.h"


uint8_t power=0;
void boosterRegulate(void *data){
  //led_blue_toggle();
  if (ACSR & 1<<ACO){
    if (power<20) power++;
  }
  else {
    power -= 1;
    if (power>200) power=0;
  }
  OCR0A=255-power;
  
}

#include "sin64.h"


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


const uint16_t band[16]={0x000, 0x222, 0x333, 0x555, 0x666, 0x777, 0x999, 0xaaa, 0xbbb, 0xccc, 0xdddd, 0xeee, 0xeee, 0xfff, 0xfff, 0xfff};

uint16_t bands[132];

void drawBand(uint8_t y, uint16_t mask){
  uint8_t i;

  for (i=0; i<16; i++){
    bands[y++] |= band[i]&mask;
  }
  for (i=15; i<16; i--){
    bands[y++] |= band[i]&mask;
  }
}

uint8_t calcSinPos(uint8_t offset){
  uint8_t result=65;
  int16_t tmp=offset;
  if (tmp>=128) { tmp = 255-tmp; }

  if (tmp>63) {
    tmp = sin64[127-tmp];
  }    
  else {
    tmp= sin64[tmp];
  }
  
  tmp = tmp*(60-16)/255;

  if (offset>=128){ tmp=-tmp; }



  return (66-16)+tmp;
}

int main(){
  uint8_t *ACMUX = (uint8_t *)0x7D; //Broken header files?

  timer1_callback boosterRegulateCallBack;
  uint8_t x,y;
  uint8_t a,b,c;
  uint16_t colour;

  uint8_t rPos=10;
  uint8_t gPos=50;
  uint8_t bPos=90;


  watchdog_disable();
  minimus32_init();
  clock_prescale_none();

  initPorts();
  lcdInit();

  

  /* ===== DC-DC booster code ===== */
  //comp setup
  ACSR  = 1<<ACBG;
  *ACMUX = 3-1; //select input

  //pwm setup (fast pwm on OC0A), no clock divide
  OCR0A = 255; //duty 255 == off
  TCCR0A = 0xC3;
  TCCR0B = 0x01;
  DDRB |= 0b10000000;
  
  //Setup regulation loop
  timer1_clock_init();
  timer1_clock_register_callback(0, 1, 1, &boosterRegulate, 0, &boosterRegulateCallBack);

  colour=0;

  while (1){
    rPos++;
    gPos-=2;
    bPos+=3;
    for(y=0; y<132; y++){
      bands[y]=0;
    }
    drawBand(calcSinPos(rPos),0xf00);
    drawBand(calcSinPos(gPos),0xf0);
    drawBand(calcSinPos(bPos),0xf);
    /*    writeByte(DISPON,1);
    for (i=0; i<300000; i++){}
    writeByte(DISPOFF,1);
    for (i=0; i<300000; i++){}*/
    writeByte(PASET, 1);
    writeByte(0,0);
    writeByte(131,0);
    writeByte(CASET, 1);
    writeByte(0,0);
    writeByte(131,0);
    writeByte(RAMWR,1);
    for (y=0; y<132; y++){
      colour = bands[y];
      //RRRRGGGGBBBB
      a=0xff & (colour>>4); //RG
      b=((colour & 0xf)<<4)|(0x0f&(colour>>8)); //BR
      c=colour & 0xff; //GB
      
      for (x=0; x<132/2; x++){
	fastData(a);
	fastData(b);
	fastData(c);
      }
      //colour++;
    }
  }
}
