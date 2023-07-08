#ifndef MEMORY_HANDLING_H
#define MEMORY_HANDLING_H

#define NUM_BANKS 128
#define BANK_SIZE 64
#define BASE_ADDR 0xb700

typedef struct MemoryBank {
    char data[BANK_SIZE];
    int start_address;
    char allocated;
    // next_in_chunk is 1 if the next bank is part of the same chunk
    char next_in_chunk;
    struct MemoryBank *next;
} MemoryBank;

/*
    Handles (in order): 
        - virtual routines
        - valid address checking
        - adjustments for memory operations and heap bank operations
*/
int memory_operation_handling(
    int address, 
    int *reg_bank, 
    char *data_mem, 
    int rs2, 
    int *pc, 
    char *virt_mem, 
    int *operation,
    MemoryBank **head
);

// Malloc implementation for the heap bank
int heap_malloc(MemoryBank **head, int size);

// Returns a pointer to the MemoryBank of that address
MemoryBank *heap_get_ptr(MemoryBank *head, int address);


#endif // MEMORY_HANDLING_H