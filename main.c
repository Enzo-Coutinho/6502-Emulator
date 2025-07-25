#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"

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