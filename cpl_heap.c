#include <stdio.h>
#include <stdlib.h>

#define MEM_SIZE 20

int mem[MEM_SIZE]; // Memory array: 0 = free, 1 = allocated

typedef struct Block {
    int start;
    int size;
    struct Block* next;
} Block;

Block* freeList = NULL;
Block* allocList = NULL;

void init_mem() {
    freeList = (Block*)malloc(sizeof(Block));
    freeList->start = 0;
    freeList->size = MEM_SIZE;
    freeList->next = NULL;
    for (int i = 0; i < MEM_SIZE; i++) mem[i] = 0;
}

void show_mem() {
    printf("Memory: ");
    for (int i = 0; i < MEM_SIZE; i++) printf("%d ", mem[i]);
    printf("\nFree List: ");
    Block* temp = freeList;
    while (temp) {
        printf("[Start: %d, Size: %d] ", temp->start, temp->size);
        temp = temp->next;
    }
    printf("\nAllocated List: ");
    temp = allocList;
    while (temp) {
        printf("[Start: %d, Size: %d] ", temp->start, temp->size);
        temp = temp->next;
    }
    printf("\n");
}

void merge_free() {
    Block* cur = freeList;
    while (cur && cur->next) {
        if (cur->start + cur->size == cur->next->start) {
            cur->size += cur->next->size;
            Block* temp = cur->next;
            cur->next = cur->next->next;
            free(temp);
        } else {
            cur = cur->next;
        }
    }
}

void allocate(int size) {
    Block* best = NULL;
    Block* prev = NULL;
    Block* cur = freeList;
    Block* bestPrev = NULL;

    while (cur) {
        if (cur->size >= size) {
            if (!best || cur->size < best->size) {
                best = cur;
                bestPrev = prev;
            }
        }
        prev = cur;
        cur = cur->next;
    }

    if (!best) {
        printf("Allocation failed: Not enough memory\n");
        return;
    }

    int start = best->start;
    for (int i = start; i < start + size; i++) mem[i] = 1;

    Block* newBlock = (Block*)malloc(sizeof(Block));
    newBlock->start = start;
    newBlock->size = size;
    newBlock->next = allocList;
    allocList = newBlock;

    best->start += size; // the remaining size of best after allocating size = best->size - size.
    best->size -= size;
    if (best->size == 0) {
        if (bestPrev) bestPrev->next = best->next;
        else freeList = best->next;
        free(best);
    }

    printf("Allocated %d units at %d\n", size, start);
}

void free_mem(int start) {
    Block* prev = NULL;
    Block* cur = allocList;

    while (cur) {
        if (cur->start == start) {
            for (int i = start; i < start + cur->size; i++) mem[i] = 0;

            if (prev) prev->next = cur->next;
            else allocList = cur->next;
            
            Block* newFree = (Block*)malloc(sizeof(Block));
            newFree->start = start;
            newFree->size = cur->size;
            newFree->next = freeList;
            freeList = newFree;

            free(cur);
            merge_free();
            printf("Freed memory at %d\n", start);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
    printf("Free failed: Invalid start address\n");
}

int main() {
    init_mem();
    allocate(5);
    allocate(3);
    show_mem();
    free_mem(0);
    show_mem();
    allocate(2);
    show_mem();
    return 0;
}
