#ifndef _ADAFRUIT_ST7789H_
#define _ADAFRUIT_ST7789H_

#include "Adafruit_ST77xx.h"

/// Subclass of ST77XX type display for ST7789 TFT Driver
class Adafruit_ST7789 : public Adafruit_ST77xx {
  public:
    Adafruit_ST7789(int8_t cs, int8_t dc, int8_t mosi, int8_t sclk,
      int8_t rst = -1);
    Adafruit_ST7789(int8_t cs, int8_t dc, int8_t rst);
    Adafruit_ST7789(SPIClass *spiClass, int8_t cs, int8_t dc, int8_t rst);

    void setRotation(uint8_t m);
    void init(uint16_t width, uint16_t height);
};

#endif // _ADAFRUIT_ST7789H_
