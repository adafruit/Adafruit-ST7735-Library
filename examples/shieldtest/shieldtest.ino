/***************************************************
  This is an example sketch for the Adafruit 1.8" TFT shield with joystick
  ----> http://www.adafruit.com/products/802

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 pins are required to
  interface
  One pin is also needed for the joystick, we use analog 3
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SD.h>
#include <SPI.h>

#if defined(ARDUINO_ARCH_SAM)
#undef __FlashStringHelper::F(string_literal)
#define F(string_literal) string_literal
#endif

// TFT display and SD card will share the hardware SPI interface.
// Hardware SPI pins are specific to the Arduino board type and
// cannot be remapped to alternate pins.  For Arduino Uno,
// Duemilanove, etc., pin 11 = MOSI, pin 12 = MISO, pin 13 = SCK.
//
// However, because the TFT shield is hard wired to use specific pins,
// Adafruit has come up with a software SPI library. This library
// allows the Mega, Leonardo, Due, and any other board which does not
// have their hardware SPI interface in the same place to be able to
// use this shield without jumpering pins.
//
// Instructions for using the software SPI library are available at
// http://learn.adafruit.com/adafruit-data-logger-shield/for-the-mega-and-leonardo
// and the code is available at https://github.com/adafruit/SD
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || \
    defined(__AVR_ATmega32U4__) || defined(ARDUINO_ARCH_SAM)
#define SPI_SCK  13
#define SPI_DI   12
#define SPI_DO   11

#define SD_CS     4  // Chip select line for SD card
#define TFT_CS   10  // Chip select line for TFT display
#define TFT_DC    8  // Data/command line for TFT
#define TFT_RST   0  // Reset line for TFT. You can also connect this to the Arduino reset

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, SPI_DO, SPI_SCK, TFT_RST);
#else // Diecimila / Duemilanove / Uno / etc.
#define SD_CS     4  // Chip select line for SD card
#define TFT_CS   10  // Chip select line for TFT display
#define TFT_DC    8  // Data/command line for TFT
#define TFT_RST  -1  // Reset line for TFT. You can also connect this to the Arduino reset

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
#endif

// Because the Arduino Due uses 3.3 volts instead of 5 volts, the voltage
// being read by the analog pin changes. If you are using a different
// type of Arduino that runs on 3.3 volts, you may also have to use these
// values.
//
// These positions assume that you are looking at the shield with the
// microSD card socket on the lower left and the joystick in the lower right.
// The screen will be directly above both of them. The text printed by this program should
// not appear rotated or upside down.
#if defined(ARDUINO_ARCH_SAM)
#define JOYSTICK_DOWN    50
#define JOYSTICK_RIGHT   200
#define JOYSTICK_SELECT  350
#define JOYSTICK_UP      600
#define JOYSTICK_LEFT    950
#define JOYSTICK_NONE    1023
#else
#define JOYSTICK_DOWN    50
#define JOYSTICK_RIGHT   150
#define JOYSTICK_SELECT  250
#define JOYSTICK_UP      500
#define JOYSTICK_LEFT    650
#define JOYSTICK_NONE    1023
#endif

void bmpDraw(char*, uint8_t, uint8_t);
uint16_t read16(File);
uint32_t read32(File);

void setup(void) {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // Our supplier changed the 1.8" display slightly after Jan 10, 2012
  // so that the alignment of the TFT had to be shifted by a few pixels
  // this just means the init code is slightly different. Check the
  // color of the tab to see which init code to try. If the display is
  // cut off or has extra 'random' pixels on the top & left, try the
  // other option!
  // If you are seeing red and green color inversion, use Black Tab

  // If your TFT's plastic wrap has a Black Tab, use the following:
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  // If your TFT's plastic wrap has a Red Tab, use the following:
  //tft.initR(INITR_REDTAB);   // initialize a ST7735R chip, red tab
  // If your TFT's plastic wrap has a Green Tab, use the following:
  //tft.initR(INITR_GREENTAB); // initialize a ST7735R chip, green tab

  Serial.println("OK!");
  tft.fillScreen(0x0000);
  Serial.println("Initializing SD card...");
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega32U4__) || defined(ARDUINO_ARCH_SAM)
  if (!SD.begin(SD_CS, SPI_DO, SPI_DI, SPI_SCK)) {
#else
  if (!SD.begin(SD_CS)) {
#endif
    Serial.println("SD card initialization failed!");
    while (true);
  }
  Serial.println("SD card initialization done.");

}

// Check the joystick position
int checkJoystick(bool serialPrint = false)
{
  int joystickState = analogRead(3);
  if (serialPrint)
  {
    Serial.print("Joystick state = ");
    Serial.println(joystickState);
  }
  if (joystickState < JOYSTICK_DOWN)   return JOYSTICK_DOWN;
  if (joystickState < JOYSTICK_RIGHT)  return JOYSTICK_RIGHT;
  if (joystickState < JOYSTICK_SELECT) return JOYSTICK_SELECT;
  if (joystickState < JOYSTICK_UP)     return JOYSTICK_UP;
  if (joystickState < JOYSTICK_LEFT)   return JOYSTICK_LEFT;
  return JOYSTICK_NONE;
}

int joystickHistory = 0;

void loop() {
  int b = checkJoystick(true);
  tft.setTextSize(3);
  if (b == JOYSTICK_DOWN) {
    tft.setTextColor(ST7735_RED);
    tft.setCursor(0, 10);
    tft.print("Down ");
    joystickHistory |= 1;
  }
  if (b == JOYSTICK_LEFT) {
    tft.setTextColor(ST7735_YELLOW);
    tft.setCursor(0, 35);
    tft.print("Left ");
    joystickHistory |= 2;
  }
  if (b == JOYSTICK_UP) {
    tft.setTextColor(ST7735_GREEN);
    tft.setCursor(0, 60);
    tft.print("Up");
    joystickHistory |= 4;
  }
  if (b == JOYSTICK_RIGHT) {
    tft.setTextColor(ST7735_BLUE);
    tft.setCursor(0, 85);
    tft.print("Right");
    joystickHistory |= 8;
  }
  if ((b == JOYSTICK_SELECT) && (joystickHistory == 0xF)) {
    tft.setTextColor(ST7735_MAGENTA);
    tft.setCursor(0, 110);
    tft.print("SELECT");
    joystickHistory |= 8;
    delay(2000);
    bmpDraw("parrot.bmp", 0, 0);
  }
  delay(100);
}

// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster.  20 pixels seems a
// good balance.

#if defined(ARDUINO_ARCH_AVR)
#define BUFFPIXEL 20
#elif defined(ARDUINO_ARCH_SAM)
#define BUFFPIXEL 85 // the most pixels you can buffer with an 8 bit index
// To buffer more pixels, change the type of buffidx to int or long
//#define BUFFPIXEL 128 // one entire row of pixels
#endif

void bmpDraw(char *filename, uint8_t x, uint8_t y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3 * BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();

  if ((x >= tft.width()) || (y >= tft.height())) return;

  Serial.println();
  Serial.print("Loading image '");
  Serial.print(filename);
  Serial.println('\'');

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.print("File not found");
    return;
  }

  // Parse BMP header
  if (read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.print("File size: "); Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print("Image Offset: "); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print("Header size: "); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if (read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print("Bit Depth: "); Serial.println(bmpDepth);
      if ((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print("Image size: ");
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if (bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if ((x + w - 1) >= tft.width())  w = tft.width()  - x;
        if ((y + h - 1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x + w - 1, y + h - 1);

        for (row = 0; row < h; row++) { // For each scanline...

          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if (flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if (bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col = 0; col < w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format, push to display
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            tft.pushColor(tft.Color565(r, g, b));
          } // end pixel
        } // end scanline
        Serial.print("Loaded in ");
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if (!goodBmp) Serial.println("BMP format not recognized.");
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}
