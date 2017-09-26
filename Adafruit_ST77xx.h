/***************************************************
  This is a library for the Adafruit 1.8" SPI display.

This library works with the Adafruit 1.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/358
The 1.8" TFT shield
  ----> https://www.adafruit.com/product/802
The 1.44" TFT breakout
  ----> https://www.adafruit.com/product/2088
as well as Adafruit raw 1.8" TFT display
  ----> http://www.adafruit.com/products/618

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#ifndef _ADAFRUIT_ST77XXH_
#define _ADAFRUIT_ST77XXH_

#include "Arduino.h"
#include "Print.h"
#include <Adafruit_GFX.h>

#if defined(__AVR__) || defined(CORE_TEENSY)
  #include <avr/pgmspace.h>
  #define USE_FAST_IO
  typedef volatile uint8_t RwReg;
#elif defined(ARDUINO_STM32_FEATHER)
  typedef volatile uint32 RwReg;
  #define USE_FAST_IO
#elif defined(ARDUINO_FEATHER52)
  typedef volatile uint32_t RwReg;
  #define USE_FAST_IO
#elif defined(ESP8266)
  #include <pgmspace.h>
#elif defined(__SAM3X8E__)
  #undef __FlashStringHelper::F(string_literal)
  #define F(string_literal) string_literal
  #include <include/pio.h>
  #define PROGMEM
  #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
  #define pgm_read_word(addr) (*(const unsigned short *)(addr))
  typedef unsigned char prog_uchar;
#endif


// for 1.44 and mini
#define ST7735_TFTWIDTH_128  128
// for mini
#define ST7735_TFTWIDTH_80   80
// for 1.44" display
#define ST7735_TFTHEIGHT_128 128
// for 1.8" and mini display
#define ST7735_TFTHEIGHT_160  160

#define ST_CMD_DELAY   0x80    // special signifier for command lists

#define ST77XX_NOP     0x00
#define ST77XX_SWRESET 0x01
#define ST77XX_RDDID   0x04
#define ST77XX_RDDST   0x09

#define ST77XX_SLPIN   0x10
#define ST77XX_SLPOUT  0x11
#define ST77XX_PTLON   0x12
#define ST77XX_NORON   0x13

#define ST77XX_INVOFF  0x20
#define ST77XX_INVON   0x21
#define ST77XX_DISPOFF 0x28
#define ST77XX_DISPON  0x29
#define ST77XX_CASET   0x2A
#define ST77XX_RASET   0x2B
#define ST77XX_RAMWR   0x2C
#define ST77XX_RAMRD   0x2E

#define ST77XX_PTLAR   0x30
#define ST77XX_COLMOD  0x3A
#define ST77XX_MADCTL  0x36

#define ST77XX_MADCTL_MY  0x80
#define ST77XX_MADCTL_MX  0x40
#define ST77XX_MADCTL_MV  0x20
#define ST77XX_MADCTL_ML  0x10
#define ST77XX_MADCTL_RGB 0x00

#define ST77XX_RDID1   0xDA
#define ST77XX_RDID2   0xDB
#define ST77XX_RDID3   0xDC
#define ST77XX_RDID4   0xDD


// Color definitions
#define	ST77XX_BLACK   0x0000
#define	ST77XX_BLUE    0x001F
#define	ST77XX_RED     0xF800
#define	ST77XX_GREEN   0x07E0
#define ST77XX_CYAN    0x07FF
#define ST77XX_MAGENTA 0xF81F
#define ST77XX_YELLOW  0xFFE0
#define ST77XX_WHITE   0xFFFF


class Adafruit_ST77xx : public Adafruit_GFX {

 public:

  Adafruit_ST77xx(int8_t CS, int8_t RS, int8_t SID, int8_t SCLK, int8_t RST = -1);
  Adafruit_ST77xx(int8_t CS, int8_t RS, int8_t RST = -1);

  void     setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1),
           pushColor(uint16_t color),
           fillScreen(uint16_t color),
           drawPixel(int16_t x, int16_t y, uint16_t color),
           drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color),
           drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color),
           fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
             uint16_t color),
           setRotation(uint8_t r),
           invertDisplay(boolean i);
  uint16_t Color565(uint8_t r, uint8_t g, uint8_t b);
  uint16_t color565(uint8_t r, uint8_t g, uint8_t b) { return Color565(r, g, b); } // sigh

  /* These are not for current use, 8-bit protocol only!
  uint8_t  readdata(void),
           readcommand8(uint8_t);
  uint16_t readcommand16(uint8_t);
  uint32_t readcommand32(uint8_t);
  void     dummyclock(void);
  */


 protected:
  uint8_t  _colstart, _rowstart, _xstart, _ystart; // some displays need this changed

  void     displayInit(const uint8_t *addr);
  void     spiwrite(uint8_t),
           writecommand(uint8_t c),
           writedata(uint8_t d),
           commonInit(const uint8_t *cmdList);
//uint8_t  spiread(void);


 private:

  inline void CS_HIGH(void);
  inline void CS_LOW(void);
  inline void DC_HIGH(void);
  inline void DC_LOW(void);

  boolean  _hwSPI;

  int8_t  _cs, _dc, _rst, _sid, _sclk;

#if defined(USE_FAST_IO)
  volatile RwReg  *dataport, *clkport, *csport, *dcport;

  #if defined(__AVR__) || defined(CORE_TEENSY)  // 8 bit!
    uint8_t  datapinmask, clkpinmask, cspinmask, dcpinmask;
  #else    // 32 bit!
    uint32_t  datapinmask, clkpinmask, cspinmask, dcpinmask;
  #endif
#endif

};

#endif
