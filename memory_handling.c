#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "memory_handling.h"

// frees a chunk of heap banks starting at the given address
static int heap_free(MemoryBank *head, int address);

int memory_operation_handling(
    int address, 
    int *reg_bank, 
    char *data_mem, 
    int rs2, 
    int *pc, 
    char *virt_mem, 
    int *operation,
    MemoryBank **head
) {
    // VM Memory Layout:
    // 0x0000 - 0x03FF: Instruction Memory
    // 0x0400 - 0x07FF: Data Memory
    // 0x0800 - 0x08FF: Virtual Routines
    // 0x0900 - 0xB6FF: Reserved
    // 0xB700+        : Heap Banks

    // // Debugging purposes
    // printf("Memory Operation: %d %08x\n", address, address);

    int value = reg_bank[rs2];
    // CASE: Virtual Routines
    switch (address) {
        case 0x0800: // Console Write Character
            putchar((char) value);
            *operation = 100;
            return 0;
        case 0x0804: // Console Write Signed Integer
            printf("%d", value);
            *operation = 100;
            return 0;
        case 0x0808: // Console Write Unsigned Integer
            printf("%x", (uint32_t) value);
            *operation = 100;
            return 0;
        case 0x080C: // Halt
            printf("CPU Halt Requested\n");
            return 0;
        case 0x0812: // Console Read Character
            virt_mem[0x0012] = getchar();
            *operation = *operation + 100;
            return 0;
        case 0x0816: // Console Read Signed Integer
        {
            int *temp = (int *) &virt_mem[0x016];
            scanf("%d", temp);
            *operation = *operation + 100;
            return 0;
        }
        case 0x0820: // Dump PC
            printf("%08x\n", *pc);
            *operation = 100;
            return 0;
        case 0x0824: // Dump Register Banks
            for (int i=0; i<32; i++) {
                // Print format found in 'Invalid 1' test case
                printf("R[%d] = 0x%08x;\n", i, reg_bank[i]);
            }
            *operation = 100;
            return 0;
        case 0x0828: // Dump Memory Word
        {
            int32_t mem_word = *((int32_t *)&data_mem[value]);
            printf("%08x\n", mem_word);
            int32_t *virt_mem_int = (int32_t *) &virt_mem[0x28];
            *virt_mem_int = mem_word;
            *operation = 100;
            return 0;
        }
        case 0x0830: // Malloc
        {
            // R[28] stores the pointer
            int starting_address = heap_malloc(head, reg_bank[rs2]);
            if (starting_address != 0) {
                reg_bank[28] = starting_address;
            } else {
                reg_bank[28] = 0;
            }
            *operation = 400;
            return 0;
        }
        case 0x0834: // Free
            *operation = 400;
            if (heap_free(*head, reg_bank[rs2])) {
                // Error code
                *operation = 500;
            }
            return 0;
        default:
            break;
    }

    // CASE: Error checks on address
    // out of bounds invalid
    if (address < 0x0000 || address >= 0xb700 + NUM_BANKS * BANK_SIZE) {
        return 1;
    }
    // invalid virtual routine
    else if (address > 0x0800 && address < 0x0900) {
        return 1;
    }
    // store operation to instruction memory invalid
    else if (address < 0x0400 && *operation > 18) {
        return 1;
    }
    // CASE: inst.operation modification (needed for main's switch)
    // load operation to instruction memory valid
    else if (address < 0x0400) {
        *operation += 200;
    }
    // heap bank valid
    else if (address >= 0xb700 && address < 0xb700 + NUM_BANKS * BANK_SIZE && *operation < 100) {
        *operation += 400;
    }
    // address >= 0x0400 && address < 0x0800 (data memory)
    return 0;
}


int heap_malloc(MemoryBank **head, int size) {
    MemoryBank *current = *head;
    MemoryBank *prev = NULL;
    // eg (100 + 64 - 1) / 64 = 2
    int required_banks = (size + BANK_SIZE - 1) / BANK_SIZE;
    int consecutive_banks = 0;
    if (required_banks > NUM_BANKS) {
        return 0;
    }

    // CASE: List is empty
    // will allocate and return if so
    if (*head == NULL) {
        *head = (MemoryBank *)malloc(sizeof(MemoryBank));
        memset((*head)->data, 0, BANK_SIZE);
        (*head)->start_address = BASE_ADDR;
        (*head)->allocated = 1;
        (*head)->next = NULL;
        (*head)->next_in_chunk = 1;
        prev = *head;
        for (int i = 1; i < required_banks; i++) {
            current = (MemoryBank *)malloc(sizeof(MemoryBank));
            memset(current->data, 0, BANK_SIZE);
            current->start_address = prev->start_address + BANK_SIZE;
            current->allocated = 1;
            current->next = NULL;
            current->next_in_chunk = 1;
            prev->next = current;
            prev = current;
        }
        // mark the cutoff for this stretch of chunks
        prev->next_in_chunk = 0;
        return BASE_ADDR;
    }

    // CASE: existing LL has enough consecutive banks:
    //       ie consecutive banks have been previously allocated then freed
    // will allocate and return if so
    while (current != NULL) {
        if (!current->allocated) {
            consecutive_banks++;
            // There is an empty consecutive stretch, allocate
            if (consecutive_banks == required_banks) {
                prev = prev->next;
                for (int i = 0; i < required_banks; i++) {
                    prev->allocated = 1;
                    prev->next_in_chunk = 1;
                    // we already know prev->data will be filled with 0s,
                    // since this was previously allocated then 'freed'
                    prev = prev->next;
                }
                // now, current = prev
                // mark the cutoff for this stretch of chunks
                current->next_in_chunk = 0;
                // return address of the start of the first bank
                return current->start_address - (required_banks - 1) * BANK_SIZE;
            }
        } else {
            consecutive_banks = 0;
            // prev stores the start of the consecutive banks (not necessarily 'previous')
            prev = current;   
        }

        current = current->next;
    }

    // CASE: existing LL does not have enough consecutive banks
    //       note that this also marks the case where the end of the LL
    //       has empty banks, but less than the required amount
    // will extend the LL, allocate, then return if so
    // in the following examples, prev will be A (the last allocated node)
    // H - ... - A - N
    // H - ... - A - U - N
    // H - ... - A - U - U - U - N
    if (prev->start_address + BANK_SIZE * required_banks < BASE_ADDR + NUM_BANKS * BANK_SIZE) {
        // Allocate each bank
        for (int i = 0; i < required_banks; i++) {
            MemoryBank *new_bank;
            // end of LL
            if (prev->next == NULL) {
                new_bank = (MemoryBank *)malloc(sizeof(MemoryBank));
            } 
            // previously allocated then freed
            else {
                new_bank = prev->next;
            }
            memset(new_bank->data, 0, BANK_SIZE);
            new_bank->start_address = prev->start_address + BANK_SIZE;
            new_bank->allocated = 1;
            new_bank->next = NULL;
            new_bank->next_in_chunk = 1;
            prev->next = new_bank;
            prev = new_bank;
        }
        prev->next_in_chunk = 0;
        return prev->start_address - (required_banks - 1) * BANK_SIZE;
    }

    // CASE: sufficient space does not exist, allocation failed
    return 0;
}

static int heap_free(MemoryBank *head, int address) {
    MemoryBank *current = head;
    address = address - (address % BANK_SIZE);
    // should be the start address of the bank
    while (current != NULL) {
        if (current->start_address == address) {
            // Found the starting bank
            while (current != NULL && current->allocated) {
                current->allocated = 0;
                // fill char array with 0s
                memset(current->data, 0, BANK_SIZE);
                current = current->next;
                if (current != NULL && current->next_in_chunk == 0) {
                    break;
                }
            }
            return 0;
        }
        current = current->next;
    }
    return 1;
}

// Returns a pointer to the memory address
MemoryBank *heap_get_ptr(MemoryBank* head, int address) {
    if (address < BASE_ADDR || address >= BASE_ADDR + NUM_BANKS * BANK_SIZE) {
        return NULL;
    }

    address = address - (address % BANK_SIZE);

    MemoryBank *current = head;

    if (head == NULL) {
        return NULL;
    }

    while (current != NULL) {
        if (address == current->start_address) {
            if (!current->allocated) {
                return NULL;
            }
            // return a pointer to the start of the chunk related in memory
            return current;
        }
        current = current->next;
    }

    return NULL;
}
