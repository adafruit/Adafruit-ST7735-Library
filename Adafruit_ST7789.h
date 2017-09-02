#include "Adafruit_ST77xx.h"


class Adafruit_ST7789 : public Adafruit_ST77xx {

 public:
 Adafruit_ST7789(int8_t cs, int8_t dc, int8_t sid, int8_t sclk, int8_t rst) : 
  Adafruit_ST77xx(cs, dc, sid, sclk, rst) {}
  
 Adafruit_ST7789(int8_t cs, int8_t dc, int8_t rst) : 
  Adafruit_ST77xx(cs, dc, rst) {}
  
  // the tab types are so weird we need to do this 'by hand'
  void  setRotation(uint8_t m);

  void  init(uint16_t width, uint16_t height);

 private:

};
