// Platform-specific configuration.
// Ideally all the #ifdef, etc. magic specific to a board goes here so that
// standard Arduino & AVR functions are available.
#ifndef _PLATFORM_SHIM_H_
#define _PLATFORM_SHIM_H_

// Old Arduino IDE shim.
#if ARDUINO >= 100
  #include "Arduino.h"
  #include "Print.h"
#else
  // Legacy Arduino IDE uses different header.
  #include "WProgram.h"
#endif

// pgmspace.h (AVR flash memory) shim.
#if defined(__SAM3X8E__)
  //
  // Arduino Due
  //
  // Work-around lack of pgmspace functions with the Due.
  #include <include/pio.h>
  #define PROGMEM
  #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
  #define pgm_read_word(addr) (*(const unsigned short *)(addr))
  typedef unsigned char prog_uchar;
  #undef __FlashStringHelper::F(string_literal)
  #define F(string_literal) string_literal
#elif defined(ESP8266)
  //
  // ESP8266
  //
  // pgmspace functions are in a different path on ESP8266 vs. AVRs.
  #include <pgmspace.h>
#elif defined(__AVR__)
  //
  // AVR processors (Uno, Leonard, Mega)
  //
  #include <avr/pgmspace.h>
#endif

// Port size and pin type shim.
// Define the size of the types that can store pin number values:
//  - PortSizeType: size of a GPIO port on the board.
//  - PinType: type that can store a GPIO pin number.
//  - OptionalPinType: type that can store an optional GPIO pin number.  Allows
//                     the type to store a -1 value to represent unused.
// Note that there's a normal and optional version.  The optional version is
// signed so it can be set to -1 to represent an unused pin.
#if defined(__AVR__) || defined(CORE_TEENSY)
  // AVR and Teensy 2/3 platforms.
  typedef uint8_t PortSizeType;
  typedef uint8_t PinType;
  typedef int8_t OptionalPinType;
#elif defined(__arm__)
  // Assume 32-bit types on all ARM platforms like Due, Zero, etc.
  typedef RwReg PortSizeType;
  typedef uint32_t PinType;
  typedef int32_t OptionalPinType;
#else
  #error Unknown platform.  Make sure the right board type is selected!
#endif


#endif  // End header include guard.
