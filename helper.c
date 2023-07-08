#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "memory_handling.h"
#include "helper.h"

void heap_free_all(MemoryBank* head) {
    MemoryBank *current = head;
    while (current != NULL) {
        MemoryBank *next = current->next;
        free(current);
        current = next;
    }
}

void error_and_free(int* reg_bank, struct blob *b, char* virt_mem, MemoryBank *head) {
    free(reg_bank);
    free(b);
    free(virt_mem);
    heap_free_all(head);
    exit(1);
}

int get_instruction(char *inst_mem, int pc) {
    int instruction = 0;
    // Assuming little-endian byte order 
    for (int i = 0; i < 4; ++i) {
        instruction |= ((unsigned char)inst_mem[pc + i]) << (8 * i);
    }
    return instruction;
}

// Separate function may take slightly more runtime,
// but it increases readability
static const int get_operation_number(int instruction) {
    uint32_t opcode = instruction & 0x7F;
    uint32_t func3 = (instruction >> 12) & 0x7;
    uint32_t func7 = (instruction >> 25) & 0x7F;
    switch (opcode) {
        case 0b0110011:
            switch (func3) {
                case 0b000:
                    // add : sub
                    return (func7 == 0b0000000) ? 1 : 3 ;
                case 0b100:
                    return 5; // xor
                case 0b110:
                    return 7; // or
                case 0b111:
                    return 9; // and
                case 0b001:
                    return 11; // sll
                case 0b101:
                    // srl : sra
                    return (func7 == 0b0000000) ? 12 : 13;
                case 0b010:
                    return 22; // slt
                case 0b011:
                    return 24; // sltu
            }
            break;
        case 0b0010011:
            switch (func3) {
                case 0b000:
                    return 2; // addi
                case 0b100:
                    return 6; // xori
                case 0b110:
                    return 8; // ori
                case 0b111:
                    return 10; // andi
                case 0b010:
                    return 23; // slti
                case 0b011:
                    return 25; // sltiu
            }
            break;
        case 0b0000011:
            switch (func3) {
                case 0b000:
                    return 14; // lb
                case 0b001:
                    return 15; // lh
                case 0b010:
                    return 16; // lw
                case 0b100:
                    return 17; // lbu
                case 0b101:
                    return 18; // lhu
            }
            break;
        case 0b0100011:
            switch (func3) {
                case 0b000:
                    return 19; // sb
                case 0b001:
                    return 20; // sh
                case 0b010:
                    return 21; // sw
            }
            break;
        case 0b1100011:
            switch (func3) {
                case 0b000:
                    return 26; // beq
                case 0b001:
                    return 27; // bne
                case 0b100:
                    return 28; // blt
                case 0b110:
                    return 29; // bltu
                case 0b101:
                    return 30; // bge
                case 0b111:
                    return 31; // bgeu
            }
            break;
        case 0b0110111:
            return 4; // lui
        case 0b1101111:
            return 32; // jal
        case 0b1100111:
            return 33; // jalr
    }

    return -1; // unknown
}

struct decoded_instruction decode_instruction(int instruction) {
    struct decoded_instruction decoded;
    uint32_t opcode = instruction & 0x7F;
    decoded.rd = (instruction >> 7) & 0x1F;
    decoded.rs1 = (instruction >> 15) & 0x1F;
    decoded.rs2 = (instruction >> 20) & 0x1F;
    // Error handling for invalid operation is in main's switch
    // func3, func7 used here exclusively
    decoded.operation = get_operation_number(instruction);

    switch (opcode) {
        case 0b0010011: // I-type
        case 0b0000011:
        case 0b1100111:
            decoded.imm = (instruction >> 20) & 0xFFF;
            // Sign extension
            if (decoded.imm & 0x800) {
                decoded.imm |= 0xFFFFF000;
            }
            break;
        case 0b0110011: // R-type
            decoded.imm = 0;
            break;
        case 0b0100011: // S-type
            decoded.imm = ((instruction >> 25) & 0x7F) << 5 |     // 11:5
                          ((instruction >> 7) & 0x1F);            // 4:0
            // Sign extension
            if (decoded.imm & 0x800) {
                decoded.imm |= 0xFFFFF000;
            }
            break;
        case 0b1100011: // SB-type
            decoded.imm = ((instruction >> 31) & 0x1) << 12 |     // 12
                          ((instruction >> 25) & 0x3F) << 5 |     // 10:5
                          ((instruction >> 8) & 0xF) << 1 |       // 4:1
                          ((instruction >> 7) & 0x1) << 11;       // 11
            // Sign extension
            if (decoded.imm & 0x1000) {
                decoded.imm |= 0xFFFFE000;
            }
            break;
        case 0b0110111: // U-type
        case 0b0010111:
            decoded.imm = (instruction & 0xFFFFF000);
            break;
        case 0b1101111: // UJ-type
            decoded.imm = ((instruction >> 31) & 0x1) << 20 |     // 20
                          ((instruction >> 21) & 0x3FF) << 1 |    // 10:1     
                          ((instruction >> 20) & 0x1) << 11 |     // 11
                          ((instruction >> 12) & 0xFF) << 12;     // 19:12
            // Sign extension
            if (decoded.imm & 0x100000) {
                decoded.imm |= 0xFFE00000;
            }
            break;
        default:
            decoded.imm = 0;
            break;
    }

    return decoded;
}
