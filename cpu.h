#ifndef CPU_H
#define CPU_H

#include <inttypes.h>
#include "instructions.h"


typedef __uint8_t Byte;
typedef __uint16_t Word;
typedef __uint32_t DWord;

#define MAX_MEMORY (1024 * 64) 

struct MEMORY
{
   Byte data[MAX_MEMORY]; 
};

struct CPU
{
    Word PC;
    Word SP;

    Byte _reg_accumulator, _reg_X, _reg_Y;

    struct PROCESSOR_STATUS
    {
        Byte _carry : 1;
        Byte _zero : 1;
        Byte _interrupt_disable : 1;
        Byte _decimal_mode : 1;
        Byte : 1;
        Byte _break_command : 1;
        Byte _overflow : 1;
        Byte _negative : 1;
    } processor_status;
};

void reset_cpu(struct CPU * cpu, struct MEMORY * memory);
void initialize_memory(struct MEMORY * memory);

void execute_cpu(struct CPU * cpu, DWord cycles, struct MEMORY * memory);

void setLDAstruction(struct CPU * cpu);

Byte fetch_instruction_byte_cpu(struct CPU * cpu, DWord * cycles, struct MEMORY * memory);
Word fetch_instruction_word_cpu(struct CPU * cpu, DWord * cycles, struct MEMORY * memory);

Byte read_byte(Byte address, DWord * cycles, struct MEMORY * memory);

void write_word(Word value, DWord * cycles, DWord address, struct MEMORY * memory);

#endif