#include "Adafruit_ST7789.h"
#include "Adafruit_ST77xx.h"

// CONSTRUCTORS ************************************************************

/*!
    @brief  Instantiate Adafruit ST7789 driver with software SPI
    @param  cs    Chip select pin #
    @param  dc    Data/Command pin #
    @param  mosi  SPI MOSI pin #
    @param  sclk  SPI Clock pin #
    @param  rst   Reset pin # (optional, pass -1 if unused)
*/
Adafruit_ST7789::Adafruit_ST7789(int8_t cs, int8_t dc, int8_t mosi, int8_t sclk,
                                 int8_t rst)
    : Adafruit_ST77xx(320, 240, cs, dc, mosi, sclk, rst) {}

/*!
    @brief  Instantiate Adafruit ST7789 driver with hardware SPI
    @param  cs   Chip select pin #
    @param  dc   Data/Command pin #
    @param  rst  Reset pin # (optional, pass -1 if unused)
*/
Adafruit_ST7789::Adafruit_ST7789(int8_t cs, int8_t dc, int8_t rst)
    : Adafruit_ST77xx(320, 240, cs, dc, rst) {}

/*!
    @brief  Instantiate Adafruit ST7789 driver with hardware SPI
    @param  w    LCD total width
    @param  h    LCD total height
    @param  cs   Chip select pin #
    @param  dc   Data/Command pin #
    @param  rst  Reset pin # (optional, pass -1 if unused)
*/
Adafruit_ST7789::Adafruit_ST7789(uint8_t w, uint8_t h, int8_t cs, int8_t dc,
                                 int8_t rst)
    : Adafruit_ST77xx(w, h, cs, dc, rst) {}

#if !defined(ESP8266)
/*!
    @brief  Instantiate Adafruit ST7789 driver with selectable hardware SPI
    @param  spiClass  Pointer to an SPI device to use (e.g. &SPI1)
    @param  cs        Chip select pin #
    @param  dc        Data/Command pin #
    @param  rst       Reset pin # (optional, pass -1 if unused)
*/
Adafruit_ST7789::Adafruit_ST7789(SPIClass *spiClass, int8_t cs, int8_t dc,
                                 int8_t rst)
    : Adafruit_ST77xx(320, 240, spiClass, cs, dc, rst) {}
#endif // end !ESP8266

// SCREEN INITIALIZATION ***************************************************

// Rather than a bazillion writecommand() and writedata() calls, screen
// initialization commands and arguments are organized in these tables
// stored in PROGMEM.  The table may look bulky, but that's mostly the
// formatting -- storage-wise this is hundreds of bytes more compact
// than the equivalent code.  Companion function follows.

// clang-format off

static const uint8_t PROGMEM
  generic_st7789[] =  {                // Init commands for 7789 screens
    9,                              //  9 commands in list:
    ST77XX_SWRESET,   ST_CMD_DELAY, //  1: Software reset, no args, w/delay
      150,                          //     ~150 ms delay
    ST77XX_SLPOUT ,   ST_CMD_DELAY, //  2: Out of sleep mode, no args, w/delay
      10,                          //      10 ms delay
    ST77XX_COLMOD , 1+ST_CMD_DELAY, //  3: Set color mode, 1 arg + delay:
      0x55,                         //     16-bit color
      10,                           //     10 ms delay
    ST77XX_MADCTL , 1,              //  4: Mem access ctrl (directions), 1 arg:
      0x08,                         //     Row/col addr, bottom-top refresh
    ST77XX_CASET  , 4,              //  5: Column addr set, 4 args, no delay:
      0x00,
      0,        //     XSTART = 0
      0,
      240,  //     XEND = 240
    ST77XX_RASET  , 4,              //  6: Row addr set, 4 args, no delay:
      0x00,
      0,             //     YSTART = 0
      320>>8,
      320&0xFF,  //     YEND = 320
    ST77XX_INVON  ,   ST_CMD_DELAY,  //  7: hack
      10,
    ST77XX_NORON  ,   ST_CMD_DELAY, //  8: Normal display on, no args, w/delay
      10,                           //     10 ms delay
    ST77XX_DISPON ,   ST_CMD_DELAY, //  9: Main screen turn on, no args, delay
      10 };                          //    10 ms delay

// clang-format on

/**************************************************************************/
/*!
    @brief  Initialization code common to all ST7789 displays
    @param  width  Display width
    @param  height Display height
    @param  mode   SPI data mode; one of SPI_MODE0, SPI_MODE1, SPI_MODE2
                   or SPI_MODE3 (do NOT pass the numbers 0,1,2 or 3 -- use
                   the defines only, the values are NOT the same!)
*/
/**************************************************************************/
void Adafruit_ST7789::init(uint16_t width, uint16_t height, uint8_t mode) {
  // Save SPI data mode. commonInit() calls begin() (in Adafruit_ST77xx.cpp),
  // which in turn calls initSPI() (in Adafruit_SPITFT.cpp), passing it the
  // value of spiMode. It's done this way because begin() really should not
  // be modified at this point to accept an SPI mode -- it's a virtual
  // function required in every Adafruit_SPITFT subclass and would require
  // updating EVERY such library...whereas, at the moment, we know that
  // certain ST7789 displays are the only thing that may need a non-default
  // SPI mode, hence this roundabout approach...
  spiMode = mode;
  // (Might get added similarly to other display types as needed on a
  // case-by-case basis.)

  commonInit(NULL);

  if ((width == 240) && (height == 240)) { // 1.3" and 1.54" displays
    _colstart = 0;
    _rowstart = 80;
  } else if ((width == 135) && (height == 240)) { // 1.13" display
    _colstart = 53;
    _rowstart = 40;
  } else {
    _colstart = 0;
    _rowstart = 0;
  }
  _width = width;
  _height = height;

  displayInit(generic_st7789);

  if ((width == 135) && (height == 240)) {
    setRotation(0);
  } else {
    setRotation(0);
  }
}

/**************************************************************************/
/*!
    @brief  Set origin of (0,0) and orientation of TFT display
    @param  m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void Adafruit_ST7789::setRotation(uint8_t m) {
  uint8_t madctl = 0;

  rotation = m & 3; // can't be higher than 3

  switch (rotation) {
  case 0:
    madctl = ST77XX_MADCTL_MX | ST77XX_MADCTL_MY | ST77XX_MADCTL_RGB;
    _xstart = _colstart;
    _ystart = _rowstart;
    _width = WIDTH;
    _height = HEIGHT;
    break;
  case 1:
    madctl = ST77XX_MADCTL_MY | ST77XX_MADCTL_MV | ST77XX_MADCTL_RGB;
    _xstart = _rowstart;
    _ystart = _colstart;
    _height = WIDTH;
    _width = HEIGHT;
    break;
  case 2:
    madctl = ST77XX_MADCTL_RGB;
    if ((WIDTH == 135) && (HEIGHT == 240)) {
      _xstart = _colstart - 1;
      _ystart = _rowstart;
    } else {
      _xstart = 0;
      _ystart = 0;
    }
    _width = WIDTH;
    _height = HEIGHT;
    break;
  case 3:
    madctl = ST77XX_MADCTL_MX | ST77XX_MADCTL_MV | ST77XX_MADCTL_RGB;
    if ((WIDTH == 135) && (HEIGHT == 240)) {
      _xstart = _rowstart;
      _ystart = _colstart;
    } else {
      _xstart = 0;
      _ystart = 0;
    }
    _height = WIDTH;
    _width = HEIGHT;
    break;
  }

  sendCommand(ST77XX_MADCTL, &madctl, 1);
}
