#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>

extern char *itos(char *buf, uint32_t val, uint8_t rad, int8_t dig, char pad);
extern uint32_t get_frac_part(uint16_t val, uint8_t accu, uint8_t dig);

#endif
