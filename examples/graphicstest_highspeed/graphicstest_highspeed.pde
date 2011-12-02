// Pins SCLK and MOSI are fixed in hardware, and pin 10 (or 53) 
// must be an output
//#define sclk 13    // for MEGAs use pin 52
//#define mosi 11    // for MEGAs use pin 51
#define cs 10   // for MEGAs you probably want this to be pin 53
#define dc 9
#define rst 8  // you can also connect this to the Arduino reset

// Color definitions
#define  BLACK           0x0000
#define	BLUE            0x001F
#define	RED             0xF800
#define	GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0  
#define WHITE           0xFFFF

#include <ST7735.h>
#include <SPI.h>

// Option 1: use any pins but a little slower
//ST7735 tft = ST7735(cs, dc, mosi, sclk, rst);  

// Option 2: must use the hardware SPI pins 
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be 
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
ST7735 tft = ST7735(cs, dc, rst);    

void fillpixelbypixel(uint16_t color) {
  for (uint8_t x=0; x < tft.width; x++) {
    for (uint8_t y=0; y < tft.height; y++) {
      tft.drawPixel(x, y, color);
    }
  }
  delay(100);
}

void setup(void) {
  Serial.begin(9600);
  Serial.print("hello!");
  tft.initR();               // initialize a ST7735R chip

  Serial.println("init");
  tft.writecommand(ST7735_DISPON);
  
  uint16_t time = millis();
  tft.fillScreen(BLACK);
  time = millis() - time;
  
  Serial.println(time, DEC);
  delay(500);
  
  //
  tft.fillScreen(BLACK);
  testdrawtext("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", WHITE);
  delay(1000);
  
  //a single pixel
  tft.drawPixel(tft.width/2, tft.height/2, GREEN);
  delay(500);
  
  // line draw test
  testlines(YELLOW);
  delay(500);    
  
  // optimized lines
  testfastlines(RED, BLUE);
  delay(500);    

  testdrawrects(GREEN);
  delay(500);

  testfillrects(YELLOW, MAGENTA);
  delay(500);

  tft.fillScreen(BLACK);
  testfillcircles(10, BLUE);
  testdrawcircles(10, WHITE);
  delay(500);
  
  testroundrects();
  delay(500);
  
  testtriangles();
  delay(500);
  
  mediabuttons();
  delay(500);
  
  Serial.println("done");
  delay(1000);
}

void loop() {
  tft.writecommand(ST7735_INVON);
  delay(500);
  tft.writecommand(ST7735_INVOFF);
  delay(500);
}

void testlines(uint16_t color) {
   tft.fillScreen(BLACK);
   for (uint16_t x=0; x < tft.width; x+=6) {
     tft.drawLine(0, 0, x, tft.height-1, color);
   }
   for (uint16_t y=0; y < tft.height; y+=6) {
     tft.drawLine(0, 0, tft.width-1, y, color);
   }
   
   tft.fillScreen(BLACK);
   for (uint16_t x=0; x < tft.width; x+=6) {
     tft.drawLine(tft.width-1, 0, x, tft.height-1, color);
   }
   for (uint16_t y=0; y < tft.height; y+=6) {
     tft.drawLine(tft.width-1, 0, 0, y, color);
   }
   
   tft.fillScreen(BLACK);
   for (uint16_t x=0; x < tft.width; x+=6) {
     tft.drawLine(0, tft.height-1, x, 0, color);
   }
   for (uint16_t y=0; y < tft.height; y+=6) {
     tft.drawLine(0, tft.height-1, tft.width-1, y, color);
   }

   tft.fillScreen(BLACK);
   for (uint16_t x=0; x < tft.width; x+=6) {
     tft.drawLine(tft.width-1, tft.height-1, x, 0, color);
   }
   for (uint16_t y=0; y < tft.height; y+=6) {
     tft.drawLine(tft.width-1, tft.height-1, 0, y, color);
   }
   
}

void testdrawtext(char *text, uint16_t color) {
  tft.drawString(0, 0, text, color);
}

void testfastlines(uint16_t color1, uint16_t color2) {
   tft.fillScreen(BLACK);
   for (uint16_t y=0; y < tft.height; y+=5) {
     tft.drawHorizontalLine(0, y, tft.width, color1);
   }
   for (uint16_t x=0; x < tft.width; x+=5) {
     tft.drawVerticalLine(x, 0, tft.height, color2);
   }
}

void testdrawrects(uint16_t color) {
 tft.fillScreen(BLACK);
 for (uint16_t x=0; x < tft.width; x+=6) {
   tft.drawRect(tft.width/2 -x/2, tft.height/2 -x/2 , x, x, color);
 }
}

void testfillrects(uint16_t color1, uint16_t color2) {
 tft.fillScreen(BLACK);
 for (uint16_t x=tft.width-1; x > 6; x-=6) {
   tft.fillRect(tft.width/2 -x/2, tft.height/2 -x/2 , x, x, color1);
   tft.drawRect(tft.width/2 -x/2, tft.height/2 -x/2 , x, x, color2);
 }
}



void testfillcircles(uint8_t radius, uint16_t color) {
  for (uint8_t x=radius; x < tft.width; x+=radius*2) {
    for (uint8_t y=radius; y < tft.height; y+=radius*2) {
      tft.fillCircle(x, y, radius, color);
    }
  }  
}

void testdrawcircles(uint8_t radius, uint16_t color) {
  for (uint8_t x=0; x < tft.width+radius; x+=radius*2) {
    for (uint8_t y=0; y < tft.height+radius; y+=radius*2) {
      tft.drawCircle(x, y, radius, color);
    }
  }  
}

void testtriangles() {
  tft.fillScreen(BLACK);
  int color = 0xF800;
  int t;
  int w = 63;
  int x = 159;
  int y = 0;
  int z = 127;
  for(t = 0 ; t <= 15; t+=1) {
    tft.drawTriangle(w, y, y, x, z, x, color);
    x-=4;
    y+=4;
    z-=4;
    color+=100;
  }
}

void testroundrects() {
  tft.fillScreen(BLACK);
  int color = 100;
  int i;
  int t;
  for(t = 0 ; t <= 4; t+=1) {
  int x = 0;
  int y = 0;
  int w = 127;
  int h = 159;
    for(i = 0 ; i <= 24; i+=1) {
    tft.drawRoundRect(x, y, w, h, 5, color);
    x+=2;
    y+=3;
    w-=4;
    h-=6;
    color+=1100;
  }
  color+=100;
  }
}

void mediabuttons() {
 // play
  tft.fillScreen(BLACK);
  tft.fillRoundRect(25, 10, 78, 60, 8, WHITE);
  tft.fillTriangle(42, 20, 42, 60, 90, 40, BLACK);
  delay(500);
  // pause
  tft.fillRoundRect(25, 90, 78, 60, 8, WHITE);
  tft.fillRoundRect(39, 98, 20, 45, 5, BLACK);
  tft.fillRoundRect(69, 98, 20, 45, 5, BLACK);
  delay(500);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, GREEN);
  // pause color
  tft.fillRoundRect(39, 98, 20, 45, 5, RED);
  tft.fillRoundRect(69, 98, 20, 45, 5, RED);
  delay(500);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, BLUE);
}