#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <stdint.h>

/*
    All functions in this file correspond to their RISK-XVII
    implementation, with a few exceptions:
        - xor_reg, or_reg, and_reg are simply xor, or, and.
        - *_heap functions are memory access operations used to
        access the heap. Since they require a pointer to the head
        of the linked list of the heap, they have a separate
        implementation.


    M is memory
    R is registers
    rd is destination register
    rs1 is source register 1
    rs2 is source register 2
    imm is immediate

    Hierachy of input arguments:
    R > M > rd > rs1 > rs2 > imm
*/

/* ARITHMETIC AND LOGIC OPERATIONS */
void add(int *reg_bank, int rd, int rs1, int rs2);
void addi(int *reg_bank, int rd, int rs1, int imm);
void sub(int *reg_bank, int rd, int rs1, int rs2);
void lui(int *reg_bank, int rd, int imm);
void xor_reg(int *reg_bank, int rd, int rs1, int rs2);
void xori(int *reg_bank, int rd, int rs1, int imm);
void or_reg(int *reg_bank, int rd, int rs1, int rs2);
void ori(int *reg_bank, int rd, int rs1, int imm);
void and_reg(int *reg_bank, int rd, int rs1, int rs2);
void andi(int *reg_bank, int rd, int rs1, int imm);
void sll(int *reg_bank, int rd, int rs1, int rs2);
void srl(int *reg_bank, int rd, int rs1, int rs2);
void sra(int *reg_bank, int rd, int rs1, int rs2);
/* MEMORY ACCESS OPERATIONS */
void lb(int *reg_bank, char *data_mem, int rd, int rs1, int imm);
void lh(int *reg_bank, char *data_mem, int rd, int rs1, int imm);
void lw(int *reg_bank, char *data_mem, int rd, int rs1, int imm);
void lbu(int *reg_bank, char *data_mem, int rd, int rs1, int imm);
void lhu(int *reg_bank, char *data_mem, int rd, int rs1, int imm);
void sb(int *reg_bank, char *data_mem, int rs1, int rs2, int imm);
void sh(int *reg_bank, char *data_mem, int rs1, int rs2, int imm);
void sw(int *reg_bank, char *data_mem, int rs1, int rs2, int imm);
/* PROGAM FLOW OPERATIONS */
void slt(int *reg_bank, int rd, int rs1, int rs2);
void slti(int *reg_bank, int rd, int rs1, int imm);
void sltu(int *reg_bank, int rd, int rs1, int rs2);
void sltiu(int *reg_bank, int rd, int rs1, int imm);
void beq(int *reg_bank, int *pc, int rs1, int rs2, int imm);
void bne(int *reg_bank, int *pc, int rs1, int rs2, int imm);
void blt(int *reg_bank, int *pc, int rs1, int rs2, int imm);
void bge(int *reg_bank, int *pc, int rs1, int rs2, int imm);
void bltu(int *reg_bank, int *pc, int rs1, int rs2, int imm);
void bgeu(int *reg_bank, int *pc, int rs1, int rs2, int imm);
void jal(int *reg_bank, int *pc, int rd, int imm);
void jalr(int *reg_bank, int *pc, int rd, int rs1, int imm);
/* HEAP ACCESS OPERATIONS */
int lb_heap(int *reg_bank, MemoryBank *head, int rd, int rs1, int imm);
int lh_heap(int *reg_bank, MemoryBank *head, int rd, int rs1, int imm);
int lw_heap(int *reg_bank, MemoryBank *head, int rd, int rs1, int imm);
int lbu_heap(int *reg_bank, MemoryBank *head, int rd, int rs1, int imm);
int lhu_heap(int *reg_bank, MemoryBank *head, int rd, int rs1, int imm);
int sb_heap(int *reg_bank, MemoryBank *head, int rs1, int rs2, int imm);
int sh_heap(int *reg_bank, MemoryBank *head, int rs1, int rs2, int imm);
int sw_heap(int *reg_bank, MemoryBank *head, int rs1, int rs2, int imm);

#endif // OPERATIONS_H