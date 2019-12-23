#ifndef _I2C_H_
#define _I2C_H_

#include <xc.h>
#include <stdint.h>

#include "xtal.h"

#define SDA     GP0
#define SCL     GP1
#define TRISSDA TRISIO0

extern void i2c_init(void);
extern void i2c_start(void);
extern void i2c_stop(void);
extern uint8_t i2c_write(uint8_t data);
extern uint8_t i2c_read(uint8_t ack);

#endif

