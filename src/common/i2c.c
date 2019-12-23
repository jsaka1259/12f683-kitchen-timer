#include "i2c.h"

void i2c_init(void) {
  SCL = 1;
  SDA = 1;
  TRISSDA = 0;
}

void i2c_start(void) {
  SDA = 1;
  TRISSDA = 0;
  SCL = 1;
  __delay_us(3);
  SDA = 0;
}

void i2c_stop(void) {
  SCL = 0;
  __delay_us(1);
  SDA = 0;
  TRISSDA = 0;
  SCL = 1;
  __delay_us(1);
  SDA = 1;
  __delay_us(10);
}

uint8_t i2c_write(uint8_t data) {
  uint8_t i;
  uint8_t ack;
  uint8_t bit_pos;

  TRISSDA = 0;
  bit_pos = 0x80;
  for(i = 0; i < 8; i++) {
    SCL = 0;
    if((data & bit_pos) != 0)
      SDA = 1;
    else
      SDA = 0;
    bit_pos = bit_pos >> 1;
    SCL = 1;
  }

  SCL = 0;
  TRISSDA = 1;
  __delay_us(2);
  SCL = 1;
  __delay_us(2);
  ack = SDA;
  return ack;
}

uint8_t i2c_read(uint8_t ack) {
  uint8_t i;
  uint8_t data;
  uint8_t bit_pos;

  data = 0;
  bit_pos = 0x80;
  for(i = 0; i < 8; i++) {
    SCL = 0;
    TRISSDA = 1;
    __delay_us(3);
    SCL = 1;
    if(SDA)
      data |= bit_pos;
    bit_pos = bit_pos >> 1;
  }

  SCL = 0;
  SDA = ack;
  TRISSDA = 0;
  __delay_us(2);
  SCL = 1;
  return data;
}
