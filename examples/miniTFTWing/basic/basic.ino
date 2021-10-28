#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h> // Hardware-specific library
#include "Adafruit_miniTFTWing.h"

Adafruit_miniTFTWing ss;
#define TFT_RST    -1    // we use the seesaw for resetting to save a pin

#ifdef ESP8266
   #define TFT_CS   0
   #define TFT_DC   15
#elif defined(ESP32) && !defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2)
   #define TFT_CS   15
   #define TFT_DC   33
#elif defined(TEENSYDUINO)
   #define TFT_DC   10
   #define TFT_CS   4
#elif defined(ARDUINO_STM32_FEATHER)
   #define TFT_DC   PB4
   #define TFT_CS   PA15
#elif defined(ARDUINO_NRF52832_FEATHER)  /* BSP 0.6.5 and higher! */
   #define TFT_DC   11
   #define TFT_CS   31
#elif defined(ARDUINO_MAX32620FTHR) || defined(ARDUINO_MAX32630FTHR)
   #define TFT_DC   P5_4
   #define TFT_CS   P5_3
#else
    // Anything else, defaults!
   #define TFT_CS   5
   #define TFT_DC   6
#endif

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

void setup()   {
  Serial.begin(115200);

  /*
  while (!Serial)  delay(10);  // Wait until serial console is opened
  */

  if (!ss.begin()) {
    Serial.println("seesaw couldn't be found!");
    while(1);
  }

  Serial.print("seesaw started!\tVersion: ");
  Serial.println(ss.getVersion(), HEX);

  ss.tftReset();   // reset the display
  ss.setBacklight(TFTWING_BACKLIGHT_ON);  // turn off the backlight

  tft.initR(INITR_MINI160x80);   // initialize a ST7735S chip, mini display
  Serial.println("TFT initialized");

  tft.setRotation(1);

  tft.fillScreen(ST77XX_RED);
  delay(100);
  tft.fillScreen(ST77XX_GREEN);
  delay(100);
  tft.fillScreen(ST77XX_BLUE);
  delay(100);
  tft.fillScreen(ST77XX_BLACK);
}

void loop() {
  delay(10);
  uint32_t buttons = ss.readButtons();
  //Serial.println(buttons, BIN);

  uint16_t color;

  color = ST77XX_BLACK;
  if (! (buttons & TFTWING_BUTTON_LEFT)) {
    Serial.println("LEFT");
    color = ST77XX_WHITE;
  }
  tft.fillTriangle(150, 30, 150, 50, 160, 40, color);
 
  color = ST77XX_BLACK;
  if (! (buttons & TFTWING_BUTTON_RIGHT)) {
    Serial.println("RIGHT");
    color = ST77XX_WHITE;
  }
  tft.fillTriangle(120, 30, 120, 50, 110, 40, color);

  color = ST77XX_BLACK;
  if (! (buttons & TFTWING_BUTTON_DOWN)) {
    Serial.println("DOWN");
    color = ST77XX_WHITE;
  }
  tft.fillTriangle(125, 26, 145, 26, 135, 16, color);

  color = ST77XX_BLACK;
  if (! (buttons & TFTWING_BUTTON_UP)) {
    Serial.println("UP");
    color = ST77XX_WHITE;
  }
  tft.fillTriangle(125, 53, 145, 53, 135, 63, color);

  color = ST77XX_BLACK;
  if (! (buttons & TFTWING_BUTTON_A)) {
    Serial.println("A");
    color = ST7735_GREEN;
  }
  tft.fillCircle(30, 57, 10, color);

  color = ST77XX_BLACK;
  if (! (buttons & TFTWING_BUTTON_B)) {
    Serial.println("B");
    color = ST77XX_YELLOW;
  }
  tft.fillCircle(30, 18, 10, color);

  color = ST77XX_BLACK;
  if (! (buttons & TFTWING_BUTTON_SELECT)) {
    Serial.println("SELECT");
    color = ST77XX_WHITE;
  }
  tft.fillCircle(80, 40, 7, color);
}
