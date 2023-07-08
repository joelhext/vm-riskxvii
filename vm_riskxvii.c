#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "helper.h"
#include "operations.h"
#include "memory_handling.h"

// // Debugging purposes
// const char *operation_to_string(int operation) {
//     operation -= 1;
//     static const char *instructions[] = {
//         "ADD", "ADDI",  "SUB", "LUI",
//         "XOR", "XORI", "OR", "ORI", "AND", "ANDI",
//         "SLL", "SRL", "SRA", "LB", "LH",
//         "LW", "LBU", "LHU", "SB", "SH", "SW",
//         "SLT", "SLTI", "SLTU", "SLTIU", "BEQ", "BNE",
//         "BLT", "BLTU", "BGE", "BGEU", "JAL", "JALR"
//     };

//     if (operation >= 0 && operation < sizeof(instructions) / sizeof(instructions[0])) {
//         return instructions[operation];
//     } else {
//         return "Unknown";
//     }
// }


int main(int argc, char *argv[]) {
    // exit if there is not exactly 1 arg
    if (argc != 2) {
        printf("Usage: ./vm_riskxvii <arg>\n");
        return 1;
    }

    struct blob *blob = (struct blob *)malloc(sizeof(struct blob));

    // argument is the path to a binary file, open it
    FILE *file = fopen(argv[1], "r");
    // exit if file is null
    if (file == NULL) {
        printf("Could not open file.\n");
        free(blob);
        return 1;
    }

    size_t readCount;

    // read the first 1024 bytes into instruction memory
    readCount = fread(blob->inst_mem, 1, INST_MEM_SIZE, file);
    if(readCount != INST_MEM_SIZE){
        printf("Error: Unable to read instruction memory from file.\n");
        free(blob);
        fclose(file);
        return 1;
    }

    // read the next 1024 bytes into data memory
    readCount = fread(blob->data_mem, 1, DATA_MEM_SIZE, file);
    if(readCount != DATA_MEM_SIZE){
        printf("Error: Unable to read data memory from file.\n");
        free(blob);
        fclose(file);
        return 1;
    }
    
    fclose(file);

    // initialize the program counter (PC) and other necessary variables
    // note that memory and instructions have been initialised already
    int pc = 0;
    char *virt_mem = (char *)malloc(VIRT_MEM_SIZE * sizeof(char));
    int *reg_bank = (int *)malloc(REG_BANK_SIZE * sizeof(int));
    for (int i = 0; i < REG_BANK_SIZE; i++) {
        reg_bank[i] = 0;
    }
    for (int i=0; i<VIRT_MEM_SIZE; i++) {
        virt_mem[i] = 0;
    }

    // linked-list storing the heap bank
    // head points to NULL
    MemoryBank *head = NULL;

    int is_terminated = 0;

    // main loop
    while (!is_terminated) {
        // Get the instruction at the current PC
        int instruction = get_instruction(blob->inst_mem, pc);

        // Decode the instruction
        struct decoded_instruction inst = decode_instruction(instruction);

        // // Debugging purposes
        // printf("%02x\t%2d\t\t%2d\t%4s\t\tx%2d\t%08x %4d\t\tx%2d\t%08x %4d\t\tx%2d\t%08x %4d\t\ti  %d\n", 
        //         pc, pc, inst.operation, operation_to_string(inst.operation), 
        //         inst.rs1, reg_bank[inst.rs1], reg_bank[inst.rs1],
        //         inst.rs2, reg_bank[inst.rs2], reg_bank[inst.rs2],
        //         inst.rd, reg_bank[inst.rd], reg_bank[inst.rd],
        //         inst.imm);

        // if inst.rd, inst.rs1, inst.rs2 are out of bounds
        if (inst.rd > 31 || inst.rs1 > 31 || inst.rs2 > 31) {
            inst.operation = 500;
        }

        // Memory access operations
        if (inst.operation > 13 && inst.operation < 22) {
            int address = reg_bank[inst.rs1] + inst.imm;
            if (memory_operation_handling(
                address, 
                reg_bank, 
                blob->data_mem,
                inst.rs2, 
                &pc, 
                virt_mem, 
                &(inst.operation),
                &head
            )) {
                inst.operation = 500;
            } 
            // CPU Halt Requested - termination without errors!
            else if (address == 0x080C) {
                break;
            }
        }

        // Program flow operation error handling
        if (inst.operation > 21 && inst.operation < 33) {
            if (pc + inst.imm < 0 ||
                pc + inst.imm > INST_MEM_SIZE ||
                inst.imm % 4 != 0) 
            {
                inst.operation = 500;
            }
        }

        // Perform the operation
        switch (inst.operation) {
            /*
                ARITHMETIC AND LOGIC OPERATIONS
            */
            case 1: // ADD
                add(reg_bank, inst.rd, inst.rs1, inst.rs2);
                break;
            case 2: // ADDI
                addi(reg_bank, inst.rd, inst.rs1, inst.imm);
                break;
            case 3: // SUB
                sub(reg_bank, inst.rd, inst.rs1, inst.rs2);
                break;
            case 4: // LUI
                lui(reg_bank, inst.rd, inst.imm);
                break;
            case 5: // XOR
                xor_reg(reg_bank, inst.rd, inst.rs1, inst.rs2);
                break;
            case 6: // XORI
                xori(reg_bank, inst.rd, inst.rs1, inst.imm);
                break;
            case 7: // OR
                or_reg(reg_bank, inst.rd, inst.rs1, inst.rs2);
                break;
            case 8: // ORI
                ori(reg_bank, inst.rd, inst.rs1, inst.imm);
                break;
            case 9: // AND
                and_reg(reg_bank, inst.rd, inst.rs1, inst.rs2);
                break;
            case 10: // ANDI
                andi(reg_bank, inst.rd, inst.rs1, inst.imm);
                break;
            case 11: // SLL
                sll(reg_bank, inst.rd, inst.rs1, inst.rs2);
                break;
            case 12: // SRL
                srl(reg_bank, inst.rd, inst.rs1, inst.rs2);
                break;
            case 13: // SRA
                sra(reg_bank, inst.rd, inst.rs1, inst.rs2);
                break;
            /*
                MEMORY ACCESS OPERATIONS
            */
            case 14: // LB
                lb(reg_bank, blob->data_mem, inst.rd, inst.rs1, inst.imm);
                break;
            case 15: // LH
                lh(reg_bank, blob->data_mem, inst.rd, inst.rs1, inst.imm);
                break;
            case 16: // LW
                lw(reg_bank, blob->data_mem, inst.rd, inst.rs1, inst.imm);
                break;
            case 17: // LBU
                lbu(reg_bank, blob->data_mem, inst.rd, inst.rs1, inst.imm);
                break;
            case 18: // LHU
                lhu(reg_bank, blob->data_mem, inst.rd, inst.rs1, inst.imm);
                break;
            case 19: // SB
                sb(reg_bank, blob->data_mem, inst.rs1, inst.rs2, inst.imm);
                break;
            case 20: // SH
                sh(reg_bank, blob->data_mem, inst.rs1, inst.rs2, inst.imm);
                break;
            case 21: // SW
                sw(reg_bank, blob->data_mem, inst.rs1, inst.rs2, inst.imm);
                break;
            /*
                PROGRAM FLOW OPERATIONS
            */
            case 22: // SLT
                slt(reg_bank, inst.rd, inst.rs1, inst.rs2);
                break;
            case 23: // SLTI
                slti(reg_bank, inst.rd, inst.rs1, inst.imm);
                break;
            case 24: // SLTU
                sltu(reg_bank, inst.rd, inst.rs1, inst.rs2);
                break;
            case 25: // SLTIU
                sltiu(reg_bank, inst.rd, inst.rs1, inst.imm);
                break;
            case 26: // BEQ
                beq(reg_bank, &pc, inst.rs1, inst.rs2, inst.imm);
                break;
            case 27: // BNE
                bne(reg_bank, &pc, inst.rs1, inst.rs2, inst.imm);
                break;
            case 28: // BLT
                blt(reg_bank, &pc, inst.rs1, inst.rs2, inst.imm);
                break;
            case 29: // BLTU
                bltu(reg_bank, &pc, inst.rs1, inst.rs2, inst.imm);
                break;
            case 30: // BGE
                bge(reg_bank, &pc, inst.rs1, inst.rs2, inst.imm);
                break;
            case 31: // BGEU
                bgeu(reg_bank, &pc, inst.rs1, inst.rs2, inst.imm);
                break;
            case 32: // JAL
                jal(reg_bank, &pc, inst.rd, inst.imm);
                break;
            case 33: // JALR
                jalr(reg_bank, &pc, inst.rd, inst.rs1, inst.imm);
                break;
            /* 
                VIRTUAL ROUTINES
            */
            case 100: // Virtual Routine, already exectued
                break;
            case 114: // LB
                lb(reg_bank, virt_mem, inst.rd, inst.rs1, inst.imm - 0x0400);
                break;
            case 115: // LH
                lh(reg_bank, virt_mem, inst.rd, inst.rs1, inst.imm - 0x0400);
                break;
            case 116: // LW
                lw(reg_bank, virt_mem, inst.rd, inst.rs1, inst.imm - 0x0400);
                break;
            case 117: // LBU
                lbu(reg_bank, virt_mem, inst.rd, inst.rs1, inst.imm - 0x0400);
                break;
            case 118: // LHU
                lhu(reg_bank, virt_mem, inst.rd, inst.rs1, inst.imm - 0x0400);
                break;
            /*
                MEMORY ACCESS TO INSTRUCTION MEMORY
            */
            case 214: // LB
                lb(reg_bank, blob->inst_mem, inst.rd, inst.rs1, inst.imm + 0x0400);
                break;
            case 215: // LH
                lh(reg_bank, blob->inst_mem, inst.rd, inst.rs1, inst.imm + 0x0400);
                break;
            case 216: // LW
                lw(reg_bank, blob->inst_mem, inst.rd, inst.rs1, inst.imm + 0x0400);
                break;
            case 217: // LBU
                lbu(reg_bank, blob->inst_mem, inst.rd, inst.rs1, inst.imm + 0x0400);
                break;
            case 218: // LHU
                lhu(reg_bank, blob->inst_mem, inst.rd, inst.rs1, inst.imm + 0x0400);
                break;
            /*
                MEMORY ACCESS TO HEAP BANK
            */
            case 400: // Malloc or free
                break;
            case 414: // LB
                if (lb_heap(reg_bank, head, inst.rd, inst.rs1, inst.imm)) {
                    break;
                }
            case 415: // LH
                if (!lh_heap(reg_bank, head, inst.rd, inst.rs1, inst.imm)) {
                    break;
                }
            case 416: // LW
                if (!lw_heap(reg_bank, head, inst.rd, inst.rs1, inst.imm)) {
                    break;
                }
            case 417: // LBU
                if (!lbu_heap(reg_bank, head, inst.rd, inst.rs1, inst.imm)) {
                    break;
                }
            case 418: // LHU
                if (!lhu_heap(reg_bank, head, inst.rd, inst.rs1, inst.imm)) {
                    break;
                }
            case 419: // SB
                if (!sb_heap(reg_bank, head, inst.rs1, inst.rs2, inst.imm)) {
                    break;
                }
            case 420: // SH
                if (!sh_heap(reg_bank, head, inst.rs1, inst.rs2, inst.imm)) {
                    break;
                }
            case 421: // SW
                if (!sw_heap(reg_bank, head, inst.rs1, inst.rs2, inst.imm)) {
                    break;
                }
            /*
                DEFAULT
            */
            default:
                // Heap bank issues
                if ((inst.operation > 399 && inst.operation < 422) || inst.operation == 500) {
                    printf("Illegal Operation: 0x%08x\n", instruction);
                }
                else {
                    printf("Instruction Not Implemented: 0x%08x\n", instruction);
                }
                printf("PC = 0x%08x;\n", pc);
                for (int i=0; i<32; i++) {
                    printf("R[%d] = 0x%08x;\n", i, reg_bank[i]);
                }
                error_and_free(reg_bank, blob, virt_mem, head);
        }

        pc += 4;
        reg_bank[0] = 0;

        if (pc >= INST_MEM_SIZE) {
            is_terminated = 1;
        }
    }

    // Program finished without errors
    free(reg_bank);
    free(blob);
    free(virt_mem);
    heap_free_all(head);
    return 0;

}