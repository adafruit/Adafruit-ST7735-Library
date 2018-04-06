#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>

#define LCD_CS 10
#define DC     8
#define RESET  9
Adafruit_ST7735 tft = Adafruit_ST7735(LCD_CS,  DC, RESET);


void setup(void) {
  Serial.begin(9600);
  Serial.println("Hello! ST7735 TFT Test");
  
  // Use this initializer if you're using a 1.8" TFT
  //tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.initR(INITR_DEEKROBOT); // Deek Robot variant
  
  tft.setTextWrap(false);
  tft.fillScreen(ST7735_WHITE);
  tft.fillRect(5,5,118,150,ST7735_BLACK);
  
  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_RED);
  tft.setTextSize(1);
  tft.println("ABC");
  
  tft.setTextColor(ST7735_YELLOW);
  tft.setTextSize(2);
  tft.println("ABC");
  
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(3);
  tft.println("ABC");
  
  tft.setTextColor(ST7735_BLUE);
  tft.setTextSize(4);
  tft.println("ABC");
  
  tft.setTextColor(ST7735_RED);
  tft.setTextSize(5);
  tft.println("ABC");
  
  Serial.println("done");
}

void loop() {}
