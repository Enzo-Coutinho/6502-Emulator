#include <stdio.h>
#include <stdlib.h>

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
void execute_cpu(struct CPU * cpu, DWord cycles, struct MEMORY * memory);
Byte fetch_instruction_cpu(struct CPU * cpu, DWord * cycles, struct MEMORY * memory);
void initialize_memory(struct MEMORY * memory);

void reset_cpu(struct CPU * cpu, struct MEMORY * memory)
{
    cpu->PC = 0xFFFC;
    cpu->SP = 0x0100;
    cpu->_reg_accumulator = cpu->_reg_X = cpu->_reg_Y = 0;
    cpu->processor_status._carry = 0;
    cpu->processor_status._zero = 0;
    cpu->processor_status._interrupt_disable = 0;
    cpu->processor_status._decimal_mode = 0;
    cpu->processor_status._break_command = 0;
    cpu->processor_status._overflow = 0;
    cpu->processor_status._negative = 0;

    initialize_memory(memory);
}

void initialize_memory(struct MEMORY * memory)
{
    for(DWord i=0; i<MAX_MEMORY; i++)
    {
        memory->data[i] = 0;
    }
}

void execute_cpu(struct CPU * cpu, DWord cycles, struct MEMORY * memory)
{
    while(cycles > 0)
    {
        Byte instruction = fetch_instruction_cpu(cpu, &cycles, memory);
    }
}

Byte fetch_instruction_cpu(struct CPU * cpu, DWord * cycles, struct MEMORY * memory)
{
    Byte data = memory->data[cpu->PC];
    ++(cpu->PC);
    --(*cycles);
    return data;
}

int main()
{
    struct MEMORY memory;
    struct CPU cpu;

    reset_cpu(&cpu, &memory);
    execute_cpu(&cpu, 2, &memory);
    return 0;
}