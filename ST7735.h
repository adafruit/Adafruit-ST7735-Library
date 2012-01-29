/*************************************************** 
  This is a library for the Adafruit 1.8" SPI display.
  This library works with the Adafruit 1.8" TFT Breakout w/SD card  
  ----> http://www.adafruit.com/products/358  
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

#define swap(a, b) { uint16_t t = a; a = b; b = t; }

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// some flags for initR() :(
#define INITR_GREENTAB 0x0
#define INITR_REDTAB 0x1


#define ST7735_NOP 0x0
#define ST7735_SWRESET 0x01
#define ST7735_RDDID 0x04
#define ST7735_RDDST 0x09

#define ST7735_SLPIN  0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON  0x12
#define ST7735_NORON  0x13

#define ST7735_INVOFF 0x20
#define ST7735_INVON 0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON 0x29
#define ST7735_CASET 0x2A
#define ST7735_RASET 0x2B
#define ST7735_RAMWR 0x2C
#define ST7735_RAMRD 0x2E

#define ST7735_PTLAR 0x30
#define ST7735_COLMOD 0x3A
#define ST7735_MADCTL 0x36


#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR 0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1 0xC0
#define ST7735_PWCTR2 0xC1
#define ST7735_PWCTR3 0xC2
#define ST7735_PWCTR4 0xC3
#define ST7735_PWCTR5 0xC4
#define ST7735_VMCTR1 0xC5

#define ST7735_RDID1 0xDA
#define ST7735_RDID2 0xDB
#define ST7735_RDID3 0xDC
#define ST7735_RDID4 0xDD

#define ST7735_PWCTR6 0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1


class Adafruit_ST7735 : public Print {
 public:
  Adafruit_ST7735(uint8_t CS, uint8_t RS, uint8_t SID, 
         uint8_t SCLK, uint8_t RST);
  Adafruit_ST7735(uint8_t CS, uint8_t RS, uint8_t RST);
  void initB(void); // for ST7735B displays
  void initR(uint8_t options = INITR_GREENTAB);  // for ST7735R

  uint16_t Color565(uint8_t r, uint8_t g, uint8_t b);

  // drawing primitives!
  void pushColor(uint16_t color);
  void drawPixel(uint8_t x, uint8_t y, uint16_t color);
  void drawLine(int16_t x, int16_t y, int16_t x1, int16_t y1, uint16_t color);
  void fillScreen(uint16_t color);
  void drawVerticalLine(uint8_t x0, uint8_t y0, 
			uint8_t length, uint16_t color);
  void drawHorizontalLine(uint8_t x0, uint8_t y0, 
			  uint8_t length, uint16_t color);
  void drawFastLine(uint8_t x0, uint8_t y0, uint8_t l, 
		    uint16_t color, uint8_t flag);
  void drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, 
		uint16_t color);
  void fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, 
		uint16_t color);
  void drawCircle(uint8_t x0, uint8_t y0, uint8_t r, 
		  uint16_t color);
  void fillCircle(uint8_t x0, uint8_t y0, uint8_t r, 
		  uint16_t color);
  void drawTriangle(uint8_t x0, uint8_t y0,
          uint8_t x1, uint8_t y1,
          uint8_t x2, uint8_t y2, uint16_t color);
  void fillTriangle(int32_t x0, int32_t y0,
          int32_t x1, int32_t y1,
          int32_t x2, int32_t y2, 
          uint16_t color);
  void drawRoundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t radius,uint16_t color);
  void fillRoundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t radius, uint16_t color);
    
    void setCursor(uint16_t x, uint16_t y);
    void setTextColor(uint16_t c);
    void setTextSize(uint8_t s);
    void goHome(void);
#if ARDUINO >= 100
    virtual size_t write(uint8_t);
#else
    virtual void write(uint8_t);
#endif
    
  void drawString(uint8_t x, uint8_t y, char *c, 
		  uint16_t color, uint8_t size=1);
  void drawChar(uint8_t x, uint8_t y, char c, 
		      uint16_t color, uint8_t size=1);

  static const uint8_t TFTWIDTH = 128;
  static const uint8_t TFTHEIGHT = 160;

  uint8_t width();
  uint8_t height();

  void setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
  void setRotation(uint8_t);
  uint8_t getRotation(void);

  void dummyclock(void);
  void writecommand(uint8_t c);
  void writedata(uint8_t d);
  /*
  // these are not for use, 8-bit protocol only!
  uint8_t readdata(void);
  uint8_t readcommand8(uint8_t);
  uint16_t readcommand16(uint8_t);
  uint32_t readcommand32(uint8_t);
  */

 private:
    void drawCircleHelper(uint16_t x0, uint16_t y0, uint16_t r, uint8_t corner, uint16_t color);
    void fillCircleHelper(uint16_t x0, uint16_t y0, uint16_t r, uint8_t corner, uint16_t delta, uint16_t color);
  void spiwrite(uint8_t);
  //uint8_t spiread(void);

  uint8_t _cs, _rs, _rst, _sid, _sclk;
  uint8_t csport, rsport, sidport, sclkport;
  uint8_t cspin, rspin, sidpin, sclkpin;

  uint8_t madctl;
  uint16_t _width, _height;
  uint8_t rotation;
  uint8_t textsize;
  uint16_t cursor_x, cursor_y;
  uint16_t textcolor;

  uint8_t colstart, rowstart; // some displays need this changed
};
