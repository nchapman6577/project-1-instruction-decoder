/************
    Ahmed Alkhawar,
    CEC 470
    Project 1
    Nov 11, 2021

    This project is a simple 2-stage processor, uses four registers: PC, IR, MAR, and ACC.
    And implements two functions (fetch next instruction cycle and the execute instruction cycle) which,
    simulate the registers and memory of the processor.

    References:
    - https://erau.instructure.com/courses/134998/assignments/2597611
    - https://www.tutorialspoint.com/cprogramming
    - implements some of the example code provided in the project description
 ***********/


#include <stdio.h>

#define HALT_OPCODE 0x19

void fetchNextInstruction(void);
void executeInstruction(void);
void printState(char* msg, unsigned int adr);

unsigned char memory[65536]={0};
unsigned char ACC=0;
unsigned char IR=0;
unsigned int MAR=0;
unsigned int PC=0;

int main(int argc, char* argv[])
{
    char * msgs[7] = { "Initial state",
                       "LOAD ACC, [0x1000]",
                       "ADD ACC, [0x1001]",
                       "ADD ACC, [0x1002]",
                       "XOR ACC, 0xFF",
                       "INC ACC",
                       "STORE ACC, [0x1003]"};
    unsigned int adrs[7] = {0x1000, 0x1000, 0x1001, 0x1002, 0, 0, 0x1003};
    int step = 0;

    /* LOAD ACC, [0x1000] */
    memory[0] = 0x08;
    memory[1] = 0x10;
    memory[2] = 0x00;

   /* ADD ACC, [0x1001] */
    memory[3] = 0xb7;
    memory[4] = 0x10;
    memory[5] = 0x01;

    /* ADD ACC, [0x1002] */
    memory[6] = 0xb7;
    memory[7] = 0x10;
    memory[8] = 0x02;

    /* XOR ACC, 0xFF */
    memory[9] = 0xa6;
    memory[10] = 0xff;

    /* INC ACC */
    memory[11] = 0xd4;

    /* STOR ACC, [0x1003] */
    memory[12] = 0x00;
    memory[13] = 0x10;
    memory[14] = 0x03;

    /* HALT */
    memory[15] = 0x19;

    memory[0x1000] = 1;
    memory[0x1001] = 2;
    memory[0x1002] = 3;

// Execution loop. Continue fetching and executing
// until PC points to a HALT instruction
    printState(msgs[step], adrs[step]);
    step++;
    while (memory[PC] != HALT_OPCODE)
    {
        fetchNextInstruction();
        executeInstruction();
        printState(msgs[step], adrs[step]);
        step++;
    }

    printf("memory[0x1003] = 0x%02X\n", memory[0x1003]);
    return 0;
}

void printState(char* msg, unsigned int adr) {
    printf("%s\n", msg);
    printf("ACC: 0x%02X  IR: 0x%02X  MAR: 0x%04X  PC: 0x%02X\n", ACC, IR, MAR, PC);
    if (adr) {
           printf("memory[0x%04X]: 0x%02X\n", adr, memory[adr]);
    }
    printf("\n");
}

