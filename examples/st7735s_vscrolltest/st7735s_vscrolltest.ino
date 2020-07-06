/**************************************************************************
  This is a library for displays based on ST775S driver.
  
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional).

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by NeKuNeKo, based on Limor Fried/Ladyada graphictest script for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 **************************************************************************/
 
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>

#if defined(ARDUINO_FEATHER_ESP32) // Feather Huzzah32
  #define TFT_CS         14
  #define TFT_RST        15
  #define TFT_DC         32

#elif defined(ESP8266)
  #define TFT_CS         4
  #define TFT_RST        16                                            
  #define TFT_DC         5

#else
  // For the breakout board, you can use any 2 or 3 pins.
  // These pins will also work for the 1.8" TFT shield.
  #define TFT_CS        10
  #define TFT_RST        9 // Or set to -1 and connect to Arduino RESET pin
  #define TFT_DC         8
#endif

// #define TFT_LITE A0 // Uncomment if needed, and check the propper pin

// OPTION 1 (recommended) is to use the HARDWARE SPI pins, which are unique
// to each board and not reassignable. For Arduino Uno: MOSI = pin 11 and
// SCLK = pin 13. This is the fastest mode of operation and is required if
// using the breakout board's microSD card.

// For 1.44" and 1.8" TFT with ST7735 use:
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// OPTION 2 lets you interface the display using ANY TWO or THREE PINS,
// tradeoff being that performance is not as fast as hardware SPI above.
//#define TFT_MOSI 11  // Data out
//#define TFT_SCLK 13  // Clock out

// For ST7735-based displays, we will use this call
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);


void setup(void) 
{
  Serial.begin(9600);
  //while(!Serial);
  Serial.print(F("Hello! ST77xx TFT Test"));

  // Use this initializer if using a 1.8" TFT screen:
  //tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab

  // OR use this initializer if using a 1.8" TFT screen with offset such as WaveShare:
  // tft.initR(INITR_GREENTAB);      // Init ST7735S chip, green tab

  // OR use this initializer (uncomment) if using a 0.96" 160x80 TFT:
  //tft.initR(INITR_MINI160x80);  // Init ST7735S mini display

  // OR use this initializer (uncomment) if using a 0.96" 160x80 BGR TFT:
  tft.initR(INITR_MINI160x80BGR);  // Init ST7735S BGR mini display 

  // SPI speed defaults to SPI_DEFAULT_FREQ defined in the library, you can override it here
  // Note that speed allowable depends on chip and quality of wiring, if you go too fast, you
  // may end up with a black screen some times, or all the time.
  //tft.setSPISpeed(40000000);
  
  Serial.println(F("Initialized"));

#ifdef TFT_LITE
  pinMode(TFT_LITE, OUTPUT);
  digitalWrite(TFT_LITE, LOW);   // Set LOW / HIGH depends on your config.
  //analogWrite(TFT_LITE, 1023); // Use this if using PWM pin.
#endif

  // - Color Scheme config - 
  tft.invertDisplay(false);
  tft.fillScreen(ST77XX_BLACK); // Color Background
}

void loop() 
{
  testBounds();
  testRotation();
  testColor();
  //testColorVscroll(); // infinite Loop
  vscrolltest();      // infinite Loop 
  //hscrolltest();      // infinite Loop
}


void testBounds ()
{
  tft.setTextSize(1);
  tft.setTextWrap(true);
  
  tft.setRotation(0); 
  tft.fillScreen(ST77XX_BLACK);
  tft.drawPixel(0,  0, ST77XX_WHITE);
  tft.setRotation(0); 
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(0+20, 0+20);
  tft.print("(0,0)");
  delay(2000);

  tft.setRotation(0);
  tft.fillScreen(ST77XX_BLACK);
  tft.drawPixel(tft.width()-1,  0, ST77XX_BLUE);
  tft.setRotation(1); 
  tft.setTextColor(ST77XX_BLUE);
  tft.setCursor(0+20, 0+20);
  tft.print("(width,0)");
  delay(2000);

  tft.setRotation(0);
  tft.fillScreen(ST77XX_BLACK);
  tft.drawPixel(tft.width()-1,  tft.height()-1, ST77XX_YELLOW);
  tft.setRotation(2); 
  tft.setTextColor(ST77XX_YELLOW);
  tft.setCursor(0+20, 0+20);
  tft.print("(width,height)");
  delay(2000);

  tft.setRotation(0);
  tft.fillScreen(ST77XX_BLACK);
  tft.drawPixel(0,  tft.height()-1, ST77XX_RED);
  tft.setRotation(3); 
  tft.setTextColor(ST77XX_RED);
  tft.setCursor(0+20, 0+20);
  tft.print("(0,height)");
  delay(2000);
}


void testRotation ()
{
    tft.setTextSize(1);
    tft.setTextWrap(false);
    
    tft.setRotation(0); // 0 - Vertical
    tft.fillScreen(ST77XX_BLACK);
    tft.drawFastHLine(0, 0, tft.width(),  ST77XX_RED);
    tft.drawFastVLine(0, 0, tft.height(), ST77XX_CYAN);
    tft.setCursor(10, 10);
    tft.setTextColor(ST77XX_WHITE);
    tft.print("Rotation 0");
    tft.setCursor(10, 30);
    tft.setTextColor(ST77XX_RED);
    tft.print("width");
    tft.setCursor(10, 50);
    tft.setTextColor(ST77XX_CYAN);
    tft.print("height");
    delay(2000);
    
    tft.setRotation(1); // 1 - Horizontal
    tft.fillScreen(ST77XX_BLACK);
    tft.drawFastHLine(0, 0, tft.width(), ST77XX_RED);
    tft.drawFastVLine(0, 0, tft.height(), ST77XX_CYAN);
    tft.setCursor(10, 10);
    tft.setTextColor(ST77XX_WHITE);
    tft.print("Rotation 1");
    tft.setCursor(10, 30);
    tft.setTextColor(ST77XX_RED);
    tft.print("width");
    tft.setCursor(10, 50);
    tft.setTextColor(ST77XX_CYAN);
    tft.print("height");
    delay(2000);
        
    tft.setRotation(2); // 2 - Inverse Vertical
    tft.fillScreen(ST77XX_BLACK);
    tft.drawFastHLine(0, 0, tft.width(), ST77XX_RED);
    tft.drawFastVLine(0, 0, tft.height(), ST77XX_CYAN);
    tft.setCursor(10, 10);
    tft.setTextColor(ST77XX_WHITE);
    tft.print("Rotation 2");
    tft.setCursor(10, 30);
    tft.setTextColor(ST77XX_RED);
    tft.print("width");
    tft.setCursor(10, 50);
    tft.setTextColor(ST77XX_CYAN);
    tft.print("height");
    delay(2000);
    
    tft.setRotation(3); // 3- Inverse Horizontal
    tft.fillScreen(ST77XX_BLACK);
    tft.drawFastHLine(0, 0, tft.width(), ST77XX_RED);
    tft.drawFastVLine(0, 0, tft.height(), ST77XX_CYAN);
    tft.setCursor(10, 10);
    tft.setTextColor(ST77XX_WHITE);
    tft.print("Rotation 3");
    tft.setCursor(10, 30);
    tft.setTextColor(ST77XX_RED);
    tft.print("width");
    tft.setCursor(10, 50);
    tft.setTextColor(ST77XX_CYAN);
    tft.print("height");
    delay(2000);
}


void testColor()
{  
  tft.setRotation(0);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setTextWrap(false);
  
  tft.setCursor(0, 15);
  tft.setTextColor(ST77XX_BLACK);
  tft.print("BLACK");

  tft.setCursor(0, 30);
  tft.setTextColor(ST77XX_WHITE);
  tft.print("WHITE");

  tft.setCursor(0, 45);
  tft.setTextColor(ST77XX_YELLOW);
  tft.print("YELLOW");

  tft.setCursor(0, 60);
  tft.setTextColor(ST77XX_GREEN);
  tft.print("GREEN");

  tft.setCursor(0, 75);
  tft.setTextColor(ST77XX_BLUE);
  tft.print("BLUE");

  tft.setCursor(0, 90);
  tft.setTextColor(ST77XX_MAGENTA);
  tft.print("MAGENTA");

  tft.setCursor(0, 105);
  tft.setTextColor(ST77XX_ORANGE);
  tft.print("ORANGE");

  tft.setCursor(0, 120);
  tft.setTextColor(ST77XX_CYAN);
  tft.print("CYAN");

  tft.setCursor(0, 135);
  tft.setTextColor(ST7735_RED);
  tft.print("RED");

  delay(2000);
}


void testColorVscroll ()
{
  uint8_t TFA = 1;           // Top Fixed Area.    Minimum 1 pixel
  uint8_t BFA = 1;           // Bottom Fixed Area. Minimum 1 pixel
  bool bottom_to_top = true; // Scroll direction
  
  testColor();
  tft.setVerticalScrollConfig(TFA, BFA, bottom_to_top);
  
  uint8_t scroll = 0;
  while(true){
    tft.setVerticalScrollPointer(scroll + TFA);
    scroll++;
    if(scroll > (tft.height() - TFA - BFA))
      scroll = 0;
    delay(25);
  }  
}


void vscrolltest ()
{
  char txt[] = "ST7735 TFT vertical  scrolling";
  char nbr[5];      // Number can be up to five digits 
  uint8_t scroll;   // Vertical Scroll Area pointer
  uint16_t number;

  tft.setRotation(0); // only valid 0
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextWrap(true);
  
  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_WHITE);
  tft.print(txt);
  
  strcpy (txt, "Line:");
  tft.setTextWrap(false);
   
  uint8_t TFA = 30;          // Top Fixed Area.    Minimum 1 pixel
  uint8_t BFA = 1;           // Bottom Fixed Area. Minimum 1 pixel
  bool bottom_to_top = true; // Scroll direction
  tft.setVerticalScrollConfig(TFA, BFA, bottom_to_top);

  scroll = 0;
  number = 0;
  while(true) 
  {
    for(int i=0; i < 10; i++)
    {
      tft.setVerticalScrollPointer(scroll + TFA);
      tft.drawFastHLine(0, scroll + TFA, tft.width(), ST7735_BLACK);  // Delete lines which may appear in bottom of TFT
      scroll++;
      delay(25); // scroll speed
      if(scroll >= (tft.height() - TFA))
        scroll = 0;
    }
    
    sprintf(nbr,"%Lu",number);
    
    if(scroll == 0)
    {
      tft.setCursor(0, 150);
      tft.setTextColor(ST7735_YELLOW);
      tft.print(txt);
      
      tft.setCursor(40, 150);
      tft.setTextColor(ST7735_GREEN);
      tft.print(nbr);
    }
    else
    {
      tft.setCursor(0, 20 + scroll);
      tft.setTextColor(ST7735_YELLOW);
      tft.print(txt);
      
      tft.setCursor(40, 20 + scroll);
      tft.setTextColor(ST7735_GREEN);
      tft.print(nbr);
    }
    number++;
  } 
}


void hscrolltest() 
{
  char message [] PROGMEM = "In a village of La Mancha, the name of which I have no\
  desire to call to mind, there lived not long since one of those gentlemen that keep\
  a lance in the lance-rack, an old buckler, a lean hack, and a greyhound for coursing.";
  int x, minX;

  tft.setRotation(1);
  tft.setTextWrap(false);
  tft.setTextColor(ST77XX_WHITE);
  tft.fillScreen(ST77XX_BLACK);
    
  tft.setTextSize(1);
  tft.setCursor(0, 7);
  tft.print("TEST SCROLLING TEXT");

  tft.setTextSize(3);
  tft.setCursor(0, 40);
  tft.print("Hello world!");
  
  tft.setTextSize(2);
  x = tft.width();
  minX = -12 * strlen(message); // 12 = 6 pixels /character * textSize
  while(true)
  {
    // Option 1: fastHLines
    //for (int i = 0; i<= 20; ++i)
     // tft.writeFastHLine(0, 20+i, tft.width(), ST77XX_BLACK);

    // Option 2: fastVLines
    for (int i = 0; i< tft.width(); ++i)
      tft.writeFastVLine(0+i, 20, 16, ST77XX_BLACK);
    
    tft.setCursor(x, 20);
    tft.print(message);

    x = x-2; // scroll speed
    if (x < minX)
      x = tft.width();
  }
}
