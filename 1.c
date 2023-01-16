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
// The code does not handle fragmentation of memory, it only merges adjacent free blocks, but does not split allocated blocks to minimize fragmentation.
// The code does not handle memory alignment, it could be a problem for some systems.
// The code does not handle out of memory situations, it only prints an error message and returns NULL, but it could be improved by implementing a custom memory allocation failure handler.
// The code does not handle multi-threading, it could cause race conditions if multiple threads are trying to allocate or free memory at the same time.
// The code does not handle memory leaks, it would be helpful to have some mechanism to track the allocated blocks and check if they have been freed before the program exits.
// The code does not handle cases when the user requests memory more than the size of the memory block.
// The code does not handle cases when the user requests memory less than the size of the memory block.
// The code does not handle cases when the user requests memory less than the size of the memory block, but the memory block is smaller than the requested size.
// The code does not handle cases when the user requests memory more than the size of the memory block, but the memory block is larger than the requested size.
// The code does not handle cases when the user requests memory equal to the size of the memory block.
