#include <stdint.h>

#define PUP PB0
#define PDOWN PB1
#define PLEFT PB2
#define PRIGHT PB3
#define PCLK PB4

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

    
