#include "Adafruit_ST77xx.h"


// some flags for initR() :(
#define INITR_GREENTAB   0x0
#define INITR_REDTAB     0x1
#define INITR_BLACKTAB   0x2

#define INITR_18GREENTAB    INITR_GREENTAB
#define INITR_18REDTAB      INITR_REDTAB
#define INITR_18BLACKTAB    INITR_BLACKTAB
#define INITR_144GREENTAB   0x1
#define INITR_MINI160x80    0x4

#define ST7735_MADCTL_BGR 0x08
#define ST7735_MADCTL_MH  0x04

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1



// Color definitions
#define	ST7735_BLACK   0x0000
#define	ST7735_BLUE    0x001F
#define	ST7735_RED     0xF800
#define	ST7735_GREEN   0x07E0
#define ST7735_CYAN    0x07FF
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW  0xFFE0
#define ST7735_WHITE   0xFFFF


/// Subclass of ST77XX type display for ST7735B and ST7735R TFT Drivers
class Adafruit_ST7735 : public Adafruit_ST77xx {

 public:

/**************************************************************************/
/*!
    @brief  Instantiate Adafruit ST7735 driver with software SPI
    @param    cs    Chip select pin #
    @param    dc    Data/Command pin #
    @param    mosi  SPI MOSI pin #
    @param    sclk  SPI Clock pin #
    @param    rst   Reset pin # (optional, pass -1 if unused)
*/
/**************************************************************************/
  Adafruit_ST7735(int8_t cs, int8_t dc, int8_t mosi, int8_t sclk, int8_t rst) : 
  Adafruit_ST77xx(cs, dc, mosi, sclk, rst) {}
  
/**************************************************************************/
/*!
    @brief  Instantiate Adafruit ST7735 driver with hardware SPI
    @param    cs    Chip select pin #
    @param    dc    Data/Command pin #
    @param    rst   Reset pin # (optional, pass -1 if unused)
*/
/**************************************************************************/
 Adafruit_ST7735(int8_t cs, int8_t dc, int8_t rst) : 
  Adafruit_ST77xx(cs, dc, rst) {}

/**************************************************************************/
/*!
    @brief  Instantiate Adafruit ST7735 driver with user selected hardware SPI
    @param    cs    Chip select pin #
    @param    dc    Data/Command pin #
    @param    spiClass  A pointer to the SPIClass to use for HW SPI
    @param    rst   Reset pin # (optional, pass -1 if unused)
*/
/**************************************************************************/
 Adafruit_ST7735(int8_t cs, int8_t dc, SPIClass *spiClass, int8_t rst) :
  Adafruit_ST77xx(cs, dc, spiClass, rst) {}

  // the tab types are so weird we need to do this 'by hand'
  void  setRotation(uint8_t m);

  void  initB(void);                             // for ST7735B displays
  void  initR(uint8_t options = INITR_GREENTAB); // for ST7735R

 private:
  uint8_t  tabcolor;
};
