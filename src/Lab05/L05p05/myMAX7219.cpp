#include <Arduino.h>
#include <SPI.h>
#include "myMAX7219.h"

void MAX7219_write_reg(uint8_t addr, uint8_t data) {
  digitalWrite( CS_PIN, LOW );
  SPI.transfer( addr );
  SPI.transfer( data );
  digitalWrite( CS_PIN, HIGH );
}

void MAX7219_init(void) {
  MAX7219_write_reg( REG_DECODE_MODE, 0x00 );  // decode mode: no decode for digits 0-7
  MAX7219_write_reg( REG_INTENSITY, 0x07 );    // set intensity: 0x07=15/32
  MAX7219_write_reg( REG_SCAN_LIMIT, 0x07 );   // scan limit: display digits 0-7
  MAX7219_write_reg( REG_SHUTDOWN, 0x01 );     // shutdown: normal operation
  MAX7219_write_reg( REG_DISPLAY_TEST, 0x00 ); // display test: no display test
}

void MAX7219_test(void) {
  MAX7219_write_reg( REG_SHUTDOWN, 0x01 );     // normal operation
  MAX7219_write_reg( REG_DISPLAY_TEST, 0x01 ); // enter display test mode
  delay(100);
  MAX7219_write_reg( REG_DISPLAY_TEST, 0x00 ); // exit display test mode
  MAX7219_write_reg( REG_SHUTDOWN, 0x00 );     // shutdown operation
  delay(900);
}
