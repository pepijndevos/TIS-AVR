#include <stdint.h>

typedef enum Pin {
    PUP=PB0,
    PDOWN=PB1,
    PLEFT=PB2,
    PRIGHT=PB3,
    PCLK=PB4
} Pin;

typedef struct BusState {
    unsigned int read:1;
    unsigned int write:1;
    unsigned int rread:1;
    unsigned int rwrite:1;
    Pin pin:4;
    uint8_t data;
} BusState;

typedef enum Instruction {
    NOP,
    MOV,
    SWP,
    SAV,
    ADD,
    SUB,
    NEG,
    JMP,
    JEZ,
    JNZ,
    JGZ,
    JLZ,
    JRO,
} Instruction;

typedef struct {
    Instruction cmd;
    int8_t param1;
    int8_t param2;
} Command;

    
