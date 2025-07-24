#include <stdio.h>
#include <stdlib.h>

typedef __uint8_t Byte;
typedef __uint16_t Word;
typedef __uint32_t DWord;

#define MAX_MEMORY (1024 * 64) 

enum OPCODES
{
    __LDA_IM = 0xA9,
    __LDA_ZP = 0xA5,
    __LDA_ZPX = 0xB5,
    __JSR = 0x20,
};

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
        Byte instruction = fetch_instruction_byte_cpu(cpu, &cycles, memory);
        switch(instruction)
        {
            case __LDA_IM:
            {
                Byte value = fetch_instruction_byte_cpu(cpu, &cycles, memory);
                cpu->_reg_accumulator = value;
                setLDAstruction(cpu);
                break;
            }
            case __LDA_ZP:
            {
                Byte zero_page_address = fetch_instruction_byte_cpu(cpu, &cycles, memory);
                cpu->_reg_accumulator = read_byte(zero_page_address, &cycles, memory);
                setLDAstruction(cpu);
                break;
            }
            case __LDA_ZPX:
            {
                Byte zero_page_address = fetch_instruction_byte_cpu(cpu, &cycles, memory);
                zero_page_address += cpu->_reg_X;
                --cycles;
                cpu->_reg_accumulator = read_byte(zero_page_address, &cycles, memory);
                setLDAstruction(cpu);
                break;
            }
            case __JSR:
            {
                Word subroutine_address = fetch_instruction_word_cpu(cpu, &cycles, memory);
                write_word(cpu->PC - 1, cycles, cpu->SP, memory);
                cpu->PC = subroutine_address;
                --cycles;
            }
            default:
            {
                printf("Instruction not exist %d", instruction);
            }
        }
    }
}

Byte fetch_instruction_byte_cpu(struct CPU * cpu, DWord * cycles, struct MEMORY * memory)
{
    Byte data = memory->data[cpu->PC];
    ++(cpu->PC);
    --(*cycles);
    return data;
}

Word fetch_instruction_word_cpu(struct CPU * cpu, DWord * cycles, struct MEMORY * memory)
{
    Word data = memory->data[cpu->PC];
    ++(cpu->PC);
    data |= (memory->data[cpu->PC] << 8);
    ++(cpu->PC);
    (*cycles) -= 2;
    return data;
}

Byte read_byte(Byte address, DWord * cycles, struct MEMORY * memory)
{
    Byte data = memory->data[address];
    --(*cycles);
    return data;
}

void write_word(Word value, DWord * cycles, DWord address, struct MEMORY * memory)
{
    memory->data[address] = value && 0xFF;
    memory->data[address + 1] = (value >> 8); // why is 8 to right?
    cycles -= 2;
}

void setLDAstruction(struct CPU * cpu)
{
    cpu->processor_status._zero = (cpu->_reg_accumulator == 0);
    cpu->processor_status._negative = (cpu->_reg_accumulator & (1 << 7)) > 0;
}

int main()
{
    struct MEMORY memory;
    struct CPU cpu;

    reset_cpu(&cpu, &memory);

    memory.data[0xFFFC] = __JSR;
    memory.data[0xFFFD] = 0x42;
    memory.data[0xFFFE] = 0x42;
    memory.data[0x4242] = __LDA_IM;
    memory.data[0x4243] = 0x84;
    
    execute_cpu(&cpu, 9, &memory);
    return 0;
}