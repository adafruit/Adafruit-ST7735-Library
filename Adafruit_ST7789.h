#ifndef _ADAFRUIT_ST7789H_
#define _ADAFRUIT_ST7789H_

#include "Adafruit_ST77xx.h"

// Define the screen rotation position in a clockwise direction
//
//        NONE
//  LEFT   +    RIGHT
//       BOTTOM
//
#define ST77XX_ROTATE_NONE              2
#define ST77XX_ROTATE_RIGHT             1
#define ST77XX_ROTATE_LEFT              3
#define ST77XX_ROTATE_BOTTOM            0
// Flip the screen horizontally, useful in some applications that use mirrors.
#define ST77XX_FLIP_HORIZONTAL          true

/// Subclass of ST77XX type display for ST7789 TFT Driver
class Adafruit_ST7789 : public Adafruit_ST77xx {
public:
  Adafruit_ST7789(int8_t cs, int8_t dc, int8_t mosi, int8_t sclk,
                  int8_t rst = -1);
  Adafruit_ST7789(int8_t cs, int8_t dc, int8_t rst);
#if !defined(ESP8266)
  Adafruit_ST7789(SPIClass *spiClass, int8_t cs, int8_t dc, int8_t rst);
#endif // end !ESP8266

  void setRotation(uint8_t m);
  void setRotation(uint8_t r, bool flip);
  void init(uint16_t width, uint16_t height, uint8_t spiMode = SPI_MODE0);

protected:
  uint8_t _colstart2 = 0, ///< Offset from the right
      _rowstart2 = 0;     ///< Offset from the bottom

private:
  uint16_t windowWidth;
  uint16_t windowHeight;
};

#endif // _ADAFRUIT_ST7789H_
