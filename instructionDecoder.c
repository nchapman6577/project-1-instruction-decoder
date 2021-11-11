/*
By:Ahmed Alkhawar, Naimah-Joy Chapman, Jorge Santos
*/
void fetchNextInstruction(void);
void executeInstruction(void);

extern unsigned char memory[65536];
extern unsigned char ACC;
extern unsigned char IR;
extern unsigned int MAR;
extern unsigned int PC;

/* Constant to decode math instructions */
#define MATH_OP 0x80
#define MATH_AND 0x0
#define MATH_OR  0x1
#define MATH_XOR 0x2
#define MATH_ADD 0x3
#define MATH_SUB 0x4
#define MATH_INC 0x5
#define MATH_DEC 0x6
#define MATH_NOT 0x7

#define MATH_MASK 0x70
/* Math operation addressing */
#define MATH_DEST_MASK    0x0C
#define MATH_SOUR_MASK    0x03
#define MATH_INDIRECT_ADR 0x0
#define MATH_ACCUM_ADR    0x1
#define MATH_MAR_ADR      0x2
#define MATH_CONST_ADR    0x2
#define MATH_MEMORY_ADR   0x3

/* Check if instruction is math */
int isMath(unsigned char code) {
    return code & MATH_OP;
}

/* Return a code for math instruction */
unsigned char mathFunc(unsigned char code) {
    return (code & MATH_MASK) >> 4;
}

/* Get destination part from math instruction */
unsigned char mathDest(unsigned char code) {
    return (code & MATH_DEST_MASK) >> 2;
}

/* Get source part from math instruction */
unsigned char mathSour(unsigned char code) {
    return (code & MATH_SOUR_MASK);
}

/* Check if math operation is byte-wise */
unsigned char mathIsByte(unsigned char code) {
    return mathDest(code) == MATH_ACCUM_ADR || mathSour(code) == MATH_ACCUM_ADR;
}

/* Constants for memory operations */
#define MEM_OP 0xF0
/* Direction */
#define MEM_ST 0
#define MEM_LD 1
/* Register */
#define REG_ACC 0
#define REG_MAR 1
/* Method */
#define MEM_ADR   0
#define MEM_CONST 1
#define MEM_INDIR 2

/* Check if code is memory operation */
int isMem(unsigned int code) {
    return (code & MEM_OP) == 0;
}

/* Get memory access direction */
unsigned char memFunction(unsigned char code) {
    return (code & 0x08) >> 3;
}
/* Store/Load ACC */
unsigned char memACC(unsigned char code) {
    return (code & 0x04) == 0;
}
/* Get memory address method */
unsigned char memMethod(unsigned char code) {
    return (code & 0x03);
}

/* Constants for brunch commands */
#define BRA 0x0
#define BRZ 0x1
#define BNE 0x2
#define BLT 0x3
#define BLE 0x4
#define BGT 0x5
#define BGE 0x6

/* Check if code is branch operation */
int isBranch(unsigned char code)
{
    return (code & 0xf8);
}

/* Get branch type */
unsigned char branchType(unsigned char code) {
    return code & 0x7;
}

unsigned char MSB=0; // Immed. data of Most significant byte
unsigned char LSB=0; // Least significant byte

/* Get the next command */
void fetchNextInstruction(void)
{
    IR = memory[PC++]; // get next instruction

    /* Process math operation */
    if (isMath(IR)) {
        unsigned char dest = mathDest(IR);
        unsigned char sour = mathSour(IR);
        if (dest == MATH_MEMORY_ADR)  {
            /* Next two bytes hold destination address */
            MSB = memory[PC++];
            LSB = memory[PC++];
        }
        if (sour == MATH_CONST_ADR) {
            /* Next byte(s) hold constant */
            if (dest == MATH_ACCUM_ADR) {
                /* 1-bytes */
                MSB = memory[PC++];
            }
            if (dest == MATH_MAR_ADR) {
                /* 2-bytes */
                MSB = memory[PC++];
                LSB = memory[PC++];
            }
        }
        if (sour == MATH_MEMORY_ADR)  {
            /* Next two bytes hold source address */
            MSB = memory[PC++];
            LSB = memory[PC++];
        }
    }

    /* Process memory operations */
    else if (isMem(IR)) {
        unsigned char method = memMethod(IR);
        if (method == MEM_ADR) {
            /* Next two bytes hold source address */
            MSB = memory[PC++];
            LSB = memory[PC++];
        }
        if (method == MEM_CONST) {
            /* Next byte(s) hold a constant */
            if (memACC(IR)) {
                /* 1-byte */
                MSB = memory[PC++];
            }
            else {
                /* 2-bytes */
                MSB = memory[PC++];
                LSB = memory[PC++];
            }
        }
    }

    /* Process branch operations */
    else if (isBranch(IR)) {
        MSB = memory[PC++];
        LSB = memory[PC++];
    }
    /* Other commands does not have any additional operands */
}

/* Get source operand for math operation
 * sour - source type
 * byte == 1 for an operation with single byte
 */
unsigned int getSource(unsigned char sour, unsigned char byte) {
    unsigned int op = 0;
    unsigned int adr;
    switch (sour) {
        case MATH_INDIRECT_ADR:
            /* Indirect acess */
            op = memory[MAR];
            if (!byte) {
                op <<= 8;
                op += memory[MAR+1];
            }
            break;
        case MATH_ACCUM_ADR:
            /* Get operand from ACC */
            op = ACC;
            break;
        case MATH_CONST_ADR:
            /* Form operand from the constant */
            op = MSB;
            if (!byte) {
                /* Two bytes constant */
                op <<= 8;
                op += LSB;
            }
            break;
        case MATH_MEMORY_ADR:
            /* Get operand from a memory */
            adr = (MSB << 8) + LSB;
            op = memory[adr];
            if (!byte) {
                /* Get second byte */
                op <<= 8;
                op += memory[adr+1];
            }
            //printf("** GetSource: sour = 0x%1X, byte = %d, adr = 0x%04X (0x%02X 0x%02X), op = 0x%02X\n", sour, byte, adr, MSB, LSB, op);
            break;
    }
    return op;
}

/* Get destination operand for math operation
 * dest - source type
 * byte == 1 for an operation with single byte
 */
unsigned int getDestination(unsigned char dest, unsigned char byte) {
    unsigned int op = 0;
    unsigned int adr;
    switch (dest) {
        case MATH_INDIRECT_ADR:
            /* Indirect acess */
            op = memory[MAR];
            if (!byte) {
                op <<= 8;
                op += memory[MAR+1];
            }
            break;
        case MATH_ACCUM_ADR:
            /* Get operand from ACC */
            op = ACC;
            break;
        case MATH_MAR_ADR:
            /* Get operand from MAR */
            op = MAR;
            break;
        case MATH_MEMORY_ADR:
            /* Get operand from a memory */
            adr = (MSB << 8) + LSB;
            op = memory[adr];
            if (!byte) {
                /* Get second byte */
                op <<= 8;
                op += memory[adr+1];
            }
            break;
    }
    return op;
}

/* Do math operation
 * op1, op2 - operands
 * func - code for math function
 * return result of operation
 */
unsigned int doMath(unsigned int op1, unsigned int op2, unsigned char func) {
    unsigned int res;

    switch (func) {
        case MATH_AND:
            res = op1 & op2;
            break;
        case MATH_OR:
            res = op1 | op2;
            break;
        case MATH_XOR:
            res = op1 ^ op2;
            break;
        case MATH_ADD:
            res = op1 + op2;
            break;
        case MATH_SUB:
            res = op1 - op2;
            break;
        case MATH_INC:
            res = op1 + 1;
            break;
        case MATH_DEC:
            res = op1 - 1;
            break;
        case MATH_NOT:
            res = ~op1;
            break;
    }

    //printf("** DoMath: op1 = 0x%02X, op2 = 0x%02X, func = 0x%1X, res = 0x%02X\n", op1, op2, func, res);
    return res;
}

/* Save result ofmath operation to the destination
 * dest - destination
 * res - result value
 * byte == 1 if operation was with single byte
 */
void saveResult(unsigned char dest, unsigned int res, unsigned char byte)
{
    unsigned int adr;
    switch (dest) {
        case MATH_INDIRECT_ADR:
            /* Indirect acess */
            if (byte) {
                memory[MAR] = (unsigned char) (res & 0xFF);
            }
            else {
                /* Save 2 bytes */
                memory[MAR] = (unsigned char) (res & 0xFF00);
                memory[MAR+1] = (unsigned char) (res & 0xFF);
            }
            break;
        case MATH_ACCUM_ADR:
            /* Save to ACC */
            //printf("** save to ACC 0x%02X\n", res);
            ACC = (unsigned char) (res & 0xFF);
            break;
        case MATH_MAR_ADR:
        /* Save to MAR */
            MAR = res & 0xFFFF;
            break;
        case MATH_MEMORY_ADR:
        /* Save to a memory */
            adr = (MSB << 8) + LSB;
            if (byte) {
                memory[adr] = (unsigned char) (res & 0xFF);
            }
            else {
                /* save 2 bytes */
                memory[adr] = (unsigned char) (res & 0xFF00);
                memory[adr+1] = (unsigned char) (res & 0xFF);
            }
            break;
    }
}

/* Load data from the memory
 * reg ==  if one-byte register is used
 * method - memory access method
 */
void memLoad(unsigned char reg, unsigned char method)
{
    unsigned int adr;
    // printf("** LOAD: method = 0x%X,  reg = %d\n", method, reg);
    switch (method) {
        case MEM_ADR:
            /* Operand is memory address */
            adr = (MSB << 8) + LSB;
            if (reg) {
                /* Load 1 byte in ACC */
                // printf("** adr = 0x%04X\n", adr);
                ACC = memory[adr];
            }
            else {
                /* Load 2 bytes in MAR */
                MAR = (memory[adr] << 8) + memory[adr+1];
            }
            break;
        case MEM_CONST:
            /* Load a constant */
            if (reg) {
                /* 1 byte in ACC */
                ACC = MSB;
            }
            else {
                /* 2 bytes in MAR */
                MAR = (MSB << 8) + LSB;
            }
            break;
        case MEM_INDIR:
            /* Indirect memory access */
            if (reg) {
                /* 1 bytes in ACC */
                ACC = memory[MAR];
            }
            else {
                /* 2 bytes in MAR */
                MAR = (memory[MAR] << 8) + memory[MAR+1];
            }
            break;
    }
}

/* Store data to the memory
 * reg ==  if one-byte register is used
 * method - memory access method
 */
void memStore(unsigned char reg, unsigned char method)
{
    unsigned int adr;
    switch (method) {
        case MEM_ADR:
            /* Operand is memory address */
            adr = (MSB << 8) + LSB;
            if (reg) {
                /* 1 byte from ACC */
                memory[adr] = ACC;
            }
            else {
                /* 2 bytes from MAR */
                memory[adr] = (unsigned char ) ((MAR & 0xFF00) >> 8);
                memory[adr+1] = (unsigned char) (MAR & 0xFF);
            }
            break;
        case MEM_INDIR:
            /* Indirect memory access */
            if (reg) {
                /* 1 byte from ACC */
                memory[MAR] = ACC;
            }
            else {
                /* 2 bytes from MAR */
                adr = MAR;
                memory[adr] = (unsigned char ) ((MAR & 0xFF00) >> 8);
                memory[adr+1] = (unsigned char) (MAR & 0xFF);
            }
            break;
    }
}

/* Do branch operation
 * type - a type of branching
 * new_adr - a new address to put in PC
 */
void brunch(unsigned char type, unsigned int new_adr)
{
    switch (type) {
        case BRA:
            PC = new_adr;
            break;
        case BRZ:
            if (ACC == 0) {
                PC = new_adr;
            }
            break;
        case BNE:
            if (ACC != 0) {
                PC = new_adr;
            }
            break;
        case BLT:
            if (ACC & 0x80) {
                PC = new_adr;
            }
            break;
        case BLE:
            if ( (ACC & 0x80) || (ACC == 0) ) {
                PC = new_adr;
            }
            break;
        case BGT:
            if ( (ACC & 0x80 == 0) && (ACC != 0) ) {
                PC = new_adr;
            }
            break;
        case BGE:
            if (ACC &0x80 == 0) {
                PC = new_adr;
            }
            break;
    }

}

void executeInstruction(void)
{
    /* Math operations */
    if (isMath(IR)) {
        unsigned char dest = mathDest(IR);              // Get destination code
        unsigned char sour = mathSour(IR);              // Get source code
        unsigned char func = mathFunc(IR);              // Get function code
        unsigned char byte = mathIsByte(IR);            // 1-byte or 2-bytes operation
        unsigned int op1 = getDestination(dest, byte);  // Get first operand
        unsigned int op2 = getSource(sour, byte);       // Get second operand
        unsigned int res = doMath(op1, op2, func);      // Get result

        saveResult(dest, res, byte);                    // Save result to destination

    }
    /* Memory operation */
    else if (isMem(IR)) {
        unsigned char method = memMethod(IR);           // Get address method
        unsigned char reg = memACC(IR);                 // 1-byte or 2-bytes operation
        if (IR & 0x08) {                                // Select direcion
            memLoad(reg, method);
        }
        else {
            memStore(reg, method);
        }
    }
    /* Branch operation */
    else if (isBranch(IR)) {
        unsigned char type = branchType(IR);            // Get brach type
        unsigned new_adr = (MSB << 8) + LSB;            // Form new address
        brunch(type, new_adr);                          // Do branching
    }
}
