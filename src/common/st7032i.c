#include "st7032i.h"

static void delay_100ms(uint16_t time) {
  time *= 4;

  while(time) {
    __delay_ms(25);
    time--;
  }
}

void st7032i_cmd(uint8_t cmd) {
  // I2C Start Condition
  i2c_start();
  // ST7032I Device Address
  i2c_write(ST7032I_I2C_ADDR);
  // Set Command Mode
  i2c_write(0x00);
  // Output Command
  i2c_write(cmd);
  // I2C Stop Condition
  i2c_stop();
  /* Clear or Home */
  if((cmd == 0x01) || (cmd == 0x02))
    __delay_us(2);
  else
    __delay_us(30);
}

void st7032i_init(void) {
  delay_100ms(1);
  // 8bit 2line Noraml mode
  st7032i_cmd(0x38);
  // 8bit 2line Extend mode
  st7032i_cmd(0x39);
  // OSC 183Hz BIAS 1/5
  st7032i_cmd(0x14);
  // CONTRAST
  st7032i_cmd(0x70 + (ST7032I_CONTRAST & 0x0F));
  st7032i_cmd(0x5C + (ST7032I_CONTRAST >> 4));
#ifdef VDD_5V
  // Follower for 5V
  st7032i_cmd(0x6A);
#else
  // Follower for 3.3V
  st7032i_cmd(0x6C);
#endif
  delay_100ms(3);
  // Set Normal mode
  st7032i_cmd(0x38);
  // Display On
  st7032i_cmd(0x0C);
  // Clear Display
  st7032i_cmd(0x01);
}

void st7032i_putc(const char c) {
  // I2C Start Condition
  i2c_start();
  // ST7032I Device Address
  i2c_write(ST7032I_I2C_ADDR);
  // Set Data Mode
  i2c_write(0x40);
  // Output Data
  i2c_write(c);
  // I2C Stop Condition
  i2c_stop();
  __delay_us(30);
}

void st7032i_puts(const char *buf) {
  uint8_t i = 0;

  while(buf[i] != 0x00)
    st7032i_putc(buf[i++]);
}

void st7032i_clear(void) {
  // Clear Display
  st7032i_cmd(0x01);
}
