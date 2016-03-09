#include <avr/io.h>
#include <util/delay.h>

#include "data.h"

Command program[15];
uint8_t pc=0;

uint8_t bus[4];
uint8_t ddr[4];

uint8_t read_pin(uint8_t pin) {
    DDRB &= ~(_BV(pin));
    return bit_is_set(PINB, pin);
}

void write_pin(uint8_t pin, uint8_t val) {
  DDRB = val ? (DDRB & ~(_BV(pin))) : (DDRB | _BV(pin));
}

void transfer(void) {
  uint8_t i, j;
  for(i=0;i<8;i++) {
    loop_until_bit_is_set(PINB, PCLK);
    for(j=0;j<4;j++) {
      if(ddr[j]){//output
        write_pin(j, bus[j] & _BV(j));
      }
    }
    loop_until_bit_is_clear(PINB, PCLK);
    for(j=0;j<4;j++) {
      if(!ddr[j]){//input
        bus[j] = bus[j]<<1 & read_pin(j);
      }
    }
  }
}

void init(void) {
  // High-Z input
  PORTB = 0;
  DDRB = 0;
}

int main(void) {
  init();
  while(1) {
    transfer();
    write_pin(PB3, 1);
    _delay_ms(1000);
    write_pin(PB3, 0);
    _delay_ms(1000);
  }
  return 0;
}
