#ifndef _ADAFRUIT_ST7789H_
#define _ADAFRUIT_ST7789H_

#include "Adafruit_ST77xx.h"
// extern class ExpanderPin;

/// Subclass of ST77XX type display for ST7789 TFT Driver
class Adafruit_ST7789 : public Adafruit_ST77xx {
  public:
  Adafruit_ST7789(int8_t cs, int8_t dc, int8_t mosi, int8_t sclk,
    int8_t rst = -1);
    Adafruit_ST7789(int8_t cs, int8_t dc, int8_t rst);
    #if !defined(ESP8266)
    Adafruit_ST7789(SPIClass *spiClass, int8_t cs, int8_t dc, int8_t rst);
    #endif // end !ESP8266
    #if defined(ARDUINO_ARDUINO_NESSO_N1)
  Adafruit_ST7789(int8_t cs, ExpanderPin *dc, ExpanderPin *rst = NULL);
  Adafruit_ST7789(int8_t cs, int8_t dc, ExpanderPin *rst = NULL);
  Adafruit_ST7789(ExpanderPin *cs, ExpanderPin *dc, ExpanderPin *rst = NULL);
  Adafruit_ST7789(SPIClass *spiClass, int8_t cs, int8_t dc, ExpanderPin *rst);
  Adafruit_ST7789(SPIClass *spiClass, int8_t cs, ExpanderPin *dc, ExpanderPin *rst);
  Adafruit_ST7789(SPIClass *spiClass, ExpanderPin *cs, ExpanderPin *dc, ExpanderPin *rst);
#endif

void setRotation(uint8_t m);
void init(uint16_t width, uint16_t height, uint8_t spiMode = SPI_MODE0);

protected:
uint8_t _colstart2 = 0, ///< Offset from the right
_rowstart2 = 0;     ///< Offset from the bottom
#if defined(ARDUINO_ARDUINO_NESSO_N1)
  ExpanderPin *cs = nullptr;
  ExpanderPin *dc = nullptr;
  ExpanderPin *rst = nullptr;
#endif

private:
  uint16_t windowWidth;
  uint16_t windowHeight;
};

#endif // _ADAFRUIT_ST7789H_
