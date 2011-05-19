// Graphics library by ladyada/adafruit 
// MIT license

#define swap(a, b) { uint16_t t = a; a = b; b = t; }

#include <WProgram.h>

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


class ST7735 {
 public:
  ST7735(uint8_t CS, uint8_t RS, uint8_t SID, 
	 uint8_t SCLK, uint8_t RST);
  ST7735(uint8_t CS, uint8_t RS, uint8_t RST);
  void initB(void);
  void initR(void);

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

  void drawString(uint8_t x, uint8_t y, char *c, 
		  uint16_t color, uint8_t size=1);
  void drawChar(uint8_t x, uint8_t y, char c, 
		      uint16_t color, uint8_t size=1);

  static const uint8_t width = 128;
  static const uint8_t height = 160;

  void setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
  void setRotation(uint8_t);
  uint8_t getRotation(void);

  void dummyclock(void);
  void writecommand(uint8_t);
  void writedata(uint8_t);
  /*
  // these are not for use, 8-bit protocol only!
  uint8_t readdata(void);
  uint8_t readcommand8(uint8_t);
  uint16_t readcommand16(uint8_t);
  uint32_t readcommand32(uint8_t);
  */

 private:
  void spiwrite(uint8_t);
  //uint8_t spiread(void);

  uint8_t _cs, _rs, _rst, _sid, _sclk;
  uint8_t csport, rsport, sidport, sclkport;
  uint8_t cspin, rspin, sidpin, sclkpin;

  uint8_t madctl;
};
