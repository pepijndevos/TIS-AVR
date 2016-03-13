#include <stdint.h>

typedef enum Port {
    // Pins
    UP=PB0,
    DOWN=PB1,
    LEFT=PB2,
    RIGHT=PB3,
    CLK=PB4,
    // Registers
    ACC,
} Port;

typedef struct BusState {
    unsigned int read:1;
    unsigned int write:1;
    unsigned int rread:1;
    unsigned int rwrite:1;
    Port pin:4;
    uint8_t data;
} BusState;

// i indicates immediate value
// r indicates register
typedef enum Instruction {
    NOP,
    MOVi,
    MOVr,
    SWP,
    SAV,
    ADDi,
    ADDr,
    SUBi,
    SUBr,
    NEG,
    JMP,
    JEZ,
    JNZ,
    JGZ,
    JLZ,
    JROi,
    JROr,
} Instruction;

typedef struct {
    Instruction cmd;
    int8_t param1;
    int8_t param2;
} Command;

    
