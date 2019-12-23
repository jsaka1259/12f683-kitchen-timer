#include <xc.h>
#include <stdint.h>

#include "common/common.h"

// CONFIG
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select bit (MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Detect (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)

uint8_t flag   = 0;
uint8_t state  = 0;
uint8_t item   = 0;
uint8_t flag1  = 0;
uint8_t sec1   = 0;
uint8_t min1   = 0;
uint8_t flag2  = 0;
uint8_t sec2   = 0;
uint8_t min2   = 0;
char    msg1[] = " 1 xx:xx";
char    msg2[] = " 2 xx:xx";

void process(void);
void cursor(uint8_t item);
void display(void);
void buzzer(uint8_t cont);

void __interrupt() T1ISR(void) {
  TMR1H  = 0x85;
  TMR1L  = 0xF0;
  TMR1IF = 0;
  flag   = 1;
  GP2    = 1;

  if (flag1) {
    if (sec1 == 0 && min1 > 0) {
        sec1 = 59;
        min1--;
    } else {
      sec1--;
      if (sec1 == 0 && min1 <= 0) {
        buzzer(1);
        flag1 = 0;
        state = 5;
      }
    }
  }

  if (flag2) {
    if (sec2 == 0 && min2 > 0) {
      sec2 = 59;
      min2--;
    } else {
      sec2--;
      if (sec2 == 0 && min2 <= 0) {
        buzzer(2);
        flag2 = 0;
        state = 5;
      }
    }
  }

  GP2 = 0;
  TMR1ON = (flag1 == 0 && flag2 == 0) ? 0 : TMR1ON;
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
  TMR1H  = 0x85;
  TMR1L  = 0xf0;
  TMR1IE = 1;
  TMR1ON = 0;

  PEIE = 1;
  GIE  = 1;

  while(1) {
    if (flag) {
      display();
      flag = 0;
    }

    if (GP4 == 0 || GP5 == 0) {
      process();
      while (GP4 == 0 || GP5 == 0)
        ;
      __delay_ms(40);
    }
  }
}

void process(void) {
  switch (state) {
    case 0:
      if (GP4 == 0) {
        item = (item + 1) % 4;
        cursor(item);
      }
      if (GP5 == 0)
        state++;
      break;
    case 1:
      if (GP4 == 0) {
        switch (item) {
          case 0:
            if (flag1)
              min1 < 29 ? min1++ : 0;
            else
              min1 = (min1 + 1) % 30;
            break;
          case 1:
            if (flag1)
              sec1 = sec1 < 50 ? sec1 + 10 : sec1 + (59 - sec1);
            else
              sec1 = sec1 < 50 ? sec1 + 10 : sec1 < 59 ? sec1 + (59 - sec1) : 0;
            break;
          case 2:
            if (flag2)
              min2 < 29 ? min2++ : 0;
            else
              min2 = (min2 + 1) % 30;
            break;
          case 3:
            if (flag2)
              sec2 = sec2 < 50 ? sec2 + 10 : sec2 + (59 - sec2);
            else
              sec2 = sec2 < 50 ? sec2 + 10 : sec2 < 59 ? sec2 + (59 - sec2) : 0;
            break;
          default:
            break;
        }
        display();
      }
      if (GP5 == 0)
        state++;
      break;
    case 2:
      if (GP4 == 0) {
        state = 0;
        cursor(item);
      }
      if (GP5 == 0) {
        switch (item) {
          case 0:
            flag1 = (min1 != 0 || sec1 != 0) ? flag1 ^ 1 : 0;
            flag2 = (min2 != 0 || sec2 != 0) ? flag1     : 0;
            break;
          case 1:
            flag1 = (min1 != 0 || sec1 != 0) ? flag1 ^ 1 : 0;
            break;
          case 2:
            flag2 = (min2 != 0 || sec2 != 0) ? flag2 ^ 1 : 0;
            flag1 = (min1 != 0 || sec1 != 0) ? flag2     : 0;
            break;
          case 3:
            flag2 = (min2 != 0 || sec2 != 0) ? flag2 ^ 1 : 0;
            break;
          default:
            break;
        }
        display();
        cursor(item);
        TMR1ON = (flag1 || flag2) ? 1 : 0;
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
  if (flag1) {
    itos(&msg1[3], min1, 10, 2, '0');
    itos(&msg1[6], sec1, 10, 2, '0');
    msg1[0] = '*';
    msg1[5] = ':';
    st7032i_cmd(0x80);
    st7032i_puts(msg1);
  } else {
    itos(&msg1[3], min1, 10, 2, '0');
    itos(&msg1[6], sec1, 10, 2, '0');
    msg1[0] = ' ';
    msg1[5] = ':';
    st7032i_cmd(0x80);
    st7032i_puts(msg1);
  }
  if (flag2) {
    itos(&msg2[3], min2, 10, 2, '0');
    itos(&msg2[6], sec2, 10, 2, '0');
    msg2[0] = '*';
    msg2[5] = ':';
    st7032i_cmd(0xC0);
    st7032i_puts(msg2);
  } else {
    itos(&msg2[3], min2, 10, 2, '0');
    itos(&msg2[6], sec2, 10, 2, '0');
    msg2[0] = ' ';
    msg2[5] = ':';
    st7032i_cmd(0xC0);
    st7032i_puts(msg2);
  }
}

void buzzer(uint8_t cont) {
  switch (cont) {
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
