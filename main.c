#include <avr/io.h>
#include <util/delay.h>

#include "data.h"

Command program[15] = {
  {MOVr, UP, ACC},
  {ADDr, ACC, 0},
  {MOVr, ACC, DOWN},
  {JMP, 0, 0}
};
uint8_t pc=0;
uint8_t acc;
uint8_t bak;

BusState bus;

uint8_t read_pin(uint8_t pin) {
    DDRB &= ~(_BV(pin));
    return bit_is_set(PINB, pin);
}

void write_pin(uint8_t pin, uint8_t val) {
  DDRB = val ? (DDRB & ~(_BV(pin))) : (DDRB | _BV(pin));
}

void negotiate(void) {
  loop_until_bit_is_set(PINB, CLK);
  if(bus.write) {
    // Pull line low to indicate desire to write
    write_pin(bus.pin, 0);
  }
  loop_until_bit_is_clear(PINB, CLK);
  // If pin reads low, other side desires to write
  if(!bus.write) {
    bus.rwrite = !read_pin(bus.pin);
  }
  loop_until_bit_is_set(PINB, CLK);
  if(bus.read) {
    // Pull line low to indicate desire to read
    write_pin(bus.pin, 0);
  }
  loop_until_bit_is_clear(PINB, CLK);
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
    bus.data=0; // Clear data
    for(i=0;i<8;i++) {
      loop_until_bit_is_set(PINB, CLK);
      loop_until_bit_is_clear(PINB, CLK);
      bus.data |= read_pin(bus.pin) << i;
    }
    bus.read=0; // We fulfilled the request
  } else if(bus.write && bus.rread) { // We want to write, other side wants to read
    for(i=0;i<8;i++) {
      loop_until_bit_is_set(PINB, CLK);
      write_pin(bus.pin, _BV(i) & bus.data);
      loop_until_bit_is_clear(PINB, CLK);
    }
    bus.write=0; // We fulfilled the request
  } else { // Nothing happens, wait for the next cycle
    for(i=0;i<8;i++) {
      loop_until_bit_is_set(PINB, CLK);
      loop_until_bit_is_clear(PINB, CLK);
    }
  }
}

uint8_t read_register(Port reg) {
  if(reg==ACC){
    return acc;
  }
  bus.pin = reg;
  bus.read = 1;
  while(bus.read) {
    transfer();
  }
  return bus.data;
}

void write_register(Port reg, uint8_t val) {
  if(reg==ACC){
    acc=val;
    return;
  }
  bus.pin = reg;
  bus.write = 1;
  bus.data = val;
  while(bus.write) {
    transfer();
  }
}

void execute(void) {
  Command inst = program[pc];
  uint8_t tmp;
  switch(inst.cmd) {
    case NOP: break;
    case MOVr:
      inst.param1=read_register(inst.param1);
      // fall through
    case MOVi:
      write_register(inst.param2, inst.param1);
      break;
    case SWP:
      tmp=acc;
      acc=bak;
      bak=tmp;
      break;
    case SAV:
      bak=acc;
      break;
    case ADDr:
      inst.param1=read_register(inst.param1);
      // fall through
    case ADDi:
      acc+=inst.param1;
      break;
   case SUBr:
      inst.param1=read_register(inst.param1);
      // fall through
    case SUBi:
      acc-=inst.param1;
      break;
    case NEG:
      acc=-acc;
      break;
    case JMP:
      pc=inst.param1;
      return; // Don't increment pc
    case JEZ:
      if (acc==0) {
        pc=inst.param1;
        return; // Don't increment pc
      }
      break;
    case JNZ:
      if (acc!=0) {
        pc=inst.param1;
        return; // Don't increment pc
      }
      break;
    case JGZ:
      if (acc>0) {
        pc=inst.param1;
        return; // Don't increment pc
      }
      break;
    case JLZ:
      if (acc<0) {
        pc=inst.param1;
        return; // Don't increment pc
      }
      break;
    case JROr:
      inst.param1=read_register(inst.param1);
      // fall through
    case JROi:
      pc+=inst.param1;
      return; // Don't increment pc

  }
  pc++;
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
    transfer();
    execute();
  }
  return 0;
}
