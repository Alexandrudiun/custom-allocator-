#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE 4096 // size of each memory block

typedef struct memory_block {
    size_t size;
    int status; // 0 = free, 1 = allocated
    struct memory_block *next;
} memory_block;

memory_block *head; // head of the linked list of memory blocks

void init_memory_allocator(size_t size) {
    // initialize the memory allocator with a single block of memory
    head = (memory_block *)malloc(size);
    head->size = size;
    head->status = 0; // set the block as free
    head->next = NULL;
}

memory_block *allocate_memory(size_t size) {
    memory_block *current = head;
    memory_block *prev = NULL;
    while (current != NULL) {
        if (current->status == 0 && current->size >= size) {
            // found a suitable free block
            current->status = 1; // mark the block as allocated
            return current;
        }
        prev = current;
        current = current->next;
    }
    printf("Error: Not enough memory\n");
    return NULL;
}

void free_memory(memory_block *block) {
    block->status = 0; // mark the block as free
    memory_block *current = head;
    memory_block *prev = NULL;
    while (current != NULL) {
        if (current->next != NULL && current->status == 0 && current->next->status == 0) {
            // merge adjacent free blocks
            current->size += current->next->size;
            current->next = current->next->next;
            free(current->next);
        }
        prev = current;
        current = current->next;
    }
}

int main() {
    init_memory_allocator(BLOCK_SIZE * 10);
    memory_block *block1 = allocate_memory(BLOCK_SIZE * 2);
    memory_block *block2 = allocate_memory(BLOCK_SIZE * 3);
    free_memory(block1);
    memory_block *block3 = allocate_memory(BLOCK_SIZE);
    return 0;
}
