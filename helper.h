#ifndef HELPER_H
#define HELPER_H

#include <stdint.h>

#include "memory_handling.h"

#define INST_MEM_SIZE 1024 // bytes
#define DATA_MEM_SIZE 1024 // bytes
#define VIRT_MEM_SIZE 256 // bytes
#define REG_BANK_SIZE 32 // ints
#define HEAP_SIZE 128 * 64 // bytes

// Struct to hold instruction and data memory
// (0x0000 - 0x03FF) and (0x0400 - 0x07FF)
struct blob {
    char inst_mem[INST_MEM_SIZE];
    char data_mem[DATA_MEM_SIZE];
};

// Struct to hold decoded instruction
// func3, func7 are not needed
struct decoded_instruction {
    uint32_t rd;
    uint32_t rs1;
    uint32_t rs2;
    int32_t operation;
    int32_t imm;
};

// Frees all memory banks in the linked list
void heap_free_all(MemoryBank *head);

//  Frees all malloc'd objects and exits with error code 1
void error_and_free(int* reg_bank, struct blob *b, char* virt_mem, MemoryBank* head);

// Returns the 32-bit instruction at the given pc
int get_instruction(char *inst_mem, int pc);

// Decodes the given 32-bit instruction
struct decoded_instruction decode_instruction(int instruction);

#endif // HELPER_H