#include <avr/io.h>
#include <util/delay.h>

#include "data.h"

Command program[15];
uint8_t pc=0;

BusState bus;

uint8_t read_pin(uint8_t pin) {
    DDRB &= ~(_BV(pin));
    return bit_is_set(PINB, pin);
}

void write_pin(uint8_t pin, uint8_t val) {
  DDRB = val ? (DDRB & ~(_BV(pin))) : (DDRB | _BV(pin));
}

void negotiate(void) {
  loop_until_bit_is_set(PINB, PCLK);
  if(bus.write) {
    // Pull line low to indicate desire to write
    write_pin(bus.pin, 0);
  }
  loop_until_bit_is_clear(PINB, PCLK);
  // If pin reads low, other side desires to write
  if(!bus.write) {
    bus.rwrite = !read_pin(bus.pin);
  }
  loop_until_bit_is_set(PINB, PCLK);
  if(bus.read) {
    // Pull line low to indicate desire to read
    write_pin(bus.pin, 0);
  }
  loop_until_bit_is_clear(PINB, PCLK);
  // If pin reads low, other side desires to read
  if(!bus.read) {
    bus.rread = !read_pin(bus.pin);
  }
  // If both sides want to read/write, they will hold the pin low
  // but not read it. So a deadlock occurs as expected.
  // If one side wants to read, and the other write,
  // A 0 is sent back and forth.
  // If one side is idle the other side reads 1,
  // indicating the other side is not ready.
  // So the systems loops until a read/write match is foud.
}

void transfer(void) {
  uint8_t i;
  negotiate();
  if(bus.read && bus.rwrite) { // We want to read, other side wants to write
    bus.read=0; // We fulfilled the request
    bus.data=0; // Clear data
    for(i=0;i<8;i++) {
      loop_until_bit_is_set(PINB, PCLK);
      loop_until_bit_is_clear(PINB, PCLK);
      bus.data |= read_pin(bus.pin) << i;
    }
  } else if(bus.write && bus.rread) { // We want to write, other side wants to read
    bus.write=0; // We fulfilled the request
    for(i=0;i<8;i++) {
      loop_until_bit_is_set(PINB, PCLK);
      write_pin(bus.pin, _BV(i) & bus.data);
      loop_until_bit_is_clear(PINB, PCLK);
    }
  } else { // Nothing happens, wait for the next cycle
    for(i=0;i<8;i++) {
      loop_until_bit_is_set(PINB, PCLK);
      loop_until_bit_is_clear(PINB, PCLK);
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
  bus.pin=0;
  while(1) {
    bus.read=1;
    transfer();
    //write_pin(PB3, 1);
    //_delay_ms(100);
    bus.write=1;
    transfer();
    //write_pin(PB3, 0);
    //_delay_ms(100);
  }
  return 0;
}
