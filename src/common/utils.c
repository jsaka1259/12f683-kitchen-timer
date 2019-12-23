#include "utils.h"

char *itos(char *buf, uint32_t val, uint8_t rad, int8_t dig, char pad) {
  uint8_t minus = 0;

  if (rad != 2 && rad != 8 && rad != 10 && rad != 16)
    rad = 10;

  if (rad == 10 && val & 0x8000) {
    minus = 1;
    val = -val;
  }

  buf[dig] = 0;
  buf += dig;

  for (; dig > 0; dig--) {
    buf--;
    *buf = "0123456789ABCDEF"[val % rad];
    val /= rad;
    if (!val)
      break;
  }

  for (dig--; dig > 0; dig--) {
    buf--;
    if (minus) {
      minus = 0;
      *buf = '-';
    } else
      *buf = pad;
  }

  return buf;
}

uint32_t get_frac_part(uint16_t val, uint8_t accu, uint8_t dig) {
  uint32_t frac = 0;
  uint32_t n = 1;

  accu = accu & 0x0f;
  dig = dig < 10 ? dig : 9;
  while (dig--)
    n *= 10;

  for (int8_t i = accu - 1; i >= 0; i--)
    frac += (n / (1 << (accu - i))) * ((val & (1 << i)) >> i);

  return frac;
}
