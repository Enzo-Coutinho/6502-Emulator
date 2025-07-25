#include "cpu.h"

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