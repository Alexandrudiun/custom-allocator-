// #include <stdio.h>
// #include <stdlib.h>

// #define BLOCK_SIZE 4096 // size of each memory block

// typedef struct memory_block {
//     size_t size;
//     int status; // 0 = free, 1 = allocated
//     struct memory_block *next;
// } memory_block;

// memory_block *head; // head of the linked list of memory blocks

// void init_memory_allocator(size_t size) {
//     // initialize the memory allocator with a single block of memory
//     head = (memory_block *)malloc(size);
//     head->size = size;
//     head->status = 0; // set the block as free
//     head->next = NULL;
// }

// memory_block *allocate_memory(size_t size) {
//     memory_block *current = head;
//     memory_block *prev = NULL;
//     while (current != NULL) {
//         if (current->status == 0 && current->size >= size) {
//             // found a suitable free block
//             current->status = 1; // mark the block as allocated
//             return current;
//         }
//         prev = current;
//         current = current->next;
//     }
//     printf("Error: Not enough memory\n");
//     return NULL;
// }

// void free_memory(memory_block *block) {
//     block->status = 0; // mark the block as free
//     memory_block *current = head;
//     memory_block *prev = NULL;
//     while (current != NULL) {
//         if (current->next != NULL && current->status == 0 && current->next->status == 0) {
//             // merge adjacent free blocks
//             current->size += current->next->size;
//             current->next = current->next->next;
//             free(current->next);
//         }
//         prev = current;
//         current = current->next;
//     }
// }

// int main() {
//     init_memory_allocator(BLOCK_SIZE * 10);
//     memory_block *block1 = allocate_memory(BLOCK_SIZE * 2);
//     memory_block *block2 = allocate_memory(BLOCK_SIZE * 3);
//     free_memory(block1);
//     memory_block *block3 = allocate_memory(BLOCK_SIZE);
//     return 0;
// }
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


// the new version


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define BLOCK_SIZE 4096 // size of each memory block

typedef struct memory_block {
    size_t size;
    int status; // 0 = free, 1 = allocated
    struct memory_block *next;
    struct memory_block *prev;
} memory_block;

memory_block *head; // head of the linked list of memory blocks
pthread_mutex_t lock;

void init_memory_allocator(size_t size) {
    // initialize the memory allocator with a single block of memory
    head = (memory_block *)malloc(size);
    head->size = size;
    head->status = 0; // set the block as free
    head->next = NULL;
    head->prev = NULL;
    pthread_mutex_init(&lock, NULL);
}

memory_block *allocate_memory(size_t size) {
    pthread_mutex_lock(&lock);
    memory_block *current = head;
    memory_block *prev = NULL;
    while (current != NULL) {
        if (current->status == 0 && current->size >= size) {
            // found a suitable free block
            if (current->size > size + sizeof(memory_block)) {
                // split the block to minimize fragmentation
                memory_block *new_block = (memory_block *)((char *)current + sizeof(memory_block) + size);
                new_block->size = current->size - sizeof(memory_block) - size;
                new_block->status = 0;
                new_block->next = current->next;
                new_block->prev = current;
                if (current->next != NULL) {
                    current->next->prev = new_block;
                }
                current->size = size;
                current->next = new_block;
            }
            current->status = 1; // mark the block as allocated
            pthread_mutex_unlock(&lock);
            return current;
        }
        prev = current;
        current = current->next;
    }
    pthread_mutex_unlock(&lock);
    printf("Error: Not enough memory\n");
    return NULL;
}

void free_memory(memory_block *block) {
    pthread_mutex_lock(&lock);
    block->status = 0; // mark the block as free
    if (block->prev != NULL && block->prev->status == 0) {
        // merge with previous free block
        block->prev->size += sizeof(memory_block) + block->size;
        block->prev->next = block->next;
        if (block->next != NULL) {
            block->next->prev = block->prev;
        }
        block = block->prev;
    }
    if (block->next != NULL && block->next->status == 0) {
        // merge with next free block
        block->size += sizeof(memory_block) + block->next->size;
        block->next = block->next->next;
        if (block->next != NULL) {
            block->next->prev = block;
        }
    }
    pthread_mutex_unlock(&lock);
}

void *handle_memory_leak() {
    // function that is called when the program exits to check for memory leaks
    memory_block *current = head;
    int leaks = 0;
    while (current != NULL) {
        if (current->status == 1) {
            leaks++;
            printf("Memory leak detected: block at %p of size %zu bytes\n", current, current->size);
        }
        current = current->next;
    }
    if (leaks == 0) {
        printf("No memory leaks detected\n");
    }
}

int main() {
    init_memory_allocator(BLOCK_SIZE * 10);
    atexit(handle_memory_leak);
    memory_block *block1 = allocate_memory(BLOCK_SIZE * 2);
    memory_block *block2 = allocate_memory(BLOCK_SIZE * 3);
    free_memory(block1);
    memory_block *block3 = allocate_memory(BLOCK_SIZE);
    return 0;
}
