// Graphics library by ladyada/adafruit with init code from Rossum 
// MIT license

#include "ST7735.h"
#include "glcdfont.c"
#include <avr/pgmspace.h>
#include "pins_arduino.h"
#include "wiring_private.h"
#include <SPI.h>

ST7735::ST7735(uint8_t cs, uint8_t rs, uint8_t sid, 
  uint8_t sclk, uint8_t rst) {
  _cs = cs;
  _rs = rs;
  _sid = sid;
  _sclk = sclk;
  _rst = rst;
}

ST7735::ST7735(uint8_t cs, uint8_t rs,  uint8_t rst) {
  _cs = cs;
  _rs = rs;
  _sid = 0;
  _sclk = 0;
  _rst = rst;
}


inline void ST7735::spiwrite(uint8_t c) {

  //Serial.println(c, HEX);

  if (!_sid) {
    SPI.transfer(c);
    return;
  }

  volatile uint8_t *sclkportreg = portOutputRegister(sclkport);
  volatile uint8_t *sidportreg = portOutputRegister(sidport);

  int8_t i;

  *sclkportreg |= sclkpin;

  for (i=7; i>=0; i--) {
    *sclkportreg &= ~sclkpin;
    //SCLK_PORT &= ~_BV(SCLK);
    
    if (c & _BV(i)) {
      *sidportreg |= sidpin;
      //digitalWrite(_sid, HIGH);
      //SID_PORT |= _BV(SID);
    } else {
      *sidportreg &= ~sidpin;
      //digitalWrite(_sid, LOW);
      //SID_PORT &= ~_BV(SID);
    }
    
    *sclkportreg |= sclkpin;
    //SCLK_PORT |= _BV(SCLK);
  }
}


void ST7735::writecommand(uint8_t c) {
  *portOutputRegister(rsport) &= ~ rspin;
  //digitalWrite(_rs, LOW);

  *portOutputRegister(csport) &= ~ cspin;
  //digitalWrite(_cs, LOW);

  //Serial.print("C ");
  spiwrite(c);

  *portOutputRegister(csport) |= cspin;
  //digitalWrite(_cs, HIGH);
}


void ST7735::writedata(uint8_t c) {
  *portOutputRegister(rsport) |= rspin;
  //digitalWrite(_rs, HIGH);

  *portOutputRegister(csport) &= ~ cspin;
    //digitalWrite(_cs, LOW);

  //Serial.print("D ");
  spiwrite(c);

  *portOutputRegister(csport) |= cspin;
  //digitalWrite(_cs, HIGH);
} 


void ST7735::setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
  writecommand(ST7735_CASET);  // column addr set
  writedata(0x00);
  writedata(x0+2);   // XSTART 
  writedata(0x00);
  writedata(x1+2);   // XEND

  writecommand(ST7735_RASET);  // row addr set
  writedata(0x00);
  writedata(y0+1);    // YSTART
  writedata(0x00);
  writedata(y1+1);    // YEND

  writecommand(ST7735_RAMWR);  // write to RAM
}

void ST7735::pushColor(uint16_t color) {
  *portOutputRegister(rsport) |= rspin;
  *portOutputRegister(csport) &= ~ cspin;

  spiwrite(color >> 8);    
  spiwrite(color);   

  *portOutputRegister(csport) |= cspin;
}

void ST7735::drawPixel(uint8_t x, uint8_t y,uint16_t color) {
  if ((x >= width) || (y >= height)) return;

  setAddrWindow(x,y,x+1,y+1);

  // setup for data
  *portOutputRegister(rsport) |= rspin;
  *portOutputRegister(csport) &= ~ cspin;

  spiwrite(color >> 8);    
  spiwrite(color);   

  *portOutputRegister(csport) |= cspin;

}


void ST7735::fillScreen(uint16_t color) {
  setAddrWindow(0, 0, width-1, height-1);

  // setup for data
  *portOutputRegister(rsport) |= rspin;
  *portOutputRegister(csport) &= ~ cspin;

  for (uint8_t x=0; x < width; x++) {
    for (uint8_t y=0; y < height; y++) {
      spiwrite(color >> 8);    
      spiwrite(color);    
    }
  }

  *portOutputRegister(csport) |= cspin;
}

void ST7735::initB(void) {
  // set pin directions
  pinMode(_rs, OUTPUT);

  if (_sclk) {
    pinMode(_sclk, OUTPUT);
    sclkport = digitalPinToPort(_sclk);
    sclkpin = digitalPinToBitMask(_sclk);

    pinMode(_sid, OUTPUT);
    sidport = digitalPinToPort(_sid);
    sidpin = digitalPinToBitMask(_sid);
  } else {
    // using the hardware SPI
    SPI.begin();
    SPI.setDataMode(SPI_MODE3);
  }
  // toggle RST low to reset; CS low so it'll listen to us
  pinMode(_cs, OUTPUT);
  digitalWrite(_cs, LOW);
  cspin = digitalPinToBitMask(_cs);
  csport = digitalPinToPort(_cs);

  rspin = digitalPinToBitMask(_rs);
  rsport = digitalPinToPort(_rs);

  if (_rst) {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(500);
    digitalWrite(_rst, LOW);
    delay(500);
    digitalWrite(_rst, HIGH);
    delay(500);
  }

  writecommand(ST7735_SWRESET); // software reset
  delay(50);
  writecommand(ST7735_SLPOUT);  // out of sleep mode
  delay(500);
  
  writecommand(ST7735_COLMOD);  // set color mode
  writedata(0x05);        // 16-bit color
  delay(10);
  
  writecommand(ST7735_FRMCTR1);  // frame rate control
  writedata(0x00);  // fastest refresh
  writedata(0x06);  // 6 lines front porch
  writedata(0x03);  // 3 lines backporch
  delay(10);
  
  writecommand(ST7735_MADCTL);  // memory access control (directions)
  writedata(0x08);  // row address/col address, bottom to top refresh
  madctl = 0x08;

  writecommand(ST7735_DISSET5);  // display settings #5
  writedata(0x15);  // 1 clock cycle nonoverlap, 2 cycle gate rise, 3 cycle oscil. equalize
  writedata(0x02);  // fix on VTL
 
  writecommand(ST7735_INVCTR);  // display inversion control
  writedata(0x0);  // line inversion
 
  writecommand(ST7735_PWCTR1);  // power control
  writedata(0x02);      // GVDD = 4.7V 
  writedata(0x70);      // 1.0uA
  delay(10);
  writecommand(ST7735_PWCTR2);  // power control
  writedata(0x05);      // VGH = 14.7V, VGL = -7.35V 
  writecommand(ST7735_PWCTR3);  // power control
  writedata(0x01);      // Opamp current small 
  writedata(0x02);      // Boost frequency
  
  
  writecommand(ST7735_VMCTR1);  // power control
  writedata(0x3C);      // VCOMH = 4V
  writedata(0x38);      // VCOML = -1.1V
  delay(10);
  
  writecommand(ST7735_PWCTR6);  // power control
  writedata(0x11); 
  writedata(0x15);
  
  writecommand(ST7735_GMCTRP1);
  writedata(0x09);
  writedata(0x16);
  writedata(0x09);
  writedata(0x20);
  writedata(0x21);
  writedata(0x1B);
  writedata(0x13);
  writedata(0x19);
  writedata(0x17);
  writedata(0x15);
  writedata(0x1E);
  writedata(0x2B);
  writedata(0x04);
  writedata(0x05);
  writedata(0x02);
  writedata(0x0E);
  writecommand(ST7735_GMCTRN1);
  writedata(0x0B); 
  writedata(0x14); 
  writedata(0x08); 
  writedata(0x1E); 
  writedata(0x22); 
  writedata(0x1D); 
  writedata(0x18); 
  writedata(0x1E); 
  writedata(0x1B); 
  writedata(0x1A); 
  writedata(0x24); 
  writedata(0x2B); 
  writedata(0x06); 
  writedata(0x06); 
  writedata(0x02); 
  writedata(0x0F); 
  delay(10);
  
  writecommand(ST7735_CASET);  // column addr set
  writedata(0x00);
  writedata(0x02);   // XSTART = 2
  writedata(0x00);
  writedata(0x81);   // XEND = 129

  writecommand(ST7735_RASET);  // row addr set
  writedata(0x00);
  writedata(0x02);    // XSTART = 1
  writedata(0x00);
  writedata(0x81);    // XEND = 160

  writecommand(ST7735_NORON);  // normal display on
  delay(10);
  
  writecommand(ST7735_DISPON);
  delay(500);
}



void ST7735::initR(void) {
  // set pin directions
  pinMode(_rs, OUTPUT);

  if (_sclk) {
    pinMode(_sclk, OUTPUT);
    sclkport = digitalPinToPort(_sclk);
    sclkpin = digitalPinToBitMask(_sclk);

    pinMode(_sid, OUTPUT);
    sidport = digitalPinToPort(_sid);
    sidpin = digitalPinToBitMask(_sid);
  } else {
    // using the hardware SPI
    SPI.begin();
    SPI.setDataMode(SPI_MODE3);
  }
  // toggle RST low to reset; CS low so it'll listen to us
  pinMode(_cs, OUTPUT);
  digitalWrite(_cs, LOW);
  cspin = digitalPinToBitMask(_cs);
  csport = digitalPinToPort(_cs);

  rspin = digitalPinToBitMask(_rs);
  rsport = digitalPinToPort(_rs);

  if (_rst) {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(500);
    digitalWrite(_rst, LOW);
    delay(500);
    digitalWrite(_rst, HIGH);
    delay(500);
  }

  writecommand(ST7735_SWRESET); // software reset
  delay(150);

  writecommand(ST7735_SLPOUT);  // out of sleep mode
  delay(500);

  writecommand(ST7735_FRMCTR1);  // frame rate control - normal mode
  writedata(0x01);  // frame rate = fosc / (1 x 2 + 40) * (LINE + 2C + 2D)
  writedata(0x2C); 
  writedata(0x2D); 

  writecommand(ST7735_FRMCTR2);  // frame rate control - idle mode
  writedata(0x01);  // frame rate = fosc / (1 x 2 + 40) * (LINE + 2C + 2D)
  writedata(0x2C); 
  writedata(0x2D); 

  writecommand(ST7735_FRMCTR3);  // frame rate control - partial mode
  writedata(0x01); // dot inversion mode
  writedata(0x2C); 
  writedata(0x2D); 
  writedata(0x01); // line inversion mode
  writedata(0x2C); 
  writedata(0x2D); 
  
  writecommand(ST7735_INVCTR);  // display inversion control
  writedata(0x07);  // no inversion

  writecommand(ST7735_PWCTR1);  // power control
  writedata(0xA2);      
  writedata(0x02);      // -4.6V
  writedata(0x84);      // AUTO mode

  writecommand(ST7735_PWCTR2);  // power control
  writedata(0xC5);      // VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD

  writecommand(ST7735_PWCTR3);  // power control
  writedata(0x0A);      // Opamp current small 
  writedata(0x00);      // Boost frequency

  writecommand(ST7735_PWCTR4);  // power control
  writedata(0x8A);      // BCLK/2, Opamp current small & Medium low
  writedata(0x2A);     

  writecommand(ST7735_PWCTR5);  // power control
  writedata(0x8A);    
  writedata(0xEE);     

  writecommand(ST7735_VMCTR1);  // power control
  writedata(0x0E);  

  writecommand(ST7735_INVOFF);    // don't invert display

  writecommand(ST7735_MADCTL);  // memory access control (directions)
  writedata(0xC8);  // row address/col address, bottom to top refresh
  madctl = 0xC8;
  
  writecommand(ST7735_COLMOD);  // set color mode
  writedata(0x05);        // 16-bit color

  writecommand(ST7735_CASET);  // column addr set
  writedata(0x00);
  writedata(0x00);   // XSTART = 0
  writedata(0x00);
  writedata(0x7F);   // XEND = 127

  writecommand(ST7735_RASET);  // row addr set
  writedata(0x00);
  writedata(0x00);    // XSTART = 0
  writedata(0x00);
  writedata(0x9F);    // XEND = 159

  writecommand(ST7735_GMCTRP1);
  writedata(0x02);
  writedata(0x1c);
  writedata(0x07);
  writedata(0x12);
  writedata(0x37);
  writedata(0x32);
  writedata(0x29);
  writedata(0x2d);
  writedata(0x29);
  writedata(0x25);
  writedata(0x2B);
  writedata(0x39);
  writedata(0x00);
  writedata(0x01);
  writedata(0x03);
  writedata(0x10);
  writecommand(ST7735_GMCTRN1);
  writedata(0x03); 
  writedata(0x1d); 
  writedata(0x07); 
  writedata(0x06); 
  writedata(0x2E); 
  writedata(0x2C); 
  writedata(0x29); 
  writedata(0x2D); 
  writedata(0x2E); 
  writedata(0x2E); 
  writedata(0x37); 
  writedata(0x3F); 
  writedata(0x00); 
  writedata(0x00); 
  writedata(0x02); 
  writedata(0x10); 
  
  writecommand(ST7735_DISPON);
  delay(100);

  writecommand(ST7735_NORON);  // normal display on
  delay(10);
}


// draw a string from memory

void ST7735::drawString(uint8_t x, uint8_t y, char *c, 
			uint16_t color, uint8_t size) {
  while (c[0] != 0) {
    drawChar(x, y, c[0], color, size);
    x += size*6;
    c++;
    if (x + 5 >= width) {
      y += 10;
      x = 0;
    }
  }
}
// draw a character
void ST7735::drawChar(uint8_t x, uint8_t y, char c, 
		      uint16_t color, uint8_t size) {
  for (uint8_t i =0; i<5; i++ ) {
    uint8_t line = pgm_read_byte(font+(c*5)+i);
    for (uint8_t j = 0; j<8; j++) {
      if (line & 0x1) {
	if (size == 1) // default size
	  drawPixel(x+i, y+j, color);
	else {  // big size
	  fillRect(x+i*size, y+j*size, size, size, color);
	} 
      }
      line >>= 1;
    }
  }
}

// fill a circle
void ST7735::fillCircle(uint8_t x0, uint8_t y0, uint8_t r, uint16_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  drawVerticalLine(x0, y0-r, 2*r+1, color);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
  
    drawVerticalLine(x0+x, y0-y, 2*y+1, color);
    drawVerticalLine(x0-x, y0-y, 2*y+1, color);
    drawVerticalLine(x0+y, y0-x, 2*x+1, color);
    drawVerticalLine(x0-y, y0-x, 2*x+1, color);
  }
}

// draw a circle outline
void ST7735::drawCircle(uint8_t x0, uint8_t y0, uint8_t r, 
			uint16_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  drawPixel(x0, y0+r, color);
  drawPixel(x0, y0-r, color);
  drawPixel(x0+r, y0, color);
  drawPixel(x0-r, y0, color);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
  
    drawPixel(x0 + x, y0 + y, color);
    drawPixel(x0 - x, y0 + y, color);
    drawPixel(x0 + x, y0 - y, color);
    drawPixel(x0 - x, y0 - y, color);
    
    drawPixel(x0 + y, y0 + x, color);
    drawPixel(x0 - y, y0 + x, color);
    drawPixel(x0 + y, y0 - x, color);
    drawPixel(x0 - y, y0 - x, color);
    
  }
}

uint8_t ST7735::getRotation() {
  return madctl;
}

void ST7735::setRotation(uint8_t m) {
  madctl = m;
  writecommand(ST7735_MADCTL);  // memory access control (directions)
  writedata(madctl);  // row address/col address, bottom to top refresh
}

// draw a rectangle
void ST7735::drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, 
		      uint16_t color) {
  // smarter version
  drawHorizontalLine(x, y, w, color);
  drawHorizontalLine(x, y+h-1, w, color);
  drawVerticalLine(x, y, h, color);
  drawVerticalLine(x+w-1, y, h, color);
}

void ST7735::fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, 
		      uint16_t color) {
  // smarter version

  setAddrWindow(x, y, x+w, y+h);

  // setup for data
  digitalWrite(_rs, HIGH);
  digitalWrite(_cs, LOW);

  for (x=0; x < w; x++) {
    for (y=0; y < h; y++) {
      spiwrite(color >> 8);    
      spiwrite(color);    
    }
  }
  digitalWrite(_cs, HIGH);
}

void ST7735::drawVerticalLine(uint8_t x, uint8_t y, uint8_t length, uint16_t color)
{
  if (x >= width) return;
  if (y+length >= height) length = height-y-1;

  drawFastLine(x,y,length,color,1);
}

void ST7735::drawHorizontalLine(uint8_t x, uint8_t y, uint8_t length, uint16_t color)
{
  if (y >= height) return;
  if (x+length >= width) length = width-x-1;

  drawFastLine(x,y,length,color,0);
}

void ST7735::drawFastLine(uint8_t x, uint8_t y, uint8_t length, 
			  uint16_t color, uint8_t rotflag)
{
  if (rotflag) {
    setAddrWindow(x, y, x, y+length);
  } else {
    setAddrWindow(x, y, x+length, y+1);
  }
  // setup for data
  digitalWrite(_rs, HIGH);
  digitalWrite(_cs, LOW);

  while (length--) {
    spiwrite(color >> 8);    
    spiwrite(color);    
  }
  digitalWrite(_cs, HIGH);
}


// bresenham's algorithm - thx wikpedia
void ST7735::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, 
		      uint16_t color) {
  uint16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  uint16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;}

  for (; x0<=x1; x0++) {
    if (steep) {
      drawPixel(y0, x0, color);
    } else {
      drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}


//////////
/*
uint8_t ST7735::spiread(void) {
  uint8_t r = 0;
  if (_sid > 0) {
    r = shiftIn(_sid, _sclk, MSBFIRST);
  } else {
    //SID_DDR &= ~_BV(SID);
    //int8_t i;
    //for (i=7; i>=0; i--) {
    //  SCLK_PORT &= ~_BV(SCLK);
    //  r <<= 1;
    //  r |= (SID_PIN >> SID) & 0x1;
    //  SCLK_PORT |= _BV(SCLK);
    //}
    //SID_DDR |= _BV(SID);
    
  }
  return r;
}



void ST7735::dummyclock(void) {

  if (_sid > 0) {
    digitalWrite(_sclk, LOW);
    digitalWrite(_sclk, HIGH);
  } else {
    // SCLK_PORT &= ~_BV(SCLK);
    //SCLK_PORT |= _BV(SCLK);
  }
}
uint8_t ST7735::readdata(void) {
  *portOutputRegister(rsport) |= rspin;

  *portOutputRegister(csport) &= ~ cspin;

  uint8_t r = spiread();

  *portOutputRegister(csport) |= cspin;

  return r;

} 

uint8_t ST7735::readcommand8(uint8_t c) {
  digitalWrite(_rs, LOW);

  *portOutputRegister(csport) &= ~ cspin;

  spiwrite(c);

  digitalWrite(_rs, HIGH);
  pinMode(_sid, INPUT); // input!
  digitalWrite(_sid, LOW); // low
  spiread();
  uint8_t r = spiread();


  *portOutputRegister(csport) |= cspin;


  pinMode(_sid, OUTPUT); // back to output
  return r;
}


uint16_t ST7735::readcommand16(uint8_t c) {
  digitalWrite(_rs, LOW);
  if (_cs)
    digitalWrite(_cs, LOW);

  spiwrite(c);
  pinMode(_sid, INPUT); // input!
  uint16_t r = spiread();
  r <<= 8;
  r |= spiread();
  if (_cs)
    digitalWrite(_cs, HIGH);

  pinMode(_sid, OUTPUT); // back to output
  return r;
}

uint32_t ST7735::readcommand32(uint8_t c) {
  digitalWrite(_rs, LOW);
  if (_cs)
    digitalWrite(_cs, LOW);
  spiwrite(c);
  pinMode(_sid, INPUT); // input!

  dummyclock();
  dummyclock();

  uint32_t r = spiread();
  r <<= 8;
  r |= spiread();
  r <<= 8;
  r |= spiread();
  r <<= 8;
  r |= spiread();
  if (_cs)
    digitalWrite(_cs, HIGH);

  pinMode(_sid, OUTPUT); // back to output
  return r;
}

*/
