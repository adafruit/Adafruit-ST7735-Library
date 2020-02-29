/*
 * This is an example sketch that shows how to toggle the display
 * on and off at runtime to avoid screen burn-in.
 * 
 * The sketch also demonstrates how to erase a previous value by re-drawing the 
 * older value in the screen background color prior to writing a new value in
 * the same location. This avoids the need to call fillScreen() to erase the
 * entire screen followed by a complete redraw of screen contents.
 * 
 * Originally written by Phill Kelley. BSD license.
 * Adapted for ST77xx by Melissa LeBlanc-Williams
 */

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

#ifdef ADAFRUIT_HALLOWING
  #define TFT_CS        39 // Hallowing display control pins: chip select
  #define TFT_RST       37 // Display reset
  #define TFT_DC        38 // Display data/command select
  #define TFT_BACKLIGHT  7 // Display backlight pin

#elif defined(ARDUINO_FEATHER_ESP32) // Feather Huzzah32
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

#define SerialDebugging true

// OPTION 1 (recommended) is to use the HARDWARE SPI pins, which are unique
// to each board and not reassignable. For Arduino Uno: MOSI = pin 11 and
// SCLK = pin 13. This is the fastest mode of operation and is required if
// using the breakout board's microSD card.

// For 1.44" and 1.8" TFT with ST7735 (including HalloWing) use:
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// For 1.3", 1.54", and 2.0" TFT with ST7789:
//Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// OPTION 2 lets you interface the display using ANY TWO or THREE PINS,
// tradeoff being that performance is not as fast as hardware SPI above.
//#define TFT_MOSI 11  // Data out
//#define TFT_SCLK 13  // Clock out
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// connect a push button between ground and...
const uint8_t   Button_pin              = 2;

// color definitions
const uint16_t  Display_Color_Black        = 0x0000;
const uint16_t  Display_Color_Blue         = 0x001F;
const uint16_t  Display_Color_Red          = 0xF800;
const uint16_t  Display_Color_Green        = 0x07E0;
const uint16_t  Display_Color_Cyan         = 0x07FF;
const uint16_t  Display_Color_Magenta      = 0xF81F;
const uint16_t  Display_Color_Yellow       = 0xFFE0;
const uint16_t  Display_Color_White        = 0xFFFF;

// The colors we actually want to use
uint16_t        Display_Text_Color         = Display_Color_Black;
uint16_t        Display_Backround_Color    = Display_Color_Blue;

// assume the display is off until configured in setup()
bool            isDisplayVisible        = false;

// declare size of working string buffers. Basic strlen("d hh:mm:ss") = 10
const size_t    MaxString               = 16;

// the string being displayed on the SSD1331 (initially empty)
char oldTimeString[MaxString]           = { 0 };

// the interrupt service routine affects this
volatile bool   isButtonPressed         = false;


// interrupt service routine
void senseButtonPressed() {
    if (!isButtonPressed) {
        isButtonPressed = true;
    }
}


void displayUpTime() {

    // calculate seconds, truncated to the nearest whole second
    unsigned long upSeconds = millis() / 1000;

    // calculate days, truncated to nearest whole day
    unsigned long days = upSeconds / 86400;

    // the remaining hhmmss are
    upSeconds = upSeconds % 86400;

    // calculate hours, truncated to the nearest whole hour
    unsigned long hours = upSeconds / 3600;

    // the remaining mmss are
    upSeconds = upSeconds % 3600;

    // calculate minutes, truncated to the nearest whole minute
    unsigned long minutes = upSeconds / 60;

    // the remaining ss are
    upSeconds = upSeconds % 60;

    // allocate a buffer
    char newTimeString[MaxString] = { 0 };

    // construct the string representation
    sprintf(
        newTimeString,
        "%lu %02lu:%02lu:%02lu",
        days, hours, minutes, upSeconds
    );

    // has the time string changed since the last tft update?
    if (strcmp(newTimeString,oldTimeString) != 0) {

        // yes! home the cursor
        tft.setCursor(0,0);

        // change the text color to the background color
        tft.setTextColor(Display_Backround_Color);

        // redraw the old value to erase
        tft.print(oldTimeString);

        // home the cursor
        tft.setCursor(0,0);
        
        // change the text color to foreground color
        tft.setTextColor(Display_Text_Color);
    
        // draw the new time value
        tft.print(newTimeString);
    
        // and remember the new value
        strcpy(oldTimeString,newTimeString);
    }
}

void setup() {

    // button press pulls pin LOW so configure HIGH
    pinMode(Button_pin,INPUT_PULLUP);

    // use an interrupt to sense when the button is pressed
    attachInterrupt(digitalPinToInterrupt(Button_pin), senseButtonPressed, FALLING);

    #if (SerialDebugging)
    Serial.begin(115200); while (!Serial); Serial.println();
    #endif

    // settling time
    delay(250);

    // ignore any power-on-reboot garbage
    isButtonPressed = false;

    #ifdef ADAFRUIT_HALLOWING
      // HalloWing is a special case. It uses a ST7735R display just like the
      // breakout board, but the orientation and backlight control are different.
      tft.initR(INITR_HALLOWING);        // Initialize HalloWing-oriented screen
      pinMode(TFT_BACKLIGHT, OUTPUT);
      digitalWrite(TFT_BACKLIGHT, HIGH); // Backlight on
    #else
      // Use this initializer if using a 1.8" TFT screen:
      tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab
    
      // OR use this initializer (uncomment) if using a 1.44" TFT:
      //tft.initR(INITR_144GREENTAB); // Init ST7735R chip, green tab
    
      // OR use this initializer (uncomment) if using a 0.96" 180x60 TFT:
      //tft.initR(INITR_MINI160x80);  // Init ST7735S mini display
    
      // OR use this initializer (uncomment) if using a 1.54" 240x240 TFT:
      //tft.init(240, 240);           // Init ST7789 240x240

      // OR use this initializer (uncomment) if using a 2.0" 320x240 TFT:
      //tft.init(240, 320);           // Init ST7789 320x240
  
      // SPI speed defaults to SPI_DEFAULT_FREQ defined in the library, you can override it here
      // Note that speed allowable depends on chip and quality of wiring, if you go too fast, you
      // may end up with a black screen some times, or all the time.
      //tft.setSPISpeed(40000000);
    #endif

    // initialise the display
    tft.setFont();
    tft.fillScreen(Display_Backround_Color);
    tft.setTextColor(Display_Text_Color);
    tft.setTextSize(1);

    // the display is now on
    isDisplayVisible = true;

}


void loop() {

    // unconditional display, regardless of whether display is visible
    displayUpTime();

    // has the button been pressed?
    if (isButtonPressed) {
        
        // yes! toggle display visibility
        isDisplayVisible = !isDisplayVisible;

        // apply
        tft.enableDisplay(isDisplayVisible);

        #if (SerialDebugging)
        Serial.print("button pressed @ ");
        Serial.print(millis());
        Serial.print(", display is now ");
        Serial.println((isDisplayVisible ? "ON" : "OFF"));
        #endif

        // confirm button handled
        isButtonPressed = false;
        
    }

    // no need to be in too much of a hurry
    delay(100);
   
}
