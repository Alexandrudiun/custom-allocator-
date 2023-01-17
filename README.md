# memory-allocator

1. This code is an implementation of a simple memory allocator that uses a linked list to manage blocks of memory. The memory allocator is thread-safe, using a pthread_mutex_t lock to ensure only one thread can access the memory at a time.

2. The init_memory_allocator function initializes the memory allocator with a single block of memory of the specified size. The allocate_memory function takes a size as an argument and returns a pointer to a block of memory of that size. If there is not enough memory to allocate the requested block, the function returns NULL and an error message is printed.

3. The free_memory function takes a pointer to a block of memory as an argument and marks it as free. The function also checks if the previous or next block is also free and merges them to minimize fragmentation.

4. The handle_memory_leak function is called when the program exits and checks if there are any blocks of memory that have not been freed. If there are any leaks, the function prints the memory location and size of the leak.

5. In the main function, the memory allocator is initialized with a total size of 10 * BLOCK_SIZE. The program allocate 3 blocks of memory, free one and then exits.
The function atexit(handle_memory_leak) is also called to check for memory leaks at the end of the program.
