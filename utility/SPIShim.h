// SPI transaction shim functions.
//
// These functions start a SPI transaction using Arduino SPI transaction support
// if available.  On old platforms they instead just initialize the SPI bus with
// the desired clock speed, bit order, and data mode.
//
// General usage:
//  - Include SPIShim.h in the library code.
//  - Before a SPI transaction (series of SPI reads/writes) is made call the
//    spiStart function and pass it the following parameters:
//      - Desired SPI clock speed in hz, the actual SPI clock speed will be set
//        as close as possible without going over.
//      - Bit order, either LSBFIRST or MSBFIRST.
//      - SPI mode, one of SPI_MODE0, SPI_MODE1, SPI_MODE2, SPI_MODE3.
//  - After a SPI transaction is finished call spiEnd.
//  - That's it!  On newer IDEs the right thing will be done with built-in SPI
//    transaction support.  Older IDEs will work but not use transactions.
#ifndef _SPI_SHIM_H_
#define _SPI_SHIM_H_

#include "SPI.h"

inline void spiStart(uint32_t clockHz, uint8_t bitOrder, uint8_t dataMode) {
  #if defined(SPI_HAS_TRANSACTION)
    // Newer IDE, just use the Arduino SPI transaction support.
    SPI.beginTransaction(SPISettings(clockHz, bitOrder, dataMode));
  #else
    // No SPI transaction support, just set the SPI state as directed.
    // First set the SPI clock divider to get close to the desird clock speed
    // (without going over).
    uint8_t divider;
    #if defined(__SAM3X8E__)
      // On the Due set the clock divider with any value from 1 - 255.
      if (clockHz < (F_CPU / 255)) {
        divider = 255;
      } else if (clockHz >= F_CPU) {
        divider = 1;
      } else {
        divider = (F_CPU / (clockHz + 1)) + 1;  // Trick used to round up, same
                                                // as Arduino's SPI.h code.
      }
    #elif defined(__AVR__)
      // For AVR platform the SPI clock divider can only be set to a few power
      // of two values.
      // These if cases cascade from slowest to fastest clock speed.
      if (clockHz <= (F_CPU / 128)) {
        divider = SPI_CLOCK_DIV128;
      else if (clockHz <= (F_CPU / 64)) {
        divider = SPI_CLOCK_DIV64;
      }
      else if (clockHz <= (F_CPU / 32)) {
        divider = SPI_CLOCK_DIV32;
      }
      else if (clockHz <= (F_CPU / 16)) {
        divider = SPI_CLOCK_DIV16;
      }
      else if (clockHz <= (F_CPU / 8)) {
        divider = SPI_CLOCK_DIV8;
      }
      else if (clockHz <= (F_CPU / 4)) {
        divider = SPI_CLOCK_DIV4;
      }
      else {
        // Set the fastest SPI clock speed possible.
        divider = SPI_CLOCK_DIV2;
      }
    #else
      #error Unknown platform! Is the right board selected?  Also try upgrading to the latest Arduino IDE with native SPI transaction support.
    #endif
    SPI.setClockDivider(divider);
    SPI.setBitOrder(bitOrder);
    SPI.setDataMode(dataMode);
  #endif
}

inline void spiEnd() {
  #if defined(SPI_HAS_TRANSACTION)
    // Newer IDE, stop the SPI transaction.
    SPI.endTransaction();
  #endif
  // Old IDE doesn't need to do anything else.
}

#endif  // End header include guard.
