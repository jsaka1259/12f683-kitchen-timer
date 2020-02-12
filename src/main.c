#include <xc.h>
#include <stdint.h>

#include "common/common.h"

// CONFIG
#pragma config FOSC = INTOSCIO
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config MCLRE = OFF
#pragma config CP = OFF
#pragma config CPD = OFF
#pragma config BOREN = OFF
#pragma config IESO = OFF
#pragma config FCMEN = OFF

#define TMR1H_VAL 0x85
#define TMR1L_VAL 0xf0

#define SEL_SW GP4
#define ENT_SW GP5

#define SEL_SW_ON 0x01
#define ENT_SW_ON 0x02

#define CHATT_CNT 20

static uint8_t display_on = 0;
static uint8_t state      = 0;
static uint8_t item       = 0;

static uint8_t tmr1_on = 0;
static uint8_t sec1    = 0;
static uint8_t min1    = 0;
static char    msg1[]  = " 1 xx:xx";

static uint8_t tmr2_on = 0;
static uint8_t sec2    = 0;
static uint8_t min2    = 0;
static char    msg2[]  = " 2 xx:xx";


uint8_t read_sw(void) {
  static uint8_t sw;
  static uint8_t sel_cnt[2];
  static uint8_t ent_cnt[2];

  if ((sw & SEL_SW_ON) == 0) {
    if (SEL_SW == 0)
      sel_cnt[0]++;
    else
      sel_cnt[0] = 0;

    if (sel_cnt[0] > CHATT_CNT) {
      sel_cnt[0] = 0;
      sw |= SEL_SW_ON;
      return sw;
    }
  } else {
    if (SEL_SW == 1)
      sel_cnt[1]++;
    else
      sel_cnt[1] = 0;

    if (sel_cnt[1] > CHATT_CNT) {
      sel_cnt[1] = 0;
      sw &= ~SEL_SW_ON;
    }
  }

  if ((sw & ENT_SW_ON) == 0) {
    if (ENT_SW == 0)
      ent_cnt[0]++;
    else
      ent_cnt[0] = 0;

    if (ent_cnt[0] > CHATT_CNT) {
      ent_cnt[0] = 0;
      sw |= ENT_SW_ON;
      return sw;
    }
  } else {
    if (ENT_SW == 1)
      ent_cnt[1]++;
    else
      ent_cnt[1] = 0;

    if (ent_cnt[1] > CHATT_CNT) {
      ent_cnt[1] = 0;
      sw &= ~ENT_SW_ON;
    }
  }

  return 0;
}

void cursor(uint8_t item) {
  switch (item) {
    case 0:
      st7032i_cmd(0x84);
      break;
    case 1:
      st7032i_cmd(0x87);
      break;
    case 2:
      st7032i_cmd(0xC4);
      break;
    case 3:
      st7032i_cmd(0xC7);
      break;
    default:
      break;
  }
  st7032i_cmd(0x0F);
}

void display(void) {
  if (tmr1_on)
    msg1[0] = '*';
  else
    msg1[0] = ' ';

  if (tmr2_on)
    msg2[0] = '*';
  else
    msg2[0] = ' ';

  itos(&msg1[3], min1, 10, 2, '0');
  itos(&msg1[6], sec1, 10, 2, '0');
  msg1[5] = ':';
  itos(&msg2[3], min2, 10, 2, '0');
  itos(&msg2[6], sec2, 10, 2, '0');
  msg2[5] = ':';

  st7032i_cmd(0x80);
  st7032i_puts(msg1);
  st7032i_cmd(0xC0);
  st7032i_puts(msg2);
}

void buzzer(uint8_t mode) {
  switch (mode) {
    case 0:
      T2CON   = 0;
      CCP1CON = 0;
      break;
    case 1:
      PR2     = 61;
      TMR2    = 0;
      T2CON   = 0x04;
      CCP1CON = 0x0C;
      CCPR1L  = 31;
      state   = 5;
      break;
    case 2:
      PR2     = 81;
      TMR2    = 0;
      T2CON   = 0x04;
      CCP1CON = 0x0C;
      CCPR1L  = 41;
      state   = 5;
      break;
    default:
      break;
  }
}

void process(uint8_t sw) {
  switch (state) {
    case 0:
      if (sw & SEL_SW_ON) {
        item = (item + 1) % 4;
        cursor(item);
      }

      if (sw & ENT_SW_ON)
        state++;
      break;
    case 1:
      if (sw & SEL_SW_ON) {
        switch (item) {
          case 0:
            if (tmr1_on)
              min1 < 29 ? min1++ : 0;
            else
              min1 = (min1 + 1) % 30;
            break;
          case 1:
            if (tmr1_on)
              sec1 = sec1 < 50 ? sec1 + 10 : sec1 + (59 - sec1);
            else
              sec1 = sec1 < 50 ? sec1 + 10 : sec1 < 59 ? sec1 + (59 - sec1) : 0;
            break;
          case 2:
            if (tmr2_on)
              min2 < 29 ? min2++ : 0;
            else
              min2 = (min2 + 1) % 30;
            break;
          case 3:
            if (tmr2_on)
              sec2 = sec2 < 50 ? sec2 + 10 : sec2 + (59 - sec2);
            else
              sec2 = sec2 < 50 ? sec2 + 10 : sec2 < 59 ? sec2 + (59 - sec2) : 0;
            break;
          default:
            break;
        }
        display();
      }

      if (sw & ENT_SW_ON)
        state++;
      break;
    case 2:
      if (sw & SEL_SW_ON) {
        state = 0;
        cursor(item);
      }

      if (sw & ENT_SW_ON) {
        switch (item) {
          case 0:
            tmr1_on = (min1 != 0 || sec1 != 0) ? tmr1_on ^ 1 : 0;
            tmr2_on = (min2 != 0 || sec2 != 0) ? tmr1_on     : 0;
            break;
          case 1:
            tmr1_on = (min1 != 0 || sec1 != 0) ? tmr1_on ^ 1 : 0;
            break;
          case 2:
            tmr2_on = (min2 != 0 || sec2 != 0) ? tmr2_on ^ 1 : 0;
            tmr1_on = (min1 != 0 || sec1 != 0) ? tmr2_on     : 0;
            break;
          case 3:
            tmr2_on = (min2 != 0 || sec2 != 0) ? tmr2_on ^ 1 : 0;
            break;
          default:
            break;
        }
        display();
        cursor(item);
        TMR1ON = (tmr1_on || tmr2_on) ? 1 : 0;
        state  = 0;
      }
      break;
    case 5:
      buzzer(0);
      state = 0;
      cursor(item);
      break;
    default:
      break;
  }
}

void __interrupt() T1ISR(void) {
  TMR1H      = TMR1H_VAL;
  TMR1L      = TMR1L_VAL;
  TMR1IF     = 0;
  display_on = 1;
  GP2        = 1;

  if (tmr1_on) {
    if (sec1 == 0 && min1 > 0) {
        sec1 = 59;
        min1--;
    } else {
      sec1--;
      if (sec1 == 0 && min1 <= 0) {
        buzzer(1);
        tmr1_on = 0;
        state   = 5;
      }
    }
  }

  if (tmr2_on) {
    if (sec2 == 0 && min2 > 0) {
      sec2 = 59;
      min2--;
    } else {
      sec2--;
      if (sec2 == 0 && min2 <= 0) {
        buzzer(2);
        tmr2_on = 0;
        state   = 5;
      }
    }
  }

  GP2 = 0;
  TMR1ON = (tmr1_on == 0 && tmr2_on == 0) ? 0 : TMR1ON;
}

void main(void) {
  OSCCON = 0x40;
  GPIO   = 0x00;
  ANSEL  = 0x00;
  CMCON0 = 0x07;
  TRISIO = 0x38;
  WPU    = 0x30;
  nGPPU  = 0;

  i2c_init();
  st7032i_init();
  st7032i_cmd(0x80);
  st7032i_puts("Kitchen ");
  st7032i_cmd(0xc0);
  st7032i_puts("   Timer");
  __delay_ms(2000);
  display();
  cursor(item);

  T1CON  = 0x30;
  TMR1H  = TMR1H_VAL;
  TMR1L  = TMR1L_VAL;
  TMR1IE = 1;
  TMR1ON = 0;

  PEIE = 1;
  GIE  = 1;

  uint8_t sw = 0;

  while(1) {
    sw = read_sw();

    if (display_on) {
      display();
      display_on = 0;
    }

    if (sw)
      process(sw);
  }
}
