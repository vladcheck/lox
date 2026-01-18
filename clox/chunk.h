#ifndef clox_chunk_h
#define clox_chunk_h

#include "memory.h"
#include "value.h"

typedef enum
{
    OP_RETURN,
    OP_CONSTANT,
} OpCode;

// Chunk acts like a dynamic array
typedef struct
{
    int count; // bytes in use
    int capacity;
    uint8_t *code;
    ValueArray constants;
} Chunk;

void initChunk(Chunk *chunk);
void freeChunk(Chunk *chunk);
void writeChunk(Chunk *chunk, uint8_t byte);
int addConstant(Chunk *chunk, Value value);

#endif