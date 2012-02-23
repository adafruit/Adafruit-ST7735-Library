/*************************************************** 
  This is an example sketch for the Adafruit 1.8" SPI display.
  This library works with the Adafruit 1.8" TFT Breakout w/SD card  
  ----> http://www.adafruit.com/products/358  
  as well as Adafruit raw 1.8" TFT display  
  ----> http://www.adafruit.com/products/618
 
  Check out the links above for our tutorials and wiring diagrams 
  These displays use SPI to communicate, 4 or 5 pins are required to  
  interface (RST is optional) 
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include <Adafruit_ST7735.h>
#include <SD.h>
#include <SPI.h>

// If we are using the hardware SPI interface, these are the pins (for future ref)
#define sclk 13
#define mosi 11

// You can also just connect the reset pin to +5V (we do a software reset)
#define rst 8

// these pins are required
#define cs 10
#define dc 9

// Color definitions
#define	BLACK           0x0000
#define	BLUE            0x001F
#define	RED             0xF800
#define	GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0  
#define WHITE           0xFFFF

// to draw images from the SD card, we will share the hardware SPI interface
Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);

// For Arduino Uno/Duemilanove, etc
//  connect the SD card with MOSI going to pin 11, MISO going to pin 12 and SCK going to pin 13 (standard)
//  Then pin 4 goes to CS (or whatever you have set up)
#define SD_CS 4    // Set the chip select line to whatever you use (4 doesnt conflict with the library)

// the file itself
File bmpFile;

// information we extract about the bitmap file
int bmpWidth, bmpHeight;
uint8_t bmpDepth, bmpImageoffset;

void setup(void) {
  Serial.begin(9600);
   
  pinMode(cs, OUTPUT);
  digitalWrite(cs, HIGH);
     
  // Our supplier changed the 1.8" display slightly after Jan 10, 2012
  // so that the alignment of the TFT had to be shifted by a few pixels
  // this just means the init code is slightly different. Check the
  // color of the tab to see which init code to try. If the display is
  // cut off or has extra 'random' pixels on the top & left, try the
  // other option!

  // If your TFT's plastic wrap has a Green Tab, use the following
  tft.initR(INITR_GREENTAB);               // initialize a ST7735R chip
  // If your TFT's plastic wrap has a Red Tab, use the following
  // since the display is shifted a little in memory
  //tft.initR(INITR_REDTAB);               // initialize a ST7735R chip

  // Just do a simple test
  tft.writecommand(ST7735_DISPON);
  
  Serial.print("Initializing SD card...");

  if (!SD.begin(SD_CS)) {
    Serial.println("failed!");
    return;
  }
  Serial.println("SD OK!");
  
  bmpFile = SD.open("parrot.bmp");

  if (! bmpFile) {
    Serial.println("didnt find image");
    while (1);
  }
  
  if (! bmpReadHeader(bmpFile)) { 
     Serial.println("bad bmp");
     bmpFile.close();
     return;
  }
  
  Serial.print("image size "); 
  Serial.print(bmpWidth, DEC);
  Serial.print(", ");
  Serial.println(bmpHeight, DEC);
  

  bmpdraw(bmpFile, 0, 0);
  bmpFile.close();
}

void loop() {
}


void testfastlines(uint16_t color1, uint16_t color2) {
   tft.fillScreen(BLACK);
   for (uint16_t y=0; y < tft.height(); y+=5) {
     tft.drawHorizontalLine(0, y, tft.width(), color1);
   }
   for (uint16_t x=0; x < tft.width(); x+=5) {
     tft.drawVerticalLine(x, 0, tft.height(), color2);
   }
}

/*********************************************/
// This procedure reads a bitmap and draws it to the screen
// its sped up by reading many pixels worth of data at a time
// instead of just one pixel at a time. increading the buffer takes
// more RAM but makes the drawing a little faster. 20 pixels' worth
// is probably a good place

#define BUFFPIXEL 20

void bmpdraw(File f, int x, int y) {
  bmpFile.seek(bmpImageoffset);
  
  uint32_t time = millis();
  uint16_t p; 
  uint8_t g, b;
  int i, j;
  
  uint8_t sdbuffer[3 * BUFFPIXEL];  // 3 * pixels to buffer
  uint8_t buffidx = 3*BUFFPIXEL;
  
  //Serial.print("rotation = "); Serial.println(tft.getRotation(), DEC);
  
  //set up the 'display window'
  tft.setAddrWindow(x, y, x+bmpWidth-1, y+bmpHeight-1);
  
  uint8_t rotback = tft.getRotation();
  //tft.setRotation();
  
  for (i=0; i< bmpHeight; i++) {
    // bitmaps are stored with the BOTTOM line first so we have to move 'up'
  
    for (j=0; j<bmpWidth; j++) {
      // read more pixels
      if (buffidx >= 3*BUFFPIXEL) {
        bmpFile.read(sdbuffer, 3*BUFFPIXEL);
        buffidx = 0;
      }
      
      // convert pixel from 888 to 565
      b = sdbuffer[buffidx++];     // blue
      g = sdbuffer[buffidx++];     // green
      p = sdbuffer[buffidx++];     // red
      
      p >>= 3;
      p <<= 6;
      
      g >>= 2;
      p |= g;
      p <<= 5;
      
      b >>= 3;
      p |= b;
     //Serial.print(p, HEX);
      // write out the 16 bits of color
      //tft.drawPixel(i, j, p);
      tft.pushColor(p);
    }
  }
  Serial.print(millis() - time, DEC);
  Serial.println(" ms");
}

boolean bmpReadHeader(File f) {
   // read header
  uint32_t tmp;
  
  if (read16(f) != 0x4D42) {
    // magic bytes missing
    return false;
  }
 
  // read file size
  tmp = read32(f);  
  Serial.print("size 0x"); Serial.println(tmp, HEX);
  
  // read and ignore creator bytes
  read32(f);
  
  bmpImageoffset = read32(f);  
  Serial.print("offset "); Serial.println(bmpImageoffset, DEC);
  
  // read DIB header
  tmp = read32(f);
  Serial.print("header size "); Serial.println(tmp, DEC);
  bmpWidth = read32(f);
  bmpHeight = read32(f);

  
  if (read16(f) != 1)
    return false;
    
  bmpDepth = read16(f);
  Serial.print("bitdepth "); Serial.println(bmpDepth, DEC);

  if (read32(f) != 0) {
    // compression not supported!
    return false;
  }
  
  Serial.print("compression "); Serial.println(tmp, DEC);

  return true;
}

/*********************************************/

// These read data from the SD card file and convert them to big endian 
// (the data is stored in little endian format!)

// LITTLE ENDIAN!
uint16_t read16(File f) {
  uint16_t d;
  uint8_t b;
  b = f.read();
  d = f.read();
  d <<= 8;
  d |= b;
  return d;
}


// LITTLE ENDIAN!
uint32_t read32(File f) {
  uint32_t d;
  uint16_t b;
 
  b = read16(f);
  d = read16(f);
  d <<= 16;
  d |= b;
  return d;
}


