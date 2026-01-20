#ifndef clox_chunk_h
#define clox_chunk_h

#include "memory.h"
#include "value.h"

typedef enum
{
    OP_RETURN,
    OP_CONSTANT,
    OP_CONSTANT_LONG,
    OP_NEGATE,
    OP_PRINT,
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    OP_NOT,
    OP_OR,
    OP_XOR,
    OP_AND,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_ADD,
    OP_DIAMOND,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_POP // pops value off of stack and forgets it
} OpCode;

// Chunk acts like a dynamic array
typedef struct
{
    int count; // bytes in use
    int capacity;
    uint8_t *code;
    int *lines;
    ValueArray constants;
} Chunk;

void initChunk(Chunk *chunk);
void freeChunk(Chunk *chunk);
void writeChunk(Chunk *chunk, uint8_t byte, int line);
int addConstant(Chunk *chunk, Value value);

#endif