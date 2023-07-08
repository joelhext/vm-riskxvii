#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "memory_handling.h"

/*

    Note: the stdint library has been used for some variables to ensure
    the correct number of bits 

    Note: although the spec says imm << 12 for lui, this has already
    been performed in the binary of the input instructions.
    This is the case for all imm << n

    Note: storage is little-endian for memory operations

    Note: in program-flow operations, the program counter is reduced by 4
    to offest the increase by 4 in the main loop

    Note: the heap access operations that use more than 1 byte require 
    special implementations to handle the cases where the heap access 
    overflows to the next chunk

*/

/*

    ARITHMETIC AND LOGIC OPERATIONS

*/

// Add
void add(int *reg_bank, int rd, int rs1, int rs2) {
    reg_bank[rd] = reg_bank[rs1] + reg_bank[rs2];
}

// Add immediate
void addi(int *reg_bank, int rd, int rs1, int imm) {
    reg_bank[rd] = reg_bank[rs1] +imm;
}

// Subtract
void sub(int *reg_bank, int rd, int rs1, int rs2) {
    reg_bank[rd] = reg_bank[rs1] - reg_bank[rs2];
}

// Load upper immediate
void lui(int *reg_bank, int rd, int imm) {
    reg_bank[rd] = imm;
}

// Exclusive or
void xor_reg(int *reg_bank, int rd, int rs1, int rs2) {
    int32_t *reg = (int32_t *) reg_bank;
    reg[rd] = reg[rs1] ^ reg[rs2];
}

// Exclusive or immediate
void xori(int *reg_bank, int rd, int rs1, int imm) {
    int32_t *reg = (int32_t *) reg_bank;
    reg[rd] = reg[rs1] ^ imm;
}

// Or
void or_reg(int *reg_bank, int rd, int rs1, int rs2) {
    int32_t *reg = (int32_t *) reg_bank;
    reg[rd] = reg[rs1] | reg[rs2];
}

// Or immediate
void ori(int *reg_bank, int rd, int rs1, int imm) {
    int32_t *reg = (int32_t *) reg_bank;
    reg[rd] = reg[rs1] | imm;
}

// And
void and_reg(int *reg_bank, int rd, int rs1, int rs2) {
    int32_t *reg = (int32_t *) reg_bank;
    reg[rd] = reg[rs1] & reg[rs2];
}

// And immediate
void andi(int *reg_bank, int rd, int rs1, int imm) {
    int32_t *reg = (int32_t *) reg_bank;
    reg[rd] = reg[rs1] & imm;
}

// Shift left logical
void sll(int *reg_bank, int rd, int rs1, int rs2) {
    int32_t *reg = (int32_t *) reg_bank;
    reg[rd] = reg[rs1] << (reg[rs2] & 0x1F);
}

// Shift right logical
void srl(int *reg_bank, int rd, int rs1, int rs2) {
    int32_t *reg = (int32_t *) reg_bank;
    reg[rd] = (uint32_t) reg[rs1] >> (reg[rs2] & 0x1F);
}

// Shift right arithmetic
void sra(int *reg_bank, int rd, int rs1, int rs2) {
    int32_t *reg = (int32_t *) reg_bank;
    reg[rd] = reg[rs1] >> (reg[rs2] & 0x1F);
}

/*

    MEMORY ACCESS OPERATIONS

*/

// Load byte
void lb(int *reg_bank, char *data_mem, int rd, int rs1, int imm) {
    int32_t *reg = (int32_t *) reg_bank;
    uint16_t address = reg[rs1] + imm;
    // memory starts at 0x0400 in our VM, however we need
    // to adjust for the variables stored locally in our C
    address = address - 0x0400;
    reg[rd] = (int8_t)data_mem[address];
}

// Load half word
void lh(int *reg_bank, char *data_mem, int rd, int rs1, int imm) {
    int32_t *reg = (int32_t *) reg_bank;
    uint16_t address = reg[rs1] + imm;
    address = address - 0x0400;
    unsigned char *data_mem_unsigned = (unsigned char *)data_mem;
    reg[rd] = 
        (int16_t)(data_mem_unsigned[address] | 
        (data_mem_unsigned[address + 1] << 8));
}

// Load word
void lw(int *reg_bank, char *data_mem, int rd, int rs1, int imm) {
    int32_t *reg = (int32_t *) reg_bank;
    int32_t address = reg[rs1] + imm;
    address = address - 0x0400;
    unsigned char *data_mem_unsigned = (unsigned char *)data_mem;
    reg[rd] = 
        (data_mem_unsigned[address] | 
        (data_mem_unsigned[address + 1] << 8) | 
        (data_mem_unsigned[address + 2] << 16) | 
        (data_mem_unsigned[address + 3] << 24));
}

// Load byte unsigned
void lbu(int *reg_bank, char *data_mem, int rd, int rs1, int imm) {
    int32_t *reg = (int32_t *) reg_bank;
    uint16_t address = reg[rs1] + imm;
    address = address - 0x0400;
    reg[rd] = (uint8_t) data_mem[address];
}

// Load half word unsigned
void lhu(int *reg_bank, char *data_mem, int rd, int rs1, int imm) {
    int32_t *reg = (int32_t *) reg_bank;
    uint16_t address = reg[rs1] + imm;
    address = address - 0x0400;
    unsigned char *data_mem_unsigned = (unsigned char *)data_mem;
    reg[rd] = 
        (uint16_t)(data_mem_unsigned[address] | 
        (data_mem_unsigned[address + 1] << 8));
}

// Store byte
void sb(int *reg_bank, char *data_mem, int rs1, int rs2, int imm) {
    int32_t *reg = (int32_t *) reg_bank;
    uint16_t address = reg[rs1] + imm;
    address = address - 0x0400;
    data_mem[address] = reg[rs2] & 0xFF;
}

// Store half word
void sh(int *reg_bank, char *data_mem, int rs1, int rs2, int imm) {
    int32_t *reg = (int32_t *) reg_bank;
    uint16_t address = reg[rs1] + imm;
    address = address - 0x0400;
    data_mem[address] = (reg[rs2]) & 0xFF;
    data_mem[address + 1] = (reg[rs2] >> 8) & 0xFF;
}

// Store word
void sw(int *reg_bank, char *data_mem, int rs1, int rs2, int imm) {
    int32_t *reg = (int32_t *) reg_bank;
    uint16_t address = reg[rs1] + imm;
    address = address - 0x0400;
    data_mem[address] = (reg[rs2] & 0xFF);
    data_mem[address + 1] = (reg[rs2] >> 8) & 0xFF;
    data_mem[address + 2] = (reg[rs2] >> 16) & 0xFF;
    data_mem[address + 3] = (reg[rs2] >> 24) & 0xFF;
}

/*

    PROGRAM FLOW OPERATIONS

*/

// Set less than
void slt(int *reg_bank, int rd, int rs1, int rs2) {
    int32_t *reg = (int32_t *) reg_bank;
    reg[rd] = (reg[rs1] < reg[rs2]) ? 1 : 0;
}

// Set less than immediate
void slti(int *reg_bank, int rd, int rs1, int imm) {
    int32_t *reg = (int32_t *) reg_bank;
    reg[rd] = (reg[rs1] < imm) ? 1 : 0;
}

// Set less than unsigned
void sltu(int *reg_bank, int rd, int rs1, int rs2) {
    int32_t *reg = (int32_t *) reg_bank;
    reg[rd] = ((uint32_t) reg[rs1] < (uint32_t) reg[rs2]) ? 1 : 0;
}

// Set less than immediate unsigned
void sltiu(int *reg_bank, int rd, int rs1, int imm) {
    int32_t *reg = (int32_t *) reg_bank;
    reg[rd] = ((uint32_t) reg[rs1] < (uint32_t) imm) ? 1 : 0;
}

// Branch if equal
void beq(int *reg_bank, int *pc, int rs1, int rs2, int imm) {
    if (reg_bank[rs1] == reg_bank[rs2]) {
        *pc += imm;
        *pc = *pc - 4;  
    }
}

// Branch not equal
void bne(int *reg_bank, int *pc, int rs1, int rs2, int imm) {
    if (reg_bank[rs1] != reg_bank[rs2]) {
        *pc += imm;
        *pc = *pc - 4;
    }
}

// Branch less than
void blt(int *reg_bank, int *pc, int rs1, int rs2, int imm) {
    if (reg_bank[rs1] < reg_bank[rs2]) {
        *pc += imm;
        *pc = *pc - 4;
    }
}

// Branch greater than or equal to
void bge(int *reg_bank, int *pc, int rs1, int rs2, int imm) {
    if (reg_bank[rs1] >= reg_bank[rs2]) {
        *pc += imm;
        *pc = *pc - 4;
    }
}

// Branch less than unsigned
void bltu(int *reg_bank, int *pc, int rs1, int rs2, int imm) {
    uint32_t u_rs1 = (uint32_t)reg_bank[rs1];
    uint32_t u_rs2 = (uint32_t)reg_bank[rs2];
    if (u_rs1 < u_rs2) {
        *pc += imm;
        *pc = *pc - 4;
    }
}

// Branch greater than or equal to unsigned
void bgeu(int *reg_bank, int *pc, int rs1, int rs2, int imm) {
    uint32_t u_rs1 = (uint32_t)reg_bank[rs1];
    uint32_t u_rs2 = (uint32_t)reg_bank[rs2];
    if (u_rs1 >= u_rs2) {
        *pc += imm;
        *pc = *pc - 4;
    }
}

// Jump and Link
void jal(int *reg_bank, int *pc, int rd, int imm) {
    int32_t *registers = (int32_t *)reg_bank;
    registers[rd] = *pc + 4;
    *pc = *pc + imm;
    *pc = *pc - 4;
}

// Jump and Link Register
void jalr(int *reg_bank, int *pc, int rd, int rs1, int imm) {
    int32_t *registers = (int32_t *)reg_bank;
    int32_t next_pc = *pc + 4;
    *pc = registers[rs1] + imm;
    registers[rd] = next_pc;
    *pc = *pc - 4;
}


/*

    HEAP ACCESS OPERATIONS

*/

// Load byte
int lb_heap(int *reg_bank, MemoryBank *head, int rd, int rs1, int imm) {
    int32_t *reg = (int32_t *) reg_bank;
    int address = reg[rs1] + imm;
    MemoryBank *chunk = heap_get_ptr(head, address);
    if (chunk == NULL) {
        return 1;
    }
    reg[rd] = (uint8_t) chunk->data[address % BANK_SIZE];
    return 0;
}

// Load half word
int lh_heap(int *reg_bank, MemoryBank *head, int rd, int rs1, int imm) {
    int32_t *reg = (int32_t *) reg_bank;
    int address = reg[rs1] + imm;
    MemoryBank *chunk = heap_get_ptr(head, address);
    if (chunk == NULL) {
        return 1;
    }
    // if it overflows to the next chunk
    if (address % BANK_SIZE + 1 == BANK_SIZE) {
        MemoryBank *chunk2 = heap_get_ptr(head, address + 1);
        if (chunk2 == NULL) {
            return 1;
        }
        reg[rd] = 
            (int16_t)(chunk->data[BANK_SIZE - 1] | 
            (chunk2->data[0] << 8));
    }
    else {
        reg[rd] = 
            (chunk->data[address % BANK_SIZE] | 
            (chunk->data[address % BANK_SIZE + 1] << 8));
    }
    return 0;
}

// Load word
int lw_heap(int *reg_bank, MemoryBank *head, int rd, int rs1, int imm) {
    int32_t *reg = (int32_t *) reg_bank;
    int address = reg[rs1] + imm;
    unsigned char *data_mem_unsigned;
    uint32_t result = 0;
    MemoryBank *current_chunk;
    current_chunk = heap_get_ptr(head, address);
    for (int i = 0; i < 4; i++) {
        // if it overflows to the next chunk
        if (address % BANK_SIZE + i == BANK_SIZE) {
            current_chunk = heap_get_ptr(head, address + i);
        }
        if (current_chunk == NULL) {
            return 1;
        }
        data_mem_unsigned = (unsigned char *)current_chunk->data;
        result |= data_mem_unsigned[address % BANK_SIZE] << (8 * i);
        address++;
    }

    reg[rd] = result;
    return 0;
}

// Load byte unsigned
int lbu_heap(int *reg_bank, MemoryBank *head, int rd, int rs1, int imm) {
    int32_t *reg = (int32_t *) reg_bank;
    int address = reg[rs1] + imm;
    MemoryBank *chunk = heap_get_ptr(head, address);
    if (chunk == NULL) {
        return 1;
    }
    reg[rd] = (uint8_t) chunk->data[address % BANK_SIZE];
    return 0;
}

// Load half word unsigned
int lhu_heap(int *reg_bank, MemoryBank *head, int rd, int rs1, int imm) {
    int32_t *reg = (int32_t *) reg_bank;
    int address = reg[rs1] + imm;
    MemoryBank *chunk = heap_get_ptr(head, address);
    if (chunk == NULL) {
        return 1;
    }

    // if it overflows to the next chunk
    if (address % BANK_SIZE + 1 == BANK_SIZE) {
        MemoryBank *chunk2 = heap_get_ptr(head, address + 1);
        if (chunk2 == NULL) {
            return 1;
        }
        reg[rd] = 
            (int16_t)(chunk->data[BANK_SIZE - 1] | 
            (chunk2->data[0] << 8));
    }
    else {
        reg[rd] = 
            (int16_t)(chunk->data[address % BANK_SIZE] | 
            (chunk->data[address % BANK_SIZE + 1] << 8));
    }
    return 0;
}

// Store byte
int sb_heap(int *reg_bank, MemoryBank *head, int rs1, int rs2, int imm) {
    int32_t *reg = (int32_t *) reg_bank;
    uint16_t address = reg[rs1] + imm;
    MemoryBank *chunk = heap_get_ptr(head, address);
    if (chunk == NULL) {
        return 1;
    }
    chunk->data[address % BANK_SIZE] = reg[rs2] & 0xFF;
    return 0;
}

// Store half word
int sh_heap(int *reg_bank, MemoryBank *head, int rs1, int rs2, int imm) {
    int32_t *reg = (int32_t *) reg_bank;
    uint16_t address = reg[rs1] + imm;
    MemoryBank *chunk = heap_get_ptr(head, address);
    if (chunk == NULL) {
        return 1;
    }
    // if it overflows to the next chunk
    if (address % BANK_SIZE + 1 == BANK_SIZE) {
        MemoryBank *chunk2 = heap_get_ptr(head, address + 1);
        if (chunk2 == NULL) {
            return 1;
        }
        chunk->data[63] = reg[rs2] & 0xFF;
        chunk2->data[0] = (reg[rs2] >> 8) & 0xFF;
    }
    else {
        chunk->data[address % BANK_SIZE] = reg[rs2] & 0xFF;
        chunk->data[address % BANK_SIZE] = (reg[rs2] >> 8) & 0xFF;
    }
    return 0;
}

// Store word
int sw_heap(int *reg_bank, MemoryBank *head, int rs1, int rs2, int imm) {
    int32_t *reg = (int32_t *) reg_bank;
    uint16_t address = reg[rs1] + imm;
    MemoryBank *chunk = heap_get_ptr(head, address);
    if (chunk == NULL) {
        return 1;
    }
    int i=0;
    while (i < 4) {
        // if it overflows to the next chunk
        if (address % BANK_SIZE + i == BANK_SIZE) {
            chunk = heap_get_ptr(head, address + i);
        }
        chunk->data[address % BANK_SIZE + i] = (reg[rs2] >> (i*8)) & 0xFF;
        i++;
    }
    return 0;
}